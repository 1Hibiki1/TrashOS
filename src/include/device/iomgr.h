#ifndef _IO_H_
#define _IO_H_

#include <device/device.h>
#include <kernel/process.h>
#include <libk/arrayqueue.h>
#include <device/request.h>

typedef kernel::ArrayQueue<Request*>* req_q_t;

class IOManager{
    public:
        IOManager();
        void add_request(Request* req);
        void process_requests();
    private:
        req_q_t req_q;
};


#endif
