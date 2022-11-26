#include <kernel/syscall.h>
#include <kernel/kernel.h>

// TODO: parse path
// TODO: set ERRNO

kernel::Vector<char*>* fnamelist = nullptr;

// https://pubs.opengroup.org/onlinepubs/007904875/functions/open.html
int _open(const char *path, int oflag){
    Process* proc = kernel::get_scheduler()->get_cur_proc();
    Drive* root_dev = kernel::get_root_drive();
    File* f = nullptr;
    file_descriptor* filedes = nullptr;

    // TODO: just find vnode and check if it is device??
    if(kernel::strlen(path) >= 5 && (kernel::strncmp("/dev/", path, 5) == 0)){
        path += 5;

        kernel::get_scheduler()->run();
        // TODO: move these names to macros?
        if(kernel::strcmp(path, "tty0") == 0)
            return 1;
        if(kernel::strcmp(path, "kbd") == 0)
            return 0;

        // TODO: handle other devices
        return -1;
    }

    vnode* vn = kernel::get_vfs()->find_vnode(path);

    if(!vn){
        f = root_dev->load_file(path);
        if(!f){
            return -1;
        }
        vn = new vnode(f, FILE_TYPE_DATA);
        kernel::get_vfs()->add_vnode(vn);
    }

    filedes = new file_descriptor(vn);
    proc->add_filedes(filedes);

    return filedes->fd_num;
}


// https://pubs.opengroup.org/onlinepubs/009604599/functions/read.html
ssize_t _read(int fildes, void *buf, size_t nbyte){
    if(nbyte == 0){
        kernel::get_scheduler()->run();
        return 0;
    }
    
    Process* proc = kernel::get_scheduler()->get_cur_proc();
    file_descriptor* fd = proc->get_filedes(fildes);
    if(!fd){
        kernel::get_scheduler()->run();
        return -1;
    }

    if(fildes == 0){
        while(proc != kernel::get_scheduler()->get_fg_proc()){
            __asm("nop  \n");
        }
    }

    // TODO: handle devices

    // TODO: what should we do if file is not already opened??

    File* f = fd->vn_ptr->f;

    if(nbyte > SSIZE_MAX){
        // ...
        kernel::get_scheduler()->run();
        return -1;
    }

    // TODO: handle read from pipe/FIFO
    // TODO: handle O_NONBLOCK flag
    // TODO: ingterrupted

    if(fd->cursor > fd->vn_ptr->f->get_size()){
        kernel::get_scheduler()->run();
        return 0;
    }

    kernel::size_t i = 0;
    if(f->exists_on_disk()){
        kernel::size_t blk_sz = kernel::get_root_drive()->get_blk_sz();
        kernel::size_t n_blk = fd->cursor / blk_sz;
        kernel::size_t idx = fd->cursor % blk_sz;

        kernel::u8* temp_buf = (kernel::u8*)kernel::malloc(blk_sz);
        kernel::u8* dest_buf = (kernel::u8*)buf;
        kernel::get_root_drive()->read_nfile_chunk(temp_buf, f, n_blk++);

        for(i = 0; i < nbyte && i < f->get_size(); i++, idx++){
            if(idx > blk_sz){
                idx = 0;
                kernel::get_root_drive()->read_nfile_chunk(temp_buf, f, n_blk++);
            }
            dest_buf[i] = temp_buf[idx];
        }

        kernel::free(temp_buf);
    }

    else {
        kernel::u8* src_buf = (kernel::u8*)buf;
        kernel::u8* dat_ptr = fd->vn_ptr->f->get_data_ptr();

        for(i = 0; i < nbyte; i++){
            src_buf[i] = dat_ptr[i];
        }
    }

    fd->cursor += i;

    kernel::get_scheduler()->run();
    return i;
}

