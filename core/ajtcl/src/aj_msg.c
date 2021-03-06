/**
 * @file
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

/**
 * Per-module definition of the current module for debug logging.  Must be defined
 * prior to first inclusion of aj_debug.h
 */
#define AJ_MODULE MSG

#include <stdarg.h>

#include <ajtcl/aj_target.h>
#include <ajtcl/aj_status.h>
#include <ajtcl/aj_msg.h>
#include <ajtcl/aj_msg_priv.h>
#include <ajtcl/aj_bufio.h>
#include <ajtcl/aj_connect.h>
#include <ajtcl/aj_guid.h>
#include <ajtcl/aj_peer.h>
#include <ajtcl/aj_util.h>
#include <ajtcl/aj_crypto.h>
#include <ajtcl/aj_introspect.h>
#include <ajtcl/aj_std.h>
#include <ajtcl/aj_bus.h>
#include <ajtcl/aj_debug.h>
#include <ajtcl/aj_config.h>
#include <ajtcl/aj_authorisation.h>

#ifdef AJ_ARDP
#include "aj_ardp.h"
#endif

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
#ifndef NDEBUG
uint8_t dbgMSG = 0;
#endif

#define AJ_STRUCT_CLOSE          ')'
#define AJ_DICT_ENTRY_CLOSE      '}'

/*
 * The size of the MAC for encrypted messages
 */
#define MAC_LENGTH 8

/*
 * gcc defines __va_copy() other compilers allow direct assignent of a va_list
 */
#ifndef __va_copy
#define __va_copy(a, b) (a) = (b)
#endif

/*
 * The types for each of the header fields.
 */
static const uint8_t TypeForHdr[] = {
    AJ_ARG_INVALID,
    AJ_ARG_OBJ_PATH,    /* AJ_HDR_OBJ_PATH     */
    AJ_ARG_STRING,      /* AJ_HDR_INTERFACE    */
    AJ_ARG_STRING,      /* AJ_HDR_MEMBER       */
    AJ_ARG_STRING,      /* AJ_HDR_ERROR_NAME   */
    AJ_ARG_UINT32,      /* AJ_HDR_REPLY_SERIAL */
    AJ_ARG_STRING,      /* AJ_HDR_DESTINATION  */
    AJ_ARG_STRING,      /* AJ_HDR_SENDER       */
    AJ_ARG_SIGNATURE,   /* AJ_HDR_SIGNATURE    */
    AJ_ARG_UINT32,      /* AJ_HDR_HANDLES      */
    AJ_ARG_INVALID,
    AJ_ARG_INVALID,
    AJ_ARG_INVALID,
    AJ_ARG_INVALID,
    AJ_ARG_INVALID,
    AJ_ARG_INVALID,
    AJ_ARG_UINT32,      /* AJ_HDR_TIMESTAMP         */
    AJ_ARG_UINT16,      /* AJ_HDR_TIME_TO_LIVE      */
    AJ_ARG_UINT32,      /* AJ_HDR_COMPRESSION_TOKEN */
    AJ_ARG_UINT32       /* AJ_HDR_SESSION_ID        */
};

#define AJ_SCALAR    0x10
#define AJ_CONTAINER 0x20
#define AJ_STRING    0x40
#define AJ_VARIANT   0x80

/**
 * Characterizes the various argument types
 */
static const uint8_t TypeFlags[] = {
    0x08 | AJ_CONTAINER,  /* AJ_ARG_STRUCT            '('  */
    0,                    /*                          ')'  */
    0x04 | AJ_CONTAINER,  /* AJ_ARG_ARRAY             'a'  */
    0x04 | AJ_SCALAR,     /* AJ_ARG_BOOLEAN           'b'  */
    0,
    0x08 | AJ_SCALAR,     /* AJ_ARG_DOUBLE            'd'  */
    0,
    0,
    0x01 | AJ_STRING,     /* AJ_ARG_SIGNATURE         'g'  */
    0x04 | AJ_SCALAR,     /* AJ_ARG_HANDLE            'h'  */
    0x04 | AJ_SCALAR,     /* AJ_ARG_INT32             'i'  */
    0,
    0,
    0,
    0,
    0x02 | AJ_SCALAR,     /* AJ_ARG_INT16             'n'  */
    0x04 | AJ_STRING,     /* AJ_ARG_OBJ_PATH          'o'  */
    0,
    0x02 | AJ_SCALAR,     /* AJ_ARG_UINT16            'q'  */
    0,
    0x04 | AJ_STRING,     /* AJ_ARG_STRING            's'  */
    0x08 | AJ_SCALAR,     /* AJ_ARG_UINT64            't'  */
    0x04 | AJ_SCALAR,     /* AJ_ARG_UINT32            'u'  */
    0x01 | AJ_VARIANT,    /* AJ_ARG_VARIANT           'v'  */
    0,
    0x08 | AJ_SCALAR,     /* AJ_ARG_INT64             'x'  */
    0x01 | AJ_SCALAR,     /* AJ_ARG_BYTE              'y'  */
    0,
    0x08 | AJ_CONTAINER,  /* AJ_ARG_DICT_ENTRY        '{'  */
    0,
    0                     /*                          '}'  */
};

/**
 * This macro makes sure that the signature contains valid characters
 * in the TypeFlags array. If the index passed is below ascii 'a'
 * or above ascii '}' and not ascii '(' or ')' then the signature is invalid.
 * Below is the macro broken into smaller chunks:
 *
 * ((t) == '(' || (t) == ')') ? (t) - '('       --> If the value is ) or (, get the value in TypeFlags
 * :
 * (((t) < 'a' || (t) > '}') ? '}' + 2 - 'a'    --> The value is too high or too low, return TypeFlags[30] (0)
 * :
 * (t) + 2 - 'a'                                --> The value is valid, get the value in TypeFlags
 */
#define TYPE_FLAG(t) TypeFlags[((t) == '(' || (t) == ')') ? (t) - '(' : (((t) < 'a' || (t) > '}') ? '}' + 2 - 'a' : (t) + 2 - 'a') ]

/**
 * Extract the alignment from the TypeFlags
 */
#define ALIGNMENT(t) (TYPE_FLAG(t) & 0xF)

/*
 * For scalar types returns the size of the type
 */
#define SizeOfType(typeId) (TYPE_FLAG(typeId) & 0xF)

/*
 *  Returns true if the specified type is represented as a number
 */
#define IsScalarType(typeId) (TYPE_FLAG(typeId) & AJ_SCALAR)

/*
 * A basic type is a scalar or one of the string types
 */
#define IsBasicType(typeId) (TYPE_FLAG(typeId) & (AJ_STRING | AJ_SCALAR))


int AJ_IsContainerType(char typeId)
{
    return (TYPE_FLAG(typeId) & AJ_CONTAINER) != 0;
}

int AJ_IsScalarType(char typeId)
{
    return IsScalarType(typeId) != 0;
}

int AJ_IsStringType(char typeId)
{
    return (TYPE_FLAG(typeId) & AJ_STRING) != 0;
}

int AJ_IsBasicType(char typeId)
{
    return IsBasicType(typeId) != 0;
}

size_t AJ_GetTypeSize(char typeId)
{
    return SizeOfType(typeId);
}


static AJ_MsgInterceptor incomingInterceptor;
static AJ_MsgInterceptor outgoingInterceptor;

void AJ_RegisterMsgInterceptors(AJ_MsgInterceptor incomingCB, AJ_MsgInterceptor outgoingCB)
{
    incomingInterceptor = incomingCB;
    outgoingInterceptor = outgoingCB;
}

static void InitArg(AJ_Arg* arg, uint8_t typeId, const void* val)
{
    if (arg) {
        arg->typeId = typeId;
        arg->flags = 0;
        arg->len = 0;
        arg->val.v_data = (void*)val;
        arg->sigPtr = NULL;
        arg->container = NULL;
    }
}

/*
 * Returns the number of bytes of padding to align the type
 */
static uint32_t PadForType(char typeId, AJ_IOBuffer* ioBuf)
{
    uint8_t* base = (ioBuf->direction == AJ_IO_BUF_RX) ? ioBuf->readPtr : ioBuf->writePtr;
    uint32_t offset = (uint32_t)(base - ioBuf->bufStart);
    uint32_t alignment = ALIGNMENT(typeId);
    return (alignment - offset) & (alignment - 1);
}

#define ENDSWAP16(v) (((v) >> 8) | ((v) << 8))
#define ENDSWAP32(v) (((v) >> 24) | (((v) & 0xFF0000) >> 8) | (((v) & 0x00FF00) << 8) | ((v) << 24))

static void EndianSwap(AJ_Message* msg, uint8_t typeId, void* data, uint32_t num)
{
    if (msg->hdr->endianess != HOST_ENDIANESS) {
        switch (SizeOfType(typeId)) {
        case 2:
            {
                uint16_t* p = (uint16_t*)data;
                while (num--) {
                    uint16_t v = *p;
                    *p++ = ENDSWAP16(v);
                }
            }
            break;

        case 4:
            {
                uint32_t* p = (uint32_t*)data;
                while (num--) {
                    uint32_t v = *p;
                    *p++ = ENDSWAP32(v);
                }
            }
            break;

        case 8:
            {
                uint32_t* p = (uint32_t*)data;
                while (num--) {
                    uint32_t v = p[0];
                    uint32_t u = p[1];
                    *p++ = ENDSWAP32(u);
                    *p++ = ENDSWAP32(v);
                }
            }
            break;
        }
    }
}

static uint32_t wrapped_offset(uint32_t a, uint32_t b)
{
    uint32_t offset;
    if (b <= a) {
        offset = a - b;
    } else {
        offset = (0xFFFFFFFFUL - b) + a + 1;
    }

    return offset;
}

/*
 * Serial number on incoming methods and signals.
 */

AJ_Status AJ_CheckIncomingSerial(AJ_SerialNum* prev, uint32_t curr)
{
    uint32_t offset;
    uint64_t mask;

    AJ_ASSERT(prev);

    if (curr == 0) {
        /* 0 is never a valid serial number */
        AJ_WarnPrintf(("AJ_CheckIncomingSerial: 0 is invalid\n"));
        return AJ_ERR_INVALID;
    }

    if (prev->serial == 0) {
        offset = 0;
        prev->offset = 0;
    } else {
        offset = wrapped_offset(curr, prev->serial);
        if (0 == offset) {
            /* Current serial number matches highest recent serial */
            AJ_WarnPrintf(("AJ_CheckIncomingSerial: Repeated serial %x %x %llx\n", curr, prev->serial, prev->offset));
            return AJ_ERR_INVALID;
        } else if (0xFFFFFFC0UL < offset) {
            /* Current serial number is in the recent past. Check mask but don't move the window */
            offset = (0xFFFFFFFFUL - offset) + 1;
            mask = ((uint64_t) 1) << offset;
            if (mask & prev->offset) {
                AJ_WarnPrintf(("AJ_CheckIncomingSerial: Repeated serial %x %x %llx\n", curr, prev->serial, prev->offset));
                return AJ_ERR_INVALID;
            }
            /* Switch this mask on to mark this serial number as "seen" */
            prev->offset |= mask;
            return AJ_OK;
        } else if (0x80000000UL <= offset) {
            /* Too far in the past */
            AJ_WarnPrintf(("AJ_CheckIncomingSerial: Invalid serial %x %x %llx\n", curr, prev->serial, prev->offset));
            return AJ_ERR_INVALID;
        }

        /* Moving window ahead. */
        if (offset < 64) {
            prev->offset <<= offset;
        } else {
            prev->offset = 0;
        }
    }

    prev->serial = curr;
    prev->offset |= 1;
    return AJ_OK;
}

