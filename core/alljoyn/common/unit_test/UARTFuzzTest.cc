/******************************************************************************
 *
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
#include <gtest/gtest.h>

#include <Status.h>
#include <qcc/Util.h>
#include <qcc/UARTStream.h>
#include <qcc/SLAPStream.h>
#include <qcc/SLAPPacket.h>
#define QCC_MODULE "TEST"
#define PACKET_SIZE 100
#define WINDOW_SIZE 4
#define BAUDRATE    115200
#define RECOVERABLE_ERRORS 1
#define UNRECOVERABLE_ERRORS 0
using namespace qcc;
/*
 * link establishment packets
 */
static const char* LinkCtrlPacketNames[5] = { "NONE", "CONN", "ACPT", "NEGO", "NRSP" };

const uint8_t BOUNDARY_BYTE         = 0xC0;
const uint8_t BOUNDARY_SUBSTITUTE   = 0xDC;
const uint8_t ESCAPE_BYTE           = 0xDB;
const uint8_t ESCAPE_SUBSTITUTE     = 0xDD;

/**
 * For every byte in the buffer buf,
 * with a P(percent/100) chance, write a random byte
 */
void RandFuzzing(uint8_t* buf, uint32_t len, uint8_t percent)
{
    uint32_t pos = 0;
    uint8_t* p = (uint8_t*)buf;
    while (pos++ < len) {
        uint8_t roll = qcc::Rand8();
        if (percent > ((100 * roll) / 256)) {
            *p = qcc::Rand8();
        }
        ++p;
    }
}

#define CRC_INIT  0xFFFF
static void CRC16_Complete(uint16_t crc,
                           uint8_t* crcBlock)
{
    static const uint8_t rev[] = { 0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf };
    crcBlock[0] = (uint8_t) (rev[crc & 0xF] << 4) | rev[(crc >> 4) & 0xF];
    crcBlock[1] = (uint8_t) (rev[(crc >> 8) & 0xF] << 4) | rev[crc >> 12];
}
class SLAPFuzzWritePacket {
  public:
    SLAPFuzzWritePacket(uint8_t len) :   m_maxPacketSize(len), m_ackNum(0), m_sequenceNum(0),
        m_payloadBuffer(new uint8_t[len]),
        m_payloadLen(0),
        m_buffer(new uint8_t[SLAP_SLIPPED_LENGTH(len)]),
        m_bufEOD(m_buffer), m_startPos(0),
        m_writePtr(m_buffer), m_slippedLen(0),
        m_endPos(0),
        m_pktType(INVALID_PACKET) { }
    void CopyFromReadPacket(uint8_t len, SLAPReadPacket* readPkt) {
        {
            uint8_t* buffer = new uint8_t[len];
            size_t actual = 0;
            size_t temp;
            uint8_t configField[3];
            switch (readPkt->GetPacketType()) {
            case RELIABLE_DATA_PACKET:
                readPkt->FillBuffer(buffer, len, actual);
                DataPacket(buffer, actual, temp);
                SetAck(readPkt->GetAckNum());
                SetSeqNum(readPkt->GetSeqNum());
                break;

            case ACK_PACKET:
                AckPacket();
                SetAck(readPkt->GetAckNum());
                break;

            case CTRL_PACKET:
                configField[0] = readPkt->GetConfigField(0);
                configField[1] = readPkt->GetConfigField(1);
                configField[2] = readPkt->GetConfigField(2);
                ControlPacket(readPkt->GetControlType(), configField);
                break;

            default:
                break;
            }
            delete [] buffer;
        }

    }
    void FuzzAck(uint8_t ack) {
        SetAck(ack);
        PrependHeader();
    }
    void FuzzSeq(uint8_t seq) {
        SetSeqNum(seq);
        PrependHeader();
    }
    void FuzzLen(uint16_t payloadLen) {
        m_payloadLen = payloadLen;
        PrependHeader();
    }
    void FuzzPktType(uint8_t pktType) {
        m_pktType = (PacketType)pktType;
        PrependHeader();
    }

    /** Destructor */
    ~SLAPFuzzWritePacket() {
        delete [] m_buffer;
        delete [] m_payloadBuffer;
    }
    /**
     * Deliver this packet to a link.
     * @param link	The link to deliver this packet to.
     */
    QStatus Deliver(Stream* link);

    void CopyToBuffer(uint8_t* buffer, size_t& len) {
        len = m_bufEOD - m_writePtr + 1;
        memcpy(buffer, m_writePtr, m_bufEOD - m_writePtr + 1);
    }

