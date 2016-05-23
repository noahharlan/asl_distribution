/******************************************************************************
 * Copyright (c) 2013, AllSeen Alliance. All rights reserved.
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

#ifndef INTROSPECTIONNODE_H_
#define INTROSPECTIONNODE_H_

#include <qcc/String.h>
#include <alljoyn/controlpanel/ControlPanelEnums.h>

namespace ajn {
namespace services {

/*
 *
 */
class IntrospectionNode {

  public:

    IntrospectionNode(qcc::String const& objectPath, WidgetType widgetType, bool secured = false);

    virtual ~IntrospectionNode();

    const qcc::String& getObjectPath() const;

    void setObjectPath(const qcc::String& objectPath);

    bool isSecured() const;

    void setSecured(bool secured);

    WidgetType getWidgetType() const;

    void setWidgetType(WidgetType widgetType);

  private:

    qcc::String m_ObjectPath;

    WidgetType m_WidgetType;

    bool m_Secured;
};

} /* namespace services */
} /* namespace ajn */

#endif /* INTROSPECTIONNODE_H_ */