/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __SOC_BROADCOM_CYGNUS_SDRAM_H__
#define __SOC_BROADCOM_CYGNUS_SDRAM_H__

#include <stdint.h>

void ddr_init2(void);
void sdram_init(void);
uint32_t sdram_size_mb(void);

#endif	/* __SOC_BROADCOM_CYGNUS_SDRAM_H__ */