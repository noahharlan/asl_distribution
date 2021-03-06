/*******************************************************************************
 *  Copyright AllSeen Alliance. All rights reserved.
 *
 *     Contributed by Qualcomm Connected Experiences, Inc.,
 *     with authorization from the AllSeen Alliance, Inc.
 *     
 *     Licensed under the Apache License, Version 2.0 (the "License");
 *     you may not use this file except in compliance with the License.
 *     You may obtain a copy of the License at
 *     
 *         http://www.apache.org/licenses/LICENSE-2.0
 *     
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *     
 *     Pursuant to Section 1 of the License, the work of authorship constituting
 *     a Work and any Contribution incorporated in the Work shall mean only that
 *     Contributor's code submissions authored by that Contributor.  Any rights
 *     granted under the License are conditioned upon acceptance of these
 *     clarifications.
 *******************************************************************************/
package org.alljoyn.validation.testing.utils.notification;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertNotNull;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicInteger;

import org.alljoyn.bus.ErrorReplyBusException;
import org.alljoyn.ns.Notification;
import org.alljoyn.ns.NotificationReceiver;
import org.alljoyn.ns.RichAudioUrl;
import org.alljoyn.validation.framework.AppUnderTestDetails;
import org.alljoyn.validation.framework.ValidationTestContext;
import org.alljoyn.validation.framework.utils.introspection.BusIntrospector;
import org.alljoyn.validation.testing.utils.about.AboutAnnouncementDetails;
import org.alljoyn.validation.testing.utils.log.Logger;
import org.alljoyn.validation.testing.utils.log.LoggerFactory;

public class NotificationValidator implements NotificationReceiver, Runnable
{
    private static final int NOTIFICATION_SERVICE_VERSION = 2;
    private static final String TAG = "NotificationValidator";
    private static final Logger logger = LoggerFactory.getLogger(TAG);
    private AtomicInteger notificationCounter = new AtomicInteger();
    private BusIntrospector busIntrospector;
    private ValidationTestContext validationTestContext;
    private NotificationValidationExceptionHandler notificationValidationExceptionHandler;
    private AboutAnnouncementDetails deviceAboutAnnouncement;
    private AppUnderTestDetails appUnderTestDetails;
    private NotificationHandler notificationHandler;

    public interface NotificationValidationExceptionHandler
    {
        void onNotificationValidationException(Exception exception);
    }

    public NotificationValidator(ValidationTestContext validationTestContext)
    {
        this.validationTestContext = validationTestContext;
        appUnderTestDetails = validationTestContext.getAppUnderTestDetails();

        notificationHandler = getNotificationHandler();
        notificationHandler.initializeForDevice(appUnderTestDetails.getDeviceId(), appUnderTestDetails.getAppId());
    }

    protected NotificationHandler getNotificationHandler()
    {
        return new NotificationHandler();
    }

    public void initializeForDevice(AboutAnnouncementDetails deviceAboutAnnouncement, BusIntrospector busIntrospector,
            NotificationValidationExceptionHandler notificationValidationExceptionHandler)
    {
        this.deviceAboutAnnouncement = deviceAboutAnnouncement;
        this.busIntrospector = busIntrospector;
        this.notificationValidationExceptionHandler = notificationValidationExceptionHandler;
    }

    @Override
    public void receive(Notification notification)
    {
        notificationHandler.receive(notification);
    }

    @Override
    public void dismiss(int arg0, UUID arg1)
    {
    }

    @Override
    public void run()
    {
        Notification notification;

        try
        {
            while ((notification = notificationHandler.getReceivedNotification()) != null)
            {
                logger.debug(String.format("Validating notification from DUT with messageId: %d", notification.getMessageId()));

                try
                {
                    boolean includedRichIconUrl = false;
                    boolean includedRichAudioUrl = false;
                    boolean includesResponseObjectPath = false;

                    assertEquals(String.format("Notification version must be %s", NOTIFICATION_SERVICE_VERSION), NOTIFICATION_SERVICE_VERSION, notification.getVersion());

                    String notifAppName = notification.getAppName();
                    assertEquals("AppName in notification does not match AboutAnnouncement", deviceAboutAnnouncement.getAppName(), notifAppName);

                    String notifdeviceName = notification.getDeviceName();
                    assertEquals("DeviceName in notification does not match AboutAnnouncement", deviceAboutAnnouncement.getDeviceName(), notifdeviceName);

                    assertNotNull("Notification messageType value not valid!", notification.getMessageType());

                    logger.debug("The received Message Id: " + notification.getMessageId());

                    String richIconUrlString = notification.getRichIconUrl();

                    if (richIconUrlString != null)
                    {
                        try
                        {
                            logger.debug("Rich icon URL: '" + richIconUrlString + "'");
                            new URL(richIconUrlString);
                            includedRichIconUrl = true;
                        }
                        catch (MalformedURLException e)
                        {
                            throw new Exception("RichIconUrl malformed: '" + richIconUrlString + "'", e);
                        }
                    }

                    List<RichAudioUrl> richAudioUrls = notification.getRichAudioUrl();

                    if (richAudioUrls != null)
                    {
                        for (RichAudioUrl richAudioUrl : richAudioUrls)
                        {
                            String urlString = richAudioUrl.getUrl();
                            try
                            {
                                new URL(urlString);
                                logger.debug("Rich audio URL: '" + urlString + "'");
                                includedRichAudioUrl = true;
                            }
                            catch (MalformedURLException e)
                            {
                                throw new Exception("RichAudioUrl malformed: '" + urlString + "'", e);
                            }
                        }
                    }

                    String responseObjectPath = notification.getResponseObjectPath();
                    if (responseObjectPath != null)
                    {
                        logger.debug("Control Panel Service response Object Path: '" + responseObjectPath + "'");

                        try
                        {
                            busIntrospector.introspect(responseObjectPath);
                        }
                        catch (ErrorReplyBusException e)
                        {
                            throw new Exception("Exception retrieving responseObject", e);
                        }

                        includesResponseObjectPath = true;
                    }
                    notificationCounter.getAndIncrement();

                    StringBuilder noteMsg = new StringBuilder();
                    noteMsg.append("Notification Received: ");
                    noteMsg.append("messageType: ");
                    noteMsg.append(notification.getMessageType());
                    noteMsg.append("; richIconUrl: ");
                    noteMsg.append(includedRichIconUrl);
                    noteMsg.append("; richAudioUrl: ");
                    noteMsg.append(includedRichAudioUrl);
                    noteMsg.append("; responseObject: ");
                    noteMsg.append(includesResponseObjectPath);
                    String noteMsgString = noteMsg.toString();
                    validationTestContext.addNote(noteMsgString);
                    logger.debug(noteMsgString);
                }
                catch (Throwable throwable)
                {
                    if (throwable instanceof Exception)
                    {
                        notificationValidationExceptionHandler.onNotificationValidationException((Exception) throwable);
                    }
                    else
                    {
                        notificationValidationExceptionHandler.onNotificationValidationException(new Exception(throwable));
                    }
                }
            }
        }
        catch (InterruptedException e)
        {
            Thread.currentThread().interrupt();
        }
    }

    public int getNumberOfNotificationsReceived()
    {
        return notificationCounter.get();
    }
}