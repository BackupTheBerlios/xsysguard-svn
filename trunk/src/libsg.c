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
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "util.h"
#include "libsg.h"

static stats_t stats;

static stat_t *stat_list = NULL;
static unsigned int stat_list_len = 0;

/******************************************************************************/

void get_host_info() {
	if (!(stats.host_info = sg_get_host_info()))
		debugf("sg_get_host_info() returned NULL");
}

void get_cpu_stats() {
	if (!(stats.cpu_stats = sg_get_cpu_stats()))
		debugf("sg_get_cpu_stats() returned NULL");
}

void get_cpu_stats_diff() {
	if (!(stats.cpu_stats_diff = sg_get_cpu_stats_diff()))
		debugf("sg_get_cpu_stats_diff() returned NULL");
}

void get_cpu_percents() {
	if (!(stats.cpu_percents = sg_get_cpu_percents()))
		debugf("sg_get_cpu_percents() returned NULL");
}

void get_mem_stats() {
	if (!(stats.mem_stats = sg_get_mem_stats()))
		debugf("sg_get_mem_stats() returned NULL");
}

void get_load_stats() {
	if (!(stats.load_stats = sg_get_load_stats()))
		debugf("sg_get_load_stats() returned NULL");
}

void get_user_stats() {
	if (!(stats.user_stats = sg_get_user_stats()))
		debugf("sg_get_user_stats() returned NULL");
}

void get_swap_stats() {
	if (!(stats.swap_stats = sg_get_swap_stats()))
		debugf("sg_get_swap_stats() returned NULL");
}

void get_fs_stats() {
	if (!(stats.fs_stats = sg_get_fs_stats(&stats.fs_entries)))
		debugf("sg_get_fs_stats() returned NULL");
}

void get_disk_io_stats() {
	if (!(stats.disk_io_stats = sg_get_disk_io_stats(&stats.disk_io_entries)))
		debugf("sg_get_disk_io_stats() returned NULL");
}

void get_disk_io_stats_diff() {
	if (!(stats.disk_io_stats_diff = sg_get_disk_io_stats_diff(&stats.disk_io_diff_entries)))
		debugf("sg_get_disk_io_stats_diff() returned NULL");
}

void get_network_io_stats() {
	if (!(stats.network_io_stats = sg_get_network_io_stats(&stats.network_io_entries)))
		debugf("sg_get_network_io_stats() returned NULL");
}

void get_network_io_stats_diff() {
	if (!(stats.network_io_stats_diff = sg_get_network_io_stats_diff(&stats.network_io_diff_entries)))
		debugf("sg_get_network_io_stats_diff() returned NULL");
}

void get_network_iface_stats() {
	if (!(stats.network_iface_stats = sg_get_network_iface_stats(&stats.network_iface_entries)))
		debugf("sg_get_network_iface_stats() returned NULL");
}

void get_page_stats() {
	if (!(stats.page_stats = sg_get_page_stats()))
		debugf("sg_get_page_stats() returned NULL");
}

void get_page_stats_diff() {
	if (!(stats.page_stats_diff = sg_get_page_stats_diff()))
		debugf("sg_get_page_stats_diff() returned NULL");
}

void get_process_stats() {
	if (!(stats.process_stats = sg_get_process_stats(&stats.process_entries)))
		debugf("sg_get_process_stats() returned NULL");
}

void get_process_count() {
	if (!(stats.process_count = sg_get_process_count()))
		debugf("sg_get_process_count() returned NULL");
}

/******************************************************************************
 *
 * stat_define_list
 *
 */

stat_define_t stat_define_list[] = {
	{ "host_info",			get_host_info },
	{ "cpu_stats",			get_cpu_stats },
	{ "cpu_stats_diff",		get_cpu_stats_diff },
	{ "cpu_percents",		get_cpu_percents },
	{ "mem_stats",			get_mem_stats },
	{ "load_stats",			get_load_stats },
	{ "user_stats",			get_user_stats },
	{ "swap_stats",			get_swap_stats },
	{ "fs_stats",			get_fs_stats },
	{ "disk_io_stats",		get_disk_io_stats },
	{ "disk_io_stats_diff",		get_disk_io_stats_diff },
	{ "network_io_stats",		get_network_io_stats },
	{ "network_io_stats_diff",	get_network_io_stats_diff },
	{ "network_iface_stats",	get_network_iface_stats },
	{ "page_stats",			get_page_stats },
	{ "page_stats_diff",		get_page_stats_diff },
	{ "process_stats",		get_process_stats },
	{ "process_count",		get_process_count },
	{ NULL, NULL }
};

/******************************************************************************
 *
 * host_info
 *
 */

void *get_host_info_os_name(var_t *var) {
	if (stats.host_info)
		return (void *) stats.host_info->os_name;
	else
		return NULL;
}

void *get_host_info_os_release(var_t *var) {
	if (stats.host_info)
		return (void *) stats.host_info->os_release;
	else
		return NULL;
}

void *get_host_info_os_version(var_t *var) {
	if (stats.host_info)
		return (void *) stats.host_info->os_version;
	else
		return NULL;
}

void *get_host_info_platform(var_t *var) {
	if (stats.host_info)
		return (void *) stats.host_info->platform;
	else
		return NULL;
}

void *get_host_info_hostname(var_t *var) {
	if (stats.host_info)
		return (void *) stats.host_info->hostname;
	else
		return NULL;
}

void *get_host_info_uptime(var_t *var) {
	int day = 0, hour =0, min = 0, sec;

	if (!stats.host_info)
		return NULL;

	if (!var->var)
		var->var = smalloc(buflen);

	sec = (int) stats.host_info->uptime;
	day  = sec / (24*60*60);
	sec  = sec % (24*60*60);
	hour = sec / (60*60);
	sec  = sec % (60*60);
	min  = sec / 60;
	sec  = sec % 60;

	if (!var->format) {
		if (day)
			snprintf((char *) var->var, buflen, "%dd %02d:%02d:%02d", day, hour, min, sec);
		else
			snprintf((char *) var->var, buflen, "%02d:%02d:%02d", hour, min, sec);
	} else {
		char *c = var->var;
		char *f = var->format;
		while (*f) {
			if (*f == '%') {
				f++;
				switch (*f) {
					case 'd':
						c += sprintf(c, "%02d", day);
						f++;
						break;
					case 'h':
						c += sprintf(c, "%02d", hour);
						f++;
						break;
					case 'm':
						c += sprintf(c, "%02d", min);
						f++;
						break;
					case 's':
						c += sprintf(c, "%02d", sec);
						f++;
						break;
					case 'H':
						c += sprintf(c, "%d", hour);
						f++;
						break;
					case 'M':
						c += sprintf(c, "%d", min);
						f++;
						break;
					case 'S':
						c += sprintf(c, "%d", sec);
						f++;
						break;
					default:
						*c++ = '%';
						*c++ = *f++;
						break;
				}
			} else {
				*c++ = *f++;
			}
		}
		*c = '\0';
	}
	return var->var;
}

