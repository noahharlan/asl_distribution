/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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

#include <alljoyn/gateway/ConnectorApp.h>
#include <alljoyn/gateway/GatewayController.h>
#include <qcc/StringUtil.h>
#include "Constants.h"
#include "PayloadAdapter.h"


namespace ajn {
namespace gwc {
using namespace gwcConsts;

ChangedSignalTask ConnectorApp::m_ChangedSignalTask;
AsyncTaskQueue ConnectorApp::m_ApplicationSignalQueue(&ConnectorApp::m_ChangedSignalTask);

ChangedSignalData::ChangedSignalData(const ajn::MsgArg* returnArgs, const qcc::String& AppId) : m_ConnectorAppStatus()
{
    m_Status = m_ConnectorAppStatus.init(returnArgs);
    if (m_Status != ER_OK) {
        QCC_LogError(m_Status, ("m_ConnectorAppStatus.init failed"));
    }
    m_AppId = AppId;
}


void ChangedSignalTask::OnTask(TaskData const* taskdata)
{
    const ChangedSignalData* d = static_cast<const ChangedSignalData*>(taskdata);
    if (m_Handler) {
        if (d->getStatus() == ER_OK) {
            m_Handler->onStatusSignal(d->getAppId(), d->getConnectorAppStatus());
        } else {
            m_Handler->onError(d->getAppId(), d->getStatus());
        }
    } else {
        QCC_DbgHLPrintf(("Got signal, no handler"));
    }
}

QStatus ConnectorApp::init(const qcc::String& gwBusName, ajn::MsgArg*appInfo)
{
    m_ConnectorCapabilities = NULL;
    m_ConfigurableRules = NULL;
    m_ConnectorAppStatus = NULL;
    m_AclWriteResponse = NULL;
    m_SignalMethod = NULL;

    ConnectorApp::m_ApplicationSignalQueue.Start();

    m_GwBusName = gwBusName;

    char* AppId;

    char* FriendlyName;

    char* ObjectPath;

    char* AppVersion;

    QStatus status = appInfo->Get("(ssos)", &AppId, &FriendlyName, &ObjectPath, &AppVersion);

    if (status == ER_OK) {
        m_AppId = AppId;

        m_FriendlyName = FriendlyName;

        m_ObjectPath = ObjectPath;

        m_AppVersion = AppVersion;
    } else { QCC_LogError(status, ("MsgArg get failed")); return status; }

    QCC_DbgTrace(("In ConnectorApp Constructor"));


    BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

    {
        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {

            status = busAttachment->CreateInterface(interfaceName.c_str(), interfaceDescription);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not create interface"));
                return status;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_GETMANIFESTFILE.c_str(), NULL, "s", "ManifestFile");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return status;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_GETMANIFESTINTERFACES.c_str(), NULL, "a((obs)a(ssb))a((obs)a(ssb))", "exposedServices,remotedServices");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return status;
            }


            status = interfaceDescription->AddMethod(AJ_METHOD_GETAPPSTATUS.c_str(), NULL, "qsqq", "installStatus,installDescription,connectionStatus,operationalStatus");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return status;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_RESTARTAPP.c_str(), NULL, "q", "restartResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return status;
            }

            status = interfaceDescription->AddSignal(AJ_SIGNAL_APPSTATUSCHANGED.c_str(), "qsqq", "installStatus,installDescription,connectionStatus,operationalStatus");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return status;
            }


            status = interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddProperty"));
                return status;
            }

            interfaceDescription->Activate();

        }
        m_SignalMethod = interfaceDescription->GetMember(AJ_SIGNAL_APPSTATUSCHANGED.c_str());
    }

    {
        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {

            status = busAttachment->CreateInterface(interfaceName.c_str(), interfaceDescription);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not create interface"));
                return status;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_CREATEACL.c_str(), "sa(obas)a(saya(obas))a{ss}a{ss}", "qso", "aclName,exposedServices,remotedApps,metadata,aclResponseCode,aclId,objectPath");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return status;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_DELETEACL.c_str(), "s", "q", "aclId,aclResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return status;
            }


            status = interfaceDescription->AddMethod(AJ_METHOD_LISTACLS.c_str(), NULL, "a(ssqo)", "aclsList");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return status;
            }

            status = interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddProperty"));
                return status;
            }

            interfaceDescription->Activate();
        }
    }

    QCC_DbgTrace(("org.alljoyn.gwagent.ctrl.App created."));

    return ER_OK;

}

