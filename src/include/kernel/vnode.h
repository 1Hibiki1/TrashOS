#pragma once
#include <fs/file.h>

struct vnode {
    vnode(File* f, FILE_TYPE typ){
        this->f = f;
        this->type = typ;
        this->refcnt = 0;
        this->cursor = 0;
    }

    File* f;
    FILE_TYPE type;
    kernel::size_t refcnt;
    kernel::size_t cursor;
};

struct file_descriptor{
    file_descriptor(vnode* vn)
    :   fd_num(0),
        vn_ptr(vn),
        cursor(0)
    {}
    
    kernel::size_t fd_num;
    vnode* vn_ptr;
    kernel::size_t cursor;
};
