#pragma once

#include <device/device.h>

class Keyboard : public GenericDevice{
    public:
        Keyboard();
        void reset();
        void disable();
        void enable();
    private:
};
