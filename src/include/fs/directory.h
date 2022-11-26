// unused

#pragma once

#include <libk/vector.h>
#include <fs/file.h>

class Directory;

using file_list_t = kernel::Vector<File*>;
using dir_list_t = kernel::Vector<Directory*>;
using file_list_ptr = kernel::Vector<File*>*;
using dir_list_ptr = kernel::Vector<Directory*>*;

class Directory{
    public:
        Directory(const char* name);

        // TODO: check if file/dir already exists
        void add_file(File* f);
        void add_subdir(Directory* d);
        File* get_file(const char* name);
        Directory* get_subdir(const char* name);
        const char* get_name();

    private:
        const char* dirname;
        kernel::size_t num_files;
        kernel::size_t num_subdirs;

        file_list_ptr file_list;
        dir_list_ptr subdir_list;
};
