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


#ifndef __UTIL_H__
#define __UTIL_H__

#include <sys/time.h>

/******************************************************************************/

typedef enum {
	false = 0,
	true
} boolean;

/******************************************************************************/

void *scalloc(size_t nmemb, size_t size);
void *smalloc(size_t size);
void *srealloc(void *ptr, size_t size);
int sgettimeofday(struct timeval *tv, struct timezone *tz);

/******************************************************************************/

int string_starts_with(char *string, char *start);
int parse_quoted_string(char *source, char *dest);
int timeval_sub(struct timeval *result, struct timeval *x, struct timeval *y);

/******************************************************************************/

#endif // __UTIL_H__

