#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <device/device.h>
#include <kernel/process.h>

#define MAX_REQ_NUM 500

enum REQ_TYPE {
    REQ_READ,
    REQ_WRITE
};

class Request{
    public:
        Request(
            REQ_TYPE typ,
            GenericDevice* dev,
            Process* proc,
            kernel::u8* buf,
            kernel::size_t sz
        )
        :
            io_buf(buf),
            io_sz(sz),
            device(dev),
            proc(proc),
            req_type(typ)
        {}

        GenericDevice* get_device(){
            return this->device;
        }

        Process* get_process(){
            return this->proc;
        }

        REQ_TYPE get_req_type(){
            return this->req_type;
        }

        kernel::u8* io_buf;
        kernel::size_t io_sz;

    private:
        GenericDevice* device;
        Process* proc;
        REQ_TYPE req_type;
};

#endif
