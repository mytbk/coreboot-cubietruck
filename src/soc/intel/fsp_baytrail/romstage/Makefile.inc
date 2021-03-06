#
# This file is part of the coreboot project.
#
# Copyright (C) 2013 Google Inc.
# Copyright (C) 2014 Sage Electronic Engineering, LLC.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc.
#

romstage-y += romstage.c
romstage-y += pmc.c
romstage-y += report_platform.c
romstage-$(CONFIG_ENABLE_BUILTIN_COM1) += uart.c