/*
 * Computes total size of a message - note header is padded to an 8 byte boundary
 */
static uint32_t MessageLen(AJ_Message* msg)
{
    return sizeof(AJ_MsgHeader) + ((msg->hdr->headerLen + 7) & 0xFFFFFFF8) + msg->hdr->bodyLen;
}

static void InitNonce(AJ_Message* msg, uint8_t role, uint8_t* nonce)
{
    uint32_t serial = msg->hdr->serialNum;
    nonce[0] = role;
    nonce[1] = (uint8_t)(serial >> 24);
    nonce[2] = (uint8_t)(serial >> 16);
    nonce[3] = (uint8_t)(serial >> 8);
    nonce[4] = (uint8_t)(serial);
}

static AJ_Status DecryptMessage(AJ_Message* msg)
{
    AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
    AJ_Status status;
    uint8_t key[16];
    uint8_t nonce[5];
    uint8_t role = AJ_ROLE_KEY_UNDEFINED;
    uint32_t mlen = MessageLen(msg);
    uint32_t hLen = mlen - msg->hdr->bodyLen;
    uint32_t macLen;
    uint32_t nonceLen;
    uint32_t extraNonceLen;
    uint32_t cryptoValsLen;
    AJ_SerialNum* incoming;

    /*
     * Use the group key for multicast and broadcast signals the session key otherwise.
     */
    if ((msg->hdr->msgType == AJ_MSG_SIGNAL) && !msg->destination) {
        status = AJ_GetGroupKey(msg->sender, key);
    } else {
        status = AJ_GetSessionKey(msg->sender, key, &role);
        /*
         * We use the oppsite role when decrypting.
         */
        role ^= 3;
        if (AJ_OK == status) {
            status = AJ_GetSerialNumbers(msg->sender, &incoming);
        }
    }
    if (status != AJ_OK) {
        AJ_ErrPrintf(("DecryptMessage(): AJ_ERR_NO_MATCH\n"));
    } else {
        InitNonce(msg, role, nonce);
        EndianSwap(msg, AJ_ARG_INT32, &msg->hdr->bodyLen, 3);
        status = AJ_Decrypt_CCM(key, ioBuf->bufStart, mlen - MAC_LENGTH, hLen, MAC_LENGTH, nonce, sizeof(nonce));
        EndianSwap(msg, AJ_ARG_INT32, &msg->hdr->bodyLen, 3);
        if (AJ_OK == status) {
            if ((AJ_MSG_METHOD_CALL == msg->hdr->msgType) || (AJ_MSG_SIGNAL == msg->hdr->msgType)) {
                /* Methods and signals */
                status = AJ_CheckIncomingSerial(incoming, msg->hdr->serialNum);
            }
        }
    }
    AJ_MemZeroSecure(key, 16);
    return status;
}

static AJ_Status EncryptMessage(AJ_Message* msg)
{
    AJ_IOBuffer* ioBuf = &msg->bus->sock.tx;
    AJ_Status status;
    uint8_t key[16];
    uint8_t nonce[5];
    uint8_t role = AJ_ROLE_KEY_UNDEFINED;
    uint32_t mlen = MessageLen(msg);
    uint32_t hlen = mlen - msg->hdr->bodyLen;

    /*
     * Check there is room to append the MAC
     */
    if (AJ_IO_BUF_SPACE(ioBuf) < MAC_LENGTH) {
        AJ_ErrPrintf(("EncryptMessage(): AJ_ERR_RESOURCES\n"));
        return AJ_ERR_RESOURCES;
    }
    msg->hdr->bodyLen += MAC_LENGTH;
    ioBuf->writePtr += MAC_LENGTH;
    /*
     * Use the group key for multicast and broadcast signals the session key otherwise.
     */
    if ((msg->hdr->msgType == AJ_MSG_SIGNAL) && !msg->destination) {
        status = AJ_GetGroupKey(NULL, key);
        if (AJ_OK == status) {
            msg->authVersion = MIN_AUTH_FALLBACK_VERSION;
        }
    } else {
        status = AJ_GetSessionKey(msg->destination, key, &role, &msg->authVersion);
    }

    if (AJ_OK == status) {
        macLen = GetMACLength(msg);
        nonceLen = GetNonceLength(msg);
        extraNonceLen = nonceLen - PREVIOUS_NONCE_LENGTH;
        cryptoValsLen = macLen + extraNonceLen;

        /*
         * Check there is room to append the MAC and Nonce
         */
        if (AJ_IO_BUF_SPACE(ioBuf) < cryptoValsLen) {
            AJ_ErrPrintf(("EncryptMessage(): AJ_ERR_RESOURCES\n"));
            AJ_MemZeroSecure(key, 16);
            return AJ_ERR_RESOURCES;
        }
        msg->hdr->bodyLen += cryptoValsLen;
        ioBuf->writePtr += cryptoValsLen;

        if (MessageRequiresLongerCryptoValues(msg, MIN_AUTH_FULL_NONCE_LENGTH)) {
            AJ_RandBytes(ioBuf->bufStart + mlen + macLen, extraNonceLen);
        }
        AJ_InfoPrintf(("EncryptMessage(): "));
        InitNonce(msg, role, nonce, sizeof(nonce), ioBuf->bufStart + mlen + macLen, extraNonceLen);
        status = AJ_Encrypt_CCM(key, ioBuf->bufStart, mlen, hlen, macLen, nonce, nonceLen);
    } else {
        AJ_ErrPrintf(("EncryptMesssage(): peer %s not authenticated", msg->destination));
        /* Leave status from AJ_GetGroupKey/AJ_GetStatusKey unmodified.
         * Caller checks for AJ_ERR_NO_MATCH.
         */
    }
    AJ_MemZeroSecure(key, 16);
    return status;
}

static AJ_Status AuthoriseOutgoingMessage(const AJ_Message* msg)
{
    if ((msg->hdr->msgType != AJ_MSG_METHOD_CALL) && (msg->hdr->msgType != AJ_MSG_SIGNAL)) {
        return AJ_OK;
    }

    return AJ_AccessControlCheckMessage(msg, msg->destination, AJ_ACCESS_OUTGOING);
}

static AJ_Status AuthoriseIncomingMessage(const AJ_Message* msg)
{
    if ((msg->hdr->msgType != AJ_MSG_METHOD_CALL) && (msg->hdr->msgType != AJ_MSG_SIGNAL)) {
        return AJ_OK;
    }

    return AJ_AccessControlCheckMessage(msg, msg->sender, AJ_ACCESS_INCOMING);
}

AJ_Status AJ_DeliverMsg(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;
    AJ_IOBuffer* ioBuf;

    if (!msg || !msg->bus) {
        return AJ_ERR_MARSHAL;
    }
    ioBuf = &msg->bus->sock.tx;
    /*
     * Update the final body length in the header
     */
    msg->hdr->bodyLen += msg->bodyBytes;
    /*
     * Check if message is being intercepted
     */
    if (outgoingInterceptor && outgoingInterceptor(msg)) {
        AJ_IO_BUF_RESET(ioBuf);
        memset(msg, 0, sizeof(AJ_Message));
        return AJ_OK;
    }
    AJ_DumpMsg("SENDING", msg, TRUE);
    if (msg->hdr->flags & AJ_FLAG_ENCRYPTED) {
        status = AuthoriseOutgoingMessage(msg);
        if (AJ_OK == status) {
            status = EncryptMessage(msg);
        }

        if (AJ_ERR_NO_MATCH == status && AJ_MSG_ERROR == msg->hdr->msgType && msg->error == AJ_ErrSecurityViolation) {
            /* Allow an unencrypted reply in this specific key-not-found error case. */
            status = AJ_OK;
        }
    }
    if (status == AJ_OK) {
        //#pragma calls = AJ_Net_Send
        status = ioBuf->send(ioBuf);
    }
    memset(msg, 0, sizeof(AJ_Message));
    return status;
}

/*
 * Make sure we have the required number of bytes in the I/O buffer
 */
static AJ_Status LoadBytes(AJ_IOBuffer* ioBuf, uint16_t numBytes, uint8_t pad, AJ_Message* msg)
{
    AJ_Status status = AJ_OK;
    AJ_Time msgTimer;
    uint32_t* timeout = &msg->timeout;

    AJ_InitTimer(&msgTimer);
    numBytes += pad;
    /*
     * Needs to be enough headroom in the buffer to satisfy the read
     */
    if (numBytes > (ioBuf->bufSize - AJ_IO_BUF_CONSUMED(ioBuf))) {
        AJ_ErrPrintf(("LoadBytes(): AJ_ERR_RESOURCES\n"));
        return AJ_ERR_RESOURCES;
    }

    AJ_InfoPrintf(("LoadBytes(): Start loop numBytes=%u, ioBufBytes=%u\n", numBytes, AJ_IO_BUF_AVAIL(ioBuf)));
    while (AJ_IO_BUF_AVAIL(ioBuf) < numBytes) {
        //#pragma calls = AJ_Net_Recv
        AJ_InfoPrintf(("LoadBytes(): numBytes=%u, ioBufBytes=%u\n", numBytes, AJ_IO_BUF_AVAIL(ioBuf)));
        status = ioBuf->recv(ioBuf, numBytes - AJ_IO_BUF_AVAIL(ioBuf), UNMARSHAL_TIMEOUT);
        if (status != AJ_OK) {
            /*
             * Ignore interrupted recv calls for now we can't handle resumption
             */
            if (status == AJ_ERR_INTERRUPTED) {
                continue;
            }
            /*
             * Timeouts after we have started to unmarshal a message are a bad sign.
             */
            if (status == AJ_ERR_TIMEOUT) {
                /*
                 * Work around recv implementations that return too soon.
                 */
                uint32_t elapsed = AJ_GetElapsedTime(&msgTimer, FALSE);
                if (*timeout > elapsed) {
                    *timeout -= elapsed;
                    continue;
                }
                AJ_ErrPrintf(("LoadBytes(): AJ_ERR_READ\n"));
                status = AJ_ERR_READ;
            } else if (status == AJ_ERR_ARDP_RECV_EXPIRED) {
                AJ_ErrPrintf(("LoadBytes(): AJ_ERR_ARDP_RECV_EXPIRED -> AJ_ERR_UNMARSHAL\n"));
                msg->expired  = TRUE;
                status = AJ_ERR_UNMARSHAL;
            }
            break;
        }
    }
    /*
     * Skip over pad bytes (The wire protocol says these should be zeroes).
     * Only skip if bytes have actually been read.
     */
    if (status == AJ_OK) {
        ioBuf->readPtr += pad;
    }
    return status;
}

/*
 * Write bytes to an I/O buffer
 */
static AJ_Status WriteBytes(AJ_Message* msg, const void* data, size_t numBytes, size_t pad)
{
    AJ_Status status = AJ_OK;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.tx;
    if (numBytes && !data) {
        AJ_ErrPrintf(("WriteBytes(): AJ_ERR_NULL\n"));
        return AJ_ERR_NULL;
    }
    while (numBytes + pad) {
        size_t canWrite = AJ_IO_BUF_SPACE(ioBuf);
        if ((numBytes + pad) > canWrite) {
            /*
             * If we have already marshaled the header we can write what we have in the buffer
             */
            if (msg->hdr) {
                AJ_ErrPrintf(("WriteBytes(): AJ_ERR_RESOURCES\n"));
                status = AJ_ERR_RESOURCES;
            } else {
                //#pragma calls = AJ_Net_Send
                status = ioBuf->send(ioBuf);
            }
            if (status != AJ_OK) {
                break;
            }
            canWrite = AJ_IO_BUF_SPACE(ioBuf);
        }
        /*
         * Write pad bytes
         */
        while (pad) {
            *ioBuf->writePtr++ = 0;
            --canWrite;
            --pad;
        }
        if (numBytes < canWrite) {
            canWrite = numBytes;
        }
        memcpy(ioBuf->writePtr, data, canWrite);
        data = (uint8_t*)data + canWrite;
        ioBuf->writePtr += canWrite;
        numBytes -= canWrite;
    }
    return status;
}

