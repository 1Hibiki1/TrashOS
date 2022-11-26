#pragma once

#include <kernel/kernel.h>

#include <user/programs/tictactoe.h>
#include <user/programs/ls.h>

void init_disk(){
    Drive* root = kernel::get_root_drive();
    root->format(FS_TYPE_StFS);

    kernel::u8 data_buf[4] = {
        (kernel::u8)(((kernel::u32)(&ttt_main) & 0xFF000000) >> 24),
        (kernel::u8)(((kernel::u32)(&ttt_main) & 0xFF0000) >> 16),
        (kernel::u8)(((kernel::u32)(&ttt_main) & 0xFF00) >> 8),
        (kernel::u8)(((kernel::u32)(&ttt_main) & 0xFF) >> 0)
    };

    File* f = new File(
        "/bin/tictactoe",
        sizeof(data_buf),
        true,
        data_buf,
        -1
    );

    root->new_file(f);
    delete f;


    data_buf[0] = (kernel::u8)(((kernel::u32)(&ls_main) & 0xFF000000) >> 24);
    data_buf[1] = (kernel::u8)(((kernel::u32)(&ls_main) & 0xFF0000) >> 16);
    data_buf[2] = (kernel::u8)(((kernel::u32)(&ls_main) & 0xFF00) >> 8);
    data_buf[3] = (kernel::u8)(((kernel::u32)(&ls_main) & 0xFF) >> 0);

    f = new File(
        "/bin/ls",
        sizeof(data_buf),
        true,
        data_buf,
        -1
    );

    root->new_file(f);
    delete f;
}