    /**
     * Get the packet type
     * @return The packet type of this packet. One of enum PacketType values.
     *
     */
    uint8_t GetPacketType() { return m_pktType; };

    /**
     * Get the sequence number in this packet
     * @return          The sequence number.
     */
    uint8_t GetSeqNum() { return m_sequenceNum; };

    /**
     * Get the Ack number in this packet
     * @return The Ack number.
     */
    uint8_t GetAckNum() { return m_ackNum; };

  private:

    /**
     * Construct the SLAPFuzzWritePacket(Data).
     * @param buf          Input buffer
     * @param num          Length of Input buffer
     * @param numSent[out] Number of bytes that were actually copied in
     * @return
     *
     */
    void DataPacket(const void* buf, size_t num, size_t& numSent) {
        m_pktType = RELIABLE_DATA_PACKET;
        m_payloadLen = (num < m_maxPacketSize) ? num : m_maxPacketSize;
        num -= m_payloadLen;
        memcpy(m_payloadBuffer, buf, m_payloadLen);
        numSent  = m_payloadLen;
        SlipPayload();
    };

    /**
     * Construct the SLAPFuzzWritePacket(Control).
     * @param type ControlPacketType to use
     * @param val Optional field value (Used in case of CONF_PKT)
     *
     */
    void ControlPacket(ControlPacketType type, uint8_t* configField) {
        m_pktType = CTRL_PACKET;
        m_payloadLen = 4;
        memcpy(m_payloadBuffer, LinkCtrlPacketNames[type], 4);

        if (type == NEGO_PKT || type == NEGO_RESP_PKT) {
            assert(configField);
            memcpy(&m_payloadBuffer[4], configField, 3);
            m_payloadLen += 3;

        }
        SlipPayload();
    }


    /**
     * Construct the SLAPFuzzWritePacket(Ack).
     */
    void AckPacket() {
        m_payloadLen = 0;
        m_pktType = ACK_PACKET;
        SlipPayload();

    };


    /**
     * Prepend the header to the data/control/ack packet.
     */
    QStatus PrependHeader();

    /**
     * Set the sequence number in this packet
     * @param           The sequence number.
     */
    void SetSeqNum(uint8_t seq) { m_sequenceNum  = seq; };

    /**
     * Set the Ack number in this packet
     * @param           The ack number.
     */
    void SetAck(uint8_t num) { m_ackNum = num; };


    /**
     * Slip the payload bytes into the buffer to send
     */
    void SlipPayload() {
        m_slippedLen = SLAP_PAYLOAD_START_POS;
        for (int i = 0; i < m_payloadLen; i++) {
            uint8_t b = m_payloadBuffer[i];
            switch (b) {
            case BOUNDARY_BYTE:
                m_buffer[m_slippedLen++] = ESCAPE_BYTE;
                m_buffer[m_slippedLen++] = BOUNDARY_SUBSTITUTE;
                break;

            case ESCAPE_BYTE:
                m_buffer[m_slippedLen++] = ESCAPE_BYTE;
                m_buffer[m_slippedLen++] = ESCAPE_SUBSTITUTE;
                break;

            default:
                m_buffer[m_slippedLen++] = b;
                break;
            }
        }
    }

  private:

    /** Private default constructor - does nothing */
    SLAPFuzzWritePacket();

    /**
     * Private Copy-constructor - does nothing
     *
     * @param other  SLAPFuzzWritePacket to copy from.
     */
    SLAPFuzzWritePacket(const SLAPFuzzWritePacket& other);

    /**
     * Private Assignment operator - does nothing.
     *
     * @param other  SLAPFuzzWritePacket to assign from.
     */
    SLAPFuzzWritePacket operator=(const SLAPFuzzWritePacket& other) { return *this; };

    size_t m_maxPacketSize;               /**< Maximum packet size supported. */

    uint8_t m_ackNum;                     /**< Acknowledge number contained in this packet */
    uint8_t m_sequenceNum;                /**< Sequence number contained in this packet */

    uint8_t* m_payloadBuffer;             /**< payload from app */
    uint16_t m_payloadLen;                /**< payload length */

    uint8_t* m_buffer;                    /**< slipped bytes */
    uint8_t* m_bufEOD;
    uint16_t m_startPos;                  /**< Starting position in the slipped buffer */
    uint8_t* m_writePtr;                  /**< Pointer to the current write position in the buffer. */
    uint16_t m_slippedLen;                /**< length of slipped bytes buffer */
    uint16_t m_endPos;                    /**< length of slipped bytes buffer */
    PacketType m_pktType;                 /**< Type of this SLAPFuzzWritePacket */

};

