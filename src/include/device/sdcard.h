#ifndef _SDCARD_H_
#define _SDCARD_H_

#include <device/disk.h>

// mostly from https://github.com/ryanj1234/SD_TUTORIAL_PART4

/*
 * CS       PB5
 * SCK      PB4
 * MOSI     PB7
 * MISO     PB6
 * 
 */

#define CMD0                0
#define CMD0_ARG            0x00000000
#define CMD0_CRC            0x94
#define CMD8                8
#define CMD8_ARG            0x0000001AA
#define CMD8_CRC            0x86
#define CMD9                9
#define CMD9_ARG            0x00000000
#define CMD9_CRC            0x00
#define CMD10               9
#define CMD10_ARG           0x00000000
#define CMD10_CRC           0x00
#define CMD13               13
#define CMD13_ARG           0x00000000
#define CMD13_CRC           0x00
#define CMD17               17
#define CMD17_CRC           0x00
#define CMD24               24
#define CMD24_CRC           0x00
#define CMD55               55
#define CMD55_ARG           0x00000000
#define CMD55_CRC           0x00
#define CMD58               58
#define CMD58_ARG           0x00000000
#define CMD58_CRC           0x00
#define ACMD41              41
#define ACMD41_ARG          0x40000000
#define ACMD41_CRC          0x00

#define SD_IN_IDLE_STATE    0x01
#define SD_READY            0x00
#define SD_R1_NO_ERROR(X)   X < 0x02

#define R3_BYTES            4
#define R7_BYTES            4

#define CMD0_MAX_ATTEMPTS       255
#define CMD55_MAX_ATTEMPTS      255
#define SD_ERROR                1
#define SD_SUCCESS              0
#define SD_MAX_READ_ATTEMPTS    1563
#define SD_READ_START_TOKEN     0xFE
#define SD_INIT_CYCLES          80

#define SD_START_TOKEN          0xFE
#define SD_ERROR_TOKEN          0x00

#define SD_DATA_ACCEPTED        0x05
#define SD_DATA_REJECTED_CRC    0x0B
#define SD_DATA_REJECTED_WRITE  0x0D

#define SD_BLOCK_LEN            512


/* R1 MACROS */
#define PARAM_ERROR(X)      X & 0b01000000
#define ADDR_ERROR(X)       X & 0b00100000
#define ERASE_SEQ_ERROR(X)  X & 0b00010000
#define CRC_ERROR(X)        X & 0b00001000
#define ILLEGAL_CMD(X)      X & 0b00000100
#define ERASE_RESET(X)      X & 0b00000010
#define IN_IDLE(X)          X & 0b00000001

/* R2 MACROS */
#define OUT_OF_RANGE(X)     X & 0b10000000
#define ERASE_PARAM(X)      X & 0b01000000
#define WP_VIOLATION(X)     X & 0b00100000
#define CARD_ECC_FAILED(X)  X & 0b00010000
#define CC_ERROR(X)         X & 0b00001000
#define ERROR(X)            X & 0b00000100
#define WP_ERASE_SKIP(X)    X & 0b00000010
#define CARD_LOCKED(X)      X & 0b00000001

/* R3 MACROS */
#define POWER_UP_STATUS(X)  X & 0x40
#define CCS_VAL(X)          X & 0x40
#define VDD_2728(X)         X & 0b10000000
#define VDD_2829(X)         X & 0b00000001
#define VDD_2930(X)         X & 0b00000010
#define VDD_3031(X)         X & 0b00000100
#define VDD_3132(X)         X & 0b00001000
#define VDD_3233(X)         X & 0b00010000
#define VDD_3334(X)         X & 0b00100000
#define VDD_3435(X)         X & 0b01000000
#define VDD_3536(X)         X & 0b10000000

/* R7 MACROS */
#define CMD_VER(X)          ((X >> 4) & 0xF0)
#define VOL_ACC(X)          (X & 0x1F)
#define VOLTAGE_ACC_27_33   0b00000001
#define VOLTAGE_ACC_LOW     0b00000010
#define VOLTAGE_ACC_RES1    0b00000100
#define VOLTAGE_ACC_RES2    0b00001000

/* DATA ERROR TOKEN */
#define SD_TOKEN_OOR(X)     X & 0b00001000
#define SD_TOKEN_CECC(X)    X & 0b00000100
#define SD_TOKEN_CC(X)      X & 0b00000010
#define SD_TOKEN_ERROR(X)   X & 0b00000001

class SDCard : public Disk{
    public:
        SDCard();
        ~SDCard(){}
        void read_blk(kernel::u8* buf, kernel::u8* adr) override;
        void write_blk(kernel::u8* buf, kernel::u8* adr) override;
    private:
};

#endif