/*
 * Write pad bytes to an I/O buffer
 */
#define WritePad(msg, pad) WriteBytes(msg, NULL, 0, pad)

AJ_Status AJ_CloseMsg(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;
    /*
     * This function is idempotent
     */
    if (msg->bus) {
        AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
        if (msg->expired == TRUE) {
            AJ_IO_BUF_RESET(ioBuf);
        } else {
            /*
             * Skip any unconsumed bytes
             */
            while (msg->bodyBytes) {
                uint16_t sz = AJ_IO_BUF_AVAIL(ioBuf);
                sz = min(sz, msg->bodyBytes);
                if (!sz) {
                    AJ_IO_BUF_RESET(ioBuf);
                    sz = min(msg->bodyBytes, ioBuf->bufSize);
                }
                status = LoadBytes(ioBuf, sz, 0, msg);
                if (status != AJ_OK) {
                    break;
                }
                msg->bodyBytes -= sz;
                ioBuf->readPtr += sz;
            }
        }

#ifndef NDEBUG
        msg->bus->currentMsg = NULL;
#endif
        memset(msg, 0, sizeof(AJ_Message));
    }
    return status;
}

AJ_Status AJ_CloseMsgAndSaveReplyContext(AJ_Message* msg, AJ_MsgReplyContext* replyCtx)
{
    if (replyCtx) {
        size_t len = strlen(msg->sender) + 1;
        if (len > sizeof(replyCtx->sender)) {
            AJ_CloseMsg(msg);
            return AJ_ERR_RESOURCES;
        }
        memcpy(replyCtx->sender, msg->sender, len + 1);
        replyCtx->bus = msg->bus;
        replyCtx->flags = msg->hdr->flags;
        replyCtx->serialNum = msg->hdr->serialNum;
        replyCtx->msgId = msg->msgId;
        replyCtx->sessionId = msg->sessionId;
    }
    return AJ_CloseMsg(msg);
}

/**
 * Get the length of the signature of the first complete type in sig
 */
static uint8_t CompleteTypeSigLen(const char* sig)
{
    if (sig) {
        const char* start = sig;
        int32_t open = 0;
        while (*sig) {
            char typeId = *sig++;
            if (typeId == AJ_STRUCT_CLOSE || typeId == AJ_DICT_ENTRY_CLOSE) {
                if (!open) {
                    return 0;
                }
                --open;
                if (!open) {
                    break;
                }
            } else if (typeId == AJ_ARG_STRUCT || typeId == AJ_ARG_DICT_ENTRY) {
                ++open;
            } else if (!open && typeId != AJ_ARG_ARRAY) {
                break;
            }
        }
        return (uint8_t)(sig - start);
    } else {
        return 0;
    }
}

/*
 * Forward declaration
 */
static AJ_Status Unmarshal(AJ_Message* msg, const char** sig, AJ_Arg* arg);

/*
 * Unmarshal a struct or dict entry argument.
 *
 * @param msg      The message
 * @param sig      The struct signature
 * @param arg      Pointer to the arg structure to return
 */
static AJ_Status UnmarshalStruct(AJ_Message* msg, const char** sig, AJ_Arg* arg, uint8_t pad)
{
    AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
    AJ_Status status = LoadBytes(ioBuf, 0, pad, msg);
    arg->val.v_data = ioBuf->readPtr;
    arg->sigPtr = *sig;
    /*
     * Consume the entire struct signature.
     */
    *sig -= 1;
    *sig += CompleteTypeSigLen(*sig);
    return status;
}

/*
 * Indicates that scalar arrays should be unmarshaled element by element. This value is set to TRUE
 * in AJ_UnmarshalContainer() for the duration of a single call to AJ_UnmarshalArgs(). If we ever
 * have to make the unmarshaler thread-safe this will need to be moved into AJ_Message.
 */
static uint8_t unmarshalScalarAsElement = FALSE;

/*
 * Unmarshal an array argument.
 *
 * @param msg   The message
 * @param sig   The array element signature
 * @param arg   Pointer to the structure to return the array
 */
static AJ_Status UnmarshalArray(AJ_Message* msg, const char** sig, AJ_Arg* arg, uint8_t pad)
{
    AJ_Status status;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
    char typeId = **sig;
    uint32_t numBytes;

    /*
     * Get the byte count for the array
     */
    status = LoadBytes(ioBuf, 4, pad, msg);
    if (status != AJ_OK) {
        return status;
    }
    EndianSwap(msg, AJ_ARG_UINT32, ioBuf->readPtr, 1);
    numBytes = *((uint32_t*)ioBuf->readPtr);
    ioBuf->readPtr += 4;
    /*
     * We are already aligned on 4 byte boundary but there may be padding after the array length if
     * the array element types align on an 8 byte boundary.
     */
    pad = PadForType(typeId, ioBuf);
    status = LoadBytes(ioBuf, numBytes, pad, msg);
    if (status != AJ_OK) {
        return status;
    }
    arg->val.v_data = ioBuf->readPtr;
    arg->sigPtr = *sig;
    arg->len = numBytes;
    if (!unmarshalScalarAsElement && IsScalarType(typeId)) {
        /*
         * For scalar types we do an inplace endian swap (if needed) and return a pointer into the read buffer.
         */
        EndianSwap(msg, typeId, (void*)arg->val.v_data, arg->len);
        ioBuf->readPtr += numBytes;
        arg->typeId = typeId;
        arg->flags = AJ_ARRAY_FLAG;
    } else {
        /*
         * For all other types the elements must be individually unmarshalled.
         */
        arg->typeId = AJ_ARG_ARRAY;
    }
    /*
     * Consume the array element signature.
     */
    *sig += CompleteTypeSigLen(*sig);
    return status;
}

/*
 * Unmarshal a single argument
 */
static AJ_Status Unmarshal(AJ_Message* msg, const char** sig, AJ_Arg* arg)
{
    AJ_Status status;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
    char typeId;
    uint32_t pad;
    uint32_t sz;

    memset(arg, 0, sizeof(AJ_Arg));

    if (!*sig || !**sig) {
        AJ_ErrPrintf(("Unmarshal(): AJ_ERR_END_OF_DATA\n"));
        return AJ_ERR_END_OF_DATA;
    }

    typeId = **sig;
    *sig += 1;
    pad = PadForType(typeId, ioBuf);

    if (IsScalarType(typeId)) {
        sz = SizeOfType(typeId);
        status = LoadBytes(ioBuf, sz, pad, msg);
        if (status != AJ_OK) {
            return status;
        }
        /*
         * For numeric types we just return a pointer into the buffer
         */
        arg->typeId = typeId;
        arg->val.v_byte = ioBuf->readPtr;
        arg->len = 0;
        ioBuf->readPtr += sz;
        EndianSwap(msg, typeId, (void*)arg->val.v_byte, 1);
    } else if (TYPE_FLAG(typeId) & (AJ_STRING | AJ_VARIANT)) {
        /*
         * Length field for a signature is 1 byte, for regular strings its 4 bytes
         */
        uint32_t lenSize = ALIGNMENT(typeId);
        /*
         * Read the string length. Note the length doesn't include the terminating NUL
         * so an empty string in encoded as two zero bytes.
         */
        status = LoadBytes(ioBuf, lenSize, pad, msg);
        if (status != AJ_OK) {
            return status;
        }
        if (lenSize == 4) {
            EndianSwap(msg, AJ_ARG_UINT32, ioBuf->readPtr, 1);
            sz = *((uint32_t*)ioBuf->readPtr);
        } else {
            sz = (uint32_t)(*ioBuf->readPtr);
        }
        ioBuf->readPtr += lenSize;
        status = LoadBytes(ioBuf, sz + 1, 0, msg);
        if (status != AJ_OK) {
            return status;
        }
        arg->typeId = typeId;
        arg->len = sz;
        arg->val.v_string = (char*)ioBuf->readPtr;
        ioBuf->readPtr += sz + 1;
        /*
         * If unmarshalling a variant store offset to start of signature
         */
        if (typeId == AJ_ARG_VARIANT) {
            msg->varOffset = (uint8_t)(sz + 1);
        }
    } else if (typeId == AJ_ARG_ARRAY) {
        status = UnmarshalArray(msg, sig, arg, pad);
    } else if ((typeId == AJ_ARG_STRUCT) || (typeId == AJ_ARG_DICT_ENTRY)) {
        arg->typeId = typeId;
        status = UnmarshalStruct(msg, sig, arg, pad);
    } else {
        AJ_ErrPrintf(("Unmarshal(): AJ_ERR_UNMARSHAL\n"));
        status = AJ_ERR_UNMARSHAL;
    }
    return status;
}

static const AJ_MsgHeader internalErrorHdr = { HOST_ENDIANESS, AJ_MSG_ERROR, 0, 0, 0, 1, 0 };

/*
 * Check that the required header fields are present for the message
 */
static AJ_Status ValidateHeader(const AJ_Message* msg)
{
    AJ_Status status = AJ_ERR_UNMARSHAL;
    /*
     * Sender field is mandatory for all messages
     */
    if ((msg->sender) && (msg->objPath)) {
        /*
         * Check required fields are present for each message type
         */
        switch (msg->hdr->msgType) {
        case AJ_MSG_SIGNAL:
            if (msg->iface && msg->member) {
                status = AJ_OK;
            }
            break;

        case AJ_MSG_METHOD_CALL:
            if (msg->destination && msg->iface && msg->member) {
                status = AJ_OK;
            }
            break;

        case AJ_MSG_ERROR:
            if (msg->destination && msg->error && msg->replySerial) {
                status = AJ_OK;
            } else {
                AJ_ErrPrintf(("The connection was rejected by the routing node\n"));
                status = AJ_ERR_REJECTED;
                return status;
            }
            break;

        case AJ_MSG_METHOD_RET:
            if (msg->destination && msg->replySerial) {
                status = AJ_OK;
            }
            break;
        }
    }
    if (status != AJ_OK) {
        AJ_ErrPrintf(("Header does not contain the required fields for the message type\n"));
    }
    return status;
}

AJ_Status AJ_ResetArgs(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;;

    if (!msg->hdr) {
        return AJ_ERR_NULL;
    }
    /*
     * Nothing to do if the message has no arguments
     */
    if (!msg->signature || (msg->signature[0] == '\0')) {
        return status;
    }
    /*
     * The arguments must fully unmarshaled before we can do a reset
     */
    while (msg->bodyBytes && (status == AJ_OK)) {
        status = AJ_SkipArg(msg);
    }
    AJ_ASSERT(msg->sigOffset == strlen(msg->signature));
    if (status == AJ_OK) {
        AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
        size_t hdrSize = sizeof(AJ_MsgHeader) + msg->hdr->headerLen + ((8 - msg->hdr->headerLen) & 7);
        /*
         * Args have already been converted to native endianess in place in the input buffer, this
         * prevents the unmarshaler from incorrectly undoing the conversion.
         */
        msg->hdr->endianess = AJ_NATIVE_ENDIAN;
        /*
         * Reset the read pointer to the start of the argument list
         */
        ioBuf->readPtr = ioBuf->bufStart + hdrSize;
        AJ_ASSERT(ioBuf->readPtr < ioBuf->writePtr);
        /*
         * Reset signature and body offsets
         */
        msg->sigOffset = 0;
        msg->bodyBytes = msg->hdr->bodyLen;
    }
    return status;
}