/******************************************************************************
 *
 * cpu_stats
 *
 */

void *get_cpu_stats_user(var_t *var) {
	if (stats.cpu_stats)
		return (void *) &(stats.cpu_stats->user);
	else
		return NULL;
}

void *get_cpu_stats_kernel(var_t *var) {
	if (stats.cpu_stats)
		return (void *) &(stats.cpu_stats->kernel);
	else
		return NULL;
}

void *get_cpu_stats_idle(var_t *var) {
	if (stats.cpu_stats)
		return (void *) &(stats.cpu_stats->idle);
	else
		return NULL;
}

void *get_cpu_stats_iowait(var_t *var) {
	if (stats.cpu_stats)
		return (void *) &(stats.cpu_stats->iowait);
	else
		return NULL;
}

void *get_cpu_stats_swap(var_t *var) {
	if (stats.cpu_stats)
		return (void *) &(stats.cpu_stats->swap);
	else
		return NULL;
}

void *get_cpu_stats_nice(var_t *var) {
	if (stats.cpu_stats)
		return (void *) &(stats.cpu_stats->nice);
	else
		return NULL;
}

void *get_cpu_stats_total(var_t *var) {
	if (stats.cpu_stats)
		return (void *) &(stats.cpu_stats->total);
	else
		return NULL;
}

/******************************************************************************
 *
 * cpu_stats_diff
 *
 */

void *get_cpu_stats_diff_user(var_t *var) {
	if (stats.cpu_stats_diff)
		return (void *) &(stats.cpu_stats_diff->user);
	else
		return NULL;
}

void *get_cpu_stats_diff_kernel(var_t *var) {
	if (stats.cpu_stats_diff)
		return (void *) &(stats.cpu_stats_diff->kernel);
	else
		return NULL;
}

void *get_cpu_stats_diff_idle(var_t *var) {
	if (stats.cpu_stats_diff)
		return (void *) &(stats.cpu_stats_diff->idle);
	else
		return NULL;
}

void *get_cpu_stats_diff_iowait(var_t *var) {
	if (stats.cpu_stats_diff)
		return (void *) &(stats.cpu_stats_diff->iowait);
	else
		return NULL;
}

void *get_cpu_stats_diff_swap(var_t *var) {
	if (stats.cpu_stats_diff)
		return (void *) &(stats.cpu_stats_diff->swap);
	else
		return NULL;
}

void *get_cpu_stats_diff_nice(var_t *var) {
	if (stats.cpu_stats_diff)
		return (void *) &(stats.cpu_stats_diff->nice);
	else
		return NULL;
}

void *get_cpu_stats_diff_total(var_t *var) {
	if (stats.cpu_stats_diff)
		return (void *) &(stats.cpu_stats_diff->total);
	else
		return NULL;
}

/******************************************************************************
 *
 * cpu_percents
 *
 */

void *get_cpu_percents_user(var_t *var) {
	if (stats.cpu_percents)
		return (void *) &(stats.cpu_percents->user);
	else
		return NULL;
}

void *get_cpu_percents_kernel(var_t *var) {
	if (stats.cpu_percents)
		return (void *) &(stats.cpu_percents->kernel);
	else
		return NULL;
}

void *get_cpu_percents_idle(var_t *var) {
	if (stats.cpu_percents)
		return (void *) &(stats.cpu_percents->idle);
	else
		return NULL;
}

void *get_cpu_percents_iowait(var_t *var) {
	if (stats.cpu_percents)
		return (void *) &(stats.cpu_percents->iowait);
	else
		return NULL;
}

void *get_cpu_percents_swap(var_t *var) {
	if (stats.cpu_percents)
		return (void *) &(stats.cpu_percents->swap);
	else
		return NULL;
}

void *get_cpu_percents_nice(var_t *var) {
	if (stats.cpu_percents)
		return (void *) &(stats.cpu_percents->nice);
	else
		return NULL;
}

/******************************************************************************
 *
 * mem_stats
 *
 */

void *get_mem_stats_total(var_t *var) {
	if (stats.mem_stats)
		return (void *) &(stats.mem_stats->total);
	else
		return NULL;
}

void *get_mem_stats_free(var_t *var) {
	if (stats.mem_stats)
		return (void *) &(stats.mem_stats->free);
	else
		return NULL;
}

void *get_mem_stats_used(var_t *var) {
	if (stats.mem_stats)
		return (void *) &(stats.mem_stats->used);
	else
		return NULL;
}

void *get_mem_stats_cache(var_t *var) {
	if (stats.mem_stats)
		return (void *) &(stats.mem_stats->cache);
	else
		return NULL;
}

/******************************************************************************
 *
 * load_stats
 *
 */

void *get_load_stats_min1(var_t *var) {
	if (stats.load_stats)
		return (void *) &(stats.load_stats->min1);
	else
		return NULL;
}

void *get_load_stats_min5(var_t *var) {
	if (stats.load_stats)
		return (void *) &(stats.load_stats->min5);
	else
		return NULL;
}

void *get_load_stats_min15(var_t *var) {
	if (stats.load_stats)
		return (void *) &(stats.load_stats->min15);
	else
		return NULL;
}

/******************************************************************************
 *
 * user_stats
 *
 */

void *get_user_stats_name_list(var_t *var) {
	if (stats.user_stats)
		return (void *) stats.user_stats->name_list;
	else
		return NULL;
}

