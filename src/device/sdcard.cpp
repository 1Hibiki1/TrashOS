#include <device/sdcard.h>
#include <cstdint>
#include <cpu/tm4c.h>
#include <debug/debug.h>
#include <kernel/kernel.h>

/*
 * TODO:
 * somehow, SSI comm causes random interrupts on PD1...
 * look at device/keyboard.cpp for details
 * THIS NEEDS TO BE FIXED
 * 
 * The problem is magically gone, there's no need to fix it lol
 */

uint8_t SD_init(void);
void SD_powerUpSeq(void);
void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t SD_readRes1(void);
void SD_readRes2(uint8_t *res);
void SD_readRes3(uint8_t *res);
void SD_readRes7(uint8_t *res);
void SD_readBytes(uint8_t *res, uint8_t n);
uint8_t SD_goIdleState(void);
void SD_sendIfCond(uint8_t *res);
void SD_sendStatus(uint8_t *res);
void SD_readOCR(uint8_t *res);
uint8_t SD_sendApp(void);
uint8_t SD_sendOpCond(void);
uint8_t SD_readSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *error);
uint8_t SD_writeSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *res);

void SD_printR1(uint8_t res);
void SD_printR2(uint8_t *res);
void SD_printR3(uint8_t *res);
void SD_printR7(uint8_t *res);
void SD_printBuf(uint8_t *buf);
void SD_printDataErrToken(uint8_t token);


#define CS_ENABLE()     GPIO_PORTB_DATA_R&=~0x20
#define CS_DISABLE()    GPIO_PORTB_DATA_R|=0x20


SDCard::SDCard()
:   Disk(SD_BLOCK_LEN)
{
    kernel::size_t delay;
    (void)delay;

	SYSCTL_RCGCSSI_R|=SYSCTL_RCGCSSI_R2;	// activate clock for SSI2
    // SYSCTL_RCGC2_R |= 0x00000002;   //  activate clock for Port B
    SYSCTL_RCGCGPIO_R |= 0x2;
	delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
	GPIO_PORTB_LOCK_R=0x4C4F434B;
	GPIO_PORTB_CR_R = 0xff;
	GPIO_PORTB_AMSEL_R &= ~0xFC;          // disable analog functionality from PB2 to PB7
	GPIO_PORTB_AFSEL_R |= 0xD0;           // enable alt funct on PB4,PB6 and PB7
	GPIO_PORTB_DEN_R |= 0xF0;             // enable digital I/O on PB4,PB5,PB6 and PB7
	GPIO_PORTB_DIR_R |= 0x20;             // make PB5 out
                                          // configure PB4,PB6 and PB7 as SSI
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0x00F0FFFF)+0x22020000;
	                                      // configure PB5 as GPIO
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF0FFFFF)+0x00000000;
	SSI2_CR1_R&=~SSI_CR1_SSE;		  // Disable SSI while configuring it
	SSI2_CR1_R&=~SSI_CR1_MS;		  //  Set as Master
	SSI2_CC_R|=SSI_CC_CS_M; //  Configure clock source
	SSI2_CC_R|=SSI_CC_CS_SYSPLL; //  Configure clock source
	SSI2_CC_R|=SSI_CPSR_CPSDVSR_M;//  Configure prescale divisor
	SSI2_CPSR_R = (SSI2_CPSR_R&~SSI_CPSR_CPSDVSR_M)+10; // must be even number
	SSI2_CR0_R |=0x0300;
	SSI2_CR0_R &= ~(SSI_CR0_SPH | SSI_CR0_SPO);
	SSI2_CR0_R = (SSI2_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
	                                        // DSS = 8-bit data
	SSI2_CR0_R = (SSI2_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
	SSI2_CR1_R|=SSI_CR1_SSE;		  // 3)Enable SSI

    if(SD_init() != SD_SUCCESS)
        kernel::log_error("SD card initialization failed\n");
    else
        kernel::log_info("SD card initialization complete\n");

    this->set_base_adr((kernel::u8*)(0x00));
}

void _delay_ms(kernel::size_t t){
    for(kernel::size_t i = 0; i < t*80000; i++)
        __asm("nop  \n");
}

int SPI_transfer(char message){
    kernel::get_cpu()->disable_interrupts();
    CS_ENABLE();
    unsigned short volatile rcvdata;

    while((SSI2_SR_R&SSI_SR_TNF)==0){};

    SSI2_DR_R = message;

    while((SSI2_SR_R&SSI_SR_RNE)==0){};
    // while((SSI2_SR_R&0x10)){};

    rcvdata = SSI2_DR_R;
    kernel::get_cpu()->enable_interrupts();
    return rcvdata;
}

/*******************************************************************************
 Initialize SD card
*******************************************************************************/
uint8_t SD_init()
{
    uint8_t res[5], cmdAttempts = 0;

    SD_powerUpSeq();

    while((res[0] = SD_goIdleState()) != SD_IN_IDLE_STATE)
    {
        cmdAttempts++;
        if(cmdAttempts == CMD0_MAX_ATTEMPTS)
        {
            return SD_ERROR;
        }
    }

    _delay_ms(1);

    SD_sendIfCond(res);
    if(res[0] != SD_IN_IDLE_STATE)
    {
        return SD_ERROR;
    }

    if(res[4] != 0xAA)
    {
        return SD_ERROR;
    }

    cmdAttempts = 0;
    do
    {
        if(cmdAttempts == CMD55_MAX_ATTEMPTS)
        {
            return SD_ERROR;
        }

        res[0] = SD_sendApp();
        if(SD_R1_NO_ERROR(res[0]))
        {
            res[0] = SD_sendOpCond();
        }

        _delay_ms(1);

        cmdAttempts++;
    }
    while(res[0] != SD_READY);

    _delay_ms(1);

    SD_readOCR(res);

    return SD_SUCCESS;
}

/*******************************************************************************
 Run power up sequence
*******************************************************************************/
void SD_powerUpSeq()
{
    // make sure card is deselected
    CS_DISABLE();

    // give SD card time to power up
    _delay_ms(10);

    // select SD card
    SPI_transfer(0xFF);
    CS_DISABLE();

    // send 80 clock cycles to synchronize
    for(uint8_t i = 0; i < SD_INIT_CYCLES; i++)
        SPI_transfer(0xFF);
}

/*******************************************************************************
 Send command to SD card
*******************************************************************************/
void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    // transmit command to sd card
    SPI_transfer(cmd|0x40);

    // transmit argument
    SPI_transfer((uint8_t)(arg >> 24));
    SPI_transfer((uint8_t)(arg >> 16));
    SPI_transfer((uint8_t)(arg >> 8));
    SPI_transfer((uint8_t)(arg));

    // transmit crc
    SPI_transfer(crc|0x01);
}