ConnectorApp::~ConnectorApp() {
    ConnectorApp::m_ApplicationSignalQueue.Stop();
}

const qcc::String& ConnectorApp::getGwBusName() {
    return m_GwBusName;
}

const qcc::String& ConnectorApp::getAppId() {
    return m_AppId;
}

const qcc::String& ConnectorApp::getFriendlyName() {
    return m_FriendlyName;
}

const qcc::String& ConnectorApp::getObjectPath() {
    return m_ObjectPath;
}

const qcc::String& ConnectorApp::getAppVersion() {
    return m_AppVersion;
}

void ConnectorApp::emptyVector()
{
    for (size_t indx = 0; indx < m_Acls.size(); indx++) {
        QStatus status = m_Acls[indx]->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_Acls[indx];
    }

    m_Acls.clear();
}

QStatus ConnectorApp::retrieveManifestFile(SessionId sessionId, qcc::String& xml)
{
    QStatus status = ER_OK;

    {
        BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }


        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLERAPP_INTERFACE.c_str(), AJ_METHOD_GETMANIFESTFILE.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to getInstalledApps failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        char*manifestFile;

        status = returnArgs[0].Get("s", &manifestFile);

        xml = qcc::String(manifestFile);

    }
end:

    return status;
}

//Important: sessionId is used only to connect to the gateway, the access rules retrieved will always erase the previous access rules.
QStatus ConnectorApp::retrieveConnectorCapabilities(SessionId sessionId, ConnectorCapabilities** connectorCapabilities)
{
    QStatus status = ER_OK;

    {
        if (m_ConnectorCapabilities) {
            delete m_ConnectorCapabilities;
            m_ConnectorCapabilities = NULL;
        }



        BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLERAPP_INTERFACE.c_str(), AJ_METHOD_GETMANIFESTINTERFACES.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetManifestInterfaces failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 2) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        m_ConnectorCapabilities = new ConnectorCapabilities();
        status = m_ConnectorCapabilities->init(returnArgs);

        if (status != ER_OK) {
            delete m_ConnectorCapabilities;
            m_ConnectorCapabilities = NULL;
            QCC_LogError(status, ("Call to ConnectorCapabilities init failed"));
            goto end;
        }

        *connectorCapabilities = m_ConnectorCapabilities;

    }
end:

    return status;

}

QStatus ConnectorApp::retrieveApplicableConnectorCapabilities(SessionId sessionId, std::vector<AnnouncementData*> const& announcements, AclRules** configurableRules) {

    QStatus status = ER_OK;
    if (m_ConfigurableRules) {
        delete m_ConfigurableRules;
        m_ConfigurableRules = NULL;
    }

    ConnectorCapabilities*tmp;

    status = retrieveConnectorCapabilities(sessionId, &tmp);

    if (status != ER_OK) {
        QCC_LogError(status, ("RetrieveConnectorCapabilities failed in RetrieveConfigurableRules"));
        return status;
    }

    std::vector<RemotedApp*> remotedApps;

    status = extractRemotedApps(m_ConnectorCapabilities->getRemotedServices(), announcements, remotedApps);
    if (status != ER_OK) {
        QCC_LogError(status, ("extractRemotedApps failed in RetrieveConfigurableRules"));

        return status;
    }


    m_ConfigurableRules = new AclRules();

    status = m_ConfigurableRules->init(m_ConnectorCapabilities->getExposedServices(), remotedApps);
    if (status != ER_OK) {
        QCC_LogError(status, ("RetrieveConnectorCapabilities failed in RetrieveConfigurableRules"));

        delete m_ConfigurableRules;
        m_ConfigurableRules = NULL;

        return status;
    }

    *configurableRules = m_ConfigurableRules;

    return status;
}

QStatus ConnectorApp::retrieveStatus(SessionId sessionId, ConnectorAppStatus** applicationStatus)
{
    QStatus status;
    {
        if (m_ConnectorAppStatus) {
            delete m_ConnectorAppStatus;
            m_ConnectorAppStatus = NULL;
        }

        BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLERAPP_INTERFACE.c_str(), AJ_METHOD_GETAPPSTATUS.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetManifestInterfaces failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 4) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        m_ConnectorAppStatus = new ConnectorAppStatus();
        status = m_ConnectorAppStatus->init(returnArgs);
        if (status != ER_OK) {
            QCC_LogError(status, ("m_ConnectorAppStatus->init failed"));
            delete m_ConnectorAppStatus;
            m_ConnectorAppStatus = NULL;

            goto end;
        }

        *applicationStatus = m_ConnectorAppStatus;
    }
