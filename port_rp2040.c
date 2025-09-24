#include <stdio.h>
#include "pca9555/common/common.h"
#include "port_rp2040.h"
#include "pca9555/pca9555.h"

#define SDA 6
#define SCL 7
#define i2c i2c1_inst

#define microseconds (1000)
#define milliseconds (1000 * microseconds)
#define seconds (1000LL * milliseconds)

#define DELAY (0.125) // seconds

HAL_Sleep sleep_nano;
HAL_I2C hal_i2c;
HAL_PinOut enable_a0;

PCA9555 i2c_device; // global variable, this is key for the driver to work in any app.

void sleep_nano_pico(int64_t nanoseconds)
{
    if (nanoseconds < 0)
    {
        panic("negative time value");
    }
    sleep_us((uint64_t)(nanoseconds / 1000));
}

void set_pin_a0(bool state)
{
    gpio_put(i2c_device._a0, state);
}

error i2c_tx_wrapper(uint16 i2c_address, byte *write_data, isize wlen, byte *read_data, isize rlen)
{
    bool hasWriteData = write_data != NULL && wlen > 0;
    bool hasReadData = read_data != NULL && rlen > 0;

    if (hasWriteData)
    {
        int result = i2c_write_blocking(&i2c, i2c_address, write_data, wlen, hasReadData);
        if (result < 0)
        {
            return -1; // Indicate an error
        }
    }

    if (hasReadData)
    {
        int result = i2c_read_blocking(&i2c, i2c_address, read_data, rlen, false);
        if (result < 0)
        {
            return -1; // Indicate an error
        }
    }

    return 0; // Success
}

error rp2040_setup(int a0)
{
    sleep_nano = &sleep_nano_pico;
    enable_a0 = &set_pin_a0;
    hal_i2c.tx = &i2c_tx_wrapper;

    // Sleep a tad bit to let connected PC reach USB.
    sleep_nano(5 * seconds);

    i2c_device = new_PCA9555(enable_a0, a0, hal_i2c);

    // Initialize the GPIO configuration arrays
    if (i2c_device._a0 > -1)
    {
        gpio_init(i2c_device._a0);
        gpio_set_dir(i2c_device._a0, GPIO_OUT);
        enable_a0(true);
    }

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico) -> hw pins 6 and 7.
    i2c_init(&i2c, 100 * 1000); // Initialize I2C0 with a baud rate of 100 kHz
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_set_function(SDA, GPIO_FUNC_I2C);

    gpio_pull_up(SCL);
    gpio_pull_up(SDA);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(SDA, SCL, GPIO_FUNC_I2C));

    printf("rp2040 setup OK.\n");

    sleep_nano(DELAY * seconds);

    return 0;
}

#define PIN_LED 7

error PCA9555_app_routine()
{
    error err;
    
    int is_responsive = PCA9555_begin(&i2c_device);

    if (is_responsive == 0) {
        printf("PCA9555 is responsive.\n");
    } else {
        printf("PCA9555 is not responsive.\n");
        return -1;
    }

    sleep_nano(DELAY * seconds);

    while(true){
        // Set pin 0 to output
        sleep_nano(DELAY * seconds);
        PCA9555_pin_mode(&i2c_device, PIN_LED, 0);  // 0 is OUTPUT mode

        sleep_nano(DELAY * seconds);
        // Write a HIGH state to pin 5
        PCA9555_write(&i2c_device, PIN_LED, 1);

        sleep_nano(DELAY * seconds);
        // Read the state of pin
        int state = PCA9555_read(&i2c_device, PIN_LED);
        printf("Pin %d state: %d\n", PIN_LED, state);

        sleep_nano(DELAY * seconds * 10);
        // Toggle pin state
        PCA9555_write(&i2c_device, PIN_LED, state ^ 1);

        sleep_nano(DELAY* seconds);
        // Read the state of pin again
        state = PCA9555_read(&i2c_device, PIN_LED);
        printf("Pin %d state after toggle: %d\n", PIN_LED, state);
        sleep_nano(DELAY * seconds * 10);
    }

    return err;
}