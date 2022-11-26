#include <device/device.h>
#include <libk/mem.h>

OutputDevice::OutputDevice(kernel::size_t buf_sz)
    : buf_size(buf_sz)
{
    this->out_buffer = (buf_sz == 0) ?
        nullptr :
        new kernel::Array<kernel::u8>(buf_sz);
}

void OutputDevice::write(kernel::u8 data){
    if( this->out_buffer->get_size() >=
        this->out_buffer->get_capcacity()){
            this->flush();
            this->out_buffer->clear();
    }
    this->out_buffer->push(data);
}


InputDevice::InputDevice(kernel::size_t buf_sz)
    : buf_size(buf_sz)
{
    this->inp_buffer = nullptr;
    if(buf_sz == 0){
        return;
    }
    this->inp_buffer = new kernel::Array<kernel::u8>(buf_sz);
}

void InputDevice::read(kernel::u8* buf, kernel::size_t n){
    kernel::memcpy(buf, this->inp_buffer, n);
}

void InputDevice::add_to_buf(kernel::u8 dat){
    if(this->inp_buffer->get_size() >= this->inp_buffer->get_capcacity()){
        this->inp_buffer->clear();
    }
    this->inp_buffer->push(dat);
}
