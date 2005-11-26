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


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Imlib2.h>
#include <string.h>
#include "common.h"
#include "util.h"
#include "x11.h"

/******************************************************************************/

unsigned int width = 120;
unsigned int height = 100;
Imlib_Updates updates;

static Display *display;
static Window window;
static int x = 100;
static int y = 100;

/******************************************************************************/

int init_x11() {
	if ((display = XOpenDisplay(NULL)) == NULL)
		dief("cannot open display");
	window = XCreateSimpleWindow(display, DefaultRootWindow(display),
			x, y, width, height, 0, 0, 0);
	XSelectInput(display, window, ExposureMask);
	XMapWindow(display, window);

	imlib_context_set_dither(1);
	imlib_context_set_anti_alias(1);
	imlib_context_set_display(display);
	imlib_context_set_visual(DefaultVisual(display, DefaultScreen(display)));
	imlib_context_set_colormap(DefaultColormap(display, DefaultScreen(display)));
	imlib_context_set_drawable(window);

	updates = imlib_updates_init();

	return ConnectionNumber(display);
}

int parse_geometry(char *geometry) {
	// FIXME handle negativ values
	return XParseGeometry(geometry, &x, &y, &width, &height);
}

void handle_xevents() {
	XEvent event;
	while (XPending(display)) {
		XNextEvent(display, &event);
		if (event.type == Expose)
			updates = imlib_update_append_rect(updates,
					event.xexpose.x, event.xexpose.y,
					event.xexpose.width, event.xexpose.height);
	}
}