void *get_user_stats_num_entries(var_t *var) {
	if (stats.user_stats)
		return (void *) &(stats.user_stats->num_entries);
	else
		return NULL;
}

/******************************************************************************
 *
 * swap_stats
 *
 */

void *get_swap_stats_total(var_t *var) {
	if (stats.swap_stats)
		return (void *) &(stats.swap_stats->total);
	else
		return NULL;
}

void *get_swap_stats_used(var_t *var) {
	if (stats.swap_stats)
		return (void *) &(stats.swap_stats->used);
	else
		return NULL;
}

void *get_swap_stats_free(var_t *var) {
	if (stats.swap_stats)
		return (void *) &(stats.swap_stats->free);
	else
		return NULL;
}

/******************************************************************************
 *
 * fs_stats
 *
 */

static sg_fs_stats *get_fs_stats_for_device_name(char *arg) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *fs_stats = NULL;
	sg_fs_stats key, *tmp;

	if (!arg)
		return NULL;
	if ((!(lastcount == count)) || (!fs_stats)) {
		size_t size = sizeof(sg_fs_stats) * stats.fs_entries;
		lastcount = count;
		fs_stats = (sg_fs_stats *) srealloc((void *) fs_stats, size);
		memcpy(fs_stats, stats.fs_stats, size);
		qsort(fs_stats, stats.fs_entries, sizeof(sg_fs_stats), sg_fs_compare_device_name);
	}
	key.device_name = arg;
	tmp = bsearch(&key, fs_stats, stats.fs_entries, sizeof(sg_fs_stats), sg_fs_compare_device_name);
	return tmp;
}

static sg_fs_stats *get_fs_stats_for_mnt_point(char *arg) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *fs_stats = NULL;
	sg_fs_stats key, *tmp;

	if (!arg)
		return NULL;
	if ((!(lastcount == count)) || (!fs_stats)) {
		size_t size = sizeof(sg_fs_stats) * stats.fs_entries;
		lastcount = count;
		fs_stats = (sg_fs_stats *) srealloc((void *) fs_stats, size);
		memcpy(fs_stats, stats.fs_stats, size);
		qsort(fs_stats, stats.fs_entries, sizeof(sg_fs_stats), sg_fs_compare_mnt_point);
	}
	key.mnt_point = arg;
	tmp = bsearch(&key, fs_stats, stats.fs_entries, sizeof(sg_fs_stats), sg_fs_compare_mnt_point);
	return tmp;
}

void *get_fs_stats_device_name(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) ret->device_name;
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) ret->device_name;
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) ret->device_name;
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_fs_type(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) ret->fs_type;
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) ret->fs_type;
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) ret->fs_type;
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_mnt_point(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) ret->mnt_point;
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) ret->mnt_point;
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) ret->mnt_point;
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_size(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->size);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->size);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->size);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_used(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->used);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->used);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->used);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_avail(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->avail);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->avail);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->avail);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_total_inodes(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->total_inodes);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->total_inodes);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->total_inodes);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_used_inodes(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->used_inodes);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->used_inodes);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->used_inodes);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_free_inodes(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->free_inodes);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->free_inodes);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->free_inodes);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_avail_inodes(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->avail_inodes);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->avail_inodes);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->avail_inodes);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_io_size(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->io_size);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->io_size);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->io_size);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_block_size(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->block_size);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->block_size);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->block_size);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_total_blocks(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->total_blocks);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->total_blocks);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->total_blocks);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_free_blocks(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->free_blocks);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->free_blocks);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->free_blocks);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_used_blocks(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->used_blocks);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->used_blocks);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->used_blocks);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_fs_stats_avail_blocks(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_fs_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->avail_blocks);
	if ((ret = get_fs_stats_for_device_name(var->format)))
		return (void *) &(ret->avail_blocks);
	if ((ret = get_fs_stats_for_mnt_point(var->format)))
		return (void *) &(ret->avail_blocks);
	debugf("fs_stats for \"%s\" not found", var->format);
	return NULL;
}

/******************************************************************************
 *
 * disk_io_stats
 *
 */

static sg_disk_io_stats *get_disk_io_stats_for_disk_name(char *arg) {
	static unsigned long long lastcount = 0;
	static sg_disk_io_stats *disk_io_stats = NULL;
	sg_disk_io_stats key, *tmp;

	if (!arg)
		return NULL;
	if ((!(lastcount == count)) || (!disk_io_stats)) {
		size_t size = sizeof(sg_disk_io_stats) * stats.disk_io_entries;
		lastcount = count;
		disk_io_stats = (sg_disk_io_stats *) srealloc((void *) disk_io_stats, size);
		memcpy(disk_io_stats, stats.disk_io_stats, size);
		qsort(disk_io_stats, stats.disk_io_entries, sizeof(sg_disk_io_stats),
				sg_disk_io_compare_name);
	}
	key.disk_name = arg;
	tmp = bsearch(&key, disk_io_stats, stats.disk_io_entries, sizeof(sg_disk_io_stats),
			sg_disk_io_compare_name);
	return tmp;
}


void *get_disk_io_stats_read_bytes(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_disk_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->read_bytes);
	if ((ret = get_disk_io_stats_for_disk_name(var->format)))
		return (void *) &(ret->read_bytes);
	debugf("disk_io_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_disk_io_stats_write_bytes(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_disk_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->write_bytes);
	if ((ret = get_disk_io_stats_for_disk_name(var->format)))
		return (void *) &(ret->write_bytes);
	debugf("disk_io_stats for \"%s\" not found", var->format);
	return NULL;
}

/******************************************************************************
 *
 * disk_io_stats_diff
 *
 */

static sg_disk_io_stats *get_disk_io_stats_diff_for_disk_name(char *arg) {
	static unsigned long long lastcount = 0;
	static sg_disk_io_stats *disk_io_stats = NULL;
	sg_disk_io_stats key, *tmp;

	if (!arg)
		return NULL;
	if ((!(lastcount == count)) || (!disk_io_stats)) {
		size_t size = sizeof(sg_disk_io_stats) * stats.disk_io_diff_entries;
		lastcount = count;
		disk_io_stats = (sg_disk_io_stats *) srealloc((void *) disk_io_stats, size);
		memcpy(disk_io_stats, stats.disk_io_stats_diff, size);
		qsort(disk_io_stats, stats.disk_io_diff_entries, sizeof(sg_disk_io_stats),
				sg_disk_io_compare_name);
	}
	key.disk_name = arg;
	tmp = bsearch(&key, disk_io_stats, stats.disk_io_diff_entries, sizeof(sg_disk_io_stats),
			sg_disk_io_compare_name);
	return tmp;
}

