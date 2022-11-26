#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <libk/array.h>
#include <libk/stdint.h>


class InputDevice {
    public:
        InputDevice(kernel::size_t buf_sz);
        virtual ~InputDevice() {
            delete this->inp_buffer;
        }

        void read(kernel::u8* buf, kernel::size_t n);
        void add_to_buf(kernel::u8 dat);

        kernel::Array<kernel::u8>* get_inp_buf(){
            return this->inp_buffer;
        }

        void set_inp_cursor(kernel::size_t c){
            this->inp_buffer->set_size(c);
        }
    protected:
        kernel::size_t buf_size;
        kernel::Array<kernel::u8>* inp_buffer;
};

class OutputDevice {
    public:
        OutputDevice(kernel::size_t buf_sz);
        virtual ~OutputDevice() {
            delete this->out_buffer;
        }

        // TODO: is it inefficient to write just one byte at a time?
        // -> ofc it is
        void write(kernel::u8 data);
        virtual void flush(){}
        virtual void flush(kernel::size_t s){}

        kernel::Array<kernel::u8>* get_out_buf(){
            return this->out_buffer;
        }

        void set_out_cursor(kernel::size_t c){
            this->out_buffer->set_size(c);
        }
    protected:
        kernel::size_t buf_size;
        kernel::Array<kernel::u8>* out_buffer;
};


class GenericDevice: public InputDevice, public OutputDevice{
    public:
        GenericDevice(kernel::size_t inp_sz, kernel::size_t out_sz)
        :   InputDevice(inp_sz),
            OutputDevice(out_sz)
        {}

        virtual ~GenericDevice(){}
};

#endif
