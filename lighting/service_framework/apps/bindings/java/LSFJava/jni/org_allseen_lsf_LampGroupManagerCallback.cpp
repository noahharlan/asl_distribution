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
 *
 ******************************************************************************/

#include <jni.h>

#include "NUtil.h"
#include "JLampGroupManagerCallback.h"

using namespace lsf;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT
void JNICALL Java_org_allseen_lsf_LampGroupManagerCallback_createNativeObject(JNIEnv*env, jobject thiz)
{
    CreateHandle<JLampGroupManagerCallback>(thiz);
}

JNIEXPORT
void JNICALL Java_org_allseen_lsf_LampGroupManagerCallback_destroyNativeObject(JNIEnv*env, jobject thiz)
{
    DestroyHandle<JLampGroupManagerCallback>(thiz);
}

#ifdef __cplusplus
}
#endif