QStatus SLAPFuzzWritePacket::Deliver(Stream* link)
{
    size_t actual;
    QStatus status = link->PushBytes(m_writePtr, m_bufEOD - m_writePtr + 1, actual);
    m_writePtr += actual;
    return status;
}

QStatus SLAPFuzzWritePacket::PrependHeader()
{
    uint8_t header[4];

    header[0] = ((m_pktType == RELIABLE_DATA_PACKET) ? (m_sequenceNum << 4) : 0x00);
    header[0] |= (m_pktType != CTRL_PACKET) ? m_ackNum : 0x00;

    /* Flow off is for future use. */
    header[1] = m_pktType;

    /*
     * high-order 8 bits of packet size
     */
    header[2] = (m_payloadLen >> 8);
    /*
     * low-order 8 bits of packet size
     */
    header[3] = (m_payloadLen & 0xFF);

    m_startPos = SLAP_PAYLOAD_START_POS - 1;
    for (int i = 3; i >= 0; i--) {
        switch (header[i]) {
        case BOUNDARY_BYTE:
            m_buffer[m_startPos--] = BOUNDARY_SUBSTITUTE;
            m_buffer[m_startPos--] = ESCAPE_BYTE;
            break;

        case ESCAPE_BYTE:
            m_buffer[m_startPos--] = ESCAPE_SUBSTITUTE;
            m_buffer[m_startPos--] = ESCAPE_BYTE;
            break;

        default:
            m_buffer[m_startPos--] = header[i];
            break;
        }

    }
    m_endPos = m_slippedLen;

    /* Add CRC to every packet */
    uint16_t crc = CRC_INIT;
    CRC16_Compute(&header[0], 4, &crc);
    CRC16_Compute(m_payloadBuffer, m_payloadLen, &crc);
    uint8_t checkCrc[2];
    CRC16_Complete(crc, checkCrc);
    for (int i = 0; i < 2; i++) {
        switch (checkCrc[i]) {
        case BOUNDARY_BYTE:
            m_buffer[m_endPos++] = ESCAPE_BYTE;
            m_buffer[m_endPos++] = BOUNDARY_SUBSTITUTE;
            break;

        case ESCAPE_BYTE:
            m_buffer[m_endPos++] = ESCAPE_BYTE;
            m_buffer[m_endPos++] = ESCAPE_SUBSTITUTE;
            break;

        default:
            m_buffer[m_endPos++] = checkCrc[i];
            break;
        }

    }

    m_buffer[m_startPos] = BOUNDARY_BYTE;
    m_buffer[m_endPos] = BOUNDARY_BYTE;
    m_writePtr = &m_buffer[m_startPos];
    m_bufEOD = &m_buffer[m_endPos];
    return ER_OK;
}
/**
 * Given a buffer of fixed length, randomly pick a way to corrupt the data (or not.)
 */
