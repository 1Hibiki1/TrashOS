#include <device/keyboard.h>
#include <kernel/kernel.h>

/*
 * TODO:
 * NOTE: THE PROBLEM DISAPPEARED. I HAVE NO IDEA HOW O_O
 * 
 * NOTE: a part of the porblem was that some pins on port B and D
 *       were "connected" (particularly PD0 and PD1). But even switching to PD2 doesn't
 *       solve the problem, somehow (it stops the random interrupts during SD card write,
 *       but not read)...
 * 
 * PROBLEM:
 * This doesn't work with sd card initialized for some reason... probably because
 * of SSI but idk why. PD1 receives random interrupts when the SD card is being read, 
 * so that leaves the bitcount variable in the handler at some value less than 10.
 * when the user actually presses a key, things are already messed up.
 * Disabling interrupts during SD read/write partially fized the issue, but there
 * is still at least one extra interupt on PD1 after re-enabling interrupts after
 * SD read is complete. Ill leave this for now, but this needs to be addressed.
 * Disabling key interrupts during a disk read is probably the worst idea lol :')  
 */

char scan_codes[][3] = {
    0x0e,'`','\0',
    0x15,'q','Q',
    0x16,'1','!',
    0x1a,'z','Z',
    0x1b,'s','S',
    0x1c,'a','A',
    0x1d,'w','W',
    0x1e,'2','@',
    0x21,'c','C',
    0x22,'x','X',
    0x23,'d','D',
    0x24,'e','E',
    0x25,'4','$',
    0x26,'3','#',
    0x29,' ',' ',
    0x2a,'v','V',
    0x2b,'f','F',
    0x2c,'t','T',
    0x2d,'r','R',
    0x2e,'5','%',
    0x31,'n','N',
    0x32,'b','B',
    0x33,'h','H',
    0x34,'g','G',
    0x35,'y','Y',
    0x36,'6','^',
    0x39,',',';',
    0x3a,'m','M',
    0x3b,'j','J',
    0x3c,'u','U',
    0x3d,'7','&',
    0x3e,'8','*',
    0x41,',','<',
    0x42,'k','K',
    0x43,'i','I',
    0x44,'o','O',
    0x45,'0',')',
    0x46,'9','(',
    0x49,'.','>',
    0x4a,'/','?',
    0x4b,'l','L',
    0x4c,';',':',
    0x4d,'p','P',
    0x4e,'-','_',
    0x52,'\'','"',
    0x54,'[','{',
    0x55,'=','+',
    0x5a,'\n','\n',
    0x5b,']','}',
    0x5d,'\\','|',
    0x61,'<','>',
    0x66,'\b','\b',
    0x69,'1',' ',
    0x6b,'4',' ',
    0x6c,'7',' ',
    0x70,'0',' ',
    0x71,',',' ',
    0x72,'2',' ',
    0x73,'5',' ',
    0x74,'6',' ',
    0x75,'8',' ',
    0x79,'+',' ',
    0x7a,'3',' ',
    0x7b,'-',' ',
    0x7c,'*',' ',
    0x7d,'9',' ',
    0,'\0','\0',
};


void keyboard_decode(kernel::u8 scan_code){
    static kernel::u8 shifted = 0;
    static kernel::u8 released = 0;

    if(scan_code == 0xf0) released = 1;
    if(scan_code == 0x12 || scan_code == 0x59) shifted = 1;

    // ignore releases
    if(released && scan_code != 0xf0){
        released = 0;
        if(scan_code == 0x12 || scan_code == 0x59) shifted = 0;
        return;
    }

    // tab
    if(scan_code == 0x0d){
        kernel::get_keyboard()->add_to_buf('\t');
        return;
    }

    // right arrow
    if(scan_code == 0x74){
        kernel::get_serial_device()->move_cursor_right();
        return;
    }

    // left arrow
    if(scan_code == 0x6b){
        kernel::get_serial_device()->move_cursor_left();
        return;
    }

    // up arrow
    if(scan_code == 0x75){
        kernel::get_serial_device()->move_cursor_up();
        return;
    }

    // down arrow
    if(scan_code == 0x72){
        kernel::get_serial_device()->move_cursor_down();
        return;
    }

    for(kernel::size_t i = 0; scan_codes[i][0] != 0; i++){
        if(scan_code == scan_codes[i][0]){
            char p = scan_codes[i][1 + shifted];
            kernel::get_keyboard()->add_to_buf(p);
            break;
        }
    }
}

kernel::u8 bitcount = 0;
kernel::u8 data = 0x00;
bool disabled = false;

void kbd_handler(){
    if(disabled)
        return;

    kernel::u8 cur_val = (GPIO_PORTE_DATA_R & 0b100) >> 1;


    if (bitcount > 0 && bitcount < 9){
        data >>= 1;
        if (cur_val)
            data |= 0x80;
    }
    if (bitcount++ == 10){
        keyboard_decode(data);
        bitcount = 0;
        data = 0x00;
    }
}


Keyboard::Keyboard()
:   GenericDevice(1024, 0)
{
    kernel::get_cpu()->portD->digital_init_pin(2, INPUT);

    kernel::get_cpu()->portD->attach_interrupt_pin(PD2, FALLING, kbd_handler);
    kernel::get_cpu()->portD->set_pdr(2);

    kernel::get_cpu()->portE->digital_init_pin(2, INPUT);
    kernel::get_cpu()->portE->set_pur(2);
}

void Keyboard::reset(){
    bitcount = 0;
}


void Keyboard::disable(){
    disabled = true;
}

void Keyboard::enable(){
    disabled = false;
}
