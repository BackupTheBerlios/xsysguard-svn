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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include "util.h"
#include "libsg.h"
#include "common.h"

/******************************************************************************/

void _debugf(char *format, ...) {
	va_list args;
	char buffer[1024];
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);
	printf("0 %d DEBUG: %s\n", strlen(buffer)+7, buffer);
}

void dief(char *format, ...) {
	va_list args;
	char buffer[1024];
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);
	printf("0 %d ERROR: %s\n", strlen(buffer)+7, buffer);
	exit(1);
}

/******************************************************************************/

void send_var(type_t var_type, unsigned int var_id, void *val) {
	if (!val)
		return;
	switch (var_type) {
		case string_type:
			printf("%u %d %s\n", var_id, strlen((char *) val), (char *) val);
			break;
		case char_type:
			printf("%u %hhd\n", var_id, *(char *) val);
			break;
		case uchar_type:
			printf("%u %hhu\n", var_id, *(unsigned char *) val);
			break;
		case short_type:
			printf("%u %hd\n", var_id, *(short *) val);
			break;
		case ushort_type:
			printf("%u %hu\n", var_id, *(unsigned short *) val);
			break;
		case int_type:
			printf("%u %d\n", var_id, *(int *) val);
			break;
		case uint_type:
			printf("%u %u\n", var_id, *(unsigned int *) val);
			break;
		case long_type:
			printf("%u %ld\n", var_id, *(long *) val);
			break;
		case ulong_type:
			printf("%u %lu\n", var_id, *(unsigned long *) val);
			break;
		case longlong_type:
			printf("%u %lld\n", var_id, *(long long *) val);
			break;
		case ulonglong_type:
			printf("%u %llu\n", var_id, *(unsigned long long *) val);
			break;
		case float_type:
			printf("%u %f\n", var_id, (double) *(float *) val);
			break;
		case double_type:
			printf("%u %f\n", var_id, *(double *) val);
			break;
		case longdouble_type:
			printf("%u %Lf\n", var_id, *(long double *) val);
			break;
		default:
			debugf("Variable %u has unknown type %d", var_id, var_type);
	}
}

int main(int argc, char **argv) {
	int n, m;
	char *buffer;
	struct timeval time_out;
	boolean allow_set_buflen = true;
	var_t *var;
	unsigned int i;

	// force line buffering
	if (setvbuf(stdin, NULL, _IOLBF, 0) != 0)
		dief("setvbuf stdin failed");
	if (setvbuf(stdout, NULL, _IOLBF, 0) != 0)
		dief("setvbuf stdout failed");

	// read configuration from stdin
	buffer = (char *) smalloc(buflen);
	while (1) {
		if (fgets(buffer, buflen, stdin) == NULL) {
			if (ferror(stdin))
				dief("error while reading from stdin");
			if (feof(stdin))
				dief("eof while reading from stdin");
			dief("unknown error while reading from stdin");
		}
		if (strlen(buffer) >= buflen-1)
			dief("configuration line too long for buffer");
		if (sscanf(buffer, "set tick %llu", &tick) == 1) {
			debugf("tick set to %llu", tick);
			continue;
		}
		if (sscanf(buffer, "set buflen %u", &buflen) == 1) {
			if (!allow_set_buflen) {
				debugf("setting buflen not allowed at this point");
				continue;
			}
			buffer = (char *) srealloc((void *) buffer, buflen);
			debugf("buflen set to %u", buflen);
			continue;
		}
		if ((n = string_starts_with(buffer, "get"))) {
			unsigned int id;
			unsigned long long mult;
			if (sscanf(buffer+n, "%u %llu %n", &id, &mult, &m) != 2)
				dief("cannot parse line: %s", buffer);
			var_list_len++;
			var_list = (var_t *) srealloc((void *) var_list, sizeof(var_t) * var_list_len);
			var = &(var_list[var_list_len - 1]);
			if (!(set_var(var, id, mult, buffer + m + n)))
				dief("cannot parse line: %s", buffer);
			allow_set_buflen = false;
			continue;
		}
		if (string_starts_with(buffer, "start"))
			break;
		dief("cannot parse line: %s", buffer);
	}
	free(buffer);

	// init libstatgrab
	init_stats();

	// read all files (due to counters)
	read_files();
	for (i=0; i < var_list_len; i++) {
		var = &(var_list[i]);
		(*(var->func))(var);
	}

	count++; // => count = 0

	// start main loop
	time_out.tv_sec = tick / 1000;
	time_out.tv_usec = (tick % 1000) * 1000;
	while (1) {
		void *val;
		struct timeval time_start, time_end, time_diff, time_sleep;
		sgettimeofday(&time_start, NULL);
		debugf("tick %llu", count);
		read_stats();
		read_files();

		for (i=0; i < var_list_len; i++) {
			var = &(var_list[i]);
			if ((count % var->mult) == 0) {
				val = (*(var->func))(var);
				send_var(var->type, var->id, val);
			}
		}

		sgettimeofday(&time_end, NULL);
		timeval_sub(&time_diff, &time_end, &time_start);
		if ((timeval_sub(&time_sleep, &time_out, &time_diff))) {
			debugf("tick interval too short");
			count++;
			continue;
		}
		select(0, NULL, NULL, NULL, &time_sleep);
		count++;
	}

	shutdown_stats();
	exit(0);
}

