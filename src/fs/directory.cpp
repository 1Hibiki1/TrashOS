// Directory class is unused
// technically its just a type of file, so this was unneeded

#include <fs/directory.h>
#include <libk/string.h>

Directory::Directory(
    const char* name
)
:   dirname(name)
{
    num_files = 0;
    num_subdirs = 0;
    file_list = nullptr;
    subdir_list = nullptr;
}

void Directory::add_file(File* f){
    if(this->file_list == nullptr)
        file_list = new file_list_t();
    this->file_list->push(f);
    this->num_files++;
}

void Directory::add_subdir(Directory* d){
    if(this->subdir_list == nullptr)
        subdir_list = new dir_list_t();
    this->subdir_list->push(d);
    this->num_subdirs++;
}

File* Directory::get_file(const char* name){
    for(kernel::size_t i = 0; i < this->num_files; i++){
        File* cur_file = this->file_list->get_idx(i);
        if(kernel::strcmp(name, cur_file->get_name()) == 0){
            return cur_file;
        }
    }

    return nullptr;
}

Directory* Directory::get_subdir(const char* name){
    for(kernel::size_t i = 0; i < this->num_subdirs; i++){
        Directory* cur_dir = this->subdir_list->get_idx(i);
        if(kernel::strcmp(name, cur_dir->get_name()) == 0){
            return cur_dir;
        }
    }

    return nullptr;
}

const char* Directory::get_name(){
    return this->dirname;
}
