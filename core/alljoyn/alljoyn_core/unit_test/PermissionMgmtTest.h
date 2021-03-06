/**
 * @file
 * Helper functions for the PermissionMgmt Test Cases
 */

/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Contributed by Qualcomm Connected Experiences, Inc.,
 *    with authorization from the AllSeen Alliance, Inc.
 *    
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *    
 *        http://www.apache.org/licenses/LICENSE-2.0
 *    
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *    
 *    Pursuant to Section 1 of the License, the work of authorship constituting
 *    a Work and any Contribution incorporated in the Work shall mean only that
 *    Contributor's code submissions authored by that Contributor.  Any rights
 *    granted under the License are conditioned upon acceptance of these
 *    clarifications.
 ******************************************************************************/

#include <qcc/platform.h>
#include <gtest/gtest.h>
#include <qcc/GUID.h>
#include <qcc/StringSink.h>
#include <qcc/StringSource.h>
#include <qcc/StringUtil.h>

#include <alljoyn/KeyStoreListener.h>
#include <alljoyn/Status.h>
#include <alljoyn/Message.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <qcc/CryptoECC.h>
#include <qcc/CertificateECC.h>
#include <qcc/Log.h>
#include <alljoyn/SecurityApplicationProxy.h>
#include <alljoyn/PermissionPolicy.h>
#include "KeyStore.h"
#include "PermissionMgmtObj.h"
#include "InMemoryKeyStore.h"

namespace ajn {

class TestApplicationStateListener : public ApplicationStateListener {
  public:
    TestApplicationStateListener() : signalApplicationStateReceived(false) { }
    void State(const char* busName, const qcc::KeyInfoNISTP256& publicKeyInfo, PermissionConfigurator::ApplicationState state);
    bool signalApplicationStateReceived;
};

class TestPermissionConfigurationListener : public PermissionConfigurationListener {
  public:
    TestPermissionConfigurationListener() : factoryResetReceived(false), policyChangedReceived(false) { }
    QStatus FactoryReset();
    void PolicyChanged();
    bool factoryResetReceived;
    bool policyChangedReceived;
};

class TestPortListener : public SessionPortListener {
  public:
    qcc::String lastJoiner;
    bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts) {
        QCC_UNUSED(sessionPort);
        QCC_UNUSED(joiner);
        QCC_UNUSED(opts);
        return true;
    }
    void SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner) {
        QCC_UNUSED(sessionPort);
        QCC_UNUSED(id);
        lastJoiner = joiner;
    }
};

