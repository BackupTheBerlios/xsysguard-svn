/*
 * This file is part of xsysguard <http://xsysguard.berlios.de>
 * Copyright 2005 Sascha Wessel <wessel@users.berlios.de>
 *
 * xsysguard is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xsysguard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xsysguard; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef __COMMON_H__
#define __COMMON_H__

#include "util.h"

#define INITBUFLEN 8096

/******************************************************************************/

typedef enum {
	invalid_type = 0,
	string_type,
	char_type,
	uchar_type,
	short_type,
	ushort_type,
	int_type,
	uint_type,
	long_type,
	ulong_type,
	longlong_type,
	ulonglong_type,
	float_type,
	double_type,
	longdouble_type
} type_t;

typedef struct var_t_struct *var_p;
typedef struct var_t_struct {
	unsigned int id; // xsysguard: widget id / xsysguardd: variable id
	unsigned long long mult;
	type_t type;
	void *(*func)(var_p);
	char *format;
	char *filebuf; // only for file
	void *var;
	void *last; // only for counters
	boolean is_counter;
} var_t;

/******************************************************************************/

extern boolean debug;
extern unsigned int buflen;
extern unsigned long long tick;
extern unsigned long long count;

extern var_t *var_list;
extern unsigned int var_list_len;

/******************************************************************************/

void _debugf(char *format, ...); // defined in xsysguard.c / xsysguardd.c
void dief(char *format, ...);   // defined in xsysguard.c / xsysguardd.c

#define debugf(...) if (debug) _debugf(__VA_ARGS__)

/******************************************************************************/

typedef struct {
	unsigned long long mult;
	char *name;
	char *buffer;
} file_t;

/******************************************************************************/

void add_file(unsigned long long mult, char *name);
void read_files();

/******************************************************************************/

unsigned int set_var(var_t *var, unsigned int id, unsigned long long mult, char *line);

/******************************************************************************/

#endif // __COMMON_H__

