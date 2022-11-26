#pragma once

#include <fs/file.h>
#include <fs/directory.h>
#include <device/disk.h>

enum FS_TYPE{
    FS_TYPE_UNKNOWN,
    FS_TYPE_StFS
};
namespace kernel{
    FS_TYPE detect_disk_fs(Disk* d);
};

class FS{
    public:
        FS();
        virtual kernel::i32 create_file_on_disk(Disk* d, File* f) = 0;

        virtual File* load_from_disk(Disk* d, const char* path) = 0;
        virtual void format_disk(Disk* d) = 0;
    protected:
};
