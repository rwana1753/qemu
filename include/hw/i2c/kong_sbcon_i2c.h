/*
 * GLADCI SBcon two-wire serial bus interface (I2C bitbang)
 *   a.k.a.
 * GLADCI I2C controller
 *
 * Copyright (c) 2006-2022 Thunder Software Technology Co., Ltd.
 * Copyright (c) 2022 Sukjin Kong <king.kong@thundersoft.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef HW_I2C_KONG_SBCON_H
#define HW_I2C_KONG_SBCON_H

#include "hw/sysbus.h"
#include "hw/i2c/bitbang_i2c.h"
#include "qom/object.h"

#define TYPE_KINGKONG_I2C "kingkong_i2c"
#define TYPE_KONG_SBCON_I2C TYPE_KINGKONG_I2C

typedef struct KongSbconI2CState KongSbconI2CState;
DECLARE_INSTANCE_CHECKER(KongSbconI2CState, KONG_SBCON_I2C,
                         TYPE_KONG_SBCON_I2C)

struct KongSbconI2CState {
    /*< private >*/
    SysBusDevice parent_obj;
    /*< public >*/

    MemoryRegion iomem;
    bitbang_i2c_interface bitbang;
    int out;
    int in;
};

#endif /* HW_I2C_KONG_SBCON_H */