void FuzzBuffer(Stream* link, uint8_t* buf, size_t& len, uint8_t recoverable_errors)
{
    size_t offset;
    uint8_t test;
    if (recoverable_errors) {
        test = qcc::Rand8() % 20;
    } else {
        test = qcc::Rand8() % 32;
    }

    size_t temp = len;
    uint8_t* bufPtr = buf;
    SLAPReadPacket readPkt(temp); //this is more than required but ok.
    assert(len >= 4);
    SLAPFuzzWritePacket writeFuzzPkt(len);
    //Calculate the offset of CRC.
    size_t offsetCRC = len - 3;
    for (size_t i = len - 2; i >= offsetCRC; i--) {
        if (buf[i] == 0xDC || buf[i] == 0xDD) {
            //escape substitute or boundary substitute.
            offsetCRC--;
        }
    }
    readPkt.DeSlip(bufPtr, temp);
    readPkt.Validate();
    assert(readPkt.GetPacketType() != -1);
    writeFuzzPkt.CopyFromReadPacket(len, &readPkt);

    switch (test) {
    case 0:
        /* Fuzz CRC */
        RandFuzzing(buf + offsetCRC, len - offsetCRC, 5);
        break;


    case 1:
        /*
         * Protect fixed header from fuzzing. i.e. fuzz payload/CRC.
         */
        offset = SLAP_HDR_LEN + 1; //1 for boundary byte
        for (size_t i = 0; i < offset; i++) {
            if (buf[i] == 0xDB) {
                //escape byte
                offset++;
            }
        }
        RandFuzzing(buf + offset, len - offset, 5);
        break;


    case 2:
        /*
         * fuzz the payload
         */

        offset = SLAP_HDR_LEN + 1;
        for (size_t i = 0; i < offset; i++) {
            if (buf[i] == 0xDB) {
                //escape byte
                offset++;
            }
        }
        if (offsetCRC < offset) {
            //Payload length is zero
            break;
        }
        RandFuzzing(buf + offset, offsetCRC - offset, 10);
        break;

    case 3:
        /*
         * Fuzz the entire message
         */
        RandFuzzing(buf, len, 1 + (qcc::Rand8() % 10));
        break;

    case 4:
        // randomly insert an escape character
        buf[qcc::Rand16() % len ] = 0xDB;
        break;

    case 5:
        // randomly insert an boundary character
        buf[qcc::Rand16() % len ] = 0xC0;
        break;

    case 6:
        // randomly insert an boundary character
        buf[qcc::Rand16() % len ] = 0xDD;
        break;

    case 7:
        // randomly insert an boundary character
        buf[qcc::Rand16() % len ] = 0xDC;
        break;


    case 20:
        /*
         * change the sequence number
         */
        writeFuzzPkt.FuzzSeq(qcc::Rand8() % 16);
        writeFuzzPkt.CopyToBuffer(buf, len);
        break;

    case 21:
        /*
         * change the ack number
         */
        writeFuzzPkt.FuzzAck(qcc::Rand8() % 16);
        writeFuzzPkt.CopyToBuffer(buf, len);
        break;

    case 22:
        /*
         * change the type field of the packet
         */
        writeFuzzPkt.FuzzPktType(qcc::Rand8() % 16);
        writeFuzzPkt.CopyToBuffer(buf, len);
        break;

    case 23:
        /*
         * change the length field of the packet
         */
        writeFuzzPkt.FuzzPktType(qcc::Rand16() % 65536);
        writeFuzzPkt.CopyToBuffer(buf, len);
        break;


//    case 24: // not ready yet.
//        /*
//         * Protect Negotiate packet header from fuzzing
//         */
//        offset = sizeof(PacketNegotiate);
//        RandFuzzing(buf + offset, len - offset, 5);
//        break;

    default:
        /*
         * don't fuzz anything
         */
        break;
    }
}
class MyUARTStream : public UARTStream {
  public:
    MyUARTStream(UARTFd fd, uint8_t recoverable_errors) : UARTStream(fd), recoverable_errors(recoverable_errors) { };
    QStatus PullBytes(void* buf, size_t numBytes, size_t& actualBytes, uint32_t timeout) {
        return UARTStream::PullBytes(buf, numBytes, actualBytes, timeout);
    }
    QStatus PushBytes(const void* buf, size_t numBytes, size_t& actualBytes) {
        assert(numBytes > 0);
        /* Fuzz the data */
        uint8_t* bufPtr = new uint8_t[numBytes + 10];
        memcpy(bufPtr, buf, numBytes);
        FuzzBuffer(this, bufPtr, numBytes, recoverable_errors);
        UARTStream::PushBytes(bufPtr, numBytes, actualBytes);
        delete [] bufPtr;
        return ER_OK;
    }
    Event& GetSourceEvent() { return UARTStream::GetSourceEvent(); }

    /**
     * Get the Event indicating that sink can accept data.
     *
     * @return Event set when socket can accept more data via PushBytes
     */
    virtual Event& GetSinkEvent() { return UARTStream::GetSinkEvent(); }
  private:
    MyUARTStream();
    Stream* stream;
    uint8_t recoverable_errors;

};

class Ep {
  public:
    Ep(UARTFd fd, IODispatch& iodisp, int packetSize, uint8_t recoverable_errors) :
        m_timer("SLAPEp", true, 1, false, 10),
        m_rawStream(fd, recoverable_errors),
        m_stream(&m_uartController, m_timer, packetSize, WINDOW_SIZE, BAUDRATE),
        m_uartController(&m_rawStream, iodisp, &m_stream)
    {
        m_timer.Start();
        m_uartController.Start();
    }
    ~Ep() {
        Stop();
        Join();
        m_stream.Close();
    }
    QStatus Stop() {
        QStatus status = m_timer.Stop();
        QStatus status1 = m_uartController.Stop();
        return (status != ER_OK) ? status : status1;

    }

