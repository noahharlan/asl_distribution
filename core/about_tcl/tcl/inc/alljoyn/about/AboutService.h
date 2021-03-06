/******************************************************************************
 * Copyright (c) 2013 - 2014, AllSeen Alliance. All rights reserved.
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

#ifndef _ABOUTSERVICE_H_
#define _ABOUTSERVICE_H_

/** @defgroup AboutService
 *
 *  @{
 */

#include <alljoyn/about/AboutIcon.h>
#include <alljoyn/services_common/PropertyStore.h>
#include <alljoyn/services_common/Services_Common.h>

/**
 * published About feature objects and interfaces
 */
extern const AJ_InterfaceDescription AJ_AboutInterfaces[];

/*
 * Following definitions are read by the application.
 */

#define NUM_ABOUT_OBJECTS (1 + NUM_ABOUT_ICON_OBJECTS)

#define ABOUT_APPOBJECTS \
    { "/About",              AJ_AboutInterfaces }, \
    ABOUT_ICON_APPOBJECTS

#define ABOUT_ANNOUNCEOBJECTS ABOUT_APPOBJECTS

/*
 * About Feature API
 */

/**
 * start the About feature passing relevant initialization parameters
 * @param servicePort
 * @param announceObjects
 * @return status
 */
AJ_Status AJ_About_Start(uint16_t servicePort, AJ_Object* announceObjects);

/**
 * handler for property getters associated with org.alljoyn.About
 * @param replyMsg
 * @param propId
 * @param context
 * @return aj_status
 */
AJ_Status AJ_About_PropGetHandler(AJ_Message* replyMsg, uint32_t propId, void* context);

/**
 * handler for property setters associated with org.alljoyn.About
 * @param replyMsg
 * @param propId
 * @param context
 * @return aj_status
 */
AJ_Status AJ_About_PropSetHandler(AJ_Message* replyMsg, uint32_t propId, void* context);

/**
 * handler for GetAboutData request in org.alljoyn.About
 * @param msg
 * @return aj_status
 */
AJ_Status AJ_About_GetAboutData(AJ_Message* msg);

/**
 * handler for GetObjectDescription request in org.alljoyn.About
 * @param replyMsg
 * @param propId
 * @param context
 * @return aj_status
 */
AJ_Status AJ_About_GetObjectDescription(AJ_Message* msg);

/**
 * send Announce signal in org.alljoyn.About
 * @param bus
 * @return aj_status
 */
AJ_Status AJ_About_Announce(AJ_BusAttachment* busAttachment);

/**
 * check if should send announcement
 * @return boolean
 */
uint8_t AJ_About_IsShouldAnnounce();

/**
 * set whether should send announcement
 * @param boolean
 */
void AJ_About_SetShouldAnnounce(uint8_t shouldAnnounce);

/**
 * connected handler
 * @param bus
 * @return status
 */
AJ_Status AJ_About_ConnectedHandler(AJ_BusAttachment* busAttachment);

/**
 * message processing
 * @param bus
 * @param msg
 * @return status
 */
AJSVC_ServiceStatus AJ_About_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* msgStatus);

/**
 * disconnect handler
 * @param bus
 * @return status
 */
AJ_Status AJ_About_DisconnectHandler(AJ_BusAttachment* busAttachment);

/** @} */
 #endif // _ABOUTSERVICE_H_