class BasePermissionMgmtTest : public testing::Test, public BusObject,
    public ProxyBusObject::PropertiesChangedListener {
  public:


    static const char* INTERFACE_NAME;
    static const char* NOTIFY_INTERFACE_NAME;
    static const char* ONOFF_IFC_NAME;
    static const char* TV_IFC_NAME;

    BasePermissionMgmtTest(const char* path) : BusObject(path),
        adminBus("PermissionMgmtTestAdmin", false),
        serviceBus("PermissionMgmtTestService", false),
        servicePort(0),
        consumerBus("PermissionMgmtTestConsumer", false),
        remoteControlBus("PermissionMgmtTestRemoteControl", false),
        adminAdminGroupGUID("00112233445566778899AABBCCDDEEFF"),
        consumerAdminGroupGUID("AABBCCDDEEFF00112233445566778899"),
        serviceGUID(),
        consumerGUID(),
        remoteControlGUID(),
        status(ER_OK),
        authComplete(false),
        serviceKeyListener(NULL),
        adminKeyListener(NULL),
        consumerKeyListener(NULL),
        remoteControlKeyListener(NULL),
        canTestStateSignalReception(false),
        currentTVChannel(1),
        volume(1),
        channelChangedSignalReceived(false),
        propertiesChangedSignalReceived(false),
        testASL(),
        testPCL()
    {
    }

    virtual void SetUp();

    virtual void TearDown();

    virtual void PropertiesChanged(ProxyBusObject& obj, const char* ifaceName, const MsgArg& changed, const MsgArg& invalidated, void* context);

    void EnableSecurity(const char* keyExchange);
    void CreateOnOffAppInterface(BusAttachment& bus, bool addService);
    void CreateTVAppInterface(BusAttachment& bus, bool addService);
    void CreateAppInterfaces(BusAttachment& bus, bool addService);
    void GenerateCAKeys();

    BusAttachment adminBus;
    BusAttachment serviceBus;
    SessionPort servicePort;
    TestPortListener servicePortListener;
    BusAttachment consumerBus;
    BusAttachment remoteControlBus;
    qcc::GUID128 adminAdminGroupGUID;
    qcc::GUID128 consumerAdminGroupGUID;
    qcc::GUID128 serviceGUID;
    qcc::GUID128 consumerGUID;
    qcc::GUID128 remoteControlGUID;
    qcc::KeyInfoNISTP256 adminAdminGroupAuthority;
    qcc::KeyInfoNISTP256 consumerAdminGroupAuthority;
    QStatus status;
    bool authComplete;

    QStatus InterestInChannelChangedSignal(BusAttachment* bus);
    void ChannelChangedSignalHandler(const InterfaceDescription::Member* member,
                                     const char* sourcePath, Message& msg);
    void SetApplicationStateSignalReceived(bool flag);
    const bool GetApplicationStateSignalReceived();
    void SetChannelChangedSignalReceived(bool flag);
    const bool GetChannelChangedSignalReceived();
    void SetPropertiesChangedSignalReceived(bool flag);
    const bool GetPropertiesChangedSignalReceived();

    void OnOffOn(const InterfaceDescription::Member* member, Message& msg);
    void OnOffOff(const InterfaceDescription::Member* member, Message& msg);
    void TVUp(const InterfaceDescription::Member* member, Message& msg);

    void TVDown(const InterfaceDescription::Member* member, Message& msg);
    void TVChannel(const InterfaceDescription::Member* member, Message& msg);
    typedef enum {
        SEND_SIGNAL_UNICAST = 0,
        SEND_SIGNAL_SESSIONCAST = 1,
        SEND_SIGNAL_BROADCAST = 2
    } SignalSendMethod;
    void TVChannelChanged(const InterfaceDescription::Member* member, Message& msg, SignalSendMethod sendMethod);
    void TVMute(const InterfaceDescription::Member* member, Message& msg);
    void TVInputSource(const InterfaceDescription::Member* member, Message& msg);
    QStatus Get(const char* ifcName, const char* propName, MsgArg& val);
    QStatus Set(const char* ifcName, const char* propName, MsgArg& val);
    const qcc::String& GetAuthMechanisms() const;

    AuthListener* serviceKeyListener;
    AuthListener* adminKeyListener;
    AuthListener* consumerKeyListener;
    AuthListener* remoteControlKeyListener;
    InMemoryKeyStoreListener adminKeyStoreListener;
    InMemoryKeyStoreListener serviceKeyStoreListener;
    InMemoryKeyStoreListener consumerKeyStoreListener;
    InMemoryKeyStoreListener remoteControlKeyStoreListener;
    void DetermineStateSignalReachable();
    bool canTestStateSignalReception;
    QStatus SetupBus(BusAttachment& bus);
    QStatus TeardownBus(BusAttachment& bus);
    QStatus JoinSessionWithService(BusAttachment& initiator, SessionId& sessionId);

  private:
    void RegisterKeyStoreListeners();

    uint32_t currentTVChannel;
    uint32_t volume;
    bool channelChangedSignalReceived;
    bool propertiesChangedSignalReceived;
    qcc::String authMechanisms;
    TestApplicationStateListener testASL;
};

class PermissionMgmtTestHelper {
  public:
    static QStatus CreateIdentityCertChain(BusAttachment& caBus, BusAttachment& issuerBus, const qcc::String& serial, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::String& alias, uint32_t expiredInSecs, qcc::IdentityCertificate* certChain, size_t chainCount, uint8_t* digest, size_t digestSize);
    static QStatus CreateIdentityCert(BusAttachment& issuerBus, const qcc::String& serial, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::String& alias, uint32_t expiredInSecs, qcc::IdentityCertificate& cert, uint8_t* digest, size_t digestSize, bool setEmptyAKI = false);
    static QStatus CreateIdentityCert(BusAttachment& issuerBus, const qcc::String& serial, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::String& alias, uint32_t expiredInSecs, qcc::String& der);

