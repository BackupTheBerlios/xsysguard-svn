/*
 * This file is part of xsysguard <http://xsysguard.berlios.de>
 * Copyright 2005 Sascha Wessel <wessel@users.berlios.de>
 *
 * Xsysguard is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Xsysguard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xsysguard; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include "libsg.h"
#include "common.h"

/******************************************************************************/

boolean debug = true;
unsigned int buflen = INITBUFLEN;
unsigned long long tick = 1000;
unsigned long long count = -1;

var_t *var_list = NULL;
unsigned int var_list_len = 0;

static file_t *file_list = NULL;
static unsigned int file_list_len = 0;

/******************************************************************************/

void add_file(unsigned long long mult, char *name) {
	unsigned int i;
	file_t *file = NULL;
	char *buffer = NULL;

	// find matching entry in file_list
	for (i=0; i < file_list_len; i++) {
		file = &(file_list[i]);
		if (strcmp(file->name, name) == 0) {
			// is mult a multiple of file->mult?
			if (mult % file->mult == 0)
				return;
			// is file->mult a multiple of mult?
			if (file->mult % mult == 0) {
				file->mult = mult;
				return;
			}
			buffer = file->buffer;
		}
	}

	// no buffer for file found?
	if (!buffer)
		buffer = (char *) smalloc(buflen);

	// add new entry
	file_list_len++;
	file_list = (file_t *) srealloc((void *) file_list, sizeof(file_t) * file_list_len);
	file = &(file_list[file_list_len - 1]);
	file->mult = mult;
	file->name = (char *) smalloc(strlen(name) + 1);
	strcpy(file->name, name);
	file->buffer = buffer;
}

void read_files() {
	int fd;
	unsigned int i;
	file_t *file;

	for (i=0; i < file_list_len; i++) {
		file = &(file_list[i]);
		if ((count % file->mult) != 0)
			continue;
		if ((fd = open(file->name, O_RDONLY, 0)) < 0) {
			debugf("cannot open file: %s", file->name);
			continue;
		}
		memset(file->buffer, '\0', buflen);
		if (read(fd, file->buffer, buflen-1) == buflen-1)
			debugf("buffer too small for file: %s", file->name);
		close(fd);
	}
}

/******************************************************************************/

void *get_date(var_t *var) {
	size_t len;
	time_t curtime;
	struct tm *loctime;

	curtime = time(NULL);
	loctime = localtime(&curtime);
	len = strftime(var->var, buflen, var->format, loctime);
	if (!len)
		return NULL;
	return var->var;
}

/******************************************************************************/

void *get_string(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (char *) var->var)))
		return NULL;
	return var->var;
}

void *get_char(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (char *) var->var)))
		return NULL;
	if (var->is_counter) {
		char tmp = *(char *) var->var;
		*(char *) var->var -= *(char *) var->last;
		*(char *) var->last = tmp;
	}
	return var->var;
}

void *get_uchar(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (unsigned char *) var->var)))
		return NULL;
	if (var->is_counter) {
		unsigned char tmp = *(unsigned char *) var->var;
		*(unsigned char *) var->var -= *(unsigned char *) var->last;
		*(unsigned char *) var->last = tmp;
	}
	return var->var;
}

void *get_short(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (short *) var->var)))
		return NULL;
	if (var->is_counter) {
		short tmp = *(short *) var->var;
		*(short *) var->var -= *(short *) var->last;
		*(short *) var->last = tmp;
	}
	return var->var;
}

void *get_ushort(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (unsigned short *) var->var)))
		return NULL;
	if (var->is_counter) {
		unsigned short tmp = *(unsigned short *) var->var;
		*(unsigned short *) var->var -= *(unsigned short *) var->last;
		*(unsigned short *) var->last = tmp;
	}
	return var->var;
}

void *get_int(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (int *) var->var)))
		return NULL;
	if (var->is_counter) {
		int tmp = *(int *) var->var;
		*(int *) var->var -= *(int *) var->last;
		*(int *) var->last = tmp;
	}
	return var->var;
}

void *get_uint(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (unsigned int *) var->var)))
		return NULL;
	if (var->is_counter) {
		unsigned int tmp = *(unsigned int *) var->var;
		*(unsigned int *) var->var -= *(unsigned int *) var->last;
		*(unsigned int *) var->last = tmp;
	}
	return var->var;
}

void *get_long(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (long *) var->var)))
		return NULL;
	if (var->is_counter) {
		long tmp = *(long *) var->var;
		*(long *) var->var -= *(long *) var->last;
		*(long *) var->last = tmp;
	}
	return var->var;
}

void *get_ulong(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (unsigned long *) var->var)))
		return NULL;
	if (var->is_counter) {
		unsigned long tmp = *(unsigned long *) var->var;
		*(unsigned long *) var->var -= *(unsigned long *) var->last;
		*(unsigned long *) var->last = tmp;
	}
	return var->var;
}

void *get_longlong(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (long long *) var->var)))
		return NULL;
	if (var->is_counter) {
		long long tmp = *(long long *) var->var;
		*(long long *) var->var -= *(long long *) var->last;
		*(long long *) var->last = tmp;
	}
	return var->var;
}

void *get_ulonglong(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (unsigned long long *) var->var)))
		return NULL;
	if (var->is_counter) {
		unsigned long long tmp = *(unsigned long long *) var->var;
		*(unsigned long long *) var->var -= *(unsigned long long *) var->last;
		*(unsigned long long *) var->last = tmp;
	}
	return var->var;
}

void *get_float(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (float *) var->var)))
		return NULL;
	if (var->is_counter) {
		float tmp = *(float *) var->var;
		*(float *) var->var -= *(float *) var->last;
		*(float *) var->last = tmp;
	}
	return var->var;
}

void *get_double(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (double *) var->var)))
		return NULL;
	if (var->is_counter) {
		double tmp = *(double *) var->var;
		*(double *) var->var -= *(double *) var->last;
		*(double *) var->last = tmp;
	}
	return var->var;
}

void *get_longdouble(var_t *var) {
	if (!(sscanf(var->filebuf, var->format, (long double *) var->var)))
		return NULL;
	if (var->is_counter) {
		long double tmp = *(long double *) var->var;
		*(long double *) var->var -= *(long double *) var->last;
		*(long double *) var->last = tmp;
	}
	return var->var;
}

/******************************************************************************/

int parse_scan_line(type_t *type, boolean *is_counter, char **destformat, char *format) {
	char buf[strlen(format)];
	char *fmt = format;
	char *c = buf;
	int size;

	*is_counter = false;
	if (!format)
		return 0;

	while (*fmt != '\0') {
		if (*fmt == '%') {
			*c++ = *fmt++;
			if (*fmt == '%') {
				*c++ = *fmt++;
				continue;
			}
			if (*fmt == '*') {
				*c++ = *fmt++;
				continue;
			}
			if (*fmt == '!') {
				fmt++;
				*is_counter = true;
			}
			break;
		} else {
			*c++ = *fmt++;
		}
	}

	while (isdigit((int) *fmt))
		*c++ = *fmt++;

	if (       string_starts_with(fmt, "[")
		|| string_starts_with(fmt, "s")
		|| string_starts_with(fmt, "c")) {
		*type = string_type;
		size = buflen;
	} else if (string_starts_with(fmt, "hhd")
		|| string_starts_with(fmt, "hhi")
		|| string_starts_with(fmt, "hhn")) {
		*type = char_type;
		size = sizeof(char);
	} else if (string_starts_with(fmt, "hho")
		|| string_starts_with(fmt, "hhu")
		|| string_starts_with(fmt, "hhx")) {
		*type = uchar_type;
		size = sizeof(unsigned char);
	} else if (string_starts_with(fmt, "hd")
		|| string_starts_with(fmt, "hi")
		|| string_starts_with(fmt, "hn")) {
		*type = short_type;
		size = sizeof(short);
	} else if (string_starts_with(fmt, "ho")
		|| string_starts_with(fmt, "hu")
		|| string_starts_with(fmt, "hx")) {
		*type = ushort_type;
		size = sizeof(unsigned short);
	} else if (string_starts_with(fmt, "d")
		|| string_starts_with(fmt, "i")
		|| string_starts_with(fmt, "n")) {
		*type = int_type;
		size = sizeof(int);
	} else if (string_starts_with(fmt, "o")
		|| string_starts_with(fmt, "u")
		|| string_starts_with(fmt, "x")) {
		*type = uint_type;
		size = sizeof(unsigned int);
	} else if (string_starts_with(fmt, "ld")
		|| string_starts_with(fmt, "li")
		|| string_starts_with(fmt, "ln")) {
		*type = long_type;
		size = sizeof(long);
	} else if (string_starts_with(fmt, "lo")
		|| string_starts_with(fmt, "lu")
		|| string_starts_with(fmt, "lx")) {
		*type = ulong_type;
		size = sizeof(unsigned long);
	} else if (string_starts_with(fmt, "Ld")
		|| string_starts_with(fmt, "Li")
		|| string_starts_with(fmt, "Ln")
		|| string_starts_with(fmt, "lld")
		|| string_starts_with(fmt, "lli")
		|| string_starts_with(fmt, "lln")) {
		*type = longlong_type;
		size = sizeof(long long);
	} else if (string_starts_with(fmt, "Lo")
		|| string_starts_with(fmt, "Lu")
		|| string_starts_with(fmt, "Lx")
		|| string_starts_with(fmt, "llo")
		|| string_starts_with(fmt, "llu")
		|| string_starts_with(fmt, "llx")) {
		*type = ulonglong_type;
		size = sizeof(unsigned long long);
	} else if (string_starts_with(fmt, "f")
		|| string_starts_with(fmt, "e")
		|| string_starts_with(fmt, "g")) {
		*type = float_type;
		size = sizeof(float);
	} else if (string_starts_with(fmt, "lf")
		|| string_starts_with(fmt, "le")
		|| string_starts_with(fmt, "lg")) {
		*type = double_type;
		size = sizeof(double);
	} else if (string_starts_with(fmt, "Lf")
		|| string_starts_with(fmt, "Le")
		|| string_starts_with(fmt, "Lg")
		|| string_starts_with(fmt, "llf")
		|| string_starts_with(fmt, "lle")
		|| string_starts_with(fmt, "llg")) {
		*type = longdouble_type;
		size = sizeof(long double);
	} else {
		*type = invalid_type;
		size = 0;
		debugf("cannot determine var type for: %s", format);
	}

	while (*fmt != '\0')
		*c++ = *fmt++;

	*c = '\0';
	*destformat = (char *) smalloc(strlen(buf)+1);
	strcpy(*destformat, buf);

	return size;
}

/******************************************************************************/

void set_date_var(var_t *var, unsigned int id, unsigned long long mult, char *format) {
	var->id = id;
	var->mult = mult;
	var->func = get_date;
	var->type = string_type;
	var->var = smalloc(buflen);
	var->format = smalloc(strlen(format) + 1);
	strcpy(var->format, format);
}

void set_file_var(var_t *var, unsigned int id, unsigned long long mult, char *filename, char *format) {
	file_t *file = NULL;
	unsigned int i, size;

	// find file in file_list
	for (i=0; i < file_list_len; i++) {
		file = &(file_list[i]);
		if (strcmp(file->name, filename) == 0)
			break;
	}

	var->id = id;
	var->mult = mult;
	var->filebuf = file->buffer;

	size = parse_scan_line(&var->type, &var->is_counter, &var->format, format);
	var->var = smalloc(size);
	var->last = smalloc(size);

	switch (var->type) {
		case string_type:
			var->func = get_string;
			memset(var->var, 0, size);
			memset(var->last, 0, size);
			break;
		case char_type:
			var->func = get_char;
			*(char *) var->var = 0;
			*(char *) var->last = 0;
			break;
		case uchar_type:
			var->func = get_uchar;
			*(unsigned char *) var->var = 0;
			*(unsigned char *) var->last = 0;
			break;
		case short_type:
			var->func = get_short;
			*(short *) var->var = 0;
			*(short *) var->last = 0;
			break;
		case ushort_type:
			var->func = get_ushort;
			*(unsigned short *) var->var = 0;
			*(unsigned short *) var->last = 0;
			break;
		case int_type:
			var->func = get_int;
			*(int *) var->var = 0;
			*(int *) var->last = 0;
			break;
		case uint_type:
			var->func = get_uint;
			*(unsigned int *) var->var = 0;
			*(unsigned int *) var->last = 0;
			break;
		case long_type:
			var->func = get_long;
			*(long *) var->var = 0;
			*(long *) var->last = 0;
			break;
		case ulong_type:
			var->func = get_ulong;
			*(unsigned long *) var->var = 0;
			*(unsigned long *) var->last = 0;
			break;
		case longlong_type:
			var->func = get_longlong;
			*(long long *) var->var = 0;
			*(long long *) var->last = 0;
			break;
		case ulonglong_type:
			var->func = get_ulonglong;
			*(unsigned long long *) var->var = 0;
			*(unsigned long long *) var->last = 0;
			break;
		case float_type:
			var->func = get_float;
			*(float *) var->var = 0.0;
			*(float *) var->last = 0.0;
			break;
		case double_type:
			var->func = get_double;
			*(double *) var->var = 0.0;
			*(double *) var->last = 0.0;
			break;
		case longdouble_type:
			var->func = get_longdouble;
			*(long double *) var->var = 0.0;
			*(long double *) var->last = 0.0;
			break;
		default:
			dief("invalid var type");
			break;
	}
}

unsigned int set_var(var_t *var, unsigned int id, unsigned long long mult, char *line) {
	static unsigned int lastid = 0;
	char buf0[strlen(line) + 1];
	char buf1[strlen(line) + 1];
	char buf2i[strlen(line) + 1];
	char *buf2 = buf2i;
	unsigned int ret = 0;
	int n = 0;

	if (id <= lastid)
		return 0;
	lastid = id;

	sscanf(line, "%[^: ]:%n", buf0, &n);
	if (n < 1)
		return 0;
	line += n;
	ret += n;
	n = 0;
	if ((n = parse_quoted_string(line, buf1))) {
		ret += n;
		if (line[n] == ':') {
			n++;
			ret++;
		} else {
			n = 0;
		}
	} else {
		if (sscanf(line, "%[^: ]:%n", buf1, &n) < 1)
			return 0;
		ret += n;
	}
	if (n < 1) {
		buf2 = NULL;
	} else {
		line += n;
		n = 0;
		if (!(n = parse_quoted_string(line, buf2)))
			if (sscanf(line, "%s%n", buf2, &n) < 1)
				return 0;
		ret += n;
	}
	if (strcmp(buf0, "file") == 0) {
		add_file(mult, buf1);
		set_file_var(var, id, mult, buf1, buf2);
	} else if (strcmp(buf0, "date") == 0) {
		set_date_var(var, id, mult, buf1);
	} else {
		add_stat(mult, buf0);
		set_stat_var(var, id, mult, buf0, buf1, buf2);
	}
	return ret;
}