/*
 * Process certain bus messages before they are handed to the application.
 * This prevents the application from swallowing a message.
 *
 * @param msg   The message
 */
static AJ_Status ProcessBusMessages(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;
    /* Check for the special case messages */
    switch (msg->msgId) {
    case AJ_REPLY_ID(AJ_METHOD_NAME_HAS_OWNER):
        AJ_InfoPrintf(("ProcessBusMessages(): AJ_REPLY_ID(AJ_METHOD_NAME_HAS_OWNER)\n"));
        status = AJ_GUID_HandleNameHasOwnerReply(msg);
        /*
         * Reset so the application can handle this too.  Don't overwrite any
         * error detected during AJ_GUID_HandleNameHasOwnerReply() above.
         */
        if (status == AJ_OK) {
            status = AJ_ResetArgs(msg);
        }
        break;

    case AJ_REPLY_ID(AJ_METHOD_ADD_MATCH):
        AJ_InfoPrintf(("ProcessBusMessages(): AJ_REPLY_ID(AJ_METHOD_ADD_MATCH)\n"));
        status = AJ_GUID_HandleAddMatchReply(msg);
        /*
         * Reset so the application can handle this too.  Don't overwrite any
         * error detected during AJ_GUID_HandleAddMatchReply() above.
         */
        if (status == AJ_OK) {
            status = AJ_ResetArgs(msg);
        }
        break;

    case AJ_REPLY_ID(AJ_METHOD_REMOVE_MATCH):
        AJ_InfoPrintf(("ProcessBusMessages(): AJ_REPLY_ID(AJ_METHOD_REMOVE_MATCH)\n"));
        status = AJ_GUID_HandleRemoveMatchReply(msg);
        /*
         * Reset so the application can handle this too.  Don't overwrite any
         * error detected during AJ_GUID_HandleRemoveMatchReply() above.
         */
        if (status == AJ_OK) {
            status = AJ_ResetArgs(msg);
        }
        break;

    case AJ_SIGNAL_NAME_OWNER_CHANGED:
        AJ_InfoPrintf(("ProcessBusMessages(): AJ_SIGNAL_NAME_OWNER_CHANGED)\n"));
        status = AJ_GUID_HandleNameOwnerChanged(msg);
        /*
         * Reset so the application can handle this too.  Don't overwrite any
         * error detected during AJ_GUID_HandleNameOwnerChanged() above.
         */
        if (status == AJ_OK) {
            status = AJ_ResetArgs(msg);
        }
        break;

    default:
        break;
    }

    return status;
}

AJ_Status AJ_UnmarshalMsg(AJ_BusAttachment* bus, AJ_Message* msg, uint32_t timeout)
{
    AJ_Status status;
    AJ_IOBuffer* ioBuf = &bus->sock.rx;
    void* hdrRaw = NULL;
    uint8_t* endOfHeader;
    uint32_t hdrPad;
    AJ_Time msgTimer;

    AJ_InfoPrintf(("AJ_UnmarshalMsg()\n"));

    AJ_InitTimer(&msgTimer);
    /*
     * Clear message then set the bus and overall timeout
     */
    memset(msg, 0, sizeof(AJ_Message));
    msg->msgId = AJ_INVALID_MSG_ID;
    msg->bus = bus;
    msg->timeout = timeout;
    /*
     * Check that the read and write pointers are within the bounds of the recv buffer
     */
    if ((ioBuf->readPtr < ioBuf->bufStart) || (ioBuf->readPtr > (ioBuf->bufStart + ioBuf->bufSize)) ||
        (ioBuf->writePtr < ioBuf->readPtr) || (ioBuf->writePtr > (ioBuf->bufStart + ioBuf->bufSize))) {
        AJ_ErrPrintf(("AJ_UnmarshalMsg(): recv buffer pointer out of bounds: AJ_ERR_IO_BUFFER\n"));
        return AJ_ERR_READ; //Buffer pointer is out of bounds, this is unrecoverable
    }
    /*
     * Move any unconsumed data to the start of the I/O buffer
     */
    AJ_IOBufRebase(ioBuf, 0);
    /*
     * Load the message header
     */
    AJ_InfoPrintf(("AJ_UnmarshalMsg(): start loop ioBufBytes=%u\n", AJ_IO_BUF_AVAIL(ioBuf)));
    while (AJ_IO_BUF_AVAIL(ioBuf) < sizeof(AJ_MsgHeader)) {
        //#pragma calls = AJ_Net_Recv
        AJ_InfoPrintf(("AJ_UnmarshalMsg(): ioBufBytes=%u\n", AJ_IO_BUF_AVAIL(ioBuf)));
        status = ioBuf->recv(ioBuf, sizeof(AJ_MsgHeader) - AJ_IO_BUF_AVAIL(ioBuf), timeout);
        if (status != AJ_OK) {
            if (status == AJ_ERR_TIMEOUT) {
                /*
                 * Work around recv implementations that return too soon.
                 */
                uint32_t elapsed = AJ_GetElapsedTime(&msgTimer, FALSE);
                if (msg->timeout > elapsed) {
                    msg->timeout -= elapsed;
                    continue;
                }
            }
            /*
             * If there were no messages to receive check if we have any methods call that have
             * timed-out and if so generate an internal error message to allow the application to
             * proceed.
             */
            if ((status == AJ_ERR_TIMEOUT) && AJ_TimedOutMethodCall(msg)) {
                msg->hdr = (AJ_MsgHeader*)&internalErrorHdr;
                msg->error = AJ_ErrTimeout;
                msg->sender = AJ_GetUniqueName(msg->bus);
                msg->destination = msg->sender;
                status = AJ_OK;
            } else if (status == AJ_ERR_ARDP_RECV_EXPIRED) {
                status = AJ_ERR_UNMARSHAL;
                msg->expired = TRUE;
            }

            return status;
        }
    }
    /*
     * Header was unmarshalled directly into the rx buffer
     */
    msg->hdr = (AJ_MsgHeader*)ioBuf->bufStart;
    AJ_ASSERT(msg->hdr);
    /* Save raw header before endian swaps */
    memcpy(&msg->raw, msg->hdr, sizeof (AJ_MsgHeader));
    ioBuf->readPtr += sizeof(AJ_MsgHeader);
    /*
     * Quick sanity check on the header - unrecoverable error if this check fails
     */
    if ((msg->hdr->endianess != AJ_LITTLE_ENDIAN) && (msg->hdr->endianess != AJ_BIG_ENDIAN)) {
        AJ_ErrPrintf(("AJ_UnmarshalMsg(): AJ_ERR_READ\n"));
        return AJ_ERR_READ;
    }
    /*
     * Endian swap header info - conveniently they are contiguous in the header
     */
    EndianSwap(msg, AJ_ARG_INT32, &msg->hdr->bodyLen, 3);
    msg->bodyBytes = msg->hdr->bodyLen;
    /*
     * The header is null-padded to an 8-byte boundary
     */
    hdrPad = (8 - msg->hdr->headerLen) & 7;
    /*
     * Make sure the header (plus pad) isn't going to overrun the buffer
     * and that the total header length doesn't overflow.
     */
    if ((msg->hdr->headerLen + hdrPad) > (ioBuf->bufSize - sizeof(AJ_MsgHeader))) {
        AJ_ErrPrintf(("AJ_UnmarshalMsg(): Header was too large: AJ_ERR_HDR_CORRUPT\n"));
        return AJ_ERR_READ; //Unrecoverable state, return read error
    }
    /*
     * Load the header
     */
    status = LoadBytes(ioBuf, msg->hdr->headerLen + hdrPad, 0, msg);
    if (status != AJ_OK) {
        return status;
    }
#ifndef NDEBUG
    /*
     * Check that messages are getting closed
     */
    AJ_ASSERT(!msg->bus->currentMsg);
    msg->bus->currentMsg = msg;
#endif
    /*
     * If the endianess of the message is different than the local host
     * endianness we need to copy the header bytes before we unmarshal the header and swizzle all
     * the integers.
     */
    if ((msg->hdr->endianess != HOST_ENDIANESS)) {
        hdrRaw = AJ_Malloc(msg->hdr->headerLen);
        if (hdrRaw) {
            memcpy(hdrRaw, ioBuf->readPtr, msg->hdr->headerLen);
        }
    }
    /*
     * Assume an empty signature
     */
    msg->signature = "";
    /*
     * We have the header in the buffer now we can unmarshal the header fields
     */
    endOfHeader = ioBuf->bufStart + sizeof(AJ_MsgHeader) + msg->hdr->headerLen;

    while (ioBuf->readPtr < endOfHeader) {
        const char* fieldSig;
        uint8_t fieldId;
        AJ_Arg hdrVal;
        /*
         * Custom unmarshal the header field - signature is "(yv)" so starts off with STRUCT aligment.
         */
        status = LoadBytes(ioBuf, 4, PadForType(AJ_ARG_STRUCT, ioBuf), msg);
        if (status != AJ_OK) {
            break;
        }
        fieldId = ioBuf->readPtr[0];
        fieldSig = (const char*)&ioBuf->readPtr[2];
        ioBuf->readPtr += 4;
        /*
         * Now unmarshal the field value
         */
        status = Unmarshal(msg, &fieldSig, &hdrVal);
        if (status != AJ_OK) {
            break;
        }
        /*
         * Check the field has the type we expect - we ignore fields we don't know
         */
        if ((fieldId <= AJ_HDR_SESSION_ID) && (TypeForHdr[fieldId] != hdrVal.typeId)) {
            AJ_ErrPrintf(("AJ_UnmarshalMsg(): AJ_ERR_UNMARSHAL\n"));
            status = AJ_ERR_UNMARSHAL;
            break;
        }
        /*
         * Set the field value in the message
         */
        switch (fieldId) {
        case AJ_HDR_OBJ_PATH:
            msg->objPath = hdrVal.val.v_objPath;
            break;

        case AJ_HDR_INTERFACE:
            msg->iface = hdrVal.val.v_string;
            break;

        case AJ_HDR_MEMBER:
            msg->member = hdrVal.val.v_string;
            break;

        case AJ_HDR_ERROR_NAME:
            msg->error = hdrVal.val.v_string;
            break;

        case AJ_HDR_REPLY_SERIAL:
            msg->replySerial = *(hdrVal.val.v_uint32);
            break;

        case AJ_HDR_DESTINATION:
            msg->destination = hdrVal.val.v_string;
            break;

        case AJ_HDR_SENDER:
            msg->sender = hdrVal.val.v_string;
            break;

        case AJ_HDR_SIGNATURE:
            msg->signature = hdrVal.val.v_signature;
            break;

        case AJ_HDR_TIMESTAMP:
            msg->timestamp = *(hdrVal.val.v_uint32);
            break;

        case AJ_HDR_TIME_TO_LIVE:
            msg->ttl = *(hdrVal.val.v_uint32);
            break;

        case AJ_HDR_SESSION_ID:
            msg->sessionId = *(hdrVal.val.v_uint32);
            break;

        case AJ_HDR_COMPRESSION_TOKEN:
            AJ_ErrPrintf(("Compressed headers not currently handled\n"));
            status = AJ_ERR_UNMARSHAL;
            break;

        case AJ_HDR_HANDLES:
        default:
            /* Ignored */
            break;
        }
    }
    /*
     * Check that the required header fields are present
     */
    if (status == AJ_OK) {
        status = ValidateHeader(msg);
    }
    /*
     * If we copied the raw header earlier we copy it back now.
     * This only happens if endianness of the message is different from the local host's endianness.
     */
    if (hdrRaw) {
        memcpy(ioBuf->bufStart + sizeof(AJ_MsgHeader), hdrRaw, msg->hdr->headerLen);
        AJ_Free(hdrRaw);
        hdrRaw = NULL;
    }
    if (ioBuf->readPtr != endOfHeader) {
        status = AJ_ERR_HDR_CORRUPT;
    }
    if (status == AJ_OK) {
        AJ_ASSERT(ioBuf->readPtr == endOfHeader);
        /*
         * Consume the header pad bytes.
         */
        ioBuf->readPtr += hdrPad;
        /*
         * If the message is encrypted load the entire message body and decrypt it.
         */
        if (msg->hdr->flags & AJ_FLAG_ENCRYPTED) {
            status = LoadBytes(ioBuf, msg->hdr->bodyLen, 0, msg);
            if (status == AJ_OK) {
                status = DecryptMessage(msg);
                /*
                 * If session key missing, reply with error message.
                 * If decryption failed, silently ignore message below.
                 */
                if ((AJ_ERR_NO_MATCH == status) && (msg->hdr->msgType == AJ_MSG_METHOD_CALL) && !(msg->hdr->flags & AJ_FLAG_NO_REPLY_EXPECTED)) {
                    AJ_Message reply;
                    AJ_Status replyStatus;
                    status = AJ_ERR_SECURITY;
                    replyStatus = AJ_MarshalStatusMsg(msg, &reply, status);
                    if (AJ_OK == replyStatus) {
                        replyStatus = AJ_DeliverMsg(&reply);
                    }
                    if (AJ_OK != replyStatus) {
                        /* Fail to send an error reply, log and continue */
                        AJ_InfoPrintf(("AJ_UnmarshalMsg(): %s\n", AJ_StatusText(replyStatus)));
                    }
                }
            }
        }
        /*
         * Toggle the AUTO_START flag so in the API no flags == 0
         *
         * Note we must do this after decrypting the message or message authentication will fail.
         */
        msg->hdr->flags ^= AJ_FLAG_AUTO_START;
        /*
         * If the message looks good try to identify it.
         */
        if (status == AJ_OK) {
            status = AJ_IdentifyMessage(msg);
        }

        /*
         * If this is the Peer.Authentication interface,
         * load the entire message into the buffer.
         * Only do this if it wasn't done above (encrypted message).
         */
        if ((AJ_BUS_MESSAGE_ID(0xFF, 0xFF, 0) & msg->msgId) == AJ_PEER_AUTHENTICATION_IFN) {
            if (!(msg->hdr->flags & AJ_FLAG_ENCRYPTED)) {
                status = LoadBytes(ioBuf, msg->hdr->bodyLen, 0, msg);
            }
        }

        /*
         * Check incoming policy
         */
        if ((AJ_OK == status) && (msg->hdr->flags & AJ_FLAG_ENCRYPTED)) {
            status = AuthoriseIncomingMessage(msg);
            if ((AJ_OK != status) && (msg->hdr->msgType == AJ_MSG_METHOD_CALL) && !(msg->hdr->flags & AJ_FLAG_NO_REPLY_EXPECTED)) {
                AJ_Message reply;
                AJ_Status replyStatus;
                replyStatus = AJ_MarshalStatusMsg(msg, &reply, status);
                if (AJ_OK == replyStatus) {
                    replyStatus = AJ_DeliverMsg(&reply);
                }
                if (AJ_OK != replyStatus) {
                    /* Fail to send an error reply, log and continue */
                    AJ_InfoPrintf(("AJ_UnmarshalMsg(): %s\n", AJ_StatusText(replyStatus)));
                }
            }
        }
    } else {
        /*
         * Consume entire header
         */
        ioBuf->readPtr = endOfHeader + hdrPad;
    }
    if (status == AJ_OK) {
        AJ_DumpMsg("RECEIVED", msg, FALSE);
    } else {
        /*
         * Silently discard message unless in debug mode
         */
        AJ_WarnPrintf(("Discarding unknown message %s\n", AJ_StatusText(status)));
        AJ_DumpMsg("DISCARDING", msg, FALSE);
        AJ_CloseMsg(msg);
    }
    if (status == AJ_OK) {
        if (incomingInterceptor && incomingInterceptor(msg)) {
            AJ_CloseMsg(msg);
            status = AJ_ERR_INTERRUPTED;
        } else {
            status = ProcessBusMessages(msg);
        }
    }
    return status;
}

