#pragma once
#include <stdint.h>
#include "slaveinfo.h"

class Ecat
{
    public:
                        Ecat(const char* interface);
                        ~Ecat();
            int         search();
            bool        is_initialized()    { return initialized; }
            int         getSlaveInfo(int slave, SlaveInfo* data, int datasize);
    static  uint16_t    SIIcrc(SlaveInfo* info);

    private:
            int         slavesDetected;
            bool        initialized;
};
