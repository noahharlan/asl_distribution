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

#include <jni.h>
#include <string.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>
#include <alljoyn/Init.h>
#include "MyAllJoynCode.h"

/* Static data */
static MyAllJoynCode* myAllJoynCode = NULL;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,
                                  void* reserved)
{
    if (AllJoynInit() != ER_OK) {
        return 1;
    }
    if (AllJoynRouterInit() != ER_OK) {
        AllJoynShutdown();
        return 1;
    }
    QCC_UseOSLogging(true);
    return JNI_VERSION_1_2;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm,
                            void* reserved)
{
    AllJoynRouterShutdown();
    AllJoynShutdown();
}

/*
 * Class:     org_allseen_sample_event_tester_BusHandler
 * Method:    initialize
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_initialize(JNIEnv* env, jobject jobj, jstring packageNameStrObj) {
    if (myAllJoynCode == NULL) {
        JavaVM* vm;
        env->GetJavaVM(&vm);
        jobject gjobj = env->NewGlobalRef(jobj);
        myAllJoynCode = new MyAllJoynCode(vm, gjobj);
    }
    jboolean iscopy;
    const char* packageNameStr = env->GetStringUTFChars(packageNameStrObj, &iscopy);
    myAllJoynCode->initialize(packageNameStr);
    env->ReleaseStringUTFChars(packageNameStrObj, packageNameStr);
}

/*
 * Class:     org_allseen_sample_event_tester_BusHandler
 * Method:    dointrospection
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)Ljava/langString;
 */
JNIEXPORT jstring JNICALL Java_org_allseen_sample_event_tester_BusHandler_doIntrospection(JNIEnv* env, jobject jobj, jstring jname, jstring jpath, jint sessionId) {
    jboolean iscopy;
    jboolean iscopy2;
    const char* sessionName = env->GetStringUTFChars(jname, &iscopy);
    const char* path = env->GetStringUTFChars(jpath, &iscopy2);
    char* xml = myAllJoynCode->introspectWithDescriptions(sessionName, path, sessionId);
    env->ReleaseStringUTFChars(jname, sessionName);
    env->ReleaseStringUTFChars(jpath, path);
    jstring ret = env->NewStringUTF(xml);
    delete xml;
    return ret;
}

JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_introspectionDone(JNIEnv* env, jobject jobj, jint sessionId) {
    myAllJoynCode->leaveSession(sessionId);
}


JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_enableEvent(JNIEnv*env, jobject jobj,
                                                                                   //event
                                                                                   jstring jEUniqueName, jstring jEPath,
                                                                                   jstring jEIface, jstring jEMember, jstring jESig)
{
    jboolean iscopy;

    const char* eUniqueName = env->GetStringUTFChars(jEUniqueName, &iscopy);
    const char* ePath = env->GetStringUTFChars(jEPath, &iscopy);
    const char* eIface = env->GetStringUTFChars(jEIface, &iscopy);
    const char* eMember = env->GetStringUTFChars(jEMember, &iscopy);
    const char* eSig = env->GetStringUTFChars(jESig, &iscopy);
    EventInfo* event = new EventInfo(eUniqueName, ePath, eIface, eMember, eSig);

    LOGTHIS("enabling an event on %s::%s", eIface, eMember);
    myAllJoynCode->enableEvent(event);

    delete event;

    env->ReleaseStringUTFChars(jEUniqueName, eUniqueName);
    env->ReleaseStringUTFChars(jEPath, ePath);
    env->ReleaseStringUTFChars(jEIface, eIface);
    env->ReleaseStringUTFChars(jEMember, eMember);
    env->ReleaseStringUTFChars(jESig, eSig);
}

/*
 * Class:     org_allseen_sample_event_tester_BusHandler
 * Method:    shutdown
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_shutdown(JNIEnv* env, jobject jobj) {
//	if(myAllJoynCode)
//		myAllJoynCode->shutdown();
//	delete myAllJoynCode;
//	myAllJoynCode = NULL;
}

#ifdef __cplusplus
}
#endif

