#include <fs/fs.h>
#include <fs/stfs.h>
#include <libk/string.h>

FS::FS(){}

FS_TYPE kernel::detect_disk_fs(Disk* d){
    kernel::u8* buf = (kernel::u8*)kernel::malloc(d->get_blk_sz());

    d->read_nblk(buf, 0);

    if(((StFS_FILE_HEADER*)buf)->signature == StFS_SIGNATURE){
        return FS_TYPE_StFS;
    }

    kernel::free(buf);
    return FS_TYPE_UNKNOWN;
}


