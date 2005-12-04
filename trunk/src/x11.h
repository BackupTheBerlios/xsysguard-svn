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


#ifndef __X11_H__
#define __X11_H__

#include <X11/Xlib.h>
#include <Imlib2.h>

/******************************************************************************/

extern unsigned int width;
extern unsigned int height;
extern Imlib_Updates updates;

/******************************************************************************/

int init_x11();
void set_font_path(char *path);
void init_font_path();
int parse_geometry(char *geometry);
void handle_xevents();

/******************************************************************************/

#endif // __X11_H__