    static QStatus CreateIdentityCert(BusAttachment& issuerBus, const qcc::String& serial, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::String& alias, qcc::String& der);

    static QStatus CreateMembershipCert(const qcc::String& serial, BusAttachment& signingBus, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::GUID128& guild, bool delegate, uint32_t expiredInSecs, qcc::MembershipCertificate& cert, bool setEmptyAKI = false);
    static QStatus CreateMembershipCert(const qcc::String& serial, BusAttachment& signingBus, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::GUID128& guild, bool delegate, uint32_t expiredInSecs, qcc::String& der);
    static QStatus CreateMembershipCert(const qcc::String& serial, BusAttachment& signingBus, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::GUID128& guild, bool delegate, qcc::String& der);
    static QStatus CreateMembershipCert(const qcc::String& serial, BusAttachment& signingBus, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::GUID128& guild, qcc::String& der);
    static bool IsPermissionDeniedError(QStatus status, Message& msg);
    static QStatus ReadClaimResponse(Message& msg, qcc::ECCPublicKey* pubKey);
    static QStatus GenerateManifest(PermissionPolicy::Rule** retRules, size_t* count);

    static QStatus RetrieveDSAPublicKeyFromKeyStore(BusAttachment& bus, qcc::ECCPublicKey* publicKey);

    static QStatus LoadCertificateBytes(Message& msg, qcc::CertificateX509& cert);
    static QStatus InstallMembership(const qcc::String& serial, BusAttachment& bus, const qcc::String& remoteObjName, BusAttachment& signingBus, const qcc::String& subject, const qcc::ECCPublicKey* subjectPubKey, const qcc::GUID128& guild);
    static QStatus InstallMembershipChain(BusAttachment& topBus, BusAttachment& secondBus, const qcc::String& serial0, const qcc::String& serial1, const qcc::String& remoteObjName, const qcc::String& secondSubject, const qcc::ECCPublicKey* secondPubKey, const qcc::String& targetSubject, const qcc::ECCPublicKey* targetPubKey, const qcc::GUID128& guild, bool setEmptyAKI = false);
    static QStatus InstallMembershipChain(BusAttachment& caBus, BusAttachment& intermediateBus, BusAttachment& targetBus, qcc::String& leafSerial, const qcc::GUID128& sgID);
    static QStatus RetrievePublicKeyFromMsgArg(MsgArg& arg, qcc::ECCPublicKey* pubKey);
    static QStatus GetPeerPublicKey(BusAttachment& bus, ProxyBusObject& remoteObj, qcc::ECCPublicKey* pubKey);
    static QStatus ExerciseOn(BusAttachment& bus, ProxyBusObject& remoteObj);
    static QStatus ExerciseOff(BusAttachment& bus, ProxyBusObject& remoteObj);
    static QStatus ExerciseTVUp(BusAttachment& bus, ProxyBusObject& remoteObj);
    static QStatus ExerciseTVDown(BusAttachment& bus, ProxyBusObject& remoteObj);
    static QStatus ExerciseTVChannel(BusAttachment& bus, ProxyBusObject& remoteObj);
    static QStatus ExerciseTVMute(BusAttachment& bus, ProxyBusObject& remoteObj);
    static QStatus ExerciseTVInputSource(BusAttachment& bus, ProxyBusObject& remoteObj);
    static QStatus JoinPeerSession(BusAttachment& initiator, BusAttachment& responder, SessionId& sessionId);
    static QStatus GetGUID(BusAttachment& bus, qcc::GUID128& guid);
    static QStatus GetPeerGUID(BusAttachment& bus, qcc::String& peerName, qcc::GUID128& peerGuid);
    static QStatus GetTVVolume(BusAttachment& bus, ProxyBusObject& remoteObj, uint32_t& volume);
    static QStatus SetTVVolume(BusAttachment& bus, ProxyBusObject& remoteObj, uint32_t volume);
    static QStatus GetTVCaption(BusAttachment& bus, ProxyBusObject& remoteObj, size_t& propertyCount);
};

}
