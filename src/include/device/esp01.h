#pragma once

#include <device/device.h>

class ESP01 : public GenericDevice{
    public:
        ESP01();
        ~ESP01();

        void send_command(const char* cmd);
        void wait_for_response();
        char* get_response();
    private:
};