    QStatus Join() {
        QStatus status = m_timer.Join();
        QStatus status1 = m_uartController.Join();
        return (status != ER_OK) ? status : status1;
    }
    Timer m_timer;                    /**< Multipurpose timer for sending/resend/acks */
    MyUARTStream m_rawStream;         /**< The raw UART stream */
    SLAPStream m_stream;              /**< The SLAP stream used for Alljoyn communication */
    UARTController m_uartController;  /**< Controller responsible for reading from UART */
};

TEST(UARTFuzzTest, DISABLED_uart_fuzz_test_recoverable)
{
    IODispatch iodisp("iodisp", 4);
    iodisp.Start();
    //This test introduces some recoverable fuzzing errors in the sent packets.
    uint8_t rxBuffer[1600];
    memset(&rxBuffer, 'R', sizeof(rxBuffer));

    uint8_t txBuffer[1600];
    memset(&txBuffer, 'T', sizeof(txBuffer));

    int blocksize = 100;

    for (int blocks = 0; blocks < 16; blocks++) {
        memset(txBuffer + (blocks * blocksize), 0x41 + (uint8_t)blocks, blocksize);
    }

    size_t x;
    UARTFd fd0;
    QStatus status = UART("/tmp/COM0", BAUDRATE, fd0);
    EXPECT_EQ(status, ER_OK);

    UARTFd fd1;
    status = UART("/tmp/COM1", BAUDRATE, fd1);
    EXPECT_EQ(status, ER_OK);

    Ep ep0(fd0, iodisp, 800, RECOVERABLE_ERRORS);
    Ep ep1(fd1, iodisp, 1000, RECOVERABLE_ERRORS);
    ep0.m_stream.ScheduleLinkControlPacket();
    ep1.m_stream.ScheduleLinkControlPacket();

    for (int iter = 0; iter < 2000; iter++) {
        printf("Iteration %d\n", iter);
        ep1.m_stream.PushBytes(txBuffer, sizeof(txBuffer), x);
        EXPECT_EQ(1600U, x);
        ep0.m_stream.PullBytes(rxBuffer, sizeof(rxBuffer), x);
        ASSERT_EQ(1600U, x);
        if (memcmp(txBuffer, rxBuffer, sizeof(txBuffer)) != 0) {
            printf("Failed Iteration %d\n", iter);
            for (size_t ii = 0; ii < sizeof(rxBuffer); ii++) {
                if (txBuffer[ii] != rxBuffer[ii]) {
                    printf("%zu %X %X\n", ii, txBuffer[ii], rxBuffer[ii]);
                }
            }
            exit(-1);
        }

    }
    /* Wait for retransmission to finish */
    qcc::Sleep(4000);
    ep0.Stop();
    ep1.Stop();
    iodisp.Stop();

    ep0.Join();
    ep1.Join();
    iodisp.Join();
}

TEST(UARTFuzzTest, DISABLED_uart_fuzz_test_unrecoverable)
{
    IODispatch iodisp("iodisp", 4);
    iodisp.Start();
    //This test introduces some unrecoverable fuzzing errors in the sent packets.
    //This is just to make sure that the program doesnt crash.

    uint8_t rxBuffer[1600];
    memset(&rxBuffer, 'R', sizeof(rxBuffer));

    uint8_t txBuffer[1600];
    memset(&txBuffer, 'T', sizeof(txBuffer));

    int blocksize = 100;

    for (int blocks = 0; blocks < 16; blocks++) {
        memset(txBuffer + (blocks * blocksize), 0x41 + (uint8_t)blocks, blocksize);
    }

    size_t x;
    UARTFd fd0;
    QStatus status = UART("/tmp/COM0", BAUDRATE, fd0);
    EXPECT_EQ(status, ER_OK);

    UARTFd fd1;
    status = UART("/tmp/COM1", BAUDRATE, fd1);
    EXPECT_EQ(status, ER_OK);

    Ep ep0(fd0, iodisp, 1600, UNRECOVERABLE_ERRORS);
    Ep ep1(fd1, iodisp, 1600, UNRECOVERABLE_ERRORS);
    ep0.m_stream.ScheduleLinkControlPacket();
    ep1.m_stream.ScheduleLinkControlPacket();

    for (int iter = 0; iter < 1000; iter++) {
        printf("Iteration %d\n", iter);
        ep1.m_stream.PushBytes(txBuffer, sizeof(txBuffer), x);
        ep0.m_stream.PullBytes(rxBuffer, sizeof(rxBuffer), x, 5000);
    }
    /* Wait for retransmission to finish */
    qcc::Sleep(4000);
    ep0.Stop();
    ep1.Stop();
    iodisp.Stop();

    ep0.Join();
    ep1.Join();
    iodisp.Join();
}