void *get_disk_io_stats_diff_read_bytes(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_disk_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->read_bytes);
	if ((ret = get_disk_io_stats_diff_for_disk_name(var->format)))
		return (void *) &(ret->read_bytes);
	debugf("disk_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

void *get_disk_io_stats_diff_write_bytes(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_disk_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->write_bytes);
	if ((ret = get_disk_io_stats_diff_for_disk_name(var->format)))
		return (void *) &(ret->write_bytes);
	debugf("disk_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

/******************************************************************************
 *
 * network_io_stats
 *
 */

static sg_network_io_stats *get_network_io_stats_for_interface_name(char *arg) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *network_io_stats = NULL;
	sg_network_io_stats key, *tmp;

	if (!arg)
		return NULL;
	if ((!(lastcount == count)) || (!network_io_stats)) {
		size_t size = sizeof(sg_network_io_stats) * stats.network_io_entries;
		lastcount = count;
		network_io_stats = (sg_network_io_stats *) srealloc((void *) network_io_stats, size);
		memcpy(network_io_stats, stats.network_io_stats, size);
		qsort(network_io_stats, stats.network_io_entries, sizeof(sg_network_io_stats),
				sg_network_io_compare_name);
	}
	key.interface_name = arg;
	tmp = bsearch(&key, network_io_stats, stats.network_io_entries, sizeof(sg_network_io_stats),
			sg_network_io_compare_name);
	return tmp;
}


void *get_network_io_stats_tx(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->tx);
	if ((ret = get_network_io_stats_for_interface_name(var->format)))
		return (void *) &(ret->tx);
	debugf("network_io_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_rx(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->rx);
	if ((ret = get_network_io_stats_for_interface_name(var->format)))
		return (void *) &(ret->rx);
	debugf("network_io_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_ipackets(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->ipackets);
	if ((ret = get_network_io_stats_for_interface_name(var->format)))
		return (void *) &(ret->ipackets);
	debugf("network_io_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_opackets(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->opackets);
	if ((ret = get_network_io_stats_for_interface_name(var->format)))
		return (void *) &(ret->opackets);
	debugf("network_io_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_ierrors(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->ierrors);
	if ((ret = get_network_io_stats_for_interface_name(var->format)))
		return (void *) &(ret->ierrors);
	debugf("network_io_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_oerrors(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->oerrors);
	if ((ret = get_network_io_stats_for_interface_name(var->format)))
		return (void *) &(ret->oerrors);
	debugf("network_io_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_collisions(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->collisions);
	if ((ret = get_network_io_stats_for_interface_name(var->format)))
		return (void *) &(ret->collisions);
	debugf("network_io_stats for \"%s\" not found", var->format);
	return NULL;
}

/******************************************************************************
 *
 * network_io_stats_diff
 *
 */

static sg_network_io_stats *get_network_io_stats_diff_for_interface_name(char *arg) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *network_io_stats = NULL;
	sg_network_io_stats key, *tmp;

	if (!arg)
		return NULL;
	if ((!(lastcount == count)) || (!network_io_stats)) {
		size_t size = sizeof(sg_network_io_stats) * stats.network_io_diff_entries;
		lastcount = count;
		network_io_stats = (sg_network_io_stats *) srealloc((void *) network_io_stats, size);
		memcpy(network_io_stats, stats.network_io_stats_diff, size);
		qsort(network_io_stats, stats.network_io_diff_entries, sizeof(sg_network_io_stats),
				sg_network_io_compare_name);
	}
	key.interface_name = arg;
	tmp = bsearch(&key, network_io_stats, stats.network_io_diff_entries, sizeof(sg_network_io_stats),
			sg_network_io_compare_name);
	return tmp;
}


void *get_network_io_stats_diff_tx(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->tx);
	if ((ret = get_network_io_stats_diff_for_interface_name(var->format)))
		return (void *) &(ret->tx);
	debugf("network_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_diff_rx(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->rx);
	if ((ret = get_network_io_stats_diff_for_interface_name(var->format)))
		return (void *) &(ret->rx);
	debugf("network_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_diff_ipackets(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->ipackets);
	if ((ret = get_network_io_stats_diff_for_interface_name(var->format)))
		return (void *) &(ret->ipackets);
	debugf("network_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_diff_opackets(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->opackets);
	if ((ret = get_network_io_stats_diff_for_interface_name(var->format)))
		return (void *) &(ret->opackets);
	debugf("network_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_diff_ierrors(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->ierrors);
	if ((ret = get_network_io_stats_diff_for_interface_name(var->format)))
		return (void *) &(ret->ierrors);
	debugf("network_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_diff_oerrors(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->oerrors);
	if ((ret = get_network_io_stats_diff_for_interface_name(var->format)))
		return (void *) &(ret->oerrors);
	debugf("network_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_io_stats_diff_collisions(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_io_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->collisions);
	if ((ret = get_network_io_stats_diff_for_interface_name(var->format)))
		return (void *) &(ret->collisions);
	debugf("network_io_stats_diff for \"%s\" not found", var->format);
	return NULL;
}

/******************************************************************************
 *
 * network_iface_stats
 *
 */

static sg_network_iface_stats *get_network_iface_stats_for_interface_name(char *arg) {
	static unsigned long long lastcount = 0;
	static sg_network_iface_stats *network_iface_stats = NULL;
	sg_network_iface_stats key, *tmp;

	if (!arg)
		return NULL;
	if ((!(lastcount == count)) || (!network_iface_stats)) {
		size_t size = sizeof(sg_network_iface_stats) * stats.network_iface_entries;
		lastcount = count;
		network_iface_stats = (sg_network_iface_stats *) srealloc((void *) network_iface_stats, size);
		memcpy(network_iface_stats, stats.network_iface_stats, size);
		qsort(network_iface_stats, stats.network_iface_entries, sizeof(sg_network_iface_stats),
				sg_network_iface_compare_name);
	}
	key.interface_name = arg;
	tmp = bsearch(&key, network_iface_stats, stats.network_iface_entries,
			sizeof(sg_network_iface_stats), sg_network_iface_compare_name);
	return tmp;
}


void *get_network_iface_stats_speed(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_iface_stats *ret = NULL;

	if ((lastcount == count) && ret)
		return (void *) &(ret->speed);
	if ((ret = get_network_iface_stats_for_interface_name(var->format)))
		return (void *) &(ret->speed);
	debugf("network_iface_stats for \"%s\" not found", var->format);
	return NULL;
}

void *get_network_iface_stats_duplex(var_t *var) {
	static unsigned long long lastcount = 0;
	static sg_network_iface_stats *ret = NULL;

	if (!((lastcount == count) && ret))
		if ((ret = get_network_iface_stats_for_interface_name(var->format)))
	if (!ret) {
		debugf("network_iface_stats for \"%s\" not found", var->format);
		return NULL;
	}
	if (ret->duplex == SG_IFACE_DUPLEX_FULL)
		return (void *) "full";
	if (ret->duplex == SG_IFACE_DUPLEX_HALF)
		return (void *) "half";
	if (ret->duplex == SG_IFACE_DUPLEX_UNKNOWN)
		return (void *) "unknown";
	debugf("network_iface_stats: unknown duplex type");
	return NULL;
}

/******************************************************************************
 *
 * page_stats
 *
 */

void *get_page_stats_pages_pagein(var_t *var) {
	if (stats.page_stats)
		return (void *) &(stats.page_stats->pages_pagein);
	else
		return NULL;
}

void *get_page_stats_pages_pageout(var_t *var) {
	if (stats.page_stats)
		return (void *) &(stats.page_stats->pages_pageout);
	else
		return NULL;
}

/******************************************************************************
 *
 * page_stats_diff
 *
 */

void *get_page_stats_diff_pages_pagein(var_t *var) {
	if (stats.page_stats)
		return (void *) &(stats.page_stats->pages_pagein);
	else
		return NULL;
}

void *get_page_stats_diff_pages_pageout(var_t *var) {
	if (stats.page_stats)
		return (void *) &(stats.page_stats->pages_pageout);
	else
		return NULL;
}

/******************************************************************************
 *
 * process_stats
 *
 */

sg_process_stats *get_process_stats_orderedby_name() {
	static unsigned long long lastcount = 0;
	static sg_process_stats *process_stats = NULL;
	if (lastcount == count && process_stats)
		return process_stats;
	lastcount = count;
	size_t size = sizeof(sg_process_stats) * stats.process_entries;
	process_stats = (sg_process_stats *) srealloc((void *) process_stats, size);
	memcpy(process_stats, stats.process_stats, size);
	qsort(process_stats, stats.process_entries, sizeof(sg_process_stats), sg_process_compare_name);
	return process_stats;
}

sg_process_stats *get_process_stats_orderedby_pid() {
	static unsigned long long lastcount = 0;
	static sg_process_stats *process_stats = NULL;
	if (lastcount == count && process_stats)
		return process_stats;
	lastcount = count;
	size_t size = sizeof(sg_process_stats) * stats.process_entries;
	process_stats = (sg_process_stats *) srealloc((void *) process_stats, size);
	memcpy(process_stats, stats.process_stats, size);
	qsort(process_stats, stats.process_entries, sizeof(sg_process_stats), sg_process_compare_pid);
	return process_stats;
}

sg_process_stats *get_process_stats_orderedby_uid() {
	static unsigned long long lastcount = 0;
	static sg_process_stats *process_stats = NULL;
	if (lastcount == count && process_stats)
		return process_stats;
	lastcount = count;
	size_t size = sizeof(sg_process_stats) * stats.process_entries;
	process_stats = (sg_process_stats *) srealloc((void *) process_stats, size);
	memcpy(process_stats, stats.process_stats, size);
	qsort(process_stats, stats.process_entries, sizeof(sg_process_stats), sg_process_compare_uid);
	return process_stats;
}

sg_process_stats *get_process_stats_orderedby_gid() {
	static unsigned long long lastcount = 0;
	static sg_process_stats *process_stats = NULL;
	if (lastcount == count && process_stats)
		return process_stats;
	lastcount = count;
	size_t size = sizeof(sg_process_stats) * stats.process_entries;
	process_stats = (sg_process_stats *) srealloc((void *) process_stats, size);
	memcpy(process_stats, stats.process_stats, size);
	qsort(process_stats, stats.process_entries, sizeof(sg_process_stats), sg_process_compare_gid);
	return process_stats;
}

sg_process_stats *get_process_stats_orderedby_size() {
	static unsigned long long lastcount = 0;
	static sg_process_stats *process_stats = NULL;
	if (lastcount == count && process_stats)
		return process_stats;
	lastcount = count;
	size_t size = sizeof(sg_process_stats) * stats.process_entries;
	process_stats = (sg_process_stats *) srealloc((void *) process_stats, size);
	memcpy(process_stats, stats.process_stats, size);
	qsort(process_stats, stats.process_entries, sizeof(sg_process_stats), sg_process_compare_size);
	return process_stats;
}

sg_process_stats *get_process_stats_orderedby_res() {
	static unsigned long long lastcount = 0;
	static sg_process_stats *process_stats = NULL;
	if (lastcount == count && process_stats)
		return process_stats;
	lastcount = count;
	size_t size = sizeof(sg_process_stats) * stats.process_entries;
	process_stats = (sg_process_stats *) srealloc((void *) process_stats, size);
	memcpy(process_stats, stats.process_stats, size);
	qsort(process_stats, stats.process_entries, sizeof(sg_process_stats), sg_process_compare_res);
	return process_stats;
}

sg_process_stats *get_process_stats_orderedby_cpu() {
	static unsigned long long lastcount = 0;
	static sg_process_stats *process_stats = NULL;
	if (lastcount == count && process_stats)
		return process_stats;
	lastcount = count;
	size_t size = sizeof(sg_process_stats) * stats.process_entries;
	process_stats = (sg_process_stats *) srealloc((void *) process_stats, size);
	memcpy(process_stats, stats.process_stats, size);
	qsort(process_stats, stats.process_entries, sizeof(sg_process_stats), sg_process_compare_cpu);
	return process_stats;
}

sg_process_stats *get_process_stats_orderedby_time() {
	static unsigned long long lastcount = 0;
	static sg_process_stats *process_stats = NULL;
	if (lastcount == count && process_stats)
		return process_stats;
	lastcount = count;
	size_t size = sizeof(sg_process_stats) * stats.process_entries;
	process_stats = (sg_process_stats *) srealloc((void *) process_stats, size);
	memcpy(process_stats, stats.process_stats, size);
	qsort(process_stats, stats.process_entries, sizeof(sg_process_stats), sg_process_compare_time);
	return process_stats;
}


typedef struct {
	unsigned int linecount;
	boolean reverse;
	sg_process_stats * (*func)();
} process_stats_t;


static void parse_process_stats_format(var_t *var) {
	char buf0[strlen(var->format) + 1];
	char buf1[strlen(var->format) + 1];
	process_stats_t *process_stats;
	unsigned int linecount = UINT_MAX;
	int n = 0;

	var->last = smalloc(sizeof(process_stats_t));
	process_stats = (process_stats_t *) var->last;
	n = sscanf(var->format, "%u:%[^: ]:%s", &linecount, buf0, buf1);
	process_stats->linecount = linecount;
	if (n > 1) {
		if (strcmp(buf0, "name") == 0)
			process_stats->func = get_process_stats_orderedby_name;
		else if (strcmp(buf0, "pid") == 0)
			process_stats->func = get_process_stats_orderedby_pid;
		else if (strcmp(buf0, "uid") == 0)
			process_stats->func = get_process_stats_orderedby_uid;
		else if (strcmp(buf0, "gid") == 0)
			process_stats->func = get_process_stats_orderedby_gid;
		else if (strcmp(buf0, "size") == 0)
			process_stats->func = get_process_stats_orderedby_size;
		else if (strcmp(buf0, "res") == 0)
			process_stats->func = get_process_stats_orderedby_res;
		else if (strcmp(buf0, "cpu") == 0)
			process_stats->func = get_process_stats_orderedby_cpu;
		else if (strcmp(buf0, "time") == 0)
			process_stats->func = get_process_stats_orderedby_time;
		else
			process_stats->func = get_process_stats_orderedby_cpu;
	} else {
		process_stats->func = get_process_stats_orderedby_cpu;
	}
	if (n > 2) {
		if (strcmp(buf1, "r") == 0)
			process_stats->reverse = true;
		else
			process_stats->reverse = false;
	} else {
		process_stats->reverse = false;
	}
}

void *get_process_stats_process_name(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries - i - 1]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%s\n", sg->process_name);
		if (charcount >= buflen-1) {
			debugf("process_stats process_name buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_proctitle(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%s\n", sg->proctitle);
		if (charcount >= buflen-1) {
			debugf("process_stats proctitle buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_pid(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	long long pid;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		pid = sg->pid;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%lld\n", pid);
		if (charcount >= buflen-1) {
			debugf("process_stats pid buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_parent(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	long long parent;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		parent = sg->parent;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%lld\n", parent);
		if (charcount >= buflen-1) {
			debugf("process_stats parent buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_pgid(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	long long pgid;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		pgid = sg->pgid;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%lld\n", pgid);
		if (charcount >= buflen-1) {
			debugf("process_stats pgid buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_uid(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	long long uid;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		uid = sg->uid;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%lld\n", uid);
		if (charcount >= buflen-1) {
			debugf("process_stats uid buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_euid(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	long long euid;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		euid = sg->euid;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%lld\n", euid);
		if (charcount >= buflen-1) {
			debugf("process_stats euid buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_gid(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	long long gid;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		gid = sg->gid;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%lld\n", gid);
		if (charcount >= buflen-1) {
			debugf("process_stats gid buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_egid(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	long long egid;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		egid = sg->egid;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%lld\n", egid);
		if (charcount >= buflen-1) {
			debugf("process_stats egid buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_proc_size(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%llu\n", sg->proc_size);
		if (charcount >= buflen-1) {
			debugf("process_stats proc_size buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_proc_resident(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%llu\n", sg->proc_resident);
		if (charcount >= buflen-1) {
			debugf("process_stats proc_resident buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_time_spent(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	unsigned int i, charcount = 0;
	unsigned long long time_spent;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		time_spent = sg->time_spent;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%llu\n", time_spent);
		if (charcount >= buflen-1) {
			debugf("process_stats time_spent buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_cpu_percent(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%.1f\n", sg->cpu_percent);
		if (charcount >= buflen-1) {
			debugf("process_stats cpu_percent buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_nice(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	unsigned int i, charcount = 0;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		charcount += snprintf(var->var+charcount, buflen-charcount, "%d\n", sg->nice);
		if (charcount >= buflen-1) {
			debugf("process_stats nice buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

void *get_process_stats_state(var_t *var) {
	process_stats_t *process_stats;
	sg_process_stats *sg_list, *sg;
	unsigned int i, charcount = 0;
	char *state;

	if (!var->var)
		var->var = smalloc(buflen);

	if (!var->last)
		parse_process_stats_format(var);

	process_stats = (process_stats_t *) var->last;

	sg_list = (*(process_stats->func))();

	for (i=0; (i < stats.process_entries) && (i < process_stats->linecount); i++) {
		if (process_stats->reverse)
			sg = &(sg_list[stats.process_entries-i]);
		else
			sg = &(sg_list[i]);
		if (!sg->process_name)
			continue;
		switch (sg->state) {
			case SG_PROCESS_STATE_RUNNING:
				state = "R";
				break;
			case SG_PROCESS_STATE_SLEEPING:
				state = "S";
				break;
			case SG_PROCESS_STATE_STOPPED:
				state = "T";
				break;
			case SG_PROCESS_STATE_ZOMBIE:
				state = "Z";
				break;
			case SG_PROCESS_STATE_UNKNOWN:
				state = "U";
				break;
			default:
				state = "u";
				break;
		}
		charcount += snprintf(var->var+charcount, buflen-charcount, "%s\n", state);
		if (charcount >= buflen-1) {
			debugf("process_stats state buffer too short (\"%s\")", var->format);
			break;
		}
	}
	return var->var;
}

/******************************************************************************
 *
 * process_count
 *
 */

void *get_process_count_total(var_t *var) {
	if (stats.process_count)
		return (void *) &(stats.process_count->total);
	else
		return NULL;
}

void *get_process_count_running(var_t *var) {
	if (stats.process_count)
		return (void *) &(stats.process_count->running);
	else
		return NULL;
}

void *get_process_count_sleeping(var_t *var) {
	if (stats.process_count)
		return (void *) &(stats.process_count->sleeping);
	else
		return NULL;
}

void *get_process_count_stopped(var_t *var) {
	if (stats.process_count)
		return (void *) &(stats.process_count->stopped);
	else
		return NULL;
}

void *get_process_count_zombie(var_t *var) {
	if (stats.process_count)
		return (void *) &(stats.process_count->zombie);
	else
		return NULL;
}

/******************************************************************************/

stat_var_define_t stat_var_define_list[] = {
	{ "host_info", "os_name",		string_type,	get_host_info_os_name },
	{ "host_info", "os_release",		string_type,	get_host_info_os_release },
	{ "host_info", "os_version",		string_type,	get_host_info_os_version },
	{ "host_info", "platform",		string_type,	get_host_info_platform },
	{ "host_info", "hostname",		string_type,	get_host_info_hostname },
	{ "host_info", "uptime",		string_type,	get_host_info_uptime },
	{ "cpu_stats", "user",			longlong_type,	get_cpu_stats_user },
	{ "cpu_stats", "kernel",		longlong_type,	get_cpu_stats_kernel },
	{ "cpu_stats", "idle",			longlong_type,	get_cpu_stats_idle },
	{ "cpu_stats", "iowait",		longlong_type,	get_cpu_stats_iowait },
	{ "cpu_stats", "swap",			longlong_type,	get_cpu_stats_swap },
	{ "cpu_stats", "nice",			longlong_type,	get_cpu_stats_nice },
	{ "cpu_stats", "total",			longlong_type,	get_cpu_stats_total },
	{ "cpu_stats_diff", "user",		longlong_type,	get_cpu_stats_diff_user },
	{ "cpu_stats_diff", "kernel",		longlong_type,	get_cpu_stats_diff_kernel },
	{ "cpu_stats_diff", "idle",		longlong_type,  get_cpu_stats_diff_idle },
	{ "cpu_stats_diff", "iowait",		longlong_type,	get_cpu_stats_diff_iowait },
	{ "cpu_stats_diff", "swap",		longlong_type,	get_cpu_stats_diff_swap },
	{ "cpu_stats_diff", "nice",		longlong_type,	get_cpu_stats_diff_nice },
	{ "cpu_stats_diff", "total",		longlong_type,	get_cpu_stats_diff_total },
	{ "cpu_percents", "user",		float_type,	get_cpu_percents_user },
	{ "cpu_percents", "kernel",		float_type,	get_cpu_percents_kernel },
	{ "cpu_percents", "idle",		float_type,	get_cpu_percents_idle },
	{ "cpu_percents", "iowait",		float_type,	get_cpu_percents_iowait },
	{ "cpu_percents", "swap",		float_type,	get_cpu_percents_swap },
	{ "cpu_percents", "nice",		float_type,	get_cpu_percents_nice },
	{ "mem_stats", "total",			longlong_type,	get_mem_stats_total },
	{ "mem_stats", "free",			longlong_type,	get_mem_stats_free },
	{ "mem_stats", "used",			longlong_type,	get_mem_stats_used },
	{ "mem_stats", "cache",			longlong_type,	get_mem_stats_cache },
	{ "load_stats", "min1",			double_type,	get_load_stats_min1 },
	{ "load_stats", "min5",			double_type,	get_load_stats_min5 },
	{ "load_stats", "min15",		double_type,	get_load_stats_min15 },
	{ "user_stats", "name_list",		string_type,	get_user_stats_name_list },
	{ "user_stats", "num_entries",		int_type,	get_user_stats_num_entries },
	{ "swap_stats", "total",		longlong_type,	get_swap_stats_total },
	{ "swap_stats", "used",			longlong_type,	get_swap_stats_used },
	{ "swap_stats", "free",			longlong_type,	get_swap_stats_free },
	{ "fs_stats", "device_name",		string_type,	get_fs_stats_device_name },
	{ "fs_stats", "fs_type",		string_type,	get_fs_stats_fs_type },
	{ "fs_stats", "mnt_point",		string_type,	get_fs_stats_mnt_point },
	{ "fs_stats", "size",			longlong_type,	get_fs_stats_size },
	{ "fs_stats", "used",			longlong_type,	get_fs_stats_used },
	{ "fs_stats", "avail",			longlong_type,	get_fs_stats_avail },
	{ "fs_stats", "total_inodes",		longlong_type,	get_fs_stats_total_inodes },
	{ "fs_stats", "used_inodes",		longlong_type,	get_fs_stats_used_inodes },
	{ "fs_stats", "free_inodes",		longlong_type,	get_fs_stats_free_inodes },
	{ "fs_stats", "avail_inodes",		longlong_type,	get_fs_stats_avail_inodes },
	{ "fs_stats", "io_size",		longlong_type,	get_fs_stats_io_size },
	{ "fs_stats", "block_size",		longlong_type,	get_fs_stats_block_size },
	{ "fs_stats", "total_blocks",		longlong_type,	get_fs_stats_total_blocks },
	{ "fs_stats", "free_blocks",		longlong_type,	get_fs_stats_free_blocks },
	{ "fs_stats", "used_blocks",		longlong_type,	get_fs_stats_used_blocks },
	{ "fs_stats", "avail_blocks",		longlong_type,	get_fs_stats_avail_blocks },
	{ "disk_io_stats", "read_bytes",	longlong_type,	get_disk_io_stats_read_bytes },
	{ "disk_io_stats", "write_bytes",	longlong_type,	get_disk_io_stats_write_bytes },
	{ "disk_io_stats_diff", "read_bytes",	longlong_type,	get_disk_io_stats_diff_read_bytes },
	{ "disk_io_stats_diff", "write_bytes",	longlong_type,	get_disk_io_stats_diff_write_bytes },
	{ "network_io_stats", "tx",		longlong_type,	get_network_io_stats_tx },
	{ "network_io_stats", "rx",		longlong_type,	get_network_io_stats_rx },
	{ "network_io_stats", "ipackets",	longlong_type,	get_network_io_stats_ipackets },
	{ "network_io_stats", "opackets",	longlong_type,	get_network_io_stats_opackets },
	{ "network_io_stats", "ierrors",	longlong_type,	get_network_io_stats_ierrors },
	{ "network_io_stats", "oerrors",	longlong_type,	get_network_io_stats_oerrors },
	{ "network_io_stats", "collisions",	longlong_type,	get_network_io_stats_collisions },
	{ "network_io_stats_diff", "tx",	longlong_type,	get_network_io_stats_diff_tx },
	{ "network_io_stats_diff", "rx",	longlong_type,	get_network_io_stats_diff_rx },
	{ "network_io_stats_diff", "ipackets",	longlong_type,	get_network_io_stats_diff_ipackets },
	{ "network_io_stats_diff", "opackets",	longlong_type,	get_network_io_stats_diff_opackets },
	{ "network_io_stats_diff", "ierrors",	longlong_type,	get_network_io_stats_diff_ierrors },
	{ "network_io_stats_diff", "oerrors",	longlong_type,	get_network_io_stats_diff_oerrors },
	{ "network_io_stats_diff", "collisions",longlong_type,	get_network_io_stats_diff_collisions },
	{ "network_iface_stats", "speed",	int_type,	get_network_iface_stats_speed },
	{ "network_iface_stats", "duplex",	string_type,	get_network_iface_stats_duplex },
	{ "page_stats", "pages_pagein",		longlong_type,	get_page_stats_pages_pagein },
	{ "page_stats", "pages_pageout",	longlong_type,	get_page_stats_pages_pageout },
	{ "page_stats_diff", "pages_pagein",	longlong_type,	get_page_stats_diff_pages_pagein },
	{ "page_stats_diff", "pages_pageout",	longlong_type,	get_page_stats_diff_pages_pageout },
	{ "process_stats", "process_name",	string_type,	get_process_stats_process_name },
	{ "process_stats", "proctitle",		string_type,	get_process_stats_proctitle },
	{ "process_stats", "pid",		string_type,	get_process_stats_pid },
	{ "process_stats", "parent",		string_type,	get_process_stats_parent },
	{ "process_stats", "pgid",		string_type,	get_process_stats_pgid },
	{ "process_stats", "uid",		string_type,	get_process_stats_uid },
	{ "process_stats", "euid",		string_type,	get_process_stats_euid },
	{ "process_stats", "gid",		string_type,	get_process_stats_gid },
	{ "process_stats", "egid",		string_type,	get_process_stats_egid },
	{ "process_stats", "proc_size",		string_type,	get_process_stats_proc_size },
	{ "process_stats", "proc_resident",	string_type,	get_process_stats_proc_resident },
	{ "process_stats", "time_spent",	string_type,	get_process_stats_time_spent },
	{ "process_stats", "cpu_percent",	string_type,	get_process_stats_cpu_percent },
	{ "process_stats", "nice",		string_type,	get_process_stats_nice },
	{ "process_stats", "state",		string_type,	get_process_stats_state },
	{ "process_count", "total",		int_type,	get_process_count_total },
	{ "process_count", "running",		int_type,	get_process_count_running },
	{ "process_count", "sleeping",		int_type,	get_process_count_sleeping },
	{ "process_count", "stopped",		int_type,	get_process_count_stopped },
	{ "process_count", "zombie",		int_type,	get_process_count_zombie },
	{ NULL, NULL, 0, NULL }
};



/******************************************************************************/

void init_stats() {
	stat_define_t *stat_define;

	sg_init();
	//sg_snapshot(); // FIXME: atm only in cvs
	for (stat_define = stat_define_list; stat_define->name; stat_define++)
		(*(stat_define->func))();
}

void shutdown_stats() {
	//sg_shutdown(); // FIXME: atm only in cvs
}

void add_stat(unsigned long long mult, char *name) {
	unsigned int i;
	stat_define_t *stat_define;
	stat_t *stat = NULL;;

	for (stat_define = stat_define_list; stat_define->name; stat_define++)
		if (strcmp(stat_define->name, name) == 0)
			break;

	if (!stat_define->name) {
		debugf("cannot find \"%s\" in stat_define_list", name);
		return;
	}

	// find matching entry in stat_list
	for (i=0; i < stat_list_len; i++) {
		stat = &(stat_list[i]);
		if (stat->func == stat_define->func) {
			// is mult a multiple of stat->mult?
			if (mult % stat->mult == 0)
				return;
			// is stat->mult a multiple of mult?
			if (stat->mult % mult == 0) {
				stat->mult = mult;
				return;
			}
		}
	}
	// no matching entry found in stat_list -> add new entry
	stat_list_len++;
	stat_list = (stat_t *) realloc((void *) stat_list, sizeof(stat_t) * stat_list_len);
	stat = &(stat_list[stat_list_len - 1]);
	stat->mult = mult;
	stat->func = stat_define->func;
}

void set_stat_var(var_t *var, unsigned int id, unsigned long long mult, char *name, char *funcname, char *format) {
	stat_var_define_t *stat_var_define;

	for (stat_var_define = stat_var_define_list; stat_var_define->name; stat_var_define++) {
		if (strcmp(stat_var_define->name, name) == 0) {
			if (strcmp(stat_var_define->funcname, funcname) == 0) {
				var->id = id;
				var->mult = mult;
				var->type = stat_var_define->type;
				var->func = stat_var_define->func;
				var->last = NULL; // needed for process_stats!
				if (format) {
					var->format = (char *) smalloc(strlen(format) + 1);
					strcpy(var->format, format);
				} else {
					var->format = NULL;
				}
				return;
			}
		}
	}
	dief("%s:%s not found in stat_var_define_list", name, funcname);
}

void read_stats() {
	unsigned int i;
	stat_t *stat;

	for (i=0; i < stat_list_len; i++) {
		stat = &stat_list[i];
		if (count % stat->mult != 0)
			continue;
		(*(stat->func))();
	}
}

