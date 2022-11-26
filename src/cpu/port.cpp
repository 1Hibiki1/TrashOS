#include <cpu/port.h>
#include <libk/util.h>
#include <cpu/tm4c.h>


Port::Port(kernel::size_t* base_adr)
    : base_adr(base_adr)

{
    this->data_adr = (kernel::add_offset_to_adr(this->base_adr, this->DATA_OFFSET));
    this->dir_adr = (kernel::add_offset_to_adr(this->base_adr, this->DIR_OFFSET));
    this->lock_adr = (kernel::add_offset_to_adr(this->base_adr, this->LOCK_OFFSET));
    this->cr_adr = (kernel::add_offset_to_adr(this->base_adr, this->CR_OFFSET));
    this->amsel_adr = (kernel::add_offset_to_adr(this->base_adr, this->AMSEL_OFFSET));
    this->afsel_adr = (kernel::add_offset_to_adr(this->base_adr, this->AFSEL_OFFSET));
    this->pctl_adr = (kernel::add_offset_to_adr(this->base_adr, this->PCTL_OFFSET));
    this->pur_adr = (kernel::add_offset_to_adr(this->base_adr, this->PUR_OFFSET));
    this->pdr_adr = (kernel::add_offset_to_adr(this->base_adr, this->PDR_OFFSET));
    this->den_adr = (kernel::add_offset_to_adr(this->base_adr, this->DEN_OFFSET));

    this->is_adr = (kernel::add_offset_to_adr(this->base_adr, this->IS_OFFSET));
    this->ibe_adr = (kernel::add_offset_to_adr(this->base_adr, this->IBE_OFFSET));
    this->iev_adr = (kernel::add_offset_to_adr(this->base_adr, this->IEV_OFFSET));
    this->icr_adr = (kernel::add_offset_to_adr(this->base_adr, this->ICR_OFFSET));
    this->im_adr = (kernel::add_offset_to_adr(this->base_adr, this->IM_OFFSET));
    this->mis_adr = (kernel::add_offset_to_adr(this->base_adr, this->MIS_OFFSET));

}


void Port::digital_init_pin(kernel::u8 pin, io_dir dir){
    kernel::u8 p_mask = (1 << (pin));

    // enable clock for port
    // heck, enable clocks for all ports :')
    kernel::write_to_adress(0b111111, (kernel::size_t*)this->CLK_ADR);

    kernel::write_to_adress(this->LOCK_VAL, this->lock_adr);
    kernel::write_to_adress(p_mask, this->cr_adr);

    // set these to 0 for now
    kernel::write_to_adress(0, this->amsel_adr);
    kernel::write_to_adress(0, this->afsel_adr);
    kernel::write_to_adress(0, this->pctl_adr);

    kernel::write_to_adress(
        kernel::read_from_adress(this->den_adr) | p_mask,
        this->den_adr
    );

    if(dir == INPUT){
        kernel::write_to_adress(
            kernel::read_from_adress(this->dir_adr) & ~(p_mask),
            this->dir_adr
        );
    }
    else
        kernel::write_to_adress(
            kernel::read_from_adress(this->dir_adr) | p_mask,
            this->dir_adr
        );
}