// https://pubs.opengroup.org/onlinepubs/009604599/functions/lseek.html
off_t _lseek(int fildes, off_t offset, int whence){
    Process* proc = kernel::get_scheduler()->get_cur_proc();
    file_descriptor* fd = proc->get_filedes(fildes);

    if(fildes == 0){
        while(proc != kernel::get_scheduler()->get_fg_proc()){
            __asm("nop  \n");
        }
    }

    if(!fd){
        kernel::get_scheduler()->run();
        return -1;
    }

    // TODO: handle device
    switch(whence){
        case SEEK_SET: {
            fd->cursor = offset;

            if(fildes == 0){
                kernel::get_keyboard()->set_inp_cursor(offset);
            }

            kernel::get_scheduler()->run();
            return fd->cursor;
        }

        case SEEK_CUR: {
            fd->cursor += offset;
            kernel::get_scheduler()->run();
            return fd->cursor;
        }

        case SEEK_END: {
            kernel::get_scheduler()->run();
            return -1;
        }

        default:
            kernel::get_scheduler()->run();
            return -1;
    }
}

// https://pubs.opengroup.org/onlinepubs/007904875/functions/write.html
ssize_t _write(int fildes, const void *buf, size_t nbyte){
    // TODO: deal with devices later
    
    Process* proc = kernel::get_scheduler()->get_cur_proc();
    file_descriptor* fd = proc->get_filedes(fildes);

    if(fildes <= 3){
        while(proc != kernel::get_scheduler()->get_fg_proc()){
            __asm("nop  \n");
        }
    }

    if(!fd){
        return -1;
    }

    // TODO: handle pipe/FIFO
    // TODO: handle O_NONBLOCK flag
    // TODO: interrupted

    File* f = fd->vn_ptr->f;

    if(nbyte > SSIZE_MAX){
        // ...
        return -1;
    }

    if(fd->cursor > f->get_size()){
        kernel::get_scheduler()->run();
        return -1;
    }

    kernel::size_t i = 0;
    if(f->exists_on_disk()){
        kernel::size_t blk_sz = kernel::get_root_drive()->get_blk_sz();
        kernel::size_t n_blk = fd->cursor / blk_sz;
        kernel::size_t idx = fd->cursor % blk_sz;

        kernel::u8* temp_buf = (kernel::u8*)kernel::malloc(blk_sz);
        kernel::u8* src_buf = (kernel::u8*)buf;
        kernel::get_root_drive()->read_nfile_chunk(temp_buf, f, n_blk);

        for(i = 0; i < nbyte && i < f->get_size(); i++, idx++){
            if(idx >= blk_sz-1){
                idx = 0;
                kernel::get_root_drive()->write_nfile_chunk(temp_buf, f, n_blk++);
                kernel::get_root_drive()->read_nfile_chunk(temp_buf, f, n_blk);
            }
            temp_buf[idx] = src_buf[i];
        }
        
        kernel::get_root_drive()->write_nfile_chunk(temp_buf, f, n_blk);

        kernel::free(temp_buf);
    }

    else {
        kernel::u8* src_buf = (kernel::u8*)buf;

        if(fildes == 1 || fildes == 2){
            src_buf[nbyte] = 0;
            kernel::get_serial_device()->puts((char*)src_buf);
            return nbyte;
        }

        kernel::u8* dat_ptr = fd->vn_ptr->f->get_data_ptr();

        for(i = 0; i < nbyte; i++){
            dat_ptr[i] = src_buf[i];
        }
    }

    fd->cursor += i;
    return i;
}


int _close(int fildes){
    if(fildes <= 3)
        return -1;
    Process* proc = kernel::get_scheduler()->get_cur_proc();
    proc->remove_filedes(fildes);
    return 0;
}

void _exit(int status){

    Process* proc = kernel::get_scheduler()->get_cur_proc();
    Process* parent = proc->get_parent();

    process_list children = proc->get_children();
    kernel::size_t nchildren = children->get_size();

    for(kernel::size_t i = 0; i < nchildren; i++){
        parent->add_child(children->get_idx(i));
    }

    if(kernel::get_scheduler()->get_fg_proc() == proc)
        kernel::get_scheduler()->set_fg_proc(proc->get_parent());
    
    proc->set_state(P_STATE_EXITED);


    while(1){
        __asm("nop  \n");
    }
}

pid_t _fork(void){
    return -1;
}

