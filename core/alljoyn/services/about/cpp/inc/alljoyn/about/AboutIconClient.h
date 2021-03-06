/**
 * @file
 * Implementation of a ProxyBusObject used to interact with a org.alljoyn.Icon
 * interface
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
#ifndef ABOUTICONCLIENT_H_
#define ABOUTICONCLIENT_H_

#include <alljoyn/BusAttachment.h>

namespace ajn {
namespace services {
/**
 * AboutIconClient enables the user of the class to interact with the remote AboutServiceIcon instance exposing the following methods:
 *  GetUrl
 *  GetContent
 *  GetVersion
 *  GetMimeType
 *  GetSize
 *
 * @deprecated The AboutIconClient class has been deprecated please see the
 * AboutIconProxy class for similar functionality as the AboutIconClient class.
 */
class AboutIconClient {
  public:

    /**
     * container to hold information about the Icon
     *
     * @deprecated The AboutIconClient::Icon class has been deprecated please see the
     * AboutIcon class for similar functionality as the AboutIconClient::Icon class.
     */
    class Icon {
      public:
        /**
         * Initialize an empty Icon
         *
         * @deprecated The AboutIconClient::Icon class has been deprecated please see the
         * AboutIcon class for similar functionality as the AboutIconClient::Icon class.
         */
        QCC_DEPRECATED(Icon()) : content(NULL), contentSize(0), m_arg() { }

        /**
         * Add the IconContent from a MsgArg
         *
         * @deprecated The AboutIconClient::Icon::Setcontent function has been
         * deprecated please see the AboutIcon::SetContent function.
         *
         * @param arg the MsgArg containing the Icon
         *
         * @return
         *   - ER_OK on success
         *   - status indicating failure otherwise
         */
        QCC_DEPRECATED(QStatus SetContent(const MsgArg &arg));

        /**
         * an array of bytes containing the image
         *
         * @deprecated The AboutIconClient::Icon class has been deprecated
         * please see the AboutIcon class for similar functionality as the
         * AboutIconClient::Icon class.
         */
        uint8_t* content;
        /**
         * the number of bytes in the content array
         *
         * @deprecated The AboutIconClient::Icon class has been deprecated
         * please see the AboutIcon class for similar functionality as the
         * AboutIconClient::Icon class.
         */
        size_t contentSize;
        /**
         * the mimetype of the image
         *
         * @deprecated The AboutIconClient::Icon class has been deprecated
         * please see the AboutIcon class for similar functionality as the
         * AboutIconClient::Icon class.
         */
        qcc::String mimetype;
      private:
        /**
         * MsgArg containing the Icon image
         */
        MsgArg m_arg;
    };

    /**
     * Construct an AboutIconClient.
     *
     * @deprecated The AboutIconClient class has been deprecated please see the
     * AboutIconProxy class for similar functionality.
     *
     * @param bus reference to BusAttachment
     */
    QCC_DEPRECATED(AboutIconClient(ajn::BusAttachment& bus));

    /**
     * Destruct AboutIconClient.
     *
     * @deprecated The AboutIconClient class has been deprecated please see the
     * AboutIconProxy class for similar functionality.
     */
    QCC_DEPRECATED(virtual ~AboutIconClient()) {
    }
    /**
     * @deprecated The AboutIconClient class and its member functions  has been
     * deprecated please see the AboutIconProxy::GetIcon function.
     *
     * @param[in] busName Unique or well-known name of AllJoyn bus
     * @param[in] url of the icon
     * @param[in] sessionId the session received  after joining AllJoyn session
     * @return ER_OK if successful
     */
    QCC_DEPRECATED(QStatus GetUrl(const char* busName, qcc::String & url, ajn::SessionId sessionId = 0));

    /**
     * @deprecated This function has been deprecated in favor of the @see GetIcon
     * member function.
     *
     * Use the GetIcon function to Get content of the icon.
     *
     * @warning the content must be freed using `delete [] content` once it is no
     *          longer needed
     *
     * @code
       size_t contentSize;
       uint8_t* content = NULL;
       status = iconClient->GetContent(busName, &content, contentSize, sessionId);
       ...
       delete [] content;
       @endcode
     *
     * @param[in] busName Unique or well-known name of AllJoyn bus
     * @param[out] content retrieves the icons payload
     * @param[out] contentSize is the size of the content payload
     * @param[in] sessionId the session received  after joining AllJoyn session
     * @return ER_OK if successful
     */
    QCC_DEPRECATED(QStatus GetContent(const char* busName, uint8_t * *content, size_t & contentSize, ajn::SessionId sessionId = 0));

    /**
     * @deprecated The AboutIconClient class and its member functions  has been
     * deprecated please see the AboutIconProxy::GetIcon function.
     *
     * @param[in] busName Unique or well-known name of AllJoyn bus
     * @param[out] icon class that holds icon content
     * @param[in] sessionId the session received  after joining AllJoyn session
     * @return ER_OK if successful
     */
    QCC_DEPRECATED(QStatus GetIcon(const char* busName, Icon & icon, ajn::SessionId sessionId = 0));
    /**
     * @deprecated The AboutIconClient class and its member functions  has been
     * deprecated please see the AboutIconProxy::GetVersion function.
     *
     * @param[in] busName Unique or well-known name of AllJoyn bus
     * @param[out] version of the AboutIcontClient
     * @param[in] sessionId the session received  after joining AllJoyn session
     * @return ER_OK if successful
     */
    QCC_DEPRECATED(QStatus GetVersion(const char* busName, int& version, ajn::SessionId sessionId = 0));

    /**
     * @deprecated The AboutIconClient class and its member functions  has been
     * deprecated please see the AboutIconProxy::GetIcon function.
     *
     * @param[in] busName Unique or well-known name of AllJoyn bus
     * @param[out] mimeType of the icon
     * @param[in] sessionId the session received  after joining AllJoyn session
     * @return ER_OK if successful
     */
    QCC_DEPRECATED(QStatus GetMimeType(const char* busName, qcc::String & mimeType, ajn::SessionId sessionId = 0));

    /**
     * @deprecated The AboutIconClient class and its member functions  has been
     * deprecated please see the AboutIconProxy::GetIcon function.
     *
     * @param[in] busName Unique or well-known name of AllJoyn bus
     * @param[out] size of the icon
     * @param[in] sessionId the session received  after joining AllJoyn session
     * @return ER_OK if successful
     */
    QCC_DEPRECATED(QStatus GetSize(const char* busName, size_t & size, ajn::SessionId sessionId = 0));

  private:
    /**
     * pointer to BusAttachment
     */
    ajn::BusAttachment* m_BusAttachment;

};

}
}

#endif /* ABOUTICONCLIENT_H_ */
