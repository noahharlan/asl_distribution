# Contributed by Qualcomm Connected Experiences, Inc.,
# with authorization from the AllSeen Alliance, Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
# Pursuant to Section 1 of the License, the work of authorship constituting
# a Work and any Contribution incorporated in the Work shall mean only that
# Contributor's code submissions authored by that Contributor.  Any rights
# granted under the License are conditioned upon acceptance of these
# clarifications.
#ifndef _AJ_CONNECT_H
#define _AJ_CONNECT_H

/**
 * @file aj_connect.h
 * @defgroup aj_connect Bus Connection Management
 * @{
 */
/******************************************************************************
 * Copyright (c) 2012-2015, AllSeen Alliance. All rights reserved.
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

#include <ajtcl/aj_target.h>
#include <ajtcl/aj_status.h>
#include <ajtcl/aj_bus.h>
#include <ajtcl/aj_disco.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set the minimum acceptable routing node protocol version.
 *
 * @param min       Minimum acceptable protocol version
 */
void AJ_SetMinProtoVersion(uint8_t min);

/**
 * Set the amount of time to wait for routing node responses.
 *
 * @param selection time for selecting routing node responses
 */
void AJ_SetSelectionTimeout(uint32_t selection);

/**
 * Get the RN selection timeout
 *
 * @return          Current selection timeout
 */
uint32_t AJ_GetSelectionTimeout(void);

/**
 * Get the minimum acceptable routing node protocol version.
 *
 * @return          Minimum acceptable protocol version
 */
uint8_t AJ_GetMinProtoVersion();

/**
 * Get the routing nodes protocol version
 *
 * @return          The routing nodes protocol version
 *                  0 if not connected to a routing node
 */
AJ_EXPORT
uint8_t AJ_GetRoutingProtoVersion(void);

/**
 * Authenticate with the daemon
 *
 * @param bus                   The bus attachment to authenticate
 *
 * @return        Return AJ_Status
 */
AJ_Status AJ_Authenticate(AJ_BusAttachment* bus);

/**
 * Establish an AllJoyn connection.
 *
 * @param  bus                The bus attachment to connect.
 * @param  serviceName        Name of a specific service to connect to, NULL for the default name.
 * @param  timeout            How long to spend attempting to connect
 *
 * @return
 *         - AJ_OK if the connection was succesfully established
 *         - AJ_ERR_TIMEOUT if the connection attempt timed out
 */
AJ_EXPORT
AJ_Status AJ_Connect(AJ_BusAttachment* bus, const char* serviceName, uint32_t timeout);

#ifdef AJ_ARDP
/**
 * Establish an ARDP-based AllJoyn UDP connection.
 *
 * @param  bus                The bus attachment to connect.
 * @param  context            The context that will be used to send and receive data
 * @param  service            The connection information
 * @param  netSock            The netSock
 *
 * @return
 *         - AJ_OK if the connection was succesfully established
 *         - AJ_ERR_TIMEOUT if the connection attempt timed out
 */
AJ_EXPORT
AJ_Status AJ_ARDP_UDP_Connect(AJ_BusAttachment* bus, void* context, const AJ_Service* service, AJ_NetSocket* netSock);

#endif

/**
 * Find a daemon, connect to it and then authenticate.
 *
 * @param  bus                The bus attachment to connect.
 * @param  serviceName        Name of a specific service to connect to, NULL for the default name.
 * @param  timeout            How long to spend attempting to connect
 *
 * @return
 *         - AJ_OK if the connection was succesfully established
 *         - AJ_ERR_TIMEOUT if the connection attempt timed out
 */
AJ_EXPORT
AJ_Status AJ_FindBusAndConnect(AJ_BusAttachment* bus, const char* serviceName, uint32_t timeout);


/**
 * Terminate an AllJoyn connection
 *
 * @param  bus  The bus attachment to disconnect.
 */
AJ_EXPORT
void AJ_Disconnect(AJ_BusAttachment* bus);

/**
 * @deprecated
 * Bus authentication password function prototype for requesting a
 * password (to authenticate with the daemon) from the application.
 *
 * @param  buffer  The buffer to receive the password
 * @param  bufLen  The size of the buffer
 *
 * @return  Returns the length of the password. If the length is zero,
 *          this will be treated as a rejected password request.
 */
typedef uint32_t (*BusAuthPwdFunc)(uint8_t* buffer, uint32_t bufLen);

/**
 * @deprecated
 * Set the callback for the application to provide a password for authentication to the daemon bus
 *
 * @param callback  The callback provided by the application
 */
AJ_EXPORT
void SetBusAuthPwdCallback(BusAuthPwdFunc callback);

/**
 * Check whether we have already attempted to connect to the routing node
 * specified by service.
 *
 * @param service   The service info struct
 *
 * @return          TRUE if blacklisted
 */
uint8_t AJ_IsRoutingNodeBlacklisted(AJ_Service* service);

void AJ_AddRoutingNodeToResponseList(AJ_Service* service);

AJ_Status AJ_SelectRoutingNodeFromResponseList(AJ_Service* service);

uint8_t AJ_GetRoutingNodeResponseListSize();

/**
 * Clear the list of blacklisted routing nodes.
 */
void AJ_InitRoutingNodeBlacklist();

void AJ_InitRoutingNodeResponselist();

#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif
