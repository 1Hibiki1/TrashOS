#include <device/drive.h>
#include <fs/stfs.h>

Drive::Drive(Disk* dsk)
:   disk(dsk)
{
    FS_TYPE typ = kernel::detect_disk_fs(dsk);
    this->fs_typ = typ;

    switch(typ){
        case FS_TYPE_StFS: {
            StFS* st = new StFS();
            this->fs = st;
            break;
        }

        default:
            this->fs = nullptr;
    }
}

kernel::i32 Drive::format(FS_TYPE typ){
    switch(typ){
        case FS_TYPE_StFS: {
            if(this->fs != nullptr)
                kernel::free(this->fs);

            StFS* st = new StFS();
            this->fs = st;

            st->format_disk(this->disk);

            break;
        }

        default:
            return -1;
    }
    return 0;
}

kernel::i32 Drive::read_nfile_chunk(kernel::u8* buf, File* f, kernel::size_t n){
    kernel::i32 start_chunk = f->get_start_chunk();
    REQUIRES(start_chunk != -1);

    this->disk->read_nblk(buf, start_chunk + n);

    // TODO: handle other file systems
    if(n == 0){
        kernel::memcpy(buf, buf+sizeof(StFS_FILE_HEADER), this->disk->get_blk_sz() - sizeof(StFS_FILE_HEADER));
    }

    return 0;
}

kernel::i32 Drive::write_nfile_chunk(kernel::u8* buf, File* f, kernel::size_t n){
    kernel::i32 start_chunk = f->get_start_chunk();
    REQUIRES(start_chunk != -1);

    if(n == 0){
        kernel::u8* temp_buf = (kernel::u8*)kernel::malloc(this->disk->get_blk_sz());
        this->disk->read_nblk(temp_buf, start_chunk);
        kernel::memcpy(temp_buf+sizeof(StFS_FILE_HEADER), buf, this->disk->get_blk_sz() - sizeof(StFS_FILE_HEADER));
        this->disk->write_nblk(temp_buf, start_chunk);
    }
    else
        this->disk->write_nblk(buf, start_chunk + n);


    return 0;
}

void Drive::getfilelist(kernel::Vector<char*>* l){
    kernel::size_t i = 0;
    bool nextalloc = true;

    char* name = nullptr;

    kernel::u8* buf = (kernel::u8*)kernel::malloc(512);

    while(nextalloc){
        this->disk->read_nblk(buf, i);
        StFS_FILE_HEADER* head = (StFS_FILE_HEADER*)buf;

        name = (char*)kernel::malloc(StFS_MAX_FILENAME_SIZE);
        kernel::strcpy(name, head->name);
        l->push(name);

        nextalloc = head->is_next_allocated;
        i += 4;
    }

    kernel::free(buf);
}

File* Drive::load_file(const char* name){
    File* f = (this->fs->load_from_disk(this->disk, name));
    return f;
}

kernel::i32 Drive::new_file(File* file){
    return this->fs->create_file_on_disk(this->disk, file);
}

void Drive::set_name(char name){
    this->name = name;
}

char Drive::get_name(){
    return this->name;
}
