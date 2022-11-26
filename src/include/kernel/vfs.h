#pragma once

#include <device/drive.h>
#include <libk/vector.h>
#include <kernel/vnode.h>

/*
 * - The VFS maintaines a list of drives, each named by an alphabet.
 * - Vnode table maintaines a list of all open files and devices, one entry
 *   for each.
 * - Each process had an open file descriptor table, which maintaines a list
 *   of file descriptors , which are just integers that map to an entry in 
 *   the vnode table.
 */

using drive_list_t = kernel::Vector<Drive*>;
using vnode_table_t = kernel::Vector<vnode*>;

class VFS{
    public:
        VFS();

        void mount_drive(Drive* d);
        void add_vnode(vnode* v);
        void remove_vnode(vnode* v);    // do a linear search? or use an idx
        vnode* find_vnode(const char* path);

    private:
        char cur_name = 'A';

        drive_list_t* drive_list;
        // open file table
        vnode_table_t* vnode_table;
};
