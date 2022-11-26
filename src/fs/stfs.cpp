#include <fs/stfs.h>
#include <fs/fs.h>
#include <libk/string.h>

File* StFS::load_from_disk(Disk* d, const char* path){
    kernel::i32 first_sect = this->find_first_sector(d, path);
    kernel::u8* buf = (kernel::u8*)kernel::malloc(d->get_blk_sz());
    File* f_data = nullptr;

    if(first_sect == -1)
        return nullptr;


    d->read_nblk(buf, first_sect);

    if(get_sig(buf) != StFS_SIGNATURE){
        // TODO: set internel kernel ERRNO
        kernel::free(buf);
        return nullptr;
    }

    f_data = new File(
        this->get_name(buf),
        this->get_size(buf),
        true,
        nullptr,
        first_sect
    );

    kernel::free(buf);
    return f_data;
}

void StFS::format_disk(Disk* d){
    kernel::u8* buf = (kernel::u8*)kernel::malloc(d->get_blk_sz());
    this->set_sig(buf);
    this->set_next_alloc(buf, false);
    this->set_name(buf, "/root");
    d->write_nblk(buf, 0);
    kernel::free(buf);
}


kernel::i32 StFS::find_first_sector(Disk* d, const char* name){
    REQUIRES(d != nullptr);

    kernel::u8* buf = (kernel::u8*)kernel::malloc(d->get_blk_sz());

    kernel::size_t nblk = 0;
    kernel::size_t jump_sz = StFS_MAX_FILE_SIZE/(d->get_blk_sz());
    d->read_nblk(buf, nblk);

    if(kernel::strcmp(name, this->get_name(buf)) == 0){
        return nblk;
    }

    while(this->get_next_alloc(buf)){
        nblk += jump_sz;
        d->read_nblk(buf, nblk);
        if(kernel::strcmp(name, this->get_name(buf)) == 0){
            return nblk;
        }
    }

    kernel::free(buf);
    
    return -1;
}

// TODO: check if we run out of space on disk
kernel::i32 StFS::find_free_block_on_disk(Disk* d){
    kernel::u8* buf = (kernel::u8*)kernel::malloc(d->get_blk_sz());

    kernel::size_t nblk = 0;
    kernel::size_t jump_sz = StFS_MAX_FILE_SIZE/(d->get_blk_sz());
    d->read_nblk(buf, nblk);

    while(this->get_next_alloc(buf)){
        nblk += jump_sz;
        d->read_nblk(buf, nblk);
    }

    kernel::free(buf);
    return nblk + jump_sz;
}

// TODO: parse path
kernel::i32 StFS::create_file_on_disk(Disk* d, File* f){
    REQUIRES(this->find_first_sector(d, f->get_name()) == -1);

    kernel::i32 f_sz = f->get_size();
    kernel::size_t blk_sz = d->get_blk_sz();

    if(f_sz > StFS_MAX_FILE_SIZE)
        return -1;

    kernel::size_t free_blk = this->find_free_block_on_disk(d);
    kernel::size_t nchunks = (f_sz + blk_sz - 1)/blk_sz;

    kernel::u8* temp_buf = (kernel::u8*)kernel::malloc(blk_sz + sizeof(StFS_FILE_HEADER));
    this->set_name(temp_buf, f->get_name());
    this->set_size(temp_buf, f->get_size());
    this->set_sig(temp_buf);
    this->set_next_alloc(temp_buf, false);
    this->set_prev_alloc(d, free_blk, true);
    temp_buf += sizeof(StFS_FILE_HEADER);

    f->set_start_chunk(free_blk);

    bool is_first_blk = true;

    if(nchunks == 0)
        nchunks = 1;

    for(kernel::size_t i = 0; i < nchunks; i++){
        kernel::memcpy(
            temp_buf,
            f->get_data_ptr() + (blk_sz*i),
            (int)f_sz > (int)blk_sz ? blk_sz : f_sz 
        );

        if(is_first_blk){
            is_first_blk = false;
            temp_buf -= sizeof(StFS_FILE_HEADER);
        }

        d->write_nblk(temp_buf, free_blk + i);

        f_sz -= blk_sz;

        if((int)f_sz < (int)blk_sz)
            break;
    }

    kernel::free(temp_buf);
    return 0;
}

kernel::size_t StFS::get_sig(kernel::u8* buf){
    REQUIRES(buf != nullptr);
    return ((StFS_FILE_HEADER*)buf)->signature;
}

void StFS::set_sig(kernel::u8* buf){
    ((StFS_FILE_HEADER*)buf)->signature = StFS_SIGNATURE;
}

bool StFS::get_next_alloc(kernel::u8* buf){
    REQUIRES(buf != nullptr);
    return ((StFS_FILE_HEADER*)buf)->is_next_allocated;
}

void StFS::set_next_alloc(kernel::u8* buf, bool x){
    ((StFS_FILE_HEADER*)buf)->is_next_allocated = x;
}

void StFS::set_prev_alloc(Disk* d, kernel::size_t blk, bool x){
    kernel::u8* buf = (kernel::u8*)kernel::malloc(d->get_blk_sz());

    //TODO: assuming 512 size blocks
    d->read_nblk(buf, blk-4);

    this->set_next_alloc(buf, true);

    d->write_nblk(buf, blk-4);
}

char* StFS::get_name(kernel::u8* buf){
    REQUIRES(buf != nullptr);
    return ((StFS_FILE_HEADER*)buf)->name;
}

// TODO: use strncpy????
void StFS::set_name(kernel::u8* buf, const char* name){
    kernel::u8 i = 0;

    for(; i < 20 && name[i] != '\0'; i++){
        ((StFS_FILE_HEADER*)buf)->name[i] = name[i];
    }

    ((StFS_FILE_HEADER*)buf)->name[i] = '\0';
}

kernel::size_t StFS::get_size(kernel::u8* buf){
    REQUIRES(buf != nullptr);

    kernel::size_t sz = ((StFS_FILE_HEADER*)buf)->size;

    ENSURES(sz <= StFS_MAX_FILE_SIZE);
    return sz;
}

void StFS::set_size(kernel::u8* buf, kernel::size_t sz){
    REQUIRES(buf != nullptr);
    REQUIRES(sz <= StFS_MAX_FILE_SIZE);

    ((StFS_FILE_HEADER*)buf)->size = sz;
}