AJ_Status AJ_SkipArg(AJ_Message* msg)
{
    AJ_Status status;
    AJ_Arg skippy;

    /*
     * Variants must be skipped atomically
     */
    if (msg->varOffset) {
        return AJ_ERR_UNEXPECTED;
    }
    status = AJ_UnmarshalArg(msg, &skippy);
    if (status != AJ_OK) {
        return status;
    }
    if (msg->varOffset) {
        status = AJ_UnmarshalArg(msg, &skippy);
    }
    if (TYPE_FLAG(skippy.typeId) == 0) {
        return AJ_ERR_SIGNATURE;
    }
    /*
     * If skipping a container skip the contents
     */
    if (TYPE_FLAG(skippy.typeId) & AJ_CONTAINER) {
        skippy.container = msg->outer;
        msg->outer = &skippy;
        if (skippy.typeId == AJ_ARG_ARRAY) {
            AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
            /*
             * Just consume the array bytes
             */
            status = LoadBytes(ioBuf, skippy.len, 0, msg);
            if (status == AJ_OK) {
                ioBuf->readPtr += skippy.len;
                msg->bodyBytes -= skippy.len;
            }
        } else {
            /*
             * Skip the individual elements
             */
            char closeType = (skippy.typeId == AJ_ARG_STRUCT) ?  AJ_STRUCT_CLOSE : AJ_DICT_ENTRY_CLOSE;
            while (*skippy.sigPtr != closeType) {
                status = AJ_SkipArg(msg);
                if (status != AJ_OK) {
                    break;
                }
            }
        }
        if (status == AJ_OK) {
            status = AJ_UnmarshalCloseContainer(msg, &skippy);
        }
        msg->outer = skippy.container;
    }
    return status;
}

const char* AJ_NextArgSig(AJ_Message* msg)
{
    AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
    AJ_Arg* container = msg->outer;
    const char* sig;

    if (msg->varOffset) {
        /*
         * Variant - get the signature from the I/O buffer
         */
        sig = (const char*)(ioBuf->readPtr - msg->varOffset);
    } else if (container) {
        /*
         * Component of a container - use the container's signature
         */
        sig = container->sigPtr;
    } else {
        /*
         * Everything else - use the message signature
         */
        sig = msg->signature + msg->sigOffset;
    }
    return sig;
}

AJ_Status AJ_UnmarshalArg(AJ_Message* msg, AJ_Arg* arg)
{
    AJ_Status status;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
    AJ_Arg* container = msg->outer;
    uint8_t* argStart = ioBuf->readPtr;
    size_t consumed;
    const char* sig = AJ_NextArgSig(msg);

    if (msg->varOffset) {
        msg->varOffset = 0;
        status = Unmarshal(msg, &sig, arg);
    } else if (container) {
        if (container->typeId == AJ_ARG_ARRAY) {
            size_t len = (uint16_t)(ioBuf->readPtr - (uint8_t*)container->val.v_data);
            /*
             * Return an error status if there are no more array elements.
             */
            if (len == container->len) {
                memset(arg, 0, sizeof(AJ_Arg));
                AJ_InfoPrintf(("AJ_UnmarshalMsg(): AJ_ERR_NO_MORE\n"));
                status = AJ_ERR_NO_MORE;
            } else {
                status = Unmarshal(msg, &sig, arg);
            }
        } else {
            status = Unmarshal(msg, &sig, arg);
            container->sigPtr = sig;
        }
    } else {
        /*
         * The signature representing the message body does not match the number of body bytes
         */
        if (!msg->bodyBytes) {
            AJ_ErrPrintf(("AJ_UnmarshalArg(): Message body length is incorrect, status = AJ_ERR_UNMARSHAL\n"));
            status = AJ_ERR_UNMARSHAL;
        } else {
            status = Unmarshal(msg, &sig, arg);
            msg->sigOffset = (uint8_t)(sig - msg->signature);
        }
    }
    consumed = (ioBuf->readPtr - argStart);
    if (consumed > msg->bodyBytes) {
        /*
         * Unrecoverable
         */
        AJ_ErrPrintf(("AJ_UnmarshalArg(): AJ_ERR_READ\n"));
        status = AJ_ERR_READ;
    } else {
        msg->bodyBytes -= (uint16_t)consumed;
    }
    return status;
}

static AJ_Status VUnmarshalArgs(AJ_Message* msg, const char** sig, va_list* argpp)
{
    AJ_Status status = AJ_OK;
    AJ_Arg arg;
    AJ_Arg container;
    va_list argp;

    __va_copy(argp, *argpp);

    container.typeId = AJ_ARG_INVALID;

    while (**sig) {
        uint8_t typeId = (uint8_t)*((*sig)++);
        void* val;

        if (!IsBasicType(typeId)) {
            if ((typeId == AJ_ARG_STRUCT) || (typeId == AJ_ARG_DICT_ENTRY)) {
                /*
                 * This function supports unmarshaling of a single level structs.
                 */

                status = AJ_UnmarshalContainer(msg, &container, typeId);
                if (status != AJ_OK) {
                    break;
                }

                status = VUnmarshalArgs(msg, sig, &argp);
                /*
                 * Upon successful return from a nested call, continue from
                 * where the inner call advanced in the signature.
                 */
                if (status == AJ_OK) {
                    char tId = *(*sig - 1);
                    if ((tId == AJ_STRUCT_CLOSE) || (tId == AJ_DICT_ENTRY_CLOSE)) {
                        status = AJ_UnmarshalCloseContainer(msg, &container);
                        if (status != AJ_OK) {
                            break;
                        }
                    } else {
                        status = AJ_ERR_UNMARSHAL;
                        break;
                    }
                    continue;
                } else {
                    break;
                }
                continue;
            }
            if ((typeId == AJ_ARG_ARRAY) && IsBasicType(**sig)) {
                const void** ptr = va_arg(argp, const void**);
                size_t* len = va_arg(argp, size_t*);
                (*sig)++;
                status = AJ_UnmarshalArg(msg, &arg);
                if (status != AJ_OK) {
                    break;
                }
                *ptr = arg.val.v_data;
                *len = arg.len;
                continue;
            }
            if ((typeId == AJ_STRUCT_CLOSE) || (typeId == AJ_DICT_ENTRY_CLOSE)) {
                break;
            }
            if (typeId == AJ_ARG_VARIANT) {
                const char* vsigExpect = va_arg(argp, const char*);
                const char* vsig;
                status = AJ_UnmarshalVariant(msg, &vsig);
                if (status == AJ_OK) {
                    if (strcmp(vsig, vsigExpect) != 0) {
                        status = AJ_ERR_SIGNATURE;
                        break;
                    }
                    status = VUnmarshalArgs(msg, &vsig, &argp);
                }
                if (status == AJ_OK) {
                    continue;
                }
            }
            status = AJ_ERR_UNEXPECTED;
            break;
        }
        status = AJ_UnmarshalArg(msg, &arg);
        if (status != AJ_OK) {
            break;
        }
        if (arg.typeId != typeId) {
            AJ_ErrPrintf(("AJ_UnmarshalArgs(): AJ_ERR_UNMARSHAL\n"));
            status = AJ_ERR_UNMARSHAL;
            break;
        }
        val = va_arg(argp, void*);
        if (IsScalarType(typeId)) {
            switch (SizeOfType(typeId)) {
            case 1:
                *((uint8_t*)val) = *arg.val.v_byte;
                break;

            case 2:
                *((uint16_t*)val) = *arg.val.v_uint16;
                break;

            case 4:
                *((uint32_t*)val) = *arg.val.v_uint32;
                break;

            case 8:
                *((uint64_t*)val) = *arg.val.v_uint64;
                break;
            }
        } else {
            *((const char**)val) = arg.val.v_string;
        }
    }
    __va_copy(*argpp, argp);
    return status;
}

