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
package org.alljoyn.validation.framework.utils.introspection;

import java.util.List;
import java.util.Set;

import org.alljoyn.validation.framework.utils.introspection.bean.IntrospectionAnnotation;
import org.alljoyn.validation.framework.utils.introspection.bean.IntrospectionArg;
import org.alljoyn.validation.framework.utils.introspection.bean.IntrospectionInterface;
import org.alljoyn.validation.framework.utils.introspection.bean.IntrospectionMethod;
import org.alljoyn.validation.framework.utils.introspection.bean.IntrospectionNode;
import org.alljoyn.validation.framework.utils.introspection.bean.IntrospectionProperty;
import org.alljoyn.validation.framework.utils.introspection.bean.IntrospectionSignal;
import org.alljoyn.validation.framework.utils.introspection.bean.IntrospectionSubNode;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class IntrospectionXmlHandler extends DefaultHandler
{
    private static final String XML_QNAME_NAME = "name";
    private static final String XML_QNAME_VALUE = "value";
    private static final String XML_QNAME_TYPE = "type";
    private static final String XML_QNAME_DIRECTION = "direction";
    private static final String XML_QNAME_ACCESS = "access";

    private static final String NODE_TAG = "node";
    private static final String INTERFACE_TAG = "interface";
    private static final String METHOD_TAG = "method";
    private static final String SIGNAL_TAG = "signal";
    private static final String PROPERTY_TAG = "property";
    private static final String ARG_TAG = "arg";
    private static final String ANNOTATION_TAG = "annotation";

    private boolean sawRootNode = false;
    private boolean processMethodArgs = false;
    private boolean processSignalArgs = false;
    private IntrospectionNode introspectionNode;
    private List<IntrospectionInterface> interfaces;
    private IntrospectionInterface ifaces;
    private Set<IntrospectionMethod> methods;
    private IntrospectionMethod method;
    private Set<IntrospectionProperty> properties;
    private Set<IntrospectionSignal> signals;
    private Set<IntrospectionAnnotation> interfaceAnnotations;
    private List<IntrospectionSubNode> subNodes;
    private List<IntrospectionArg> methodArgs;
    private IntrospectionSignal signal;
    private List<IntrospectionArg> signalArgs;
    private Set<IntrospectionAnnotation> methodAnnotations;

    public IntrospectionNode getIntrospectionNode()
    {
        return introspectionNode;
    }

    @Override
    public void startElement(String namespaceURI, String localName, String qName, Attributes attrs) throws SAXException
    {
        createIntrospectionNode(localName, attrs);
        createInterface(localName, attrs);
        createMethod(localName, attrs);
        createMethodArg(localName, attrs);
        createMethodAnnotation(localName, attrs);
        createSignal(localName, attrs);
        createSignalArg(localName, attrs);
        createProperty(localName, attrs);
        createAnnotation(localName, attrs);
        createSubNode(localName, attrs);
    }

    @Override
    public void endElement(String uri, String localName, String qName) throws SAXException
    {
        if (localName.equals(METHOD_TAG))
        {
            processMethodArgs = false;
        }
        if (localName.equals(SIGNAL_TAG))
        {
            processSignalArgs = false;
        }
    }

    private void createMethodAnnotation(String localName, Attributes attrs)
    {
        if (localName.equals(ANNOTATION_TAG) && processMethodArgs)
        {
            methodAnnotations = method.getAnnotations();
            IntrospectionAnnotation annotation = new IntrospectionAnnotation();
            annotation.setName(attrs.getValue(XML_QNAME_NAME));
            annotation.setValue(attrs.getValue(XML_QNAME_VALUE));
            methodAnnotations.add(annotation);
        }
    }

    private void createSignalArg(String localName, Attributes attrs)
    {
        if (localName.equals(ARG_TAG) && processSignalArgs)
        {
            signalArgs = signal.getArgs();
            IntrospectionArg arg = new IntrospectionArg();
            arg.setName(attrs.getValue(XML_QNAME_NAME));
            arg.setType(attrs.getValue(XML_QNAME_TYPE));
            arg.setDirection(attrs.getValue(XML_QNAME_DIRECTION));
            signalArgs.add(arg);
        }
    }

    private void createMethodArg(String localName, Attributes attrs)
    {
        if (localName.equals(ARG_TAG) && processMethodArgs)
        {
            methodArgs = method.getArgs();
            IntrospectionArg arg = new IntrospectionArg();
            arg.setName(attrs.getValue(XML_QNAME_NAME));
            arg.setType(attrs.getValue(XML_QNAME_TYPE));
            arg.setDirection(attrs.getValue(XML_QNAME_DIRECTION));
            methodArgs.add(arg);
        }
    }

    private void createSubNode(String localName, Attributes attrs)
    {
        if (localName.equals(NODE_TAG) && sawRootNode)
        {
            String name = attrs.getValue(XML_QNAME_NAME);
            if (name != null && !name.equals(introspectionNode.getName()))
            {
                subNodes = introspectionNode.getSubNodes();
                IntrospectionSubNode subNode = new IntrospectionSubNode();
                subNode.setName(name);
                subNodes.add(subNode);
            }
        }
    }

    private void createProperty(String localName, Attributes attrs) throws SAXException
    {
        if (localName.equals(PROPERTY_TAG))
        {
            properties = ifaces.getProperties();
            IntrospectionProperty property = new IntrospectionProperty();
            property.setName(attrs.getValue(XML_QNAME_NAME));
            property.setType(attrs.getValue(XML_QNAME_TYPE));
            property.setAccess(attrs.getValue(XML_QNAME_ACCESS));
            properties.add(property);
        }
    }

    private void createSignal(String localName, Attributes attrs) throws SAXException
    {
        if (localName.equals(SIGNAL_TAG))
        {
            signals = ifaces.getSignals();
            signal = new IntrospectionSignal();
            signal.setName(attrs.getValue(XML_QNAME_NAME));
            signals.add(signal);
            processSignalArgs = true;
        }
    }

    private void createAnnotation(String localName, Attributes attrs) throws SAXException
    {
        if (localName.equals(ANNOTATION_TAG) && !processMethodArgs)
        {
            interfaceAnnotations = ifaces.getAnnotations();
            IntrospectionAnnotation annotation = new IntrospectionAnnotation();
            annotation.setName(attrs.getValue(XML_QNAME_NAME));
            annotation.setValue(attrs.getValue(XML_QNAME_VALUE));
            interfaceAnnotations.add(annotation);
        }
    }

    private void createMethod(String localName, Attributes attrs) throws SAXException
    {
        if (localName.equals(METHOD_TAG))
        {
            methods = ifaces.getMethods();
            method = new IntrospectionMethod();
            method.setName(attrs.getValue(XML_QNAME_NAME));
            methods.add(method);
            processMethodArgs = true;
        }
    }

    private void createInterface(String localName, Attributes attrs) throws SAXException
    {
        if (localName.equals(INTERFACE_TAG))
        {
            interfaces = introspectionNode.getInterfaces();
            ifaces = new IntrospectionInterface();
            ifaces.setName(attrs.getValue(XML_QNAME_NAME));
            interfaces.add(ifaces);
        }
    }

    private void createIntrospectionNode(String localName, Attributes attrs) throws SAXException
    {
        if (localName.equals(NODE_TAG) && !sawRootNode)
        {
            introspectionNode = new IntrospectionNode();
            introspectionNode.setName(attrs.getValue(XML_QNAME_NAME));
            sawRootNode = true;
        }
    }
}
