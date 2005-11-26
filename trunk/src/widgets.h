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


#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include <X11/Xlib.h>
#include <Imlib2.h>
#include "x11.h"
#include "common.h"

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} color_t;


typedef enum {
	NORTH,
	WEST,
	EAST,
	SOUTH
} orientation_t;


typedef struct {
	unsigned long long mult;
	int x;
	int y;
	int width;
	int height;

	enum {
		line_widget,
		rectangle_widget,
		image_widget,
		barchart_widget,
		linechart_widget,
		areachart_widget,
		text_widget
	} type;

	var_t *var;
	unsigned int var_count;

	void *data;
} widget_t;


typedef struct {
	color_t color;
} line_widget_t;


typedef struct {
	color_t color;
} rectangle_widget_t;


typedef struct {
	char *filename;
} image_widget_t;


typedef struct {
	orientation_t orientation;
	boolean add;
	boolean static_min;
	boolean static_max;
	double min;
	double max;

	color_t *color_min;
	color_t *color_max;
	double *data;
} barchart_widget_t;


typedef struct {
	orientation_t orientation;
	boolean add;
	boolean static_min;
	boolean static_max;
	double min;
	double max;
	char *bgfile;

	Imlib_Image img;
	unsigned int data_index;

	color_t *color;
	double **data;
} linechart_widget_t;


typedef struct {
	orientation_t orientation;
	boolean add;
	boolean static_min;
	boolean static_max;
	double min;
	double max;
	char *bgfile;

	Imlib_Image img;
	unsigned int data_index;

	color_t *color_min;
	color_t *color_max;
	color_t *color_top;
	double **data;
} areachart_widget_t;


typedef struct {
	double angle;
	color_t color;
	char *font;
	char *format;

	double *factor;
} text_widget_t;

/******************************************************************************/

void render_widgets_on_drawable();
void update_widgets();
void update_widget_value(widget_t *widget);
void parse_widgets(char *buf);
void default_widgets();

/******************************************************************************/

#endif // __WIDGETS_H__

