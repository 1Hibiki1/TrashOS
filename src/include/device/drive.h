#pragma once

#include <fs/fs.h>
#include <device/disk.h>

class Drive{
    public:
        Drive(Disk* dsk);
        void set_name(char name);
        char get_name();

        File* load_file(const char* name);
        kernel::i32 new_file(File* file);
        kernel::i32 format(FS_TYPE typ);
        kernel::i32 read_nfile_chunk(
            kernel::u8* buf, File* f, kernel::size_t n
        );
        kernel::i32 write_nfile_chunk(
            kernel::u8* buf, File* f, kernel::size_t n
        );

        kernel::size_t get_blk_sz(){
            return this->disk->get_blk_sz();
        }

        void getfilelist(kernel::Vector<char*>* l);

    private:
        Disk* disk;
        FS* fs;
        FS_TYPE fs_typ;
        char name;
};