AJ_Status AJ_UnmarshalArgs(AJ_Message* msg, const char* sig, ...)
{
    AJ_Status status;
    va_list argp;

    va_start(argp, sig);
    status = VUnmarshalArgs(msg, &sig, &argp);
    va_end(argp);

    return status;
}

AJ_Status AJ_UnmarshalRaw(AJ_Message* msg, const void** data, size_t len, size_t* actual)
{
    AJ_Status status;
    size_t sz;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
    size_t hdrSize = 8 + msg->hdr->headerLen + ((8 - msg->hdr->headerLen) & 7);

    /*
     * A sig offset of 0xFF indicates we are already doing raw unnmarshaling
     */
    if (msg->sigOffset != 0xFF) {
        uint8_t typeId = msg->signature[msg->sigOffset];
        uint8_t pad;
        /*
         * There must be arguments to unmarshal
         */
        if (!typeId) {
            AJ_ErrPrintf(("AJ_UnmarshalRaw(): AJ_ERR_SIGNATURE\n"));
            return AJ_ERR_SIGNATURE;
        }
        /*
         * There may be padding before the argument
         */
        pad = PadForType(typeId, ioBuf);
        if (pad > msg->bodyBytes) {
            AJ_ErrPrintf(("AJ_UnmarshalRaw(): AJ_ERR_UNMARSHAL\n"));
            return AJ_ERR_UNMARSHAL;
        }
        status = LoadBytes(ioBuf, 0, pad, msg);
        if (status != AJ_OK) {
            return status;
        }
        msg->bodyBytes -= pad;
        /*
         * Standard signature matching is now meaningless
         */
        msg->signature = "";
        /*
         * Flag we are in raw-unmarshal mode
         */
        msg->sigOffset = 0xFF;
    }
    /*
     * Return an error if caller is attempting read off the end of the body
     */
    if (len > msg->bodyBytes) {
        AJ_ErrPrintf(("AJ_UnmarshalRaw(): AJ_ERR_UNMARSHAL\n"));
        return AJ_ERR_UNMARSHAL;
    }
    /*
     * We want to return the requested data as contiguous bytes if possible
     */
    sz = AJ_IO_BUF_AVAIL(ioBuf);
    if (sz < len) {
        AJ_IOBufRebase(ioBuf, hdrSize);
    }
    /*
     * If we try to load more than the available space we will get an error
     */
    len = min(len, AJ_IO_BUF_SPACE(ioBuf));
    status = LoadBytes(ioBuf, (uint16_t)len, 0, msg);
    if (status == AJ_OK) {
        sz = AJ_IO_BUF_AVAIL(ioBuf);
        if (sz < len) {
            len = sz;
        }
        *data = ioBuf->readPtr;
        *actual = len;
        ioBuf->readPtr += len;
        msg->bodyBytes -= (uint16_t)len;
    }
    return status;
}

AJ_Status AJ_UnmarshalContainer(AJ_Message* msg, AJ_Arg* arg, uint8_t typeId)
{
    AJ_Status status = AJ_ERR_UNMARSHAL;

    if ((TYPE_FLAG(typeId) & AJ_CONTAINER)) {
        unmarshalScalarAsElement = TRUE;
        status = AJ_UnmarshalArg(msg, arg);
        unmarshalScalarAsElement = FALSE;
        if (status == AJ_OK) {
            if (arg->typeId == typeId) {
                arg->container = msg->outer;
                msg->outer = arg;
            } else {
                AJ_ErrPrintf(("AJ_UnmarshalContainer(): AJ_ERR_UNMARSHAL\n"));
                status =  AJ_ERR_UNMARSHAL;
            }
        }
    }
    return status;
}

AJ_Status AJ_UnmarshalCloseContainer(AJ_Message* msg, AJ_Arg* arg)
{
    AJ_ASSERT(TYPE_FLAG(arg->typeId) & AJ_CONTAINER);
    AJ_ASSERT(msg->outer == arg);

    msg->outer = arg->container;

    if (arg->typeId == AJ_ARG_ARRAY) {
        AJ_IOBuffer* ioBuf = &msg->bus->sock.rx;
        /*
         * Check that all the array elements have been unmarshaled
         */
        size_t len = (uint16_t)(ioBuf->readPtr - (uint8_t*)arg->val.v_data);
        if (len != arg->len) {
            AJ_ErrPrintf(("AJ_UnmarshalCloseContainer(): AJ_ERR_UNMARSHAL\n"));
            return AJ_ERR_UNMARSHAL;
        }
    } else {
        /*
         * Check that all of the struct elements have been unmarshaled
         */
        if ((arg->typeId == AJ_ARG_STRUCT) && (*arg->sigPtr != AJ_STRUCT_CLOSE)) {
            AJ_ErrPrintf(("AJ_UnmarshalCloseContainer(): AJ_ERR_SIGNATURE\n"));
            return AJ_ERR_SIGNATURE;
        }
        if ((arg->typeId == AJ_ARG_DICT_ENTRY) && (*arg->sigPtr != AJ_DICT_ENTRY_CLOSE)) {
            AJ_ErrPrintf(("AJ_UnmarshalCloseContainer(): AJ_ERR_SIGNATURE\n"));
            return AJ_ERR_SIGNATURE;
        }
    }
    return AJ_OK;
}

AJ_Status AJ_UnmarshalVariant(AJ_Message* msg, const char** sig)
{
    AJ_Arg arg;
    AJ_Status status = AJ_UnmarshalArg(msg, &arg);
    if (status == AJ_OK) {
        if (sig) {
            *sig = arg.val.v_string;
        }
    }
    return status;
}

/*
 * Forward declaration
 */
static AJ_Status Marshal(AJ_Message* msg, const char** sig, AJ_Arg* arg);

static AJ_Status MarshalContainer(AJ_Message* msg, const char** sig, AJ_Arg* arg, uint8_t pad)
{
    AJ_Status status;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.tx;

    *sig -= 1;
    arg->sigPtr = *sig + 1;
    if (**sig == AJ_ARG_ARRAY) {
        /*
         * Reserve space for the length and save a pointer to it
         */
        status = WriteBytes(msg, NULL, 0, pad + 4);
        arg->val.v_data = ioBuf->writePtr - 4;
        /*
         * Might need to pad if the elements align on an 8 byte boundary
         */
        if (status == AJ_OK) {
            status = WritePad(msg, PadForType(arg->sigPtr[0], ioBuf));
        }
    } else {
        status = WritePad(msg, pad);
    }
    /*
     * Consume container signature
     */
    *sig += CompleteTypeSigLen(*sig);
    return status;
}

static AJ_Status Marshal(AJ_Message* msg, const char** sig, AJ_Arg* arg)
{
    AJ_Status status = AJ_OK;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.tx;
    char typeId = **sig;
    uint32_t pad = PadForType(typeId, ioBuf);
    size_t sz;

    if (!arg) {
        AJ_ErrPrintf(("Marshal(): AJ_ERR_NULL\n"));
        return AJ_ERR_NULL;
    }
    *sig += 1;
    if (IsScalarType(arg->typeId)) {
        if (arg->flags & AJ_ARRAY_FLAG) {
            uint32_t szu32;
            if ((typeId != AJ_ARG_ARRAY) || (**sig != arg->typeId)) {
                AJ_ErrPrintf(("Marshal(): AJ_ERR_MARSHAL\n"));
                return AJ_ERR_MARSHAL;
            }
            *sig += 1;
            sz = arg->len;
            szu32 = (uint32_t)sz;
            status = WriteBytes(msg, &szu32, 4, pad);
            if (status == AJ_OK) {
                /*
                 * May need to pad if the elements required 8 byte alignment
                 */
                pad = PadForType(arg->typeId, ioBuf);
            }
        } else {
            if (typeId != arg->typeId) {
                AJ_ErrPrintf(("Marshal(): AJ_ERR_MARSHAL\n"));
                return AJ_ERR_MARSHAL;
            }
            sz = SizeOfType(typeId);
        }
        if (status == AJ_OK) {
            status = WriteBytes(msg, arg->val.v_data, sz, pad);
        }
    } else if (TYPE_FLAG(typeId) & (AJ_STRING | AJ_VARIANT)) {
        if (typeId != arg->typeId) {
            AJ_ErrPrintf(("Marshal(): AJ_ERR_MARSHAL\n"));
            return AJ_ERR_MARSHAL;
        }
        sz = arg->len ? arg->len : strlen(arg->val.v_string);
        /*
         * Length field for a signature is 1 byte, for regular strings its 4 bytes
         */
        if (ALIGNMENT(typeId) == 1) {
            uint8_t szu8 = (uint8_t)sz;
            if (sz > 255) {
                return AJ_ERR_MARSHAL;
            }
            status = WriteBytes(msg, &szu8, 1, pad);
        } else {
            uint32_t szu32 = (uint32_t)sz;
            status = WriteBytes(msg, &szu32, 4, pad);
        }
        if (status == AJ_OK) {
            status = WriteBytes(msg, arg->val.v_string, sz, 0);
            /*
             * String must be NUL terminated on the wire
             */
            if (status == AJ_OK) {
                status = WritePad(msg, 1);
            }
            /*
             * If marshalling a variant store offset to start of signature
             */
            if (typeId == AJ_ARG_VARIANT) {
                msg->varOffset = (uint8_t)(sz + 1);
            }
        }
    } else if (TYPE_FLAG(typeId) & AJ_CONTAINER) {
        if (typeId != arg->typeId) {
            AJ_ErrPrintf(("Marshal(): AJ_ERR_MARSHAL\n"));
            return AJ_ERR_MARSHAL;
        }
        status = MarshalContainer(msg, sig, arg, pad);
    } else {
        AJ_ErrPrintf(("Marshal(): AJ_ERR_MARSHAL\n"));
        return AJ_ERR_MARSHAL;
    }
    return status;
}

