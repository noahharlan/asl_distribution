/**
 * @file
 * BusObject responsible for implementing the standard DBus methods (org.freedesktop.DBus.*)
 * for messages directed to the bus.
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
#ifndef _ALLJOYN_DBUSOBJ_H
#define _ALLJOYN_DBUSOBJ_H

#include <qcc/platform.h>

#include <qcc/String.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/Message.h>

#include "Bus.h"
#include "DaemonRouter.h"
#include "NameTable.h"

namespace ajn {

/** Forward Declaration */
class BusController;

/**
 * BusObject responsible for implementing the standard DBus methods (org.freedesktop.DBus.*)
 * for messages directed to the bus.
 */
class DBusObj : public BusObject, public NameListener {
  public:

    /**
     * Constructor
     *
     * @param bus             The bus instance
     * @param router          The DaemonRouter associated with the bus.
     * @param busController   Controller that created this object.
     */
    DBusObj(Bus& bus, BusController* busController);

    /**
     * Destructor
     */
    ~DBusObj();

    /**
     * Initialize and register this DBusObj instance.
     *
     * @return ER_OK if successful.
     */
    QStatus Init();

    /**
     * Stop DBusObj.
     *
     * @return ER_OK if successful.
     */
    QStatus Stop() { return ER_OK; }

    /**
     * Join DBusObj.
     *
     * @return ER_OK if successful.
     */
    QStatus Join() { return ER_OK; }

    /**
     * Called when object is successfully registered.
     */
    void ObjectRegistered(void);

    void NameOwnerChanged(const qcc::String& alias,
                          const qcc::String* oldOwner, SessionOpts::NameTransferType oldOwnerNameTransfer,
                          const qcc::String* newOwner, SessionOpts::NameTransferType newOwnerNameTransfer);

    /**
     * Respond to a bus request for the list of registered bus names (both unique and well-known).
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void ListNames(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request for the list of activatable bus names.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void ListActivatableNames(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Determine whether a given bus name is registered (owned).
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void NameHasOwner(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Return the unique name of the endpoint that owns the given well-known name.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void GetNameOwner(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to reserve a bus name
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void RequestName(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to release a bus name
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void ReleaseName(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to start a service.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void StartServiceByName(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to get the unix user id of the process associated with an endpoint.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void GetConnectionUnixUser(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to get the process id of the process associated with an endpoint.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void GetConnectionUnixProcessID(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to add a bus routing rule.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void AddMatch(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to remove a bus routing rule.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void RemoveMatch(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to get the global (for all transports/endpoints) UUID.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void GetId(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to update the activation environment.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void UpdateActivationEnvironment(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to get the list of queued owners.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void ListQueuedOwners(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to get the ADT audit session data.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void GetAdtAuditSessionData(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to get the SE Linux security context for a connection.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void GetConnectionSELinuxSecurityContext(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Respond to a bus request to reload the config file.
     *
     * @param member  Member.
     * @param msg     The incoming message.
     */
    void ReloadConfig(const InterfaceDescription::Member* member, Message& msg);


  private:

    Bus& bus;                              /**< The bus */
    DaemonRouter& router;                  /**< The daemon-side router associated with the bus */
    const InterfaceDescription* dbusIntf;  /**< org.freedesktop.DBus interface */
    BusController* busController;          /**< The BusController that created this object */


    /**
     * Called upon completion of NameTable::AddAlias operation. This callback is called
     * BEFORE any NameTable listeners are called.
     * @see NameTable
     *
     * @param aliasName    Name of alias.
     * @param disposition  Alias disposition.
     * @param context      Context passed to AddAlias
     */
    void AddAliasComplete(const qcc::String& aliasName, uint32_t disposition, void* context);

    /**
     * Called upon completion of NameTable::RemoveAlias operation. This callback is called
     * BEFORE any NameTable listeners are called.
     * @see NameTable
     *
     * @param aliasName    Name of alias.
     * @param disposition  Alias disposition.
     * @param context      Context passed to AddAlias
     */
    void RemoveAliasComplete(const qcc::String& aliasName, uint32_t disposition, void* context);

    friend class ServiceStartHandler;
};

}

#endif
