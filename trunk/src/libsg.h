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


#ifndef __STATGRAB_H__
#define __STATGRAB_H__

#include <statgrab.h>
#include "common.h"

/******************************************************************************/

typedef struct {
	sg_host_info		*host_info;
	sg_cpu_stats		*cpu_stats;
	sg_cpu_stats		*cpu_stats_diff;
	sg_cpu_percents		*cpu_percents;
	sg_mem_stats		*mem_stats;
	sg_load_stats		*load_stats;
	sg_user_stats		*user_stats;
	sg_swap_stats		*swap_stats;
	sg_fs_stats		*fs_stats;		int fs_entries;
	sg_disk_io_stats	*disk_io_stats;		int disk_io_entries;
	sg_disk_io_stats	*disk_io_stats_diff;	int disk_io_diff_entries;
	sg_network_io_stats	*network_io_stats;	int network_io_entries;
	sg_network_io_stats	*network_io_stats_diff;	int network_io_diff_entries;
	sg_network_iface_stats  *network_iface_stats;   int network_iface_entries;
	sg_page_stats		*page_stats;
	sg_page_stats		*page_stats_diff;
	sg_process_stats	*process_stats;		int process_entries;
	sg_process_count	*process_count;
} stats_t;

typedef struct {
	char *name;
	char *funcname;
	type_t type;
	void * (*func)(var_t *);
} stat_var_define_t;

typedef struct {
	char *name;
	void (*func)();
} stat_define_t;

typedef struct {
	unsigned long long mult;
	void (*func)();
} stat_t;

/******************************************************************************/

void init_stats();
void shutdown_stats();

void add_stat(unsigned long long mult, char *name);
void set_stat_var(var_t *var, unsigned int id, unsigned long long mult, char *name, char *funcname, char *format);
void read_stats();

/******************************************************************************/

#endif // __STATGRAB_H__