end:

    return status;
}

QStatus ConnectorApp::restart(SessionId sessionId,  RestartStatus& restartStatus)
{

    QStatus status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLERAPP_INTERFACE.c_str(), AJ_METHOD_RESTARTAPP.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetManifestInterfaces failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }



        status = returnArgs[0].Get("q", &restartStatus);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting restartStatus"));
            goto end;
        }

    }
end:

    return status;
}


void ConnectorApp::handleSignal(const ajn::InterfaceDescription::Member* member,
                                const char* srcPath, ajn::Message& msg)
{
    QCC_DbgHLPrintf((srcPath));

    const ajn::MsgArg* returnArgs;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, returnArgs);
    if (numArgs != 4) {
        QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
    } else {
        ChangedSignalData*taskdata = new ChangedSignalData(returnArgs, m_AppId);

        m_ApplicationSignalQueue.Enqueue(taskdata);
    }
}


QStatus ConnectorApp::setStatusSignalHandler(const ConnectorAppStatusSignalHandler*handler)
{

    QStatus status = ER_OK;

    BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

    m_ChangedSignalTask.setHandler(handler);

    status = busAttachment->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&ConnectorApp::handleSignal), m_SignalMethod, m_ObjectPath.c_str());

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the SignalHandler"));
        return status;
    } else {
        QCC_DbgPrintf(("Registered the SignalHandler successfully"));
    }

    qcc::String AJ_DISMISSER_INTERFACE_MATCH = "interface='" + AJ_GATEWAYCONTROLLERAPP_INTERFACE + "'";
    QCC_DbgPrintf(("StatusSignalHandler Match String is: %s", AJ_DISMISSER_INTERFACE_MATCH.c_str()));

    status = busAttachment->AddMatch(AJ_DISMISSER_INTERFACE_MATCH.c_str());
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add filter match."));
    }

    return status;
}

void ConnectorApp::unsetStatusSignalHandler() {
    m_ApplicationSignalQueue.Stop();

    BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

    QStatus status = busAttachment->UnregisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&ConnectorApp::handleSignal), m_SignalMethod, m_ObjectPath.c_str());

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unregister the SignalHandler"));
    } else {
        QCC_DbgPrintf(("Unregistered the SignalHandler successfully"));
    }

    m_ChangedSignalTask.unSetHandler();
    m_ApplicationSignalQueue.Start();
}


