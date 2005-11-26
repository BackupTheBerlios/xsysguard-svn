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
#include <ctype.h>
#include "util.h"
#include "common.h"

/******************************************************************************/

void *scalloc(size_t nmemb, size_t size) {
	void *ptr;

	ptr = calloc(nmemb, size);
	if (!ptr)
		dief("calloc(%zd, %zd) failed", nmemb, size);
	return ptr;
}

void *smalloc(size_t size) {
	void *ptr;

	ptr = malloc(size);
	if (!ptr)
		dief("malloc(%zd) failed", size);
	return ptr;
}

void *srealloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);

	if (!ptr)
		dief("realloc(%p, %zd) failed", ptr, size);
	return ptr;
}

int sgettimeofday(struct timeval *tv, struct timezone *tz) {
	int ret;

	ret = gettimeofday(tv, tz);
	if (ret)
		dief("gettimeofday(%p, %p) failed", tv, tz);
	return ret;
}

/******************************************************************************/

int string_starts_with(char *string, char *start) {
	int i, count = 0;

	if (!string || !start)
		return 0;

	while (isspace((int) *string)) {
		string++;
		count++;
	}

	for (i=0; i < strlen(start); i++) {
		if (string[i] == '\0')
			return 0;
		if (string[i] != start[i])
			return 0;
	}

	string += i;
	count += i;

	while (isspace((int) *string)) {
		string++;
		count++;
	}

	return count;
}

int parse_quoted_string(char *source, char *dest) {
	char* c = source;
	int n = 0;;

	if (!source || !dest)
		return 0;

	if (*c++ != '\"')
		return 0;

	while ( *c != '\"' && *c != '\0') {
		if ( *c == '\\' ) {
			c++;
			switch (*c) {
				case 'a' :
					*dest = '\a';
					break;
				case 'b':
					*dest = '\b';
					break;
				case 'f':
					*dest = '\f';
					break;
				case 'n':
					*dest = '\n';
					break;
				case 'r':
					*dest = '\r';
					break;
				case 't':
					*dest = '\t';
					break;
				case 'v':
					*dest = '\v';
					break;
				case '\\':
					*dest = '\\';
					break;
				case '?':
					*dest = '\?';
					break;
				case '\'':
					*dest = '\'';
					break;
				case '\"':
					*dest = '\"';
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					sscanf(c, "%3hho%n", dest, &n);
					c += n-1;
					break;
				case 'x':
					if (++c == '\0') {
						*dest = '\0';
						return c - source;
					}
					sscanf(c, "%2hhx%n", dest, &n);
					c += n-1;
					break;
				case '\0':
					*dest = '\0';
					return c - source;
					break;
				default:
					*dest = '\\';
					break;
			}
			c++;
			dest++;
		} else {
			*dest++ = *c++;
		}
	}

	c++;
	*dest = '\0';

	return c - source;
}

int timeval_sub(struct timeval *result, struct timeval *x, struct timeval *y) {

	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	return x->tv_sec < y->tv_sec;
}