pid_t _create_process(void (*entry)()){
    kernel::get_scheduler()->pause();

    Process* cur_proc = kernel::get_scheduler()->get_cur_proc();
    Process* proc = new Process(cur_proc, entry);

    cur_proc->add_child(proc);
    proc->set_parent(cur_proc);

    kernel::get_taskmgr()->add_process(proc);
    kernel::get_scheduler()->run();

    return proc->get_id();
}

pid_t _exec(const char *pathname){
    Drive* root = kernel::get_root_drive();
    File* exe = root->load_file(pathname);

    if(!exe){
        return -1;
    }

    kernel::u8* buf = (kernel::u8*)kernel::malloc(512);
    root->read_nfile_chunk(buf, exe, 0);

    void* entry = (void*)(
        (buf[0] << 24)+
        (buf[1] << 16)+
        (buf[2] << 8)+
        (buf[3])
    );

    kernel::free(buf);

    return _create_process((void (*)())entry);
}

pid_t _waitpid(pid_t pid, int *wstatus, int options){
    proc_list_t rdylist = kernel::get_taskmgr()->ready_list;
    kernel::size_t sz = rdylist->get_size();

    Process* p = nullptr;

    for(kernel::size_t i = 0; i < sz; i++){
        if((int)rdylist->get_idx(i)->get_id() == pid){
            p = rdylist->get_idx(i);
            break;
        }
    }

    if(!p){
        return -1;
    }

    while(p->get_state() != P_STATE_EXITED);

    delete p;
    return pid;
}

void _reload_dirs(){
    Drive* root = kernel::get_root_drive();
    fnamelist->get_internal_arr()->clear();
    root->getfilelist(fnamelist);
}

int _readdirent(const char* dirname, char* buf){
    Process* cur_proc = kernel::get_scheduler()->get_cur_proc();
    Drive* root = kernel::get_root_drive();

    if(fnamelist == nullptr){
        fnamelist = new kernel::Vector<char*>();
        root->getfilelist(fnamelist);
    }
    

    kernel::size_t sz = fnamelist->get_size();

    if(cur_proc->get_readdir_no() > sz){
        return -1;
    }
    
    kernel::size_t idx = -1;
    for(kernel::size_t i = 0; i < sz; i++){
        char* cur_name = fnamelist->get_idx(i);
        if(kernel::strncmp(dirname, cur_name, kernel::strlen(dirname)) == 0){
            idx++;
        }

        if(idx == cur_proc->get_readdir_no()){
            kernel::strcpy(buf, cur_name);
            cur_proc->set_readdir_no(idx+1);
            return 0;
        }
    }
    cur_proc->set_readdir_no(0);
    return -1;
}

int _chdir(const char *path){
    Process* cur_proc = kernel::get_scheduler()->get_cur_proc();
    Drive* root = kernel::get_root_drive();

    if(fnamelist == nullptr){
        fnamelist = new kernel::Vector<char*>();
        root->getfilelist(fnamelist);
    }

    kernel::size_t sz = fnamelist->get_size();

    for(kernel::size_t i = 0; i < sz; i++){
        char* cur_name = fnamelist->get_idx(i);
        if(kernel::strncmp(path, cur_name, kernel::strlen(path)) == 0){
            cur_proc->set_cur_dir(path);
            return 0;
        }
    }
    return -1;
}

char* _getcwd(char *buf, size_t size){
    if(!buf)
        return nullptr;

    Process* cur_proc = kernel::get_scheduler()->get_cur_proc();
    kernel::strncpy(buf, cur_proc->get_cur_dir(), size);
    return buf;
}

int _access(const char *pathname, int mode){
    Drive* root = kernel::get_root_drive();

    if(fnamelist == nullptr){
        fnamelist = new kernel::Vector<char*>();
        root->getfilelist(fnamelist);
    }

    kernel::size_t sz = fnamelist->get_size();

    for(kernel::size_t i = 0; i < sz; i++){
        char* cur_name = fnamelist->get_idx(i);
        if(kernel::strcmp(pathname, cur_name) == 0){
            return 0;
        }
    }
    return -1;
}

int _set_fg_pid(pid_t pid){
    kernel::get_taskmgr()->set_fg_proc(pid);
    return 0;
}

pid_t _wait(int *wstatus){
    return -1;
}

int _waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options){
    return -1;
}
