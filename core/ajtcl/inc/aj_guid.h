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
#ifndef _AJ_GUID_H
#define _AJ_GUID_H

/**
 * @file aj_guid.h
 * @defgroup aj_guid Globally Unique Identifier Support
 * @{
 * @file
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

#include <ajtcl/aj_target.h>
#include <ajtcl/aj_status.h>
#include <ajtcl/aj_bus.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type for a GUID
 */
#define AJ_GUID_LEN 16
typedef struct _AJ_GUID {
    uint8_t val[AJ_GUID_LEN];       /**< string for a GUID */
} AJ_GUID;

/**
 * Type for tracking serial numbers
 */
typedef struct _AJ_SerialNum {
    uint32_t serial; /**< base serial number */
    uint64_t offset; /**< offset (from base) of allowed message */
} AJ_SerialNum;

/**
 * Return a pointer to an ASCII string representation of a GUID
 *
 * @param guid   The guid to convert
 * @param buffer The buffer to store the string
 * @param bufLen The size of the buffer.
 *
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the GUID was converted
 *          - AJ_ERR_RESOURCES if the buffer was not big enough
 */
AJ_Status AJ_GUID_ToString(const AJ_GUID* guid, char* buffer, uint32_t bufLen);

/**
 * Unpacks a string into a GUID
 *
 * @param guid  Pointer to a GUID structure
 * @param str   A hex string representation of the GUID
 *
 * @return  Return AJ_Status
 *         - AJ_OK if the conversion was succsessful
 *         - An error status otherwise
 */
AJ_Status AJ_GUID_FromString(AJ_GUID* guid, const char* str);

/**
 * Clears names from the GUID map
 */
void AJ_GUID_ClearNameMap(void);

/**
 * Adds a unique name to the GUID map.
 *
 * This also adds a match rule for the name owner changed signal to
 * detect when the name mapping can be deleted.
 *
 * @param bus         The bus attachment
 * @param guid        The GUID to add
 * @param uniqueName  A unique name that maps to the GUID
 * @param serviceName A service name that maps to the GUID
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the mapping was added
 *          - AJ_ERR_RESOURCES if there is no room to the mapping
 */
AJ_Status AJ_GUID_AddNameMapping(AJ_BusAttachment* bus, const AJ_GUID* guid, const char* uniqueName, const char* serviceName);

#define AJ_ROLE_KEY_UNDEFINED   0  /**< Indicates the session key role is undefined (only applies for group keys) */
#define AJ_ROLE_KEY_INITIATOR   1  /**< Indicates the session key was initiated by this peer */
#define AJ_ROLE_KEY_RESPONDER   2  /**< Indicates the session key was initiated by the remote peer */

/**
 * Delete a name mapping from the GUID map.
 *
 * This is called when a name owner changed signal is received
 * indicating that the unique name has gone away.
 *
 * @param bus         The bus attachment
 * @param uniqueName  The unique name that went away
 */
void AJ_GUID_DeleteNameMapping(AJ_BusAttachment* bus, const char* uniqueName);

/**
 * Looks up the GUID for a name
 *
 * @param name  The unique or well-known name to lookup
 *
 * @return  Return A pointer to a GUID or NULL, if there is no mapping.
 */
const AJ_GUID* AJ_GUID_Find(const char* name);

/**
 * Sets a session key for an entry in the GUID map
 *
 * @param uniqueName The unique name for a remote peer
 * @param key        The 16 byte session key to add
 * @param role       Indicates which peer initiated the session key
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the key was added
 *          - AJ_ERR_NO_MATCH if there is no entry to the peer
 */
AJ_Status AJ_SetSessionKey(const char* uniqueName, const uint8_t* key, uint8_t role);

/**
 * Sets a group key for an entry in the GUID map
 *
 * @param uniqueName The unique name for a remote peer
 * @param key        The 16 byte session key to add
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the key was added
 *          - AJ_ERR_NO_MATCH if there is no entry to the peer
 */
AJ_Status AJ_SetGroupKey(const char* uniqueName, const uint8_t* key);

/**
 * Gets a session key for an entry from the GUID map
 *
 * @param name  The unique or well-known name for a remote peer
 * @param key   Buffer to receive the 16 byte session key
 * @param role  Indicates which peer initiated the session key
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the key was obtained
 *          - AJ_ERR_NO_MATCH if there is no entry to the peer
 */
AJ_Status AJ_GetSessionKey(const char* name, uint8_t* key, uint8_t* role);

/**
 * Gets the peer index in the name map, used for access control list
 *
 * @param name  The unique or well-known name for a remote peer
 * @param peer  The peer index
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the index was obtained
 *          - AJ_ERR_NO_MATCH if there is no entry to the peer
 */
AJ_Status AJ_GetPeerIndex(const char* name, uint32_t* peer);

/**
 * Gets serial numbers for an entry from the GUID map
 *
 * @param name         The unique or well-known name for a remote peer
 * @param incoming     The incoming serial numbers
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the information was obtained
 *          - AJ_ERR_NO_MATCH if there is no entry to the peer
 */
AJ_Status AJ_GetSerialNumbers(const char* name, AJ_SerialNum** incoming);

/**
 * Gets unique name for an entry from the GUID map
 *
 * @param name         The well-known name for a remote peer. Unique names are ok too.
 * @param unique       Unique name corresponding to the requested name
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the information was obtained
 *          - AJ_ERR_NO_MATCH if there is no entry to the peer
 */
AJ_Status AJ_GetRemoteUniqueName(const char* name, const char** unique);

/**
 * Gets a group key for an entry from the GUID map
 *
 * @param name       The unique or well-known name for a remote peer or NULL to get the local group key.
 * @param key        Buffer to receive the 16 byte group key
 *
 * @return  Return AJ_Status
 *          - AJ_OK if the key was obtained
 *          - AJ_ERR_NO_MATCH if there is no entry to the peer
 */
AJ_Status AJ_GetGroupKey(const char* name, uint8_t* key);

/**
 * Handle an add match reply message
 *
 * @param msg    The add match reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_GUID_HandleAddMatchReply(AJ_Message* msg);

/**
 * Handle a remove match reply message
 *
 * @param msg    The remove match reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_GUID_HandleRemoveMatchReply(AJ_Message* msg);

/**
 * Handle a name has owner reply message
 *
 * @param msg    The name has owner reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_GUID_HandleNameHasOwnerReply(AJ_Message* msg);

/**
 * Handle a name owner changed message
 *
 * @param msg    The name owner changed message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_GUID_HandleNameOwnerChanged(AJ_Message* msg);

#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif
