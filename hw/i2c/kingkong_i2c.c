/*
 * GLADCI SBCon two-wire serial bus interface (I2C bitbang)
 * a.k.a. GLADCI I2C controller
 *
 * Copyright (c) 2006-2022 Thunder Software Technology Co., Ltd.
 * Copyright (c) 2022 Sukjin Kong <king.kong@thundersoft.com>
 *
 * This file is derived from hw/i2c/versatile_i2c.c by Sukjing Kong
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "qemu/osdep.h"
#include "hw/i2c/kong_sbcon_i2c.h"
#include "hw/registerfields.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qom/object.h"

typedef KongSbconI2CState KingkongI2CState;
DECLARE_INSTANCE_CHECKER(KingkongI2CState, KINGKONG_I2C,
                         TYPE_KINGKONG_I2C)



REG32(CONTROL_GET, 0)
REG32(CONTROL_SET, 0)
REG32(CONTROL_CLR, 4)

#define SCL BIT(0)
#define SDA BIT(1)

static uint64_t kingkong_i2c_read(void *opaque, hwaddr offset,
                                   unsigned size)
{
    KingkongI2CState *s = (KingkongI2CState *)opaque;

    switch (offset) {
    case A_CONTROL_SET:
        return (s->out & 1) | (s->in << 1);
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%x\n", __func__, (int)offset);
        return -1;
    }
}

static void kingkong_i2c_write(void *opaque, hwaddr offset,
                                uint64_t value, unsigned size)
{
    KingkongI2CState *s = (KingkongI2CState *)opaque;

    switch (offset) {
    case A_CONTROL_SET:
        s->out |= value & 3;
        break;
    case A_CONTROL_CLR:
        s->out &= ~value;
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%x\n", __func__, (int)offset);
    }
    bitbang_i2c_set(&s->bitbang, BITBANG_I2C_SCL, (s->out & SCL) != 0);
    s->in = bitbang_i2c_set(&s->bitbang, BITBANG_I2C_SDA, (s->out & SDA) != 0);
}

static const MemoryRegionOps kingkong_i2c_ops = {
    .read = kingkong_i2c_read,
    .write = kingkong_i2c_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void kingkong_i2c_init(Object *obj)
{
    DeviceState *dev = DEVICE(obj);
    KingkongI2CState *s = KINGKONG_I2C(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    I2CBus *bus;

    bus = i2c_init_bus(dev, "i2c");
    bitbang_i2c_init(&s->bitbang, bus);
    memory_region_init_io(&s->iomem, obj, &kingkong_i2c_ops, s,
                          "kong_sbcon_i2c", 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const TypeInfo kingkong_i2c_info = {
    .name          = TYPE_KINGKONG_I2C,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(KingkongI2CState),
    .instance_init = kingkong_i2c_init,
};

static void kingkong_i2c_register_types(void)
{
    type_register_static(&kingkong_i2c_info);
}

type_init(kingkong_i2c_register_types)
