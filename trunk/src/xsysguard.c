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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"
#include "common.h"
#include "libsg.h"
#include "x11.h"
#include "widgets.h"

/******************************************************************************/

typedef struct {
	char *command;
	char *config;
	int fd;
	int fd_out;
	FILE *fp;
	pid_t pid;
} daemon_t;

daemon_t *daemon_list = NULL;
unsigned int daemon_list_len = 0;

/******************************************************************************/

void _debugf(char *format, ...) {
	va_list args;
	va_start(args, format);
	fprintf(stderr, "DEBUG: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void dief(char *format, ...) {
	va_list args;
	va_start(args, format);
	fprintf(stderr, "ERROR: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(1);
}

/******************************************************************************/

void resv_var(FILE *fp, char *command) {
	// TODO
}

void exec_daemons() {
	daemon_t *daemon;
	unsigned int i;

	for (i=0; i < daemon_list_len; i++) {
		daemon = &(daemon_list[i]);
		if (!daemon->fp) {
			pid_t pid;
			int pipe1[2];
			int pipe2[2];

			close(daemon->fd);
			close(daemon->fd_out);

			if (pipe(pipe1) < 0 || pipe(pipe2) < 0)
				dief("cannot create pipe");

			if ((pid = fork()) < 0) {
				dief("cannot fork");
			} else if (pid == 0) {
				close(pipe1[1]);
				close(pipe2[0]);
				if (pipe1[0] != STDIN_FILENO) {
					if (dup2(pipe1[0], STDIN_FILENO) != STDIN_FILENO)
						dief("dup2 error for stdin");
					close(pipe1[0]);
				}
				if (pipe2[1] != STDOUT_FILENO) {
					if (dup2(pipe2[1], STDOUT_FILENO) != STDOUT_FILENO)
						dief("dup2 error for stdout");
					close(pipe2[1]);
				}
				execl("/bin/sh", "sh", "-c", daemon->command, NULL);
				dief("cannot execute command: %s", daemon->command);
			}
			close(pipe1[0]);
			close(pipe2[1]);
			daemon->fd = pipe2[0];
			daemon->fd_out = pipe1[1];
			daemon->pid = pid;
			write(daemon->fd_out, daemon->config, strlen(daemon->config));
			if (!(daemon->fp = fdopen(daemon->fd, "r")))
				debugf("fdopen failed for %s", daemon->command);

			resv_var(daemon->fp, daemon->command);
		}
	}
}

void add_daemon() {
	// TODO
}

/******************************************************************************/

int main(int argc, char **argv) {

	struct timeval time_out;
	unsigned int i;
	fd_set fds;
	int xfd, fd_max, fd_count;


	// TODO: read configuration into buffer

	init_font_path();

	// TODO: parse configuration

	default_widgets();

	// init libstatgrab & x11
	init_stats();
	xfd = init_x11();

	// read all files (due to counters)
	read_files();
	for (i=1; i < var_list_len; i++) {
		var_t *var;
		var = &(var_list[i]);
		(*(var->func))(var);
	}

	count++; // => count = 0

	// start main loop
	time_out.tv_sec = tick / 1000;
	time_out.tv_usec = (tick % 1000) * 1000;
	while (1) {
		struct timeval time_start, time_end, time_diff, time_sleep;
		daemon_t *daemon;
		sgettimeofday(&time_start, NULL);

		debugf("tick %llu", count);
		read_stats();
		read_files();

		update_widgets();
		handle_xevents();
		exec_daemons();

		while (1) {
			render_widgets_on_drawable();

			sgettimeofday(&time_end, NULL);
			timeval_sub(&time_diff, &time_end, &time_start);
			if ((timeval_sub(&time_sleep, &time_out, &time_diff))) {
				break;
			}

			FD_ZERO(&fds);
			FD_SET(xfd, &fds);
			fd_max = xfd;
			for (i=0; i < daemon_list_len; i++) {
				daemon = &(daemon_list[i]);
				FD_SET(daemon->fd, &fds);
				if (daemon->fd > xfd)
					fd_max = daemon->fd;
			}
			fd_count = select(fd_max + 1, &fds, NULL, NULL, &time_sleep);
			if (!fd_count)
				break;
			if (FD_ISSET(xfd, &fds)) {
				handle_xevents();
				if (fd_count < 2)
					continue;
			}
			for (i=0; i < daemon_list_len; i++) {
				daemon = &(daemon_list[i]);
				if (FD_ISSET(daemon->fd, &fds))
					resv_var(daemon->fp, daemon->command);
			}
		}
		count++;
	}

	shutdown_stats();
	exit(0);
}

