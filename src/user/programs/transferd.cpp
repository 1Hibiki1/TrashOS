#include <kernel/kernel.h>
#include <kernel/syscall.h>
#include <string.h>

void transferd_main(){
    char* data = (char*)kernel::malloc(512);
    char* filedata = (char*)kernel::malloc(512);
    kernel::size_t data_idx = 0;
    kernel::size_t cur_idx = 0;

    Drive* root = kernel::get_root_drive();

    bool processing = false;
    while(1){
        size_t sz = kernel::get_wifi_module()->get_out_buf()->get_size();
        char* str = nullptr;
        if(sz > 0){
            for(kernel::size_t i = 0; i < sz; i++){
                data[data_idx++] = (kernel::get_wifi_module()->get_out_buf()->get_idx(i));
            }
            data[data_idx] = '\0';
            kernel::get_wifi_module()->get_out_buf()->clear();
            
            str = data;
        }

        kernel::size_t str_len = kernel::strlen(str);

        if(!processing && str != nullptr){
            if(str_len >= 8 && kernel::strncmp(&str[2], "CONNECT", kernel::strlen("CONNECT")) == 0){
                processing = true;
            }
        }

        if(processing && str != nullptr){
            if(str_len >= 7 && kernel::strncmp(&str[2], "CLOSED", kernel::strlen("CLOSED")) == 0){
                filedata[cur_idx] = '\0';
                cur_idx = 0;

                char* real_file_data = (char*)kernel::malloc(512);
                char name[20];
            
                kernel::memcpy(real_file_data, filedata, 512);

                kernel::size_t fname_end_idx = 0;

                while(real_file_data[fname_end_idx++] != '\n');
                filedata[fname_end_idx-1] = '\0';

                kernel::strcpy(name, "/transfers/");
                strcat(name, filedata);

                real_file_data += fname_end_idx;

                kernel::printf("\n\ncreating file ");
                kernel::printf(name);
                kernel::printf("...\n");

                File* f = new File(
                    (const char*)name,
                    kernel::strlen(real_file_data),
                    true,
                    (kernel::u8*)real_file_data,
                    -1
                );

                root->new_file(f);

                kernel::printf("\nfile ");
                kernel::printf(name);
                kernel::printf(" written\n\n");
                _reload_dirs();

                real_file_data -= fname_end_idx;
                kernel::free(real_file_data);

                processing = false;
            }
        }

        if(processing){

            char* start = str;

            while(*start != ':' && *start != '\0'){
                start++;
            }

            if(*start != '\0'){
                start++;
                kernel::size_t msglen = kernel::strlen(start);

                for(kernel::size_t i = 0; i < msglen; i++){
                    filedata[cur_idx++] = start[i];
                }
            }
        }

        data_idx = 0;
    }
}