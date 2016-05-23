/**
 * @file
 *
 * Define the abstracted socket interface for Windows
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

#include <qcc/platform.h>

// Do not change the order of these includes; they are order dependent.
#include <Winsock2.h>
#include <Mswsock.h>
#include <ws2tcpip.h>

#include <qcc/IPAddress.h>
#include <qcc/Socket.h>
#include <qcc/IfConfig.h>
#include <qcc/Util.h>
#include <qcc/Thread.h>
#include <qcc/StringUtil.h>
#include <qcc/windows/utility.h>
#include "ScatterGatherList.h"

#include <alljoyn/Status.h>

#define QCC_MODULE "NETWORK"

/* Scatter gather only support on Vista and later */
#if !defined (NTDDI_VERSION) || !defined (NTDDI_VISTA) || (NTDDI_VERSION < NTDDI_VISTA)
#define QCC_USE_SCATTER_GATHER 0
#else
#define QCC_USE_SCATTER_GATHER 1
#endif

namespace qcc {

extern String StrError();

extern void MakeSockAddr(const IPAddress& addr,
                         uint16_t port,
                         SOCKADDR_STORAGE* addrBuf,
                         socklen_t& addrSize);

extern QStatus GetSockAddr(const SOCKADDR_STORAGE* addrBuf, socklen_t addrSize,
                           IPAddress& addr, uint16_t& port);

#if QCC_USE_SCATTER_GATHER

static QStatus SendSGCommon(SocketFd sockfd,
                            SOCKADDR_STORAGE* addr,
                            socklen_t addrLen,
                            const ScatterGatherList& sg,
                            size_t& sent,
                            SendMsgFlags flags)
{
    QStatus status = ER_OK;

    QCC_DbgTrace(("SendSGCommon(sockfd = %d, *addr, addrLen, sg, sent = <>, flags = 0x%x)", sockfd, (int)flags));

    /*
     * We will usually avoid the memory allocation
     */
    WSABUF iovAuto[8];
    WSABUF* iov = sg.Size() <= ArraySize(iovAuto) ? iovAuto : new WSABUF[sg.Size()];
    ScatterGatherList::const_iterator iter = sg.Begin();
    for (size_t index = 0; iter != sg.End(); ++index, ++iter) {
        iov[index].buf = iter->buf;
        iov[index].len = iter->len;
        QCC_DbgLocalData(iov[index].buf, iov[index].len);
    }

    WSAMSG msg;
    memset(&msg, 0, sizeof(msg));
    msg.name = reinterpret_cast<LPSOCKADDR>(addr);
    msg.namelen = addrLen;
    msg.lpBuffers = iov;
    msg.dwBufferCount = sg.Size();

    DWORD dwsent;
    DWORD ret = WSASendMsg(static_cast<SOCKET>(sockfd), &msg, (int)flags, &dwsent, NULL, NULL);
    DWORD err;
    if (ret == SOCKET_ERROR && (err = WSAGetLastError()) != WSA_IO_PENDING) {
        if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS || err == WSAEINTR) {
            status = ER_WOULDBLOCK;
            sent = 0;
        } else {
            status = ER_OS_ERROR;
            QCC_LogError(status, ("Send: %s", StrError().c_str()));
        }
    }
    QCC_DbgPrintf(("Sent %u bytes", dwsent));
    sent = dwsent;

    if (iov != iovAuto) {
        delete[] iov;
    }
    return status;
}

QStatus SendSG(SocketFd sockfd, const ScatterGatherList& sg, size_t& sent, SendMsgFlags flags)
{
    QCC_DbgTrace(("SendSG(sockfd = %d, sg, sent = <>, flags = 0x%x)", sockfd, (int) flags));

    return SendSGCommon(sockfd, NULL, 0, sg, sent, flags);
}

QStatus SendToSG(SocketFd sockfd, IPAddress& remoteAddr, uint16_t remotePort,
                 const ScatterGatherList& sg, size_t& sent, SendMsgFlags flags)
{
    SOCKADDR_STORAGE addr;
    socklen_t addrLen = sizeof(addr);

    QCC_DbgTrace(("SendToSG(sockfd = %d, remoteAddr = %s, remotePort = %u, sg, sent = <>, flags = 0x%x)",
                  sockfd, remoteAddr.ToString().c_str(), remotePort, flags));

    MakeSockAddr(remoteAddr, remotePort, &addr, addrLen);
    return SendSGCommon(sockfd, &addr, addrLen, sg, sent, flags);
}

#else

QStatus SendSG(SocketFd sockfd, const ScatterGatherList& sg, size_t& sent, SendMsgFlags flags)
{
    QCC_UNUSED(flags);
    QStatus status;
    uint8_t* tmpBuf = new uint8_t[sg.MaxDataSize()];
    sg.CopyToBuffer(tmpBuf, sg.MaxDataSize());
    status = Send(sockfd, tmpBuf, sg.DataSize(), sent);
    delete[] tmpBuf;
    return status;
}

QStatus SendToSG(SocketFd sockfd, IPAddress& remoteAddr, uint16_t remotePort,
                 const ScatterGatherList& sg, size_t& sent, SemdMsgFlags flags)
{
    QCC_UNUSED(flags);
    QStatus status;
    uint8_t* tmpBuf = new uint8_t[sg.MaxDataSize()];
    sg.CopyToBuffer(tmpBuf, sg.MaxDataSize());
    status = SendTo(sockfd, remoteAddr, remotePort, tmpBuf, sg.DataSize(), sent);
    delete[] tmpBuf;
    return status;
}

