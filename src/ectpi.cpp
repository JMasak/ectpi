#include <stdio.h>
#include <ncurses.h>
#include "version.h"
#include "ethercat.h"


struct alignas(8) EscConfiguration
{
    uint16_t pdiControl;
    uint16_t pdiConfiguration;
    uint16_t syncConfiguration;
    uint16_t extendedPdiConfiguration;
    uint16_t stationAlias;
    uint16_t reserved1;
    uint16_t reserved2;
    uint16_t checksum;
};

int eeprom_read(int slave, int start, int length, uint8_t* dst)
{
    uint16_t eepromStatus = 0;
    uint8_t tmpData = 2;

    if((ec_slavecount >= slave) && (slave > 0))
    {
        ec_APWR((1- slave), ECT_REG_EEPCFG, sizeof(tmpData), &tmpData , EC_TIMEOUTRET);
        tmpData = 0;
        ec_APWR((1- slave), ECT_REG_EEPCFG, sizeof(tmpData), &tmpData , EC_TIMEOUTRET);
        ec_APRD((1- slave), ECT_REG_EEPSTAT, sizeof(eepromStatus), &eepromStatus, EC_TIMEOUTRET);
        eepromStatus = etohs(eepromStatus);
        if (eepromStatus & EC_ESTAT_R64)    // ready to read 8bytes
        {
            uint64* output = (uint64*)dst;
            for (int i = start ; i < (start + length); i+=8)
            {
                *dst = ec_readeepromAP((1 - slave), i >> 1, EC_TIMEOUTEEP);
                //*dst = __builtin_bswap64(readBuffer);
                dst++;
            }
        }
        else
        {
            uint32* output = (uint32*)dst;
            for (int i = start ; i < (start + length); i+=4)
            {
                *dst = ec_readeepromAP((1 - slave), i >> 1, EC_TIMEOUTEEP);
                //*dst = __builtin_bswap32(readBuffer);
                dst++;
            }
        }
        return 1;
    }
    return 0;
}

void calc_crc(uint8 *crc, uint8 b)
{
   int j;
   *crc ^= b;
   for(j = 0; j <= 7 ; j++ )
   {
     if(*crc & 0x80)
        *crc = (*crc << 1) ^ 0x07;
     else
        *crc = (*crc << 1);
   }
}

uint16 SIIcrc(uint8 *buf)
{
   int i;
   uint8 crc;

   crc = 0xff;
   for( i = 0 ; i <= 13 ; i++ )
   {
      calc_crc(&crc , *(buf++));
   }
   return (uint16)crc;
}

int main()
{
    initscr();
    printw("You are running Version %i.%02d.%02d of EctPi\n", MAJOR_VERSION, MINOR_VERSION, HOTFIX_NUMBER);
    if(ec_init("eth0"))
    {
	    printw("EtherCAT initialized\n");
        EscConfiguration escConfig;
        int data = 0x0000;
        int wkc = ec_BRD(0x0000, ECT_REG_TYPE, sizeof(data), &data, EC_TIMEOUTSAFE);      /* detect number of slaves */
        if (wkc > 0)
        {
            ec_slavecount = wkc;
            printw("%d Slave(s) detected!\n", ec_slavecount);
            for(int i = 1; i <= ec_slavecount; i++)
            {
                if(eeprom_read(1, 0, sizeof(escConfig), (uint8_t*)&escConfig))
                {
                    printw("PDI Control:                0x%04X\n", escConfig.pdiControl);
                    printw("PDI Configuration:          0x%04X\n", escConfig.pdiConfiguration);
                    printw("Sync Configuration:         0x%04X\n", escConfig.syncConfiguration);
                    printw("extended PDI Configuration: 0x%04X\n", escConfig.extendedPdiConfiguration);
                    printw("Station Alias:              0x%04X\n", escConfig.stationAlias);
                    printw("Reserve:                    0x%04X\n", escConfig.reserved1);
                    printw("Reserve2:                   0x%04X\n", escConfig.reserved2);
                    printw("Checksum:                   0x%04X\n", escConfig.checksum);
                }
                else
                {
                    printw("could not read Eeprom for Slave %d\n", i);
                }
            }
            uint8_t buffer[128];
            if(eeprom_read(1, 0, sizeof(buffer), buffer))
            {
                printw("Buffer: 0x");
                for(int c = 0; c < sizeof(buffer); c++)
                {
                    printw("%02X", buffer[c]);
                }
                printw("\n");
            }
        }
        else
        {
            printw("No Slaves detected!\n");
        }
        ec_close();
    }
    else
    {
        printw("Could not initialize EtherCAT\n");
    }
    getch();
    endwin();
    return 0;
}