void Port::attach_interrupt_pin(Pin p, int_mode mode, void (*handler)()){
    kernel::u8 p_mask = 0;
    kernel::u8 port_num = 0;

    install_handler(p, handler);

    // TODO: move these into another function?
    switch(p){
        case PB0:
        case PD0:
        case PE0:
        case PF0: {
            p_mask = 1;
            break;
        }

        case PB1:
        case PD1:
        case PE1:
        case PF1: {
            p_mask = 0b10;
            break;
        }

        case PA2:
        case PB2:
        case PD2:
        case PE2:
        case PF2: {
            p_mask = 0b100;
            break;
        }

        case PA3:
        case PB3:
        case PD3:
        case PE3:
        case PF3: {
            p_mask = 0b1000;
            break;
        }

        case PA4:
        case PB4:
        case PC4:
        case PE4:
        case PF4: {
            p_mask = 0b10000;
            break;
        }

        case PA5:
        case PB5:
        case PC5:
        case PE5: {
            p_mask = 0b100000;
            break;
        }

        case PA6:
        case PB6:
        case PC6:
        case PD6: {
            p_mask = 0b1000000;
            break;
        }

        case PA7:
        case PB7:
        case PC7:
        case PD7: {
            p_mask = 0b10000000;
            break;
        }
    }

    switch(p){
        case PA2:
        case PA3: 
        case PA4:
        case PA5:
        case PA6:
        case PA7: {
            port_num = 0;
            break;
        }

        case PB0:
        case PB1:
        case PB2:
        case PB3:
        case PB4:
        case PB5:
        case PB6:
        case PB7: {
            port_num = 1;
            break;
        }

        case PC4:
        case PC5:
        case PC6:
        case PC7: {
            port_num = 2;
            break;
        }

        case PD0:
        case PD1:
        case PD2:
        case PD3:
        case PD6:
        case PD7: {
            port_num = 3;
            break;
        }

        case PE0:
        case PE1:
        case PE2:
        case PE3:
        case PE4:
        case PE5: {
            port_num = 4;
            break;
        }

        case PF0:
        case PF1:
        case PF2:
        case PF3:
        case PF4: {
            port_num = 5;
            break;
        }
    }

    // GPIO_PORTF_IS_R &= ~p_mask;
    kernel::write_to_adress(
        kernel::read_from_adress(this->is_adr) & (~p_mask),
        this->is_adr
    );


    if(mode == BOTH)
        // GPIO_PORTF_IBE_R |= p_mask;
        kernel::write_to_adress(
            kernel::read_from_adress(this->ibe_adr) | p_mask,
            this->ibe_adr
        );
    else 
        // GPIO_PORTF_IBE_R &= ~p_mask;
        kernel::write_to_adress(
            kernel::read_from_adress(this->ibe_adr) & ~p_mask,
            this->ibe_adr
        );


    if(mode == RISING)
        // GPIO_PORTF_IEV_R |= p_mask;
        kernel::write_to_adress(
            kernel::read_from_adress(this->iev_adr) | p_mask,
            this->iev_adr
        );
    else 
        // GPIO_PORTF_IEV_R &= ~p_mask;
        kernel::write_to_adress(
            kernel::read_from_adress(this->iev_adr) & ~p_mask,
            this->iev_adr
        );


    // GPIO_PORTF_ICR_R |= p_mask;
    kernel::write_to_adress(
        kernel::read_from_adress(this->icr_adr) | p_mask,
        this->icr_adr
    );

    // GPIO_PORTF_IM_R |= p_mask;
    kernel::write_to_adress(
        kernel::read_from_adress(this->im_adr) | p_mask,
        this->im_adr
    );

    install_handler(p, handler);

    if(port_num == 5) kernel::write_to_adress((1 << 30), (kernel::size_t*)(0xE000E100));
    else kernel::write_to_adress((1 << port_num), (kernel::size_t*)(0xE000E100));
}

void Port::write_byte(kernel::u8 data){
    kernel::write_to_adress(data, this->data_adr);
}

kernel::u8 Port::read_byte(){
    return static_cast<kernel::u8>(kernel::read_from_adress(this->data_adr));
}

void Port::write_pin(kernel::u8 pin, kernel::u8 val){
    if(val == 1){
        this->write_byte(this->read_byte() | (1 << pin));
    }

    else {
        this->write_byte(this->read_byte() & ~(1 << pin));
    }
}

kernel::u8 Port::read_pin(kernel::u8 pin){
    return (this->read_byte() & (1 << pin)) >> pin;
}

void Port::set_pur(kernel::u8 p){
    kernel::write_to_adress(
        kernel::read_from_adress(this->pur_adr) | (1 << p),
        this->pur_adr
    );
}

void Port::set_pdr(kernel::u8 p){
    kernel::write_to_adress(
        kernel::read_from_adress(this->pdr_adr) | (1 << p),
        this->pdr_adr
    );
}