#endif

#if QCC_USE_SCATTER_GATHER

static QStatus RecvSGCommon(SocketFd sockfd, SOCKADDR_STORAGE* addr, socklen_t& addrLen,
                            ScatterGatherList& sg, size_t& received)
{
    static LPFN_WSARECVMSG WSARecvMsg = NULL;
    QStatus status = ER_OK;
    DWORD dwRecv;
    DWORD ret;

    QCC_DbgTrace(("RecvSGCommon(sockfd = &d, addr, addrLen, sg = <>, received = <>)", sockfd));

    /*
     * Get extension function pointer
     */
    if (!WSARecvMsg) {
        GUID guid = WSAID_WSARECVMSG;
        ret = WSAIoctl(static_cast<SOCKET>(sockfd), SIO_GET_EXTENSION_FUNCTION_POINTER,
                       &guid, sizeof(guid),
                       &WSARecvMsg, sizeof(WSARecvMsg),
                       &dwRecv, NULL, NULL);
        if (ret == SOCKET_ERROR) {
            status = ER_OS_ERROR;
            QCC_LogError(status, ("Receive: %s", StrError().c_str()));
            return status;
        }
    }
    /*
     * We will usually avoid the memory allocation
     */
    WSABUF iovAuto[8];
    WSABUF* iov = sg.Size() <= ArraySize(iovAuto) ? iovAuto : new WSABUF[sg.Size()];
    ScatterGatherList::const_iterator iter = sg.Begin();
    for (size_t index = 0; iter != sg.End(); ++index, ++iter) {
        iov[index].buf = iter->buf;
        iov[index].len = iter->len;
    }

    WSAMSG msg;
    memset(&msg, 0, sizeof(msg));
    msg.name = reinterpret_cast<LPSOCKADDR>(addr);
    msg.namelen = addrLen;
    msg.lpBuffers = iov;
    msg.dwBufferCount = sg.Size();

    ret = WSARecvMsg(static_cast<SOCKET>(sockfd), &msg, &dwRecv, NULL, NULL);
    if (ret == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            received = 0;
            status = ER_WOULDBLOCK;
        } else {
            status = ER_OS_ERROR;
            QCC_LogError(status, ("Receive: %s", StrError().c_str()));
        }
    } else {
        sg.SetDataSize(received);
        addrLen = msg.namelen;
        received = dwRecv;
    }
#if !defined(NDEBUG)
    QCC_DbgPrintf(("Received %u bytes", received));
    for (iter = sg.Begin(); iter != sg.End(); ++iter) {
        QCC_DbgRemoteData(iter->buf, iter->len);
    }
#endif
    if (iov != iovAuto) {
        delete[] iov;
    }
    return status;
}

QStatus RecvSG(SocketFd sockfd, ScatterGatherList& sg, size_t& received)
{
    socklen_t addrLen = 0;
    QCC_DbgTrace(("RecvSG(sockfd = %d, sg = <>, received = <>)", sockfd));

    return RecvSGCommon(sockfd, NULL, addrLen, sg, received);
}

QStatus RecvFromSG(SocketFd sockfd, IPAddress& remoteAddr, uint16_t& remotePort,
                   ScatterGatherList& sg, size_t& received)
{
    QStatus status;
    SOCKADDR_STORAGE addr;
    socklen_t addrLen = sizeof(addr);

    status = RecvSGCommon(sockfd, &addr, addrLen, sg, received);
    if (ER_OK == status) {
        GetSockAddr(&addr, addrLen, remoteAddr, remotePort);
        QCC_DbgTrace(("RecvFromSG(sockfd = %d, remoteAddr = %s, remotePort = %u, sg = <>, rcvd = %u)",
                      sockfd, remoteAddr.ToString().c_str(), remotePort, received));
    }
    return status;
}

#else

QStatus RecvSG(SocketFd sockfd, ScatterGatherList& sg, size_t& received)
{
    QStatus status = ER_OK;
    uint8_t* tmpBuf = new uint8_t[sg.MaxDataSize()];
    QCC_DbgTrace(("RecvSG(sockfd = %d, sg = <>, received = <>)", sockfd));

    status = Recv(sockfd, tmpBuf, sg.MaxDataSize(), received);
    if (ER_OK == status) {
        sg.CopyFromBuffer(tmpBuf, received);
    }
    QCC_DbgPrintf(("Received %u bytes", received));
    delete[] tmpBuf;
    return status;
}


QStatus RecvFromSG(SocketFd sockfd, IPAddress& remoteAddr, uint16_t& remotePort,
                   ScatterGatherList& sg, size_t& received)
{
    QStatus status = ER_OK;
    uint8_t* tmpBuf = new uint8_t[sg.MaxDataSize()];
    QCC_DbgTrace(("RecvToSG(sockfd = %d, remoteAddr = %s, remotePort = %u, sg = <>, sent = <>)",
                  sockfd, remoteAddr.ToString().c_str(), remotePort));

    status = RecvFrom(sockfd, remoteAddr, remotePort, tmpBuf, sg.MaxDataSize(), received);
    if (ER_OK == status) {
        sg.CopyFromBuffer(tmpBuf, received);
    }
    QCC_DbgPrintf(("Received %u bytes", received));
    delete[] tmpBuf;
    return status;
}

#endif

}