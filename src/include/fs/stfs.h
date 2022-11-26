#pragma once

#include <fs/fs.h>
#include <device/disk.h>

/*
 *  STUPID FILE SYSTEM
 *  - created by me for obvious reasons
 *  - file sizes are fixed at 2k
 *  - out of that, sizeof(StFS_FILE_HEADER) bytes are used for metadata
 *  - yeah, its bad
 */

#define StFS_MAX_FILENAME_SIZE 20
#define StFS_SIGNATURE 0x57F5DEAD
#define StFS_MAX_FILE_SIZE 2048

struct StFS_FILE_HEADER{
    kernel::size_t signature;
    kernel::size_t size;
    char name[20];
    bool is_next_allocated;
};

class StFS : public FS{
    public:
        kernel::i32 create_file_on_disk(Disk* d,File* f) override;
        File* load_from_disk(Disk* d, const char* path) override;
        void format_disk(Disk* d);

    private:
        kernel::i32 find_free_block_on_disk(Disk* d);
        kernel::i32 find_first_sector(Disk* d, const char* name);


        bool get_next_alloc(kernel::u8* buf);
        void set_next_alloc(kernel::u8* buf, bool x);
        void set_prev_alloc(Disk* d, kernel::size_t blk, bool x);

        kernel::size_t get_sig(kernel::u8* buf);
        void set_sig(kernel::u8* buf);

        char* get_name(kernel::u8* buf);
        void set_name(kernel::u8* buf, const char* name);

        kernel::size_t get_size(kernel::u8* buf);
        void set_size(kernel::u8* buf, kernel::size_t sz);
};
