#include <kernel/vfs.h>
#include <libk/string.h>

VFS::VFS(){
    this->drive_list = new kernel::Vector<Drive*>();
    this->vnode_table = new kernel::Vector<vnode*>();
}


void VFS::mount_drive(Drive* d){
    d->set_name(this->cur_name++);
    this->drive_list->push(d);
}

void VFS::add_vnode(vnode* v){
    this->vnode_table->push(v);
}

vnode* VFS::find_vnode(const char* path){
    kernel::size_t vtab_sz = this->vnode_table->get_size();

    for(kernel::size_t i = 0; i < vtab_sz; i++){
        vnode* cur_vnode = this->vnode_table->get_idx(i);
        const char* cur_name = cur_vnode->f->get_name();
        if(kernel::strcmp(cur_name, path) == 0){
            return cur_vnode;
        }
    }

    return nullptr;
}