static AJ_Status MarshalMsg(AJ_Message* msg, uint8_t msgType, uint32_t msgId, uint8_t flags)
{
    AJ_Status status = AJ_OK;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.tx;
    uint8_t fieldId;
    uint8_t secure = FALSE;

    if (!ioBuf->bufStart) {
        AJ_ErrPrintf(("MarshalMsg(): ioBuf has not been initialized\n"));
        return AJ_ERR_IO_BUFFER;
    }
#ifdef AJ_ARDP
    status = AJ_ARDP_StartMsgSend(msg->ttl);
#endif

    /*
     * Use the msgId to lookup information in the object and interface descriptions to
     * initialize the message header fields.
     */
    status = AJ_InitMessageFromMsgId(msg, msgId, msgType, &secure);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("MarshalMsg(): status=%s\n", AJ_StatusText(status)));
        return status;
    }

    AJ_IO_BUF_RESET(ioBuf);

    msg->hdr = (AJ_MsgHeader*)ioBuf->bufStart;
    memset(msg->hdr, 0, sizeof(AJ_MsgHeader));
    ioBuf->writePtr += sizeof(AJ_MsgHeader);

    msg->hdr->endianess = HOST_ENDIANESS;
    msg->hdr->msgType = msgType;
    msg->hdr->flags = flags;
    if (secure) {
        msg->hdr->flags |= AJ_FLAG_ENCRYPTED;
    }

    /*
     * The wire-protocol calls this flag NO_AUTO_START we toggle the meaning in the API
     * so the default flags value can be zero.
     */
    msg->hdr->flags ^= AJ_FLAG_AUTO_START;
    /*
     * Serial number cannot be zero (wire-spec weirdness)
     */
    do {
        msg->hdr->serialNum = msg->bus->serial++;
    } while (msg->bus->serial == 1);
    /*
     * Marshal the header fields
     */
    for (fieldId = AJ_HDR_OBJ_PATH; fieldId <= AJ_HDR_SESSION_ID; ++fieldId) {
        char typeId = TypeForHdr[fieldId];
        char buf[4];
        const char* fieldSig = &buf[2];
        AJ_Arg hdrVal;
        /*
         * Skip field id's that are not currently used.
         */
        if (typeId == AJ_ARG_INVALID) {
            continue;
        }
        InitArg(&hdrVal, typeId, NULL);
        switch (fieldId) {
        case AJ_HDR_OBJ_PATH:
            if ((msgType == AJ_MSG_METHOD_CALL) || (msgType == AJ_MSG_SIGNAL)) {
                hdrVal.val.v_objPath = msg->objPath;
            }
            break;

        case AJ_HDR_INTERFACE:
            hdrVal.val.v_string = msg->iface;
            break;

        case AJ_HDR_MEMBER:
            if (msgType != AJ_MSG_ERROR) {
                int32_t len = AJ_StringFindFirstOf(msg->member, " ");
                hdrVal.val.v_string = msg->member;
                hdrVal.len = (len >= 0) ? len : 0;
            }
            break;

        case AJ_HDR_ERROR_NAME:
            if (msgType == AJ_MSG_ERROR) {
                hdrVal.val.v_string = msg->error;
            }
            break;

        case AJ_HDR_REPLY_SERIAL:
            if ((msgType == AJ_MSG_METHOD_RET) || (msgType == AJ_MSG_ERROR)) {
                hdrVal.val.v_uint32 = &msg->replySerial;
            }
            break;

        case AJ_HDR_DESTINATION:
            hdrVal.val.v_string = msg->destination;
            break;

        case AJ_HDR_SENDER:
            hdrVal.val.v_string = AJ_GetUniqueName(msg->bus);
            break;

        case AJ_HDR_SIGNATURE:
            hdrVal.val.v_signature = msg->signature;
            break;

        case AJ_HDR_TIMESTAMP:
            if (msg->ttl) {
                AJ_Time timer;
                timer.seconds = 0;
                timer.milliseconds = 0;
                msg->timestamp = AJ_GetElapsedTime(&timer, FALSE);
                hdrVal.val.v_uint32 = &msg->timestamp;
            }
            break;

        case AJ_HDR_TIME_TO_LIVE:
            if (msg->ttl) {
                hdrVal.val.v_uint32 = &msg->ttl;
            }
            break;

        case AJ_HDR_SESSION_ID:
            if (msg->sessionId) {
                hdrVal.val.v_uint32 = &msg->sessionId;
            }
            break;

        case AJ_HDR_HANDLES:
        case AJ_HDR_COMPRESSION_TOKEN:
        default:
            continue;
        }
        /*
         * Ignore empty fields.
         */
        if (!hdrVal.val.v_data) {
            continue;
        }
        /*
         * Custom marshal the header field - signature is "(yv)" so starts off with STRUCT aligment.
         */
        buf[0] = fieldId;
        buf[1] = 1;
        buf[2] = typeId;
        buf[3] = 0;
        WriteBytes(msg, buf, 4, PadForType(AJ_ARG_STRUCT, ioBuf));
        /*
         * Now marshal the field value
         */
        Marshal(msg, &fieldSig, &hdrVal);
    }
    if (status == AJ_OK) {
        /*
         * Write the header length
         */
        msg->hdr->headerLen = (uint32_t)((ioBuf->writePtr - ioBuf->bufStart) - sizeof(AJ_MsgHeader));
        /*
         * Header must be padded to an 8 byte boundary
         */
        status = WritePad(msg, (8 - msg->hdr->headerLen) & 7);
    }
    return status;
}

AJ_Status AJ_MarshalMsgCustom(AJ_Message* msg, uint8_t msgType, uint8_t flags)
{
    return MarshalMsg(msg, msgType, msg->msgId, flags);
}

AJ_Status AJ_MarshalArg(AJ_Message* msg, AJ_Arg* arg)
{
    AJ_Status status;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.tx;
    uint8_t* argStart = ioBuf->writePtr;

    if (msg->sigOffset == 0xFF) {
        return AJ_ERR_UNEXPECTED;
    }
    if (msg->varOffset) {
        /*
         * Marshaling a variant - get the signature from the I/O buffer
         */
        const char* sig = (const char*)(argStart - msg->varOffset);
        msg->varOffset = 0;
        status = Marshal(msg, &sig, arg);
    } else if (msg->outer) {
        /*
         * Marshaling a component of a container use the container's signature
         */
        const char* sig = msg->outer->sigPtr;
        if (!*sig) {
            AJ_ErrPrintf(("AJ_MarshalArg(): AJ_ERR_END_OF_DATA\n"));
            return AJ_ERR_END_OF_DATA;
        }
        status = Marshal(msg, &sig, arg);
        /*
         * Only advance the signature for struct elements
         */
        if (msg->outer->typeId != AJ_ARG_ARRAY) {
            msg->outer->sigPtr = sig;
        }
    } else {
        const char* sig = msg->signature + msg->sigOffset;
        /*
         * Marshalling anything else use the message signature
         */
        if (!*sig) {
            AJ_ErrPrintf(("AJ_MarshalArg(): AJ_ERR_END_OF_DATA\n"));
            return AJ_ERR_END_OF_DATA;
        }
        status = Marshal(msg, &sig, arg);
        msg->sigOffset = (uint8_t)(sig - msg->signature);
    }
    if (status == AJ_OK) {
        msg->bodyBytes += (uint16_t)(ioBuf->writePtr - argStart);
    } else {
        AJ_ReleaseReplyContext(msg);
    }
    return status;
}

AJ_Arg* AJ_InitArg(AJ_Arg* arg, uint8_t typeId, uint8_t flags, const void* val, size_t len)
{
    if (!IsBasicType(typeId)) {
        memset(arg, 0, sizeof(AJ_Arg));
        return NULL;
    } else {
        arg->typeId = typeId;
        arg->flags = flags;
        arg->len = (uint16_t)len;
        arg->val.v_data = (void*)val;
        arg->sigPtr = NULL;
        arg->container = NULL;
        return arg;
    }
}

static AJ_Status VMarshalArgs(AJ_Message* msg, const char** sig, va_list* argpp)
{
    AJ_Status status = AJ_ERR_UNEXPECTED;
    AJ_Arg arg;
    AJ_Arg container;
    va_list argp;
    __va_copy(argp, *argpp);

    container.typeId = AJ_ARG_INVALID;

    while (**sig) {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        double d;
        uint8_t typeId = (uint8_t)*((*sig)++);
        void* val;

        if (!IsBasicType(typeId)) {
            if ((typeId == AJ_ARG_STRUCT) || (typeId == AJ_ARG_DICT_ENTRY)) {
                status = AJ_MarshalContainer(msg, &container, typeId);
                if (status != AJ_OK) {
                    break;
                }
                status = VMarshalArgs(msg, sig, &argp);
                /*
                 * Upon successful return from a nested call, continue from
                 * where the inner call advanced in the signature.
                 */
                if (status == AJ_OK) {
                    status = AJ_MarshalCloseContainer(msg, &container);
                }
                if (status != AJ_OK) {
                    break;
                }
                continue;
            }
            if ((typeId == AJ_ARG_ARRAY) && IsBasicType(**sig)) {
                const void* aval = va_arg(argp, const void*);
                size_t len = va_arg(argp, size_t);

                AJ_InitArg(&arg, (uint8_t)*((*sig)++), AJ_ARRAY_FLAG, aval, len);
                status = AJ_MarshalArg(msg, &arg);
                continue;
            }
            if ((typeId == AJ_STRUCT_CLOSE) || (typeId == AJ_DICT_ENTRY_CLOSE)) {
                break;
            }
            if (typeId == AJ_ARG_VARIANT) {
                const char* vsig = va_arg(argp, const char*);
                status = AJ_MarshalVariant(msg, vsig);
                if (status == AJ_OK) {
                    status = VMarshalArgs(msg, &vsig, &argp);
                }
                if (status == AJ_OK) {
                    continue;
                }
            }
            status = AJ_ERR_UNEXPECTED;
            break;
        }
        if (IsScalarType(typeId)) {
            if (SizeOfType(typeId) == 8) {
                if (typeId == 'd') {
                    d = va_arg(argp, double);
                    val = &d;
                } else {
                    u64 = va_arg(argp, uint64_t);
                    val = &u64;
                }
            } else if (SizeOfType(typeId) == 4) {
                u32 = va_arg(argp, uint32_t);
                val = &u32;
            } else if (SizeOfType(typeId) == 2) {
                u16 = (uint16_t)va_arg(argp, uint32_t);
                val = &u16;
            } else {
                u8 = (uint8_t)va_arg(argp, uint32_t);
                val = &u8;
            }
        } else {
            val = va_arg(argp, char*);
        }
        InitArg(&arg, typeId, val);
        status = AJ_MarshalArg(msg, &arg);
        if (status != AJ_OK) {
            AJ_ErrPrintf(("AJ_MarshalArgs(): status=%s\n", AJ_StatusText(status)));
            break;
        }
    }
    __va_copy(*argpp, argp);
    return status;
}

AJ_Status AJ_MarshalArgs(AJ_Message* msg, const char* sig, ...)
{
    AJ_Status status;
    va_list argp;

    va_start(argp, sig);
    status = VMarshalArgs(msg, &sig, &argp);
    va_end(argp);

    return status;
}

AJ_Status AJ_DeliverMsgPartial(AJ_Message* msg, uint32_t bytesRemaining)
{
    static AJ_MsgHeader tmpHdr;
    AJ_IOBuffer* ioBuf = &msg->bus->sock.tx;
    uint8_t typeId = msg->signature[msg->sigOffset];
    size_t pad;

    AJ_ASSERT(!msg->outer);

    if (!msg->hdr || !bytesRemaining) {
        AJ_ErrPrintf(("AJ_DeliverMsgPartial(): AJ_ERR_UNEXPECTED\n"));
        return AJ_ERR_UNEXPECTED;
    }
    /*
     * Partial delivery not currently supported for messages that must be encrypted.
     */
    if (msg->hdr->flags & AJ_FLAG_ENCRYPTED) {
        AJ_ErrPrintf(("AJ_DeliverMsgPartial(): AJ_ERR_SECURITY\n"));
        return AJ_ERR_SECURITY;
    }
    /*
     * There must be arguments to marshal
     */
    if (!typeId) {
        AJ_ErrPrintf(("AJ_DeliverMsgPartial(): AJ_ERR_SIGNATURE\n"));
        return AJ_ERR_SIGNATURE;
    }
    /*
     * Pad to the start of the argument.
     */
    pad = PadForType(typeId, ioBuf);
    if (pad) {
        AJ_Status status = WritePad(msg, pad);
        if (status != AJ_OK) {
            AJ_ErrPrintf(("AJ_DeliverMsgPartial(): status=%s\n", AJ_StatusText(status)));
            return status;
        }
    }
    /*
     * Set the body length in the header buffer.
     */
    msg->hdr->bodyLen = (uint32_t)(msg->bodyBytes + pad + bytesRemaining);
    AJ_DumpMsg("SENDING(partial)", msg, FALSE);
    /*
     * The buffer space occupied by the header may be overwritten
     */
    tmpHdr = *msg->hdr;
    msg->hdr = &tmpHdr;
    /*
     * From now on we are going to count down the remaining body bytes
     */
    msg->bodyBytes = (uint32_t)bytesRemaining;
    /*
     * Standard signature matching is now meaningless
     */
    msg->sigOffset = 0xFF;

    return AJ_OK;
}

