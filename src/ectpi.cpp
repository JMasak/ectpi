#include <stdio.h>
#include "configure.h"
#include "ethercat.h"

int main()
{
    printf("Hello World!\n");
    printf("You are running Version %i.%i\n", EctPi_VERSION_MAJOR, EctPi_VERSION_MINOR);
    if(ec_init("eth0"))
    {
	printf("EtherCAT initialized\n");
        ec_close();
    }
    else
    {
        printf("Could not initialize EtherCAT\n");
    }
    return 0;
}
