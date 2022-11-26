#pragma once
#include <libk/stdint.h>
#include <libk/string.h>
#include <libk/mem.h>

enum FILE_TYPE{
    FILE_TYPE_DEVICE,
    FILE_TYPE_DATA,
    FILE_TYPE_DIR
};

class File{
    public:
        File(
            const char* name,
            kernel::size_t sz,
            bool exists_on_disk,
            kernel::u8* dat,
            kernel::i32 start_chunk
        )
        :   filesize(sz),
            eod(exists_on_disk),
            start_chunk(start_chunk),
            data_ptr(dat)
        {
            this->filename = (char*)kernel::malloc(kernel::strlen(name)+4);
            kernel::strcpy(this->filename, name);

            this->cur_chunk = 0;
        }

        ~File(){
            kernel::free(this->filename);
        }

        const char* get_name(){
            return this->filename;
        }

        kernel::size_t get_size(){
            return this->filesize;
        }

        kernel::u8* get_data_ptr(){
            return this->data_ptr;
        }

        bool exists_on_disk(){
            return this->eod;
        }

        void set_exists_on_disk(bool e){
            this->eod = e;
        }

        kernel::i32 get_start_chunk(){
            return this->start_chunk;
        }

        void set_start_chunk(kernel::size_t ch){
            this->start_chunk = ch;
        }
    private:
        kernel::size_t filesize;

        bool eod;
        // start block on disk
        kernel::i32 start_chunk;

        // current chunk loaded into memory
        kernel::i32 cur_chunk;

        // pointer to data in memory
        kernel::u8* data_ptr;
        
        char* filename;
};
