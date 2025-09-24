// Datasheet: https://www.nxp.com/docs/en/data-sheet/PCA9555.pdf

#ifndef IMPLEMENTATIONS_H
#define IMPLEMENTATIONS_H

#include "pca9555/common/common.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pca9555/pca9555.h"

extern HAL_Sleep sleep_nano;
extern HAL_I2C hal_i2c;
extern HAL_PinOut enable_a0;

extern void sleep_nano_pico(int64_t nanoseconds);
extern error i2c_tx_wrapper(uint16 i2c_address, byte *write_data, isize wlen, byte *read_data, isize rlen);
extern error rp2040_setup(int a0);
extern error PCA9555_app_routine();

#endif // IMPLEMENTATIONS_H
