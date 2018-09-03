/*
 * coderunstartup
 *
 * Support for running code on startup
 *
 * Copyright (C) 2017/2018 BaselSayeh.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(_ARM64_CODERUNSTARTUP_H)
#define _ARM64_CODERUNSTARTUP_H

#define CODERUNSTARTUP_MAGIC 0xC0DEC0DE

#define CODERUNSTARTUP_SIZE SZ_64M

// Coderunstartup: for Huawei P9.
#define CODERUNSTARTUP_ADDR UL(0x2A000000)

#endif