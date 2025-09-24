#include "common/common.h"
#include "port_rp2040.h"

// Run the next command to check prints:
// $ minicom -b 115200 -o -D /dev/ttyACM0

int main(void)
{
    // Initialize standard input/output
    stdio_init_all();

    error err;

    int a0 = -1;
    err = rp2040_setup(a0);
    if (err != 0)
    {
        printf("Error in setup.\n");
        return -1;
    }

    err = PCA9555_app_routine();

    if (err != 0)
    {
        printf("Error in setup.\n");
    }
}