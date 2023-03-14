#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "version.h"
#include "ecat.h"
#include "slaveinfo.h"

const char* const interface = "eth0";

void printSlaveInformation(SlaveInfo* info)
{
    printw("PDI Control:        0x%04x\n", info->escInfo.pdiControl);
    printw("PDI Configuration:  0x%04x\n", info->escInfo.pdiConfiguration);
    printw("Sync Impulse:       0x%04x\n", info->escInfo.syncImpulse);
    printw("PDI Configuration2: 0x%04x\n", info->escInfo.pdiConfig2);
    printw("StationAlias:       0x%04x\n", info->escInfo.stationAlias);
    printw("Checksum Eeprom:    0x%04x\n", info->escInfo.checksum);
    printw("Checksum calc:      0x%04x\n\n", Ecat::SIIcrc(info));
    printw("Vendor ID:          0x%08x\n", info->identity.vendorId);
    printw("Product Code:       0x%08x\n", info->identity.productCode);
    printw("Revision Number:    0x%08x\n", info->identity.revisionNumber);
    printw("Serial Number:      0x%08x\n\n", info->identity.serialNumber);
    printw("Bootmbx RX Offset:  0x%04x\n", info->bootstrapMailbox.receiveOffset);
    printw("Bootmbx RX Size:    0x%04x\n", info->bootstrapMailbox.receiveSize);
    printw("Bootmbx TX Offset:  0x%04x\n", info->bootstrapMailbox.sendOffset);
    printw("Bootmbx TX Size:    0x%04x\n", info->bootstrapMailbox.sendSize);
    printw("Mailbox RX Offset:  0x%04x\n", info->mailbox.receiveOffset);
    printw("Mailbox RX Size:    0x%04x\n", info->mailbox.receiveSize);
    printw("Mailbox TX Offset:  0x%04x\n", info->mailbox.sendOffset);
    printw("Mailbox TX Size:    0x%04x\n", info->mailbox.sendSize);
    printw("Mailbox Protocols:  0x%04x\n\n", info->mailbox.mailboxProtocol);
    printw("SII Size:           %d byte(s)\n", (info->siiSize.size + 1) * 128);
    printw("SII Version:        %d\n", info->siiSize.version);
}

int main()
{
    initscr();
    printw("You are running Version %i.%02d.%02d of EctPi\n", MAJOR_VERSION, MINOR_VERSION, HOTFIX_NUMBER);
    Ecat* bus = new Ecat(interface);
    if(bus->is_initialized())   printw("Bus initialized\n");
    else                        printw("Bus is not initialized\n");

    int slaves = bus->search();
    printw("Bus scan found %d slave(s)\n", slaves);
    if(slaves)
    {
        printw("\npress any key to display info for slave 1");
        getch();
        clear();
        SlaveInfo* info = new SlaveInfo();
        if(info)
        {
            for(int i = 1; i <= slaves; i++)
            {
                printw("Slave %d:\n\n", i);
                int length = bus->getSlaveInfo(i, info, sizeof(SlaveInfo));
                printSlaveInformation(info);
                if(i < slaves)
                {
                    printw("\npress any key to display info for slave %d\n", i+1);
                    getch();
                    clear();
                }
            }
        }
        delete info;
    }
    printw("\nPress any key to exit!\n");
    getch();
    delete bus;
    endwin();
    return 0;
}