/*******************************************************************************
 Read R1 from SD card
*******************************************************************************/
uint8_t SD_readRes1()
{
    uint8_t i = 0, res1;

    // keep polling until actual data received
    while((res1 = SPI_transfer(0xFF)) == 0xFF)
    {
        i++;

        // if no data received for 8 bytes, break
        if(i > 8) break;
    }

    return res1;
}

/*******************************************************************************
 Read R2 from SD card
*******************************************************************************/
void SD_readRes2(uint8_t *res)
{
    // read response 1 in R2
    res[0] = SD_readRes1();

    // read final byte of response
    res[1] = SPI_transfer(0xFF);
}

/*******************************************************************************
 Read R3 from SD card
*******************************************************************************/
void SD_readRes3(uint8_t *res)
{
    // read response 1 in R3
    res[0] = SD_readRes1();

    // if error reading R1, return
    if(res[0] > 1) return;

    // read remaining bytes
    SD_readBytes(res + 1, R3_BYTES);
}

/*******************************************************************************
 Read R7 from SD card
*******************************************************************************/
void SD_readRes7(uint8_t *res)
{
    // read response 1 in R7
    res[0] = SD_readRes1();

    // if error reading R1, return
    if(res[0] > 1) return;

    // read remaining bytes
    SD_readBytes(res + 1, R7_BYTES);
}

/*******************************************************************************
 Read specified number of bytes from SD card
*******************************************************************************/
void SD_readBytes(uint8_t *res, uint8_t n)
{
    while(n--) *res++ = SPI_transfer(0xFF);
}

