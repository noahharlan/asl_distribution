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

#ifndef CONTROLPANELCONTROLLERUNIT_H_
#define CONTROLPANELCONTROLLERUNIT_H_

#include <qcc/String.h>
#include <alljoyn/Status.h>
#include <map>
#include <alljoyn/controlpanel/HttpControl.h>

namespace ajn {
namespace services {

class ControlPanelDevice;
class ControlPanel;
class NotificationAction;

/**
 * Class that represents a ControlPanel Unit.
 */
class ControlPanelControllerUnit {
  public:

    /**
     * ControlPanelUnit()
     * @param unitName
     * @param device
     */
    ControlPanelControllerUnit(qcc::String const& unitName, ControlPanelDevice* device);

    /**
     * ~ControlPanelUnit()
     */
    virtual ~ControlPanelControllerUnit();

    /**
     * add a HttpControl
     * @param objectPath of HTTPControl
     * @return status - success/failure
     */
    QStatus addHttpControl(qcc::String const& objectPath);

    /**
     * addControlPanel
     * @param objectPath
     * @param panelName
     * @return status - success/failure
     */
    QStatus addControlPanel(qcc::String const& objectPath, qcc::String const& panelName);

    /**
     * addNotificationAction to controlpanel unit
     * @param objectPath the objectpath of the notification action
     * @param actionName - the actionName parsed from the objectpath
     * @return status - success/failure
     */
    QStatus addNotificationAction(qcc::String const& objectPath, qcc::String const& actionName);

    /**
     * remove a Notification Action
     * @param actionName
     * @return status - success/failure
     */
    QStatus removeNotificationAction(qcc::String const& actionName);

    /**
     * Fill control panels and the HTTPControl
     */
    QStatus registerObjects();

    /**
     * Called when shutting down device
     * @return status - success/failure
     */
    QStatus shutdownUnit();

    /**
     * getDevice()
     * @return ControlPanelDevice*
     */
    ControlPanelDevice* getDevice() const;

    /**
     * getUnitName()
     * @return string
     */
    const qcc::String& getUnitName() const;

    /**
     * Get the ControlPanels of the Unit
     * @return controlPanels map
     */
    const std::map<qcc::String, ControlPanel*>& getControlPanels() const;

    /**
     * Get the NotificationActions of the Unit
     * @return NotificationActions map
     */
    const std::map<qcc::String, NotificationAction*>& getNotificationActions() const;

    /**
     * Get a ControlPanel of the Unit
     * @param panelName - name of the Panel to get
     * @return ControlPanel or NULL if it doesn't' exist
     */
    ControlPanel* getControlPanel(qcc::String const& panelName) const;

    /**
     * Get a NotificationAction of the Unit
     * @param actionName - name of the NotificaitonAction to get
     * @return NotificationAction or NULL if it doesn't exist
     */
    NotificationAction* getNotificationAction(qcc::String const& actionName) const;

    /**
     * Get the HttpControl of the Unit
     * @return httpControl
     */
    HttpControl* getHttpControl() const;

  private:

    /**
     * Name of Unit
     */
    qcc::String m_UnitName;

    /**
     * Device Unit belongs to
     */
    ControlPanelDevice* m_Device;

    /**
     * The ControlPanels of this Unit
     */
    std::map<qcc::String, ControlPanel*> m_ControlPanels;

    /**
     * The NotificationActions of this Unit
     */
    std::map<qcc::String, NotificationAction*> m_NotificationActions;

    /**
     * The HTTPControl of the Unit
     */
    HttpControl* m_HttpControl;
};

} /* namespace services */
} /* namespace ajn */
#endif /* CONTROLPANELCONTROLLERUNIT_H_ */