AJ_Status AJ_MarshalRaw(AJ_Message* msg, const void* data, size_t len)
{
    /*
     * We expect the sigOffset to have been set to 0xFF
     */
    if (msg->sigOffset != 0xFF) {
        AJ_ErrPrintf(("AJ_MarshalRaw(): AJ_ERR_UNEXPECTED\n"));
        return AJ_ERR_UNEXPECTED;
    }
    /*
     * It is a fatal error to write too many bytes
     */
    if (len > msg->bodyBytes) {
        AJ_ErrPrintf(("AJ_MarshalRaw(): AJ_ERR_WRITE\n"));
        return AJ_ERR_WRITE;
    }
    msg->bodyBytes -= (uint32_t)len;
    return WriteBytes(msg, data, len, 0);
}

AJ_Status AJ_MarshalContainer(AJ_Message* msg, AJ_Arg* arg, uint8_t typeId)
{
    AJ_Status status;

    InitArg(arg, typeId, NULL);
    status = AJ_MarshalArg(msg, arg);
    if (status == AJ_OK) {
        arg->container = msg->outer;
        msg->outer = arg;
    }
    return status;
}

AJ_Status AJ_MarshalCloseContainer(AJ_Message* msg, AJ_Arg* arg)
{
    AJ_IOBuffer* ioBuf = &msg->bus->sock.tx;
    AJ_Status status = AJ_OK;

    AJ_ASSERT(TYPE_FLAG(arg->typeId) & AJ_CONTAINER);
    AJ_ASSERT(msg->outer == arg);

    msg->outer = arg->container;

    if (arg->typeId == AJ_ARG_ARRAY) {
        uint32_t lenOffset = (uint32_t)((uint8_t*)arg->val.v_data - ioBuf->bufStart);
        /*
         * The length we marshal does not include the length field itself.
         */
        arg->len = (uint16_t)(ioBuf->writePtr - (uint8_t*)arg->val.v_data) - 4;
        /*
         * If the array element is 8 byte aligned and the array is not empty check if there was
         * padding after the length. The length we marshal should not include the padding.
         */

        if ((ALIGNMENT(*arg->sigPtr) == 8) && !(lenOffset & 4) && arg->len) {
            arg->len -= 4;
        }
        /*
         * Write array length into the buffer
         */
        *(arg->val.v_uint32) = arg->len;
    } else {
        arg->len = 0;
        /*
         * Check the signature is correctly closed.
         */
        if ((arg->typeId == AJ_ARG_STRUCT) && (*arg->sigPtr != AJ_STRUCT_CLOSE)) {
            AJ_ErrPrintf(("AJ_MarshalCloseContainer(): AJ_ERR_SIGNATURE\n"));
            return AJ_ERR_SIGNATURE;
        }
        if ((arg->typeId == AJ_ARG_DICT_ENTRY) && (*arg->sigPtr != AJ_DICT_ENTRY_CLOSE)) {
            AJ_ErrPrintf(("AJ_MarshalCloseContainer(): AJ_ERR_SIGNATURE\n"));
            return AJ_ERR_SIGNATURE;
        }
    }
    return status;
}

AJ_Status AJ_MarshalVariant(AJ_Message* msg, const char* sig)
{
    AJ_Arg arg;

    /*
     * A variant type must be a single complete type
     */
    if (CompleteTypeSigLen(sig) != strlen(sig)) {
        AJ_ErrPrintf(("AJ_MarshalVariant(): AJ_ERR_UNEXPECTED\n"));
        return AJ_ERR_UNEXPECTED;
    }
    InitArg(&arg, AJ_ARG_VARIANT, sig);
    return AJ_MarshalArg(msg, &arg);
}

AJ_Status AJ_MarshalMethodCall(AJ_BusAttachment* bus, AJ_Message* msg, uint32_t msgId, const char* destination, AJ_SessionId sessionId, uint8_t flags, uint32_t timeout)
{
    AJ_Status status;

    memset(msg, 0, sizeof(AJ_Message));
    msg->bus = bus;
    msg->destination = destination;
    msg->sessionId = sessionId;
    msg->ttl = timeout;
    status = MarshalMsg(msg, AJ_MSG_METHOD_CALL, msgId, flags);
    if (status == AJ_OK) {
        status = AJ_AllocReplyContext(msg, timeout);
    }
    return status;
}

AJ_Status AJ_MarshalSignal(AJ_BusAttachment* bus, AJ_Message* msg, uint32_t msgId, const char* destination, AJ_SessionId sessionId, uint8_t flags, uint32_t ttl)
{
    memset(msg, 0, sizeof(AJ_Message));
    msg->bus = bus;
    msg->destination = destination;
    msg->sessionId = sessionId;
    msg->ttl = ttl;
    return MarshalMsg(msg, AJ_MSG_SIGNAL, msgId, flags);
}

AJ_Status AJ_MarshalReplyMsg(const AJ_Message* methodCall, AJ_Message* reply)
{
    AJ_ASSERT(methodCall->hdr->msgType == AJ_MSG_METHOD_CALL);
    memset(reply, 0, sizeof(AJ_Message));
    reply->bus = methodCall->bus;
    reply->destination = methodCall->sender;
    reply->sessionId = methodCall->sessionId;
    reply->replySerial = methodCall->hdr->serialNum;
    return MarshalMsg(reply, AJ_MSG_METHOD_RET, methodCall->msgId, methodCall->hdr->flags & AJ_FLAG_ENCRYPTED);
}

AJ_Status AJ_MarshalErrorMsgWithInfo(const AJ_Message* methodCall, AJ_Message* reply, const char* error, const char* info)
{
    AJ_Status status;

    AJ_ASSERT(methodCall->hdr->msgType == AJ_MSG_METHOD_CALL);
    memset(reply, 0, sizeof(AJ_Message));
    reply->bus = methodCall->bus;
    reply->destination = methodCall->sender;
    reply->sessionId = methodCall->sessionId;
    reply->replySerial = methodCall->hdr->serialNum;
    reply->error = error;
    if (info) {
        reply->signature = "s";
    }
    status = MarshalMsg(reply, AJ_MSG_ERROR, methodCall->msgId, methodCall->hdr->flags & AJ_FLAG_ENCRYPTED);
    if ((status == AJ_OK) && info) {
        status = AJ_MarshalArgs(reply, "s", info);
    }
    return status;
}

AJ_Status AJ_MarshalErrorMsg(const AJ_Message* methodCall, AJ_Message* reply, const char* error)
{
    return AJ_MarshalErrorMsgWithInfo(methodCall, reply, error, NULL);
}


static const char* StatusToErrorStrings(AJ_Status status, const char** info)
{
    switch (status) {
    case AJ_ERR_NO_MATCH:
        *info = NULL;
        return AJ_ErrServiceUnknown;

    case AJ_ERR_SECURITY:
        *info = NULL;
        return AJ_ErrSecurityViolation;

    case  AJ_ERR_ACCESS:
        *info = NULL;
        return AJ_ErrPermissionDenied;

    case  AJ_ERR_SECURITY_DIGEST_MISMATCH:
        *info = NULL;
        return AJ_ErrDigestMismatch;

    case  AJ_ERR_SECURITY_INVALID_CERTIFICATE:
        *info = NULL;
        return AJ_ErrInvalidCertificate;

    case  AJ_ERR_SECURITY_DUPLICATE_CERTIFICATE:
        *info = NULL;
        return AJ_ErrDuplicateCertificate;

    case  AJ_ERR_SECURITY_CERTIFICATE_NOT_FOUND:
        *info = NULL;
        return AJ_ErrCertificateNotFound;

    case  AJ_ERR_SECURITY_POLICY_NOT_NEWER:
        *info = NULL;
        return AJ_ErrPolicyNotNewer;

    default:
        *info = AJ_StatusText(status);
        return AJ_ErrRejected;
    }
}

AJ_Status AJ_MarshalStatusMsg(const AJ_Message* methodCall, AJ_Message* reply, AJ_Status status)
{
    const char* info;
    const char* err = StatusToErrorStrings(status, &info);
    return AJ_MarshalErrorMsgWithInfo(methodCall, reply, err, info);
}

AJ_Status AJ_MarshalReplyMsgAsync(AJ_MsgReplyContext* replyCtx, AJ_Message* reply)
{
    AJ_Status status;

    if (!replyCtx) {
        return AJ_ERR_NULL;
    }
    if (!replyCtx->msgId) {
        return AJ_ERR_INVALID;
    }
    memset(reply, 0, sizeof(AJ_Message));
    reply->bus = replyCtx->bus;
    reply->destination = replyCtx->sender;
    reply->sessionId = replyCtx->sessionId;
    reply->replySerial = replyCtx->serialNum;
    status = MarshalMsg(reply, AJ_MSG_METHOD_RET, replyCtx->msgId, replyCtx->flags & AJ_FLAG_ENCRYPTED);
    /*
     * Ensure reply context is only be used once
     */
    memset(replyCtx, 0, sizeof(*replyCtx));
    return status;
}

AJ_Status AJ_MarshalErrorMsgAsync(AJ_MsgReplyContext* replyCtx, AJ_Message* reply, const char* error)
{
    return AJ_MarshalErrorMsgWithInfoAsync(replyCtx, reply, error, NULL);
}

AJ_Status AJ_MarshalErrorMsgWithInfoAsync(AJ_MsgReplyContext* replyCtx, AJ_Message* reply, const char* error, const char* info)
{
    AJ_Status status;

    if (!replyCtx) {
        return AJ_ERR_NULL;
    }
    if (!replyCtx->msgId) {
        return AJ_ERR_INVALID;
    }
    memset(reply, 0, sizeof(AJ_Message));
    reply->bus = replyCtx->bus;
    reply->destination = replyCtx->sender;
    reply->sessionId = replyCtx->sessionId;
    reply->replySerial = replyCtx->serialNum;
    reply->error = error;
    if (info) {
        reply->signature = "s";
    }
    status = MarshalMsg(reply, AJ_MSG_ERROR, replyCtx->msgId, replyCtx->flags & AJ_FLAG_ENCRYPTED);
    if ((status == AJ_OK) && info) {
        status = AJ_MarshalArgs(reply, "s", info);
    }
    /*
     * Ensure reply context is only be used once
     */
    memset(replyCtx, 0, sizeof(*replyCtx));
    return status;
}

AJ_Status AJ_MarshalStatusMsgAsync(AJ_MsgReplyContext* replyCtx, AJ_Message* reply, AJ_Status status)
{
    const char* info;
    const char* err = StatusToErrorStrings(status, &info);
    return AJ_MarshalErrorMsgWithInfoAsync(replyCtx, reply, err, info);
}
