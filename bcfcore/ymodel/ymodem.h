#ifndef YMODEM_H
#define YMODEM_H

#include <stdint.h>

#define YMODEM_PACKET_HEADER    (3)
#define YMODEM_PACKET_TRAILER   (2)
#define YMODEM_PACKET_OVERHEAD  (YMODEM_PACKET_HEADER + YMODEM_PACKET_TRAILER)
#define YMODEM_PACKET_SIZE      (128)
#define YMODEM_PACKET_1K_SIZE   (1024)

#define YMODEM_CODE_CAN_NUMBER  (5)

class YModem
{
public:
    enum Code {
        CodeNone = 0x00,
        CodeSoh  = 0x01,
        CodeStx  = 0x02,
        CodeEot  = 0x04,
        CodeAck  = 0x06,
        CodeNak  = 0x15,
        CodeCan  = 0x18,
        CodeC    = 0x43,
        CodeA1   = 0x41,
        CodeA2   = 0x61
    };

    enum Stage {
        StageNone,
        StageEstablishing,
        StageEstablished,
        StageTransmitting,
        StageFinishing,
        StageFinished
    };

    enum Status {
        StatusEstablishing,
        StatusEstablish,
        StatusTransmit,
        StatusFinish,
        StatusAbort,
        StatusTimeout,
        StatusError
    };

    YModem(uint32_t timeDivide = 499, uint32_t timeMax = 5, uint32_t errorMax = 999);

    void setTimeDivide(uint32_t timeDivide);
    uint32_t getTimeDivide();

    void setTimeMax(uint32_t timeMax);
    uint32_t getTimeMax();

    void setErrorMax(uint32_t errorMax);
    uint32_t getErrorMax();

    void receive();
    void transmit();
    void abort();

private:
    Code receivePacket();

    void receiveStageNone();
    void receiveStageEstablishing();
    void receiveStageEstablished();
    void receiveStageTransmitting();
    void receiveStageFinishing();
    void receiveStageFinished();

    void transmitStageNone();
    void transmitStageEstablishing();
    void transmitStageEstablished();
    void transmitStageTransmitting();
    void transmitStageFinishing();
    void transmitStageFinished();

    uint16_t crc16(uint8_t* buff, uint32_t len);

    virtual uint32_t callback(Status status, uint8_t* buff, uint32_t* len) = 0;

    virtual uint32_t read(uint8_t* buff, uint32_t len)  = 0;
    virtual uint32_t write(uint8_t* buff, uint32_t len) = 0;

    uint32_t timeDivide;
    uint32_t timeMax;
    uint32_t errorMax;

    uint32_t timeCount;
    uint32_t errorCount;
    uint8_t  dataCount;

    Code  code;
    Stage stage;

    uint8_t  rxBuffer[YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD];
    uint8_t  txBuffer[YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD];
    uint32_t rxLength;
    uint32_t txLength;
};

#endif
