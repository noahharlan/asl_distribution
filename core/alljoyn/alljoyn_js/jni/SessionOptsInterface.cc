/*
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
 */
#include "SessionOptsInterface.h"

#include <qcc/Debug.h>

#define QCC_MODULE "ALLJOYN_JS"

std::map<qcc::String, int32_t> _SessionOptsInterface::constants;

std::map<qcc::String, int32_t>& _SessionOptsInterface::Constants()
{
    if (constants.empty()) {
        CONSTANT("TRAFFIC_MESSAGES",       0x01);
        CONSTANT("TRAFFIC_RAW_UNRELIABLE", 0x02);
        CONSTANT("TRAFFIC_RAW_RELIABLE",   0x04);

        CONSTANT("PROXIMITY_ANY",          0xFF);
        CONSTANT("PROXIMITY_PHYSICAL",     0x01);
        CONSTANT("PROXIMITY_NETWORK",      0x02);

        CONSTANT("TRANSPORT_NONE",         0x0000);
        CONSTANT("TRANSPORT_LOCAL",        0x0001);
        CONSTANT("TRANSPORT_TCP",          0x0004);
        CONSTANT("TRANSPORT_UDP",          0x0100);
        CONSTANT("TRANSPORT_EXPERIMENTAL", 0x8000);
        CONSTANT("TRANSPORT_IP",           0x0104);
        CONSTANT("TRANSPORT_ANY",          0x0105);

//      CONSTANT("TRANSPORT_WLAN",         0x0004);
//      CONSTANT("TRANSPORT_WWAN",         0x0008);
//      CONSTANT("TRANSPORT_LAN",          0x0010);
//      CONSTANT("TRANSPORT_PROXIMITY",    0x0040);

    }
    return constants;
}

_SessionOptsInterface::_SessionOptsInterface(Plugin& plugin) :
    ScriptableObject(plugin, Constants())
{
    QCC_DbgTrace(("%s", __FUNCTION__));
}

_SessionOptsInterface::~_SessionOptsInterface()
{
    QCC_DbgTrace(("%s", __FUNCTION__));
}