QStatus ConnectorApp::createAcl(SessionId sessionId, const qcc::String& name, AclRules* aclRules, AclWriteResponse** aclWriteResponse)
{
    QStatus status = ER_OK;


    if (m_AclWriteResponse) {
        delete m_AclWriteResponse;
        m_AclWriteResponse = NULL;
    }

    {
        BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        std::map<qcc::String, qcc::String> internalMetadata;

        // Validate the rules in the ACL

        ConnectorCapabilities*connectorCapabilities;

        status = retrieveConnectorCapabilities(sessionId, &connectorCapabilities);

        if (status != ER_OK) {
            QCC_LogError(status, ("Retreiving manifest rules failed"));
            goto end;
        }

        std::vector<RuleObjectDescription*> expServicesTargetOut;

        std::vector<RuleObjectDescription*> invalidExpServices =
            Acl::validateManifObjDescs(aclRules->getExposedServices(),
                                       expServicesTargetOut,
                                       connectorCapabilities->getExposedServices());

        const std::vector<RemotedApp*> remotedAppsIn =  aclRules->getRemotedApps();

        std::vector<RemotedApp*> invalidRemotedApps;

        std::vector<RemotedApp*> remotedAppsOut;

        for (std::vector<RemotedApp*>::const_iterator itr = remotedAppsIn.begin(); itr != remotedAppsIn.end(); itr++) {
            std::vector<RuleObjectDescription*> remotedAppsTarget;

            std::vector<RuleObjectDescription*> invalidRemAppRules = Acl::validateManifObjDescs((*itr)->getRuleObjDesciptions(), remotedAppsTarget, connectorCapabilities->getRemotedServices());

            if (invalidRemAppRules.size() > 0) {
                RemotedApp*ctrlRemotedApp = new RemotedApp();
                QStatus status = ctrlRemotedApp->init((*itr), invalidRemAppRules);

                if (status != ER_OK) {
                    QCC_LogError(status, ("RemotedApp init failed"));
                    delete ctrlRemotedApp;
                    ctrlRemotedApp = NULL;

                    for (std::vector<RemotedApp*>::iterator itr = invalidRemotedApps.begin(); itr != invalidRemotedApps.end(); itr++) {
                        delete (*itr);
                    }

                    goto end;
                }

                invalidRemotedApps.push_back(ctrlRemotedApp);
            }

            //If there is no any marshaled rule, no valid rule was found -> continue
            if (remotedAppsTarget.size() == 0) {
                continue;
            }

            RemotedApp*remotedApp = new RemotedApp();

            //Populate the RemotedApp
            status = remotedApp->init((*itr), remotedAppsTarget);

            if (status != ER_OK) {
                QCC_LogError(status, ("RemotedApp init failed"));
                delete remotedApp;
                remotedApp = NULL;

                for (std::vector<RemotedApp*>::iterator itr = invalidRemotedApps.begin(); itr != invalidRemotedApps.end(); itr++) {
                    delete (*itr);
                }


                for (std::vector<RemotedApp*>::iterator itr = remotedAppsOut.begin(); itr != remotedAppsOut.end(); itr++) {
                    delete (*itr);
                }


                goto end;
            }


            remotedAppsOut.push_back(remotedApp);

            //Store this application data in the internal metadata
            qcc::String AppId;
            const uint8_t*binary_appid = (*itr)->getAppId();

            AppId = qcc::BytesToHexString(binary_appid, UUID_LENGTH);

            qcc::String keyPrefix = (*itr)->getDeviceId() + "_" + AppId;
            internalMetadata.insert(std::pair<qcc::String, qcc::String>(keyPrefix + AJSUFFIX_APP_NAME, remotedApp->getAppName()));
            internalMetadata.insert(std::pair<qcc::String, qcc::String>(keyPrefix + AJSUFFIX_DEVICE_NAME, remotedApp->getDeviceName()));
        }

        AclRules*transmittedAcessRules = new AclRules();
        QStatus status = transmittedAcessRules->init(expServicesTargetOut, remotedAppsOut);

        if (status != ER_OK) {
            QCC_LogError(status, ("AclRules init failed"));
            delete transmittedAcessRules;
            transmittedAcessRules = NULL;

            for (std::vector<RemotedApp*>::iterator itr = invalidRemotedApps.begin(); itr != invalidRemotedApps.end(); itr++) {
                delete (*itr);
            }


            for (std::vector<RemotedApp*>::iterator itr = remotedAppsOut.begin(); itr != remotedAppsOut.end(); itr++) {
                delete (*itr);
            }


            goto end;
        }

        std::vector<MsgArg*> aclRulesVector;

        status = PayloadAdapter::MarshalAclRules(*transmittedAcessRules, aclRulesVector);

        transmittedAcessRules->release();
        delete transmittedAcessRules;
        transmittedAcessRules = NULL;


        // end of marshalling

        if (status != ER_OK) {
            QCC_LogError(status, ("PayloadAdapter::MarshalAclRules failed"));
            goto end;
        }

        MsgArg*aclNameArg = new MsgArg("s", name.c_str());

        aclRulesVector.insert(aclRulesVector.begin(), aclNameArg);

        MsgArg*internalMetadataKeyValueArg = PayloadAdapter::MarshalMetadata(internalMetadata, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] internalMetadataKeyValueArg;

            goto end;
        }

        MsgArg*internalMetadataKeyValueMapArg = new MsgArg("a{ss}", internalMetadata.size(), internalMetadataKeyValueArg);

        aclRulesVector.push_back(internalMetadataKeyValueMapArg);

        for (std::vector<MsgArg*>::const_iterator itr = aclRulesVector.begin(); itr != aclRulesVector.end(); itr++) {
            (*itr)->SetOwnershipFlags(MsgArg::OwnsArgs, true);
        }

        MsgArg*args = new MsgArg[aclRulesVector.size()];

        for (size_t x = 0; x != aclRulesVector.size(); x++) {
            args[x] = *aclRulesVector[x];
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE.c_str(), AJ_METHOD_CREATEACL.c_str(), args, 5, replyMsg);

        delete [] args;
        args = NULL;

        if (status != ER_OK) {
            QCC_LogError(status, ("Call to CreateAcl failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 3) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        AclResponseCode aclResponseCode;
        char*aclId;
        char*objectPath;
        status = returnArgs[0].Get("q", &aclResponseCode);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }
        status = returnArgs[1].Get("s", &aclId);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }
        status = returnArgs[2].Get("o", &objectPath);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }

        AclRules*invalidAcessRules = new AclRules();
        status = invalidAcessRules->init(invalidExpServices, invalidRemotedApps);

        if (status != ER_OK) {
            QCC_LogError(status, ("invalid access rules init failed"));
            delete invalidAcessRules;
            invalidAcessRules = NULL;
            goto end;
        }

        m_AclWriteResponse = new AclWriteResponse(aclId, aclResponseCode, invalidAcessRules, objectPath);

        *aclWriteResponse = m_AclWriteResponse;
    }
end:

    return status;
}


QStatus ConnectorApp::retrieveAcls(SessionId sessionId, std::vector <Acl*>& acls)
{

    QStatus status = ER_OK;

    emptyVector();

    {
        BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }


        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE.c_str(), AJ_METHOD_LISTACLS.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetACLs failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        int numACLs;
        MsgArg* tempEntries;
        status = returnArgs[0].Get("a(ssqo)", &numACLs, &tempEntries);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }

        for (int i = 0; i < numACLs; i++) {

            Acl*accessControlList = new Acl();

            status = accessControlList->init(m_GwBusName, &tempEntries[i]);
            if (status != ER_OK) {
                QCC_LogError(status, ("Acl init failed"));

                delete accessControlList;
                accessControlList = NULL;

                goto end;
            }

            m_Acls.push_back(accessControlList);
            acls.push_back(accessControlList);
        }
    }
end:

    return status;
}

QStatus ConnectorApp::deleteAcl(SessionId sessionId, const qcc::String& aclId, AclResponseCode& responseCode) {

    QStatus status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayController::getInstance()->getBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }



        MsgArg aclIdArg;

        aclIdArg.Set("s", aclId.c_str());

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE.c_str(), AJ_METHOD_DELETEACL.c_str(), &aclIdArg, 1, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetManifestInterfaces failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }



        status = returnArgs[0].Get("q", &responseCode);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting restartStatus"));
            goto end;
        }

    }
end:

    return status;
}

QStatus ConnectorApp::release() {

    emptyVector();

    if (m_ConnectorCapabilities) {
        m_ConnectorCapabilities->release();
        delete m_ConnectorCapabilities;
        m_ConnectorCapabilities = NULL;
    }

    if (m_ConfigurableRules) {
        m_ConfigurableRules->release();
        delete m_ConfigurableRules;
        m_ConfigurableRules = NULL;
    }

    if (m_ConnectorAppStatus) {
        delete m_ConnectorAppStatus;
        m_ConnectorAppStatus = NULL;
    }

    if (m_AclWriteResponse) {
        m_AclWriteResponse->release();
        delete m_AclWriteResponse;
        m_AclWriteResponse = NULL;
    }

    return ER_OK;
}

QStatus
ConnectorApp::extractRemotedApps(const std::vector<RuleObjectDescription*>& remotedServices,
                                 std::vector<AnnouncementData*> const& announcements,
                                 std::vector<RemotedApp*>& remotedApps) {

    std::vector<AnnouncementData*>::const_iterator annIter;
    QStatus status = ER_OK;

    for (annIter = announcements.begin(); annIter != announcements.end(); annIter++) {

        RemotedApp*remotedApp = extractRemotedApp(remotedServices, *annIter, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("ExtractRemotedApp "));
        }

        if (remotedApp != NULL) {
            remotedApps.push_back(remotedApp);
        }
    }

    return status;
}