/*******************************************************************************
 Command Idle State (CMD0)
*******************************************************************************/
uint8_t SD_goIdleState()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(CMD0, CMD0_ARG, CMD0_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

/*******************************************************************************
 Send Interface Conditions (CMD8)
*******************************************************************************/
void SD_sendIfCond(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD8
    SD_command(CMD8, CMD8_ARG, CMD8_CRC);

    // read response
    SD_readRes7(res);
    //SD_readBytes(res + 1, R7_BYTES);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

/*******************************************************************************
 Read Status
*******************************************************************************/
void SD_sendStatus(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD13
    SD_command(CMD13, CMD13_ARG, CMD13_CRC);

    // read response
    SD_readRes2(res);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

/*******************************************************************************
 Read single 512 byte block
 token = 0xFE - Successful read
 token = 0x0X - Data error
 token = 0xFF - timeout
*******************************************************************************/
uint8_t SD_readSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint8_t res1, read = 0;
    uint16_t readAttempts;

    // set token to none
    *token = 0xFF;

    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD17
    SD_command(CMD17, addr, CMD17_CRC);

    // read R1
    res1 = SD_readRes1();

    // if response received from card
    if(res1 != 0xFF)
    {
        // wait for a response token (timeout = 100ms)
        readAttempts = 0;
        while(++readAttempts != SD_MAX_READ_ATTEMPTS)
            if((read = SPI_transfer(0xFF)) != 0xFF) break;

        // if response token is 0xFE
        if(read == SD_START_TOKEN)
        {
            // read 512 byte block
            for(uint16_t i = 0; i < SD_BLOCK_LEN; i++) *buf++ = SPI_transfer(0xFF);

            // read 16-bit CRC
            SPI_transfer(0xFF);
            SPI_transfer(0xFF);
        }

        // set token to card response
        *token = read;
    }

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

#define SD_MAX_WRITE_ATTEMPTS   3907

/*******************************************************************************
Write single 512 byte block
token = 0x00 - busy timeout
token = 0x05 - data accepted
token = 0xFF - response timeout
*******************************************************************************/
uint8_t SD_writeSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint16_t readAttempts;
    uint8_t res1, read = 0;

    // set token to none
    *token = 0xFF;
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD24
    SD_command(CMD24, addr, CMD24_CRC);

    // read response
    res1 = SD_readRes1();

    // if no error
    if(res1 == SD_READY)
    {
        // send start token
        SPI_transfer(SD_START_TOKEN);

        // write buffer to card
        for(uint16_t i = 0; i < SD_BLOCK_LEN; i++) SPI_transfer(buf[i]);

        // wait for a response (timeout = 250ms)
        readAttempts = 0;
        while(++readAttempts != SD_MAX_WRITE_ATTEMPTS)
            if((read = SPI_transfer(0xFF)) != 0xFF) { *token = 0xFF; break; }

        // if data accepted
        if((read & 0x1F) == 0x05)
        {
            // set token to data accepted
            *token = 0x05;

            // wait for write to finish (timeout = 250ms)
            readAttempts = 0;
            while(SPI_transfer(0xFF) == 0x00)
                if(++readAttempts == SD_MAX_WRITE_ATTEMPTS) { *token = 0x00; break; }
        }
    }

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

/*******************************************************************************
 Reads OCR from SD Card
*******************************************************************************/
void SD_readOCR(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    uint8_t tmp = SPI_transfer(0xFF);

    if(tmp != 0xFF) while(SPI_transfer(0xFF) != 0xFF) ;

    // send CMD58
    SD_command(CMD58, CMD58_ARG, CMD58_CRC);

    // read response
    SD_readRes3(res);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

/*******************************************************************************
 Send application command (CMD55)
*******************************************************************************/
uint8_t SD_sendApp()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(CMD55, CMD55_ARG, CMD55_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

/*******************************************************************************
 Send operating condition (ACMD41)
*******************************************************************************/
uint8_t SD_sendOpCond()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(ACMD41, ACMD41_ARG, ACMD41_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}


void SD_printR1(uint8_t res)
{
    if(res == 0xFF)
        { kernel::log_info("\tNo response\r\n"); return; }
    if(res & 0x80)
        { kernel::log_error("\tError: MSB = 1\r\n"); return; }
    // if(res == 0)
    //     { kernel::log_info("\tCard Ready\r\n"); return; }
    if(PARAM_ERROR(res))
        kernel::log_error("\tParameter Error\r\n");
    if(ADDR_ERROR(res))
        kernel::log_error("\tAddress Error\r\n");
    if(ERASE_SEQ_ERROR(res))
        kernel::log_error("\tErase Sequence Error\r\n");
    if(CRC_ERROR(res))
        kernel::log_error("\tCRC Error\r\n");
    if(ILLEGAL_CMD(res))
        kernel::log_error("\tIllegal Command\r\n");
    if(ERASE_RESET(res))
        kernel::log_error("\tErase Reset Error\r\n");
    if(IN_IDLE(res))
        kernel::log_info("\tIn Idle State\r\n");
}

void SD_printR2(uint8_t *res)
{
    SD_printR1(res[0]);

    if(res[0] == 0xFF) return;

    if(res[1] == 0x00)
        kernel::log_info("\tNo R2 Error\r\n");
    if(OUT_OF_RANGE(res[1]))
        kernel::log_error("\tOut of Range\r\n");
    if(ERASE_PARAM(res[1]))
        kernel::log_info("\tErase Parameter\r\n");
    if(WP_VIOLATION(res[1]))
        kernel::log_error("\tWP Violation\r\n");
    if(CARD_ECC_FAILED(res[1]))
        kernel::log_error("\tECC Failed\r\n");
    if(CC_ERROR(res[1]))
        kernel::log_error("\tCC Error\r\n");
    if(ERROR(res[1]))
        kernel::log_error("\tError\r\n");
    if(WP_ERASE_SKIP(res[1]))
        kernel::log_info("\tWP Erase Skip\r\n");
    if(CARD_LOCKED(res[1]))
        kernel::log_error("\tCard Locked\r\n");
}

void SD_printR3(uint8_t *res)
{
    SD_printR1(res[0]);

    if(res[0] > 1) return;

    kernel::log_info("\tCard Power Up Status: ");
    if(POWER_UP_STATUS(res[1]))
    {
        kernel::log_info("READY\r\n");
        kernel::log_info("\tCCS Status: ");
        if(CCS_VAL(res[1])){ kernel::log_info("1\r\n"); }
        else kernel::log_info("0\r\n");
    }
    else
    {
        kernel::log_info("BUSY\r\n");
    }

    kernel::log_info("\tVDD Window: ");
    if(VDD_2728(res[3])) kernel::log_info("2.7-2.8, ");
    if(VDD_2829(res[2])) kernel::log_info("2.8-2.9, ");
    if(VDD_2930(res[2])) kernel::log_info("2.9-3.0, ");
    if(VDD_3031(res[2])) kernel::log_info("3.0-3.1, ");
    if(VDD_3132(res[2])) kernel::log_info("3.1-3.2, ");
    if(VDD_3233(res[2])) kernel::log_info("3.2-3.3, ");
    if(VDD_3334(res[2])) kernel::log_info("3.3-3.4, ");
    if(VDD_3435(res[2])) kernel::log_info("3.4-3.5, ");
    if(VDD_3536(res[2])) kernel::log_info("3.5-3.6");
    kernel::log_info("\r\n");
}

void SD_printR7(uint8_t *res)
{
    SD_printR1(res[0]);

    if(res[0] > 1) return;

    kernel::log_info("\tCommand Version: ");
    kernel::log_info("%u\n", CMD_VER(res[1]));

    kernel::log_info("\tVoltage Accepted: ");
    if(VOL_ACC(res[3]) == VOLTAGE_ACC_27_33)
        kernel::log_info("2.7-3.6V\r\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_LOW)
        kernel::log_info("LOW VOLTAGE\r\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES1)
        kernel::log_info("RESERVED\r\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES2)
        kernel::log_info("RESERVED\r\n");
    else
        kernel::log_info("NOT DEFINED\r\n");

    kernel::log_info("\tEcho: ");
    kernel::log_info("%u\n", res[4]);
    kernel::log_info("\r\n");
}

void SD_printCSD(uint8_t *buf)
{
    kernel::log_info("CSD:\r\n");

    kernel::log_info("\tCSD Structure: ");
    kernel::log_info("%u\n", (buf[0] & 0b11000000) >> 6);
    kernel::log_info("\r\n");

    kernel::log_info("\tTAAC: ");
    kernel::log_info("%u", buf[1]);
    kernel::log_info("\r\n");

    kernel::log_info("\tNSAC: ");
    kernel::log_info("%u", buf[2]);
    kernel::log_info("\r\n");

    kernel::log_info("\tTRAN_SPEED: ");
    kernel::log_info("%u", buf[3]);
    kernel::log_info("\r\n");

    kernel::log_info("\tDevice Size: ");
    kernel::log_info("%u", buf[7] & 0b00111111);
    kernel::log_info("%u", buf[8]);
    kernel::log_info("%u", buf[9]);
    kernel::log_info("\r\n");
}

void SD_printBuf(uint8_t *buf)
{
    uint8_t colCount = 0;
    for(uint16_t i = 0; i < SD_BLOCK_LEN; i++)
    {
        kernel::log_info("%u", *buf++);
        if(colCount == 19)
        {
            kernel::log_info("\r\n");
            colCount = 0;
        }
        else
        {
            kernel::log_info(" ");
            colCount++;
        }
    }
    kernel::log_info("\r\n");
}

void SD_printDataErrToken(uint8_t token)
{
    if(token & 0xF0)
        kernel::log_info("\tNot Error token\r\n");
    if(SD_TOKEN_OOR(token))
        kernel::log_info("\tData out of range\r\n");
    if(SD_TOKEN_CECC(token))
        kernel::log_info("\tCard ECC failed\r\n");
    if(SD_TOKEN_CC(token))
        kernel::log_info("\tCC Error\r\n");
    if(SD_TOKEN_ERROR(token))
        kernel::log_info("\tError\r\n");
}

void SDCard::read_blk(kernel::u8* buf, kernel::u8* adr){
    kernel::u8 token, res;
    res = SD_readSingleBlock((kernel::size_t)adr, buf, &token);

    if(!(token & 0xF0))
    {
        kernel::log_error("Error token:\r\n");
        SD_printDataErrToken(token);
    }
}

void SDCard::write_blk(kernel::u8* buf, kernel::u8* adr){
    kernel::u8 token, res;
    res = SD_writeSingleBlock((kernel::size_t)adr, buf, &token);

    SD_printR1(res);
}
