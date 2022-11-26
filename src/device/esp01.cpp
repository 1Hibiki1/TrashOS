// WiFi module

#include <device/esp01.h>
#include <cpu/tm4c.h>
#include <libk/string.h>
#include <kernel/kernel.h>
#include <libk/vector.h>

// from https://github.com/CMUQ-15-348/demos/blob/f22/ESP_Demo/main.c

char line[128];
kernel::u16 line_c = 0;
bool got_ok = false;
char* response;
kernel::size_t resp_idx = 0;

char UART7_InChar(void)
{
    while ((UART7_FR_R & 0x0010) != 0)
        ; // Wait until RXFE is 0 (data is available)
    return ((char) (UART7_DR_R & 0xFF));
}

void UART7_OutChar(char data)
{
    while ((UART7_FR_R & 0x0020) != 0)
        ; // Wait until TXFE is 0 (data is available)
    UART7_DR_R = data;
}

void UART7_OutStr(const char *str)
{
    while (*str != '\0')
    {
        UART7_OutChar(*str++);
    }
}

void UART7_GetLine(char *buf, int n)
{
    int i = 0;
    while (i < n - 1)
    {
        buf[i] = UART7_InChar();
        if (buf[i] == '\n')
        {
            break;
        }
        i++;
    }
    buf[i] = '\0';
}

extern "C" void UART7_ISR(void)
{
    // Interrupt because there is data in the receive FIFO
    if ((UART7_RIS_R & 0x10) != 0)
    {
        UART7_ICR_R |= 0x10; // Acknowledge the interrupt
    }

    // Receiver timeout.  There is data in the FIFO, and we haven't received any more for 32 UART cycles.
    // This interrupt is needed because sometimes there is data that isn't handled by a previous interrupt,
    // and this makes sure we clear out the entire FIFO.
    if ((UART7_RIS_R & 0x40) != 0)
    {
        UART7_ICR_R |= 0x40; // Acknowledge the interrupt
    }

    kernel::get_cpu()->disable_interrupts();

    while ((UART7_FR_R & 0x0010) == 0)
    {
        char c = (char) (UART7_DR_R & 0xFF);
        response[resp_idx++] = c;
        line[line_c++] = c;
        if (c == '\n')
        {
            line[line_c] = '\0';
            for(kernel::u8 i = 0; line[i] != 0; i++)
                kernel::get_wifi_module()->write(line[i]);
            if (kernel::strcmp(line, "OK\r\n") == 0)
            {
                got_ok = 1;
            }
            line_c = 0;
        }
    }

    kernel::get_cpu()->enable_interrupts();
}


ESP01::ESP01()
:   GenericDevice(100, 100)
{
    // Enable UART7
    SYSCTL_RCGCUART_R |= 0x80;
    // Activate Port E
    SYSCTL_RCGCGPIO_R |= 0x10;

    // Disable UART7
    UART7_CTL_R &= ~UART_CTL_UARTEN;

    // Set speed to 115200
    UART7_IBRD_R = 43; // 80,000,000 / (16*115200) == 43.40277777777778
    UART7_FBRD_R = 26; // int(0.402777777*64 + 0.5) == 26

    // 8-bit word length, enable FIFO
    UART7_LCRH_R = 0x0070;

    // Setup PORTE pins PE0, PE1
    GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFFFFFF00) | 0x11; // Set the alternate functionality we want: UART
    GPIO_PORTE_AMSEL_R &= ~0x03; // disable analog
    GPIO_PORTE_AFSEL_R |= 0x03; // enable alt functionality
    GPIO_PORTE_DEN_R |= 0x03; // enable digital IO

    // Interrupts
    UART7_IFLS_R |= 0x00; // Trigger interrupts early
    UART7_IM_R |= 0x50; // unmask RXIM (Receive mask) and RTIM (Receive time-out mask).  See page 931/932 in datasheet for details
    NVIC_EN1_R = ((kernel::u32) 1 << (63 - 32));     // Enable interrupt 63

    // Enable RXE, TXE, and UART
    UART7_CTL_R |= (UART_CTL_UARTEN | UART_CTL_RXE | UART_CTL_TXE);

    response = (char*)kernel::malloc(512);
}

ESP01::~ESP01(){
    // delete response;
    kernel::free(response);
}

void ESP01::send_command(const char* cmd){
    resp_idx = 0;
    UART7_OutStr(cmd);
}

void ESP01::wait_for_response(){
    while(!got_ok){
        __asm("nop  \n");
    }
    got_ok = false;
    this->get_out_buf()->clear();
}

char* ESP01::get_response(){
    response[resp_idx] = '\0';
    return response;
}
