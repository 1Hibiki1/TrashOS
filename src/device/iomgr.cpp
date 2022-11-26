#include <device/iomgr.h>
#include <kernel/scheduler.h>
#include <kernel/kernel.h>

IOManager::IOManager(){
    this->req_q = new kernel::ArrayQueue<Request*>(MAX_REQ_NUM);
}

void IOManager::add_request(Request* req){
    kernel::get_scheduler()->pause();

    Process* proc = req->get_process();
    REQUIRES(proc->get_state() != P_STATE_WAITING);

    // at this point current process is not in ready list
    proc->set_state(P_STATE_WAITING);
    this->req_q->enqueue(req);

    kernel::get_scheduler()->run();
}

void IOManager::process_requests(){
    if(this->req_q->get_size() <= 0) return;

    while(this->req_q->get_size() > 0){
        Request* req = this->req_q->dequeue();
        GenericDevice* dev = req->get_device();

        if(req->get_req_type() == REQ_READ){
            dev->read(req->io_buf, req->io_sz);
        }

        else {
            kernel::size_t n = req->io_sz;
            for(kernel::size_t i = 0; i < n; i++){
                kernel::u8 data = req->io_buf[i];
                dev->write(data);
            }
            dev->flush(n);

            // wake process up
            req->get_process()->set_state(P_STATE_READY);
        }
    }
}

