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
package org.alljoyn.bus.samples;

import java.util.Map;

import org.alljoyn.bus.AboutListener;
import org.alljoyn.bus.AboutObjectDescription;
import org.alljoyn.bus.AboutProxy;
import org.alljoyn.bus.AnnotationBusException;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Mutable;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.SessionListener;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;

public class Client {
    static {
        System.loadLibrary("alljoyn_java");
    }
    static BusAttachment mBus;

    private static ProxyBusObject mProxyObj;
    private static SampleInterface mSampleInterface;

    private static boolean isJoined = false;

    static class MyAboutListener implements AboutListener {
        public void announced(String busName, int version, short port, AboutObjectDescription[] objectDescriptions, Map<String, Variant> aboutData) {
            System.out.println("Announced BusName:     " + busName);
            System.out.println("Announced Version:     " + version);
            System.out.println("Announced SessionPort: " + port);
            System.out.println("Announced ObjectDescription: ");
            if(objectDescriptions != null) {
                for(AboutObjectDescription o : objectDescriptions) {
                    System.out.println("\t" + o.path);
                    for (String s : o.interfaces) {
                        System.out.println("\t\t" + s);
                    }
                }
            }

            System.out.println("Contents of Announced AboutData:");
            try {
                for (Map.Entry<String, Variant> entry : aboutData.entrySet()) {
                    System.out.print("\tField: " + entry.getKey() + " = ");

                    if (entry.getKey().equals("AppId")) {
                        byte[] appId = entry.getValue().getObject(byte[].class);
                        for (byte b : appId) {
                            System.out.print(String.format("%02X", b));
                        }
                    } else if (entry.getKey().equals("SupportedLanguages")) {
                        String[] supportedLanguages = entry.getValue().getObject(String[].class);
                        for (String s : supportedLanguages) {
                            System.out.print(s + " ");
                        }
                    } else {
                        System.out.print(entry.getValue().getObject(String.class));
                    }
                    System.out.print("\n");
                }
            } catch (AnnotationBusException e1) {
                e1.printStackTrace();
            } catch (BusException e1) {
                e1.printStackTrace();
            }

            SessionOpts sessionOpts = new SessionOpts();
            sessionOpts.traffic = SessionOpts.TRAFFIC_MESSAGES;
            sessionOpts.isMultipoint = false;
            sessionOpts.proximity = SessionOpts.PROXIMITY_ANY;
            sessionOpts.transports = SessionOpts.TRANSPORT_ANY;

            Mutable.IntegerValue sessionId = new Mutable.IntegerValue();

            mBus.enableConcurrentCallbacks();

            Status status = mBus.joinSession(busName, port, sessionId, sessionOpts, new SessionListener());
            if (status != Status.OK) {
                return;
            }
            System.out.println(String.format("BusAttachement.joinSession successful sessionId = %d", sessionId.value));

            mProxyObj =  mBus.getProxyBusObject(busName,
                    "/example/path",
                    sessionId.value,
                    new Class<?>[] { SampleInterface.class});

            mSampleInterface = mProxyObj.getInterface(SampleInterface.class);

            System.out.println("\n\nCreating AboutProxy object and calling remote methods.");
            AboutProxy aboutProxy = new AboutProxy(mBus, busName, sessionId.value);
            System.out.println("Calling getObjectDescription:");
            try {
                AboutObjectDescription aod[] = aboutProxy.getObjectDescription();
                if(aod != null) {
                    for(AboutObjectDescription o : aod) {
                        System.out.println("\t" + o.path);
                        for (String s : o.interfaces) {
                            System.out.println("\t\t" + s);
                        }
                    }
                }
                System.out.println("Calling getAboutData:");

                Map<String, Variant> aboutData_en;
                aboutData_en = aboutProxy.getAboutData("en");

                for (Map.Entry<String, Variant> entry : aboutData_en.entrySet()) {
                    System.out.print("\tField: " + entry.getKey() + " = ");

                    if (entry.getKey().equals("AppId")) {
                        byte[] appId = entry.getValue().getObject(byte[].class);
                        for (byte b : appId) {
                            System.out.print(String.format("%02X", b));
                        }
                    } else if (entry.getKey().equals("SupportedLanguages")) {
                        String[] supportedLanguages = entry.getValue().getObject(String[].class);
                        for (String s : supportedLanguages) {
                            System.out.print(s + " ");
                        }
                    } else {
                        System.out.print(entry.getValue().getObject(String.class));
                    }
                    System.out.print("\n");
                }

                System.out.println("Calling getVersion:");
                System.out.println("\tVersion = " + aboutProxy.getVersion());
            } catch (BusException e1) {
                e1.printStackTrace();
            }
            isJoined = true;
        }
    }

    public static void main(String[] args) {
        mBus = new BusAttachment("Basic About Client Sample", BusAttachment.RemoteMessage.Receive);

        Status status = mBus.connect();
        if (status != Status.OK) {
            return;
        }
        System.out.println("BusAttachment.connect successful on " + System.getProperty("org.alljoyn.bus.address"));

        AboutListener listener = new MyAboutListener();
        mBus.registerAboutListener(listener);

        String ifaces[] = {"com.example.about.feature.interface.sample"};
        status = mBus.whoImplements(ifaces);
        if (status != Status.OK) {
            return;
        }
        System.out.println("BusAttachment.whoImplements successful " + "com.example.about.feature.interface.sample");

        while(!isJoined) {
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                System.out.println("Program interupted");
            }
        }
        System.out.println("BusAttachement.joinSession successful calling echo method");
        try {
            System.out.println("Echo : " + mSampleInterface.echo("Hello World"));
        } catch (BusException e1) {
            e1.printStackTrace();
        }
    }
}