RemotedApp* ConnectorApp::extractRemotedApp(const std::vector<RuleObjectDescription*>& remotedServices, const AnnouncementData*ann, QStatus& status) {

    if (ann == NULL) {
        status = ER_BAD_ARG_2;
        return NULL;
    }

    std::map<RuleObjectPath, std::set<RuleInterface> > remotedRules;

    ajn::services::AboutClient::ObjectDescriptions::const_iterator bod;

    for (bod = ann->GetObjectDescriptions().begin(); bod !=  ann->GetObjectDescriptions().end(); bod++) {

        const qcc::String objPath = bod->first;

        std::vector<qcc::String> ifacesToMatch = (*bod).second;

        std::vector<RuleObjectDescription*>::const_iterator moj;

        for (moj = remotedServices.begin(); moj != remotedServices.end(); moj++) {

            RuleObjectDescription*curr_moj = (*moj);

            RuleObjectPath*manop       = curr_moj->getObjectPath();
            const std::set<RuleInterface>*manifs      = curr_moj->getInterfaces();
            int manifsSize  = (int)manifs->size();

            //Check object path suitability: if manifest objPath is a prefix of BusObjDesc objPath
            //or both object paths are equal

            if ((manop->isPrefix() && stringStartWith(manop->getPath(), objPath)) ||
                (manop->getPath().compare(objPath) == 0)) {

                std::set<RuleInterface>  resIfaces;

                std::vector<qcc::String>::iterator ifacesToMatchIter = ifacesToMatch.begin();

                //Search for the interfaces that comply with the manifest interfaces
                while (ifacesToMatchIter != ifacesToMatch.end()) {

                    qcc::String iface = *ifacesToMatchIter;

                    //If there are no interfaces in the manifest, it means that all the interfaces are supported
                    //add them without display names
                    if (manifsSize == 0) {
                        resIfaces.insert(RuleInterface(iface, "", false));
                        ifacesToMatch.erase(ifacesToMatchIter++);
                        continue;
                    }

                    std::set<RuleInterface>::iterator manIface;


                    for (manIface = manifs->begin(); manIface != manifs->end(); manIface++) {

                        if ((*manIface).getName().compare(iface) == 0) {

                            resIfaces.insert(*manIface);          //found interface -> add it to the results
                            ifacesToMatch.erase(ifacesToMatchIter++);
                            break;
                        }
                    }

                    if ((ifacesToMatch.size() == 0) || (ifacesToMatchIter == ifacesToMatch.end())) {
                        break;
                    }

                    ifacesToMatchIter++;

                }        //while :: ifacesToMatch

                //not found any matched interfaces, continue to the next manifest rule
                if (resIfaces.size() == 0) {
                    continue;
                }

                //We add the manifest rule, if the manifest OP is the prefix of the BOD.objPath
                //or both object paths are equal
                RuleObjectPath storeOp(manop->getPath(), manop->getFriendlyName(), false, manop->isPrefixAllowed());
                std::map<RuleObjectPath, std::set<RuleInterface> >::const_iterator remotedIfaces = remotedRules.find(storeOp);
                if (remotedIfaces == remotedRules.end()) {
                    remotedRules.insert(std::pair<RuleObjectPath, std::set<RuleInterface> >(storeOp,  resIfaces));
                } else {
                    resIfaces.insert(remotedIfaces->second.begin(), remotedIfaces->second.end());
                }


                //We add the BOD.objPath if the manifest OP is not equal to the  BOD.objPath
                if (manop->getPath().compare(objPath)) {

                    //bodOp starts with the manOp but itself it's not a prefix
                    RuleObjectPath bodOp(objPath, "", false, manop->isPrefixAllowed());
                    remotedIfaces      = remotedRules.find(bodOp);
                    if (remotedIfaces == remotedRules.end()) {
                        remotedRules.insert(std::pair<RuleObjectPath, std::set<RuleInterface> >(bodOp,  resIfaces));
                    } else {
                        resIfaces.insert(remotedIfaces->second.begin(), remotedIfaces->second.end());
                    }
                }

            }        //if :: objPath

            //If all the BusObjectDescription interfaces have been handled, no need to continue iterating
            //over the manifest rules
            if (ifacesToMatch.size() == 0) {
                break;
            }

        }        //for :: manifest

    }        //for :: BusObjectDesc

    int rulesSize = (int)remotedRules.size();

    //Check if this announcement complies with the manifest rules
    if (rulesSize == 0) {
        return NULL;
    }

    //Create Remoted rules list
    std::vector<RuleObjectDescription*> rules;

    std::map<RuleObjectPath, std::set<RuleInterface> >::const_iterator op;

    for (op = remotedRules.begin(); op != remotedRules.end(); op++) {
        rules.push_back(new RuleObjectDescription(op->first,  op->second));
    }

    RemotedApp*remotedApp = NULL;

    remotedApp = new RemotedApp();

    status = remotedApp->init(ann->GetAboutData(), rules);

    if (status != ER_OK) {
        delete remotedApp;
        remotedApp = NULL;

        for (std::vector<RuleObjectDescription*>::iterator itr = rules.begin(); itr != rules.end(); itr++) {
            (*itr)->release();
            delete (*itr);
        }

        return NULL;
    }

    return remotedApp;
}
}
}
