#include "ethercat.h"
#include "ecat.h"



Ecat::Ecat(const char* interface)
{
    initialized = false;
    slavesDetected = 0;
    if(ec_init(interface) > 0)
    {
        initialized = true;
    }
}

Ecat::~Ecat()
{
    initialized = false;
    ec_close();
}

int Ecat::search()
{
    int data = 0x0000;
    int wkc;
    if(!initialized)    goto out;

    wkc = ec_BRD(0x0000, ECT_REG_TYPE, sizeof(data), &data, EC_TIMEOUTSAFE);      /* detect number of slaves */
    if(wkc <= 0)        goto out;

    slavesDetected = wkc;


    return slavesDetected;
out:
    return 0;
}

int Ecat::getSlaveInfo(int slave, SlaveInfo* data, int datasize)
{
    if(datasize < 128)  return 0;
    if(slave > slavesDetected) return 0;
    uint8_t tmpData = 2;
    uint16_t eepromStatus = 0;
    ec_APWR((1- slave), ECT_REG_EEPCFG, sizeof(tmpData), &tmpData , EC_TIMEOUTRET);
    ec_APWR((1- slave), ECT_REG_EEPCFG, sizeof(tmpData), &tmpData , EC_TIMEOUTRET);
    ec_APRD((1- slave), ECT_REG_EEPSTAT, sizeof(eepromStatus), &eepromStatus, EC_TIMEOUTRET);
    eepromStatus = etohs(eepromStatus);
    if (eepromStatus & EC_ESTAT_R64)    // ready to read 8bytes
    {
        uint64* output = (uint64*)data;
        for (int i = 0 ; i < 128; i+=8)
        {
            *output = ec_readeepromAP((1 - slave), i >> 1, EC_TIMEOUTEEP);
            output++;
        }
    }
    else
    {
        uint32* output = (uint32*)data;
        for (int i = 0 ; i < 128; i+=4)
        {
            *output = ec_readeepromAP((1 - slave), i >> 1, EC_TIMEOUTEEP);
            output++;
        }
    }
    return 128;
}

uint16 Ecat::SIIcrc(SlaveInfo* info)
{
    int i;
    uint8 crc;
    uint8* buf = (uint8*)info;
    crc = 0xff;
    for( i = 0 ; i <= 13 ; i++ )
    {
        crc ^= *(buf++);
        for(int j = 0; j <= 7 ; j++ )
        {
            if(crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc = (crc << 1);
        }
    }
    return (uint16)crc;
}