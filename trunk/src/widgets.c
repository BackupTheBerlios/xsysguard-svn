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


#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "common.h"
#include "util.h"
#include "libsg.h"
#include "x11.h"
#include "widgets.h"

#define round(x) ((x)>=0?(int) ((x)+0.5) : (int)((x)-0.5))
#define max(a, b) ((a) > (b)) ? (a) : (b)
#define min(a, b) ((a) > (b)) ? (b) : (a)

/******************************************************************************/

static widget_t *widget_list = NULL;
static unsigned int widget_list_len = 0;

/******************************************************************************/

char *expand_filename(char *name) {
	char *home;
	char *new;

	home = getenv("HOME");
	if (!home) {
		new = (char *) smalloc(strlen(name) + 1);
		strcpy(new, name);
	} else if (name[0] == '~') {
		new = (char *) smalloc(strlen(name) + strlen(home) + 1 - 1);
		strcpy(new, home);
		strcat(new, name + 1);
	} else if (name[0] != '/') {
		new = (char *) smalloc(strlen(name) + strlen(home) + 1 + 12);
		strcpy(new, home);
		strcat(new, "/.xsysguard/");
		strcat(new, name);
	}
	return new;
}

static orientation_t parse_orientation(char orientation_char) {
	switch (orientation_char) {
		case 'N':
		case 'n':
			return NORTH;
			break;
		case 'W':
		case 'w':
			return WEST;
			break;
		case 'E':
		case 'e':
			return EAST;
			break;
		case 'S':
		case 's':
			return SOUTH;
			break;
		default:
			return NORTH;
			break;
	}
}

static double get_as_double(type_t var_type, unsigned int var_id, void *val) {
	switch (var_type) {
		case string_type:
			debugf("cannot convert string to double: %s", (char *) val);
			break;
		case char_type:
			return (double) (*(char *) val);
			break;
		case uchar_type:
			return (double) (*(unsigned char *) val);
			break;
		case short_type:
			return (double) (*(short *) val);
			break;
		case ushort_type:
			return (double) (*(unsigned short *) val);
			break;
		case int_type:
			return (double) (*(int *) val);
			break;
		case uint_type:
			return (double) (*(unsigned int *) val);
			break;
		case long_type:
			return (double) (*(long *) val);
			break;
		case ulong_type:
			return (double) (*(unsigned long *) val);
			break;
		case longlong_type:
			return (double) (*(long long *) val);
			break;
		case ulonglong_type:
			return (double) (*(unsigned long long *) val);
			break;
		case float_type:
			return (double) (*(float *) val);
			break;
		case double_type:
			return (double) (*(double *) val);
			break;
		case longdouble_type:
			return (double) (*(long double *) val);
			break;
		default:
			dief("Variable %u has unknown type %d", var_id, var_type);
			break;
	}
	return 0.0;
}

/******************************************************************************/

static void render_line_widget(Imlib_Image buffer, widget_t *widget, int up_x, int up_y) {
	line_widget_t *widget_data;

	widget_data = (line_widget_t *) widget->data;
	imlib_context_set_image(buffer);
	imlib_context_set_color(widget_data->color.r, widget_data->color.g,
			widget_data->color.b, widget_data->color.a);
	imlib_image_draw_line(widget->x - up_x, widget->y - up_y,
			widget->x - up_x + widget->width, widget->y - up_y + widget->height, 0);
}

static void render_rectangle_widget(Imlib_Image buffer, widget_t *widget, int up_x, int up_y) {
	rectangle_widget_t *widget_data;

	widget_data = (rectangle_widget_t *) widget->data;
	imlib_context_set_image(buffer);
	imlib_context_set_color(widget_data->color.r, widget_data->color.g,
			widget_data->color.b, widget_data->color.a);
	imlib_image_fill_rectangle(widget->x - up_x, widget->y - up_y, widget->width, widget->height);
}

static void render_image_widget(Imlib_Image buffer, widget_t *widget, int up_x, int up_y) {
	image_widget_t *widget_data;
	Imlib_Image img;
	int w, h;

	widget_data = (image_widget_t *) widget->data;
	if (widget_data->filename)
		img = imlib_load_image(widget_data->filename);
	if (img) {
		imlib_context_set_image(img);
		w = imlib_image_get_width();
		h = imlib_image_get_height();
		imlib_context_set_image(buffer);
		imlib_blend_image_onto_image(img, 1, 0, 0, w, h,
				widget->x - up_x, widget->y - -up_y, w, h);
		imlib_context_set_image(img);
		imlib_free_image();
	}
}

static void render_barchart_widget(Imlib_Image buffer, widget_t *widget, int up_x, int up_y) {
	barchart_widget_t *widget_data;
	Imlib_Color_Range range;
	double min = 0.0;
	double max = 0.0;
	double sum = 0.0;
	unsigned int i;
	int clip_x, clip_y, clip_w, clip_h;
	color_t color_min, color_max;
	double angle;
	double data;
	int last = 0;

	widget_data = (barchart_widget_t *) widget->data;

	imlib_context_set_image(buffer);

	if (!widget_data->static_min || !widget_data->static_max) {
		for (i=0; i < widget->var_count; i++) {
			if (!isnan(widget_data->data[i])) {
				sum += widget_data->data[i];
				max = max(widget_data->data[i], max);
			}
		}
		if (widget_data->add) {
			max = max(sum, max);
		}
	}
	if (widget_data->static_min)
		min = widget_data->min;
	if (widget_data->static_max)
		max = widget_data->max;

	for (i=0; i < widget->var_count; i++) {
		if (!isnan(widget_data->data[i])) {
			color_min = widget_data->color_min[i];
			color_max = widget_data->color_max[i];
			range = imlib_create_color_range();
			imlib_context_set_color_range(range);
			imlib_context_set_color(color_max.r, color_max.g, color_max.b, color_max.a);
			imlib_add_color_to_color_range(0);
			imlib_context_set_color(color_min.r, color_min.g, color_min.b, color_min.a);

			data = widget_data->data[i];

			if (widget_data->orientation == NORTH) {
				imlib_add_color_to_color_range(widget->height);
				angle = 0.0;
				clip_x = widget->x - up_x;
				clip_w = widget->width;
				clip_h = round((((data - min) / (max - min)) *
						(double) (widget->height)));
				clip_y = widget->y - up_y + widget->height - clip_h;
				if (widget_data->add)
					clip_y -= last;
				last += clip_h;
			} else if (widget_data->orientation == SOUTH) {
				imlib_add_color_to_color_range(widget->height);
				angle = 180.0;
				clip_x = widget->x - up_x;
				clip_w = widget->width;
				clip_h = round((((data - min) / (max - min)) *
						(double) (widget->height)));
				clip_y = widget->y - up_y;
				if (widget_data->add)
					clip_y += last;
				last += clip_h;
			} else if (widget_data->orientation == WEST) {
				imlib_add_color_to_color_range(widget->width);
				angle = 90.0;
				clip_y = widget->y - up_y;
				clip_h = widget->height;
				clip_w = round((((data - min) / (max - min)) *
						(double) (widget->width)));
				clip_x = widget->x - up_x;
				if (widget_data->add)
					clip_x += last;
				last += clip_w;
			} else if (widget_data->orientation == EAST) {
				imlib_add_color_to_color_range(widget->width);
				angle = 270.0;
				clip_y = widget->y - up_y;
				clip_h = widget->height;
				clip_w = round((((data - min) / (max - min)) *
						(double) (widget->width)));
				clip_x = widget->x - up_x + widget->width - clip_w;
				if (widget_data->add)
					clip_x -= last;
				last += clip_w;
			} else {
				dief("unknown orientation");
			}
			imlib_context_set_cliprect(clip_x, clip_y, clip_w, clip_h);
			if (clip_w > 0 && clip_h > 0)
				imlib_image_fill_color_range_rectangle(widget->x - up_x, widget->y - up_y,
						widget->width, widget->height, angle);
			imlib_free_color_range();
		}
	}
}

static void render_linechart_widget(Imlib_Image buffer, widget_t *widget, int up_x, int up_y) {
	linechart_widget_t *widget_data;

	widget_data = (linechart_widget_t *) widget->data;
	imlib_context_set_image(buffer);
	imlib_blend_image_onto_image(widget_data->img, 1, 0, 0, widget->width, widget->height,
			widget->x - up_x, widget->y - up_y, widget->width, widget->height);
}

static void render_areachart_widget(Imlib_Image buffer, widget_t *widget, int up_x, int up_y) {
	areachart_widget_t *widget_data;

	widget_data = (areachart_widget_t *) widget->data;
	imlib_context_set_image(buffer);
	imlib_blend_image_onto_image(widget_data->img, 1, 0, 0, widget->width, widget->height,
			widget->x - up_x, widget->y - up_y, widget->width, widget->height);
}

static void render_text_widget(Imlib_Image buffer, widget_t *widget, int up_x, int up_y) {
	text_widget_t *widget_data;
	char buf[buflen];
	int n = 0, string_len, line_count = 1;
	int line_height, line_width, x, y;
	unsigned int i;
	char *c;

	widget_data = (text_widget_t *) widget->data;
	imlib_context_set_image(buffer);
	imlib_context_set_font(widget_data->font);
	imlib_context_set_color(widget_data->color.r, widget_data->color.g,
			widget_data->color.b, widget_data->color.a);

	string_len = strlen(widget_data->string);

	c = widget_data->string;
	while (*c)
		if (*c++ == '\n')
			line_count++;

	switch (widget_data->orientation) {
		case NORTH:
			imlib_context_set_direction(IMLIB_TEXT_TO_RIGHT);
			break;
		case SOUTH:
			imlib_context_set_direction(IMLIB_TEXT_TO_LEFT);
			break;
		case WEST:
			imlib_context_set_direction(IMLIB_TEXT_TO_UP);
			break;
		case EAST:
			imlib_context_set_direction(IMLIB_TEXT_TO_DOWN);
			break;
		default:
			dief("unknown orientation %hhd", widget_data->orientation);
			break;
	}

	c = widget_data->string;
	if (widget_data->orientation == NORTH || widget_data->orientation == SOUTH) {
		for (i=0; i < line_count; i++) {
			sscanf(c, "%[^\n]\n%n", buf, &n);
			c += n;
			imlib_get_text_advance(buf, &line_width, &line_height);
			switch (widget_data->alignment) {
				case 1:
					x = 0;
					y = widget->height - ((line_count - i) * line_height);
					break;
				case 2:
					x = (widget->width - line_width) / 2;
					y = widget->height - ((line_count - i) * line_height);
					break;
				case 3:
					x = widget->width - line_width;
					y = widget->height - ((line_count - i) * line_height);
					break;
				case 4:
					x = 0;
					y = ((widget->height - (line_count * line_height)) / 2)
						+ i * line_height;
					break;
				case 5:
					x = (widget->width - line_width) / 2;
					y = ((widget->height - (line_count * line_height)) / 2)
						+ i * line_height;
					break;
				case 6:
					x = widget->width - line_width;
					y = ((widget->height - (line_count * line_height)) / 2)
						+ i * line_height;
					break;
				case 7:
					x = 0;
					y = i * line_height;
					break;
				case 8:
					x = (widget->width - line_width) / 2;
					y = i * line_height;
					break;
				case 9:
					x = widget->width - line_width;
					y = i * line_height;
					break;
				default:
					dief("unknown alignment %hhd", widget_data->alignment);
					break;
			}
			imlib_context_set_cliprect(- up_x, - up_y,
					widget->width, widget->height); // FIXME
			imlib_text_draw(widget->x - up_x + x, widget->y - up_y + y, buf);
		}
	} else if (widget_data->orientation == WEST || widget_data->orientation == EAST) {
		for (i=0; i < line_count; i++) {
			sscanf(c, "%[^\n]\n%n", buf, &n);
			c += n;
			imlib_get_text_advance(buf, &line_width, &line_height);
			switch (widget_data->alignment) {
				case 1:
					x = i * line_height;;
					y = widget->height - line_width;
					break;
				case 2:
					x = ((widget->width - (line_count * line_height)) / 2)
						+ i * line_height;
					y = widget->height - line_width;
					break;
				case 3:
					x = widget->width - ((line_count - i) * line_height);
					y = widget->height - line_width;
					break;
				case 4:
					x = i * line_height;
					y = (widget->height - line_width) / 2;
					break;
				case 5:
					x = ((widget->width - (line_count * line_height)) / 2)
						+ i * line_height;
					y = (widget->height - line_width) / 2;
					break;
				case 6:
					x = widget->width - ((line_count - i) * line_height);
					y = (widget->height - line_width) / 2;
					break;
				case 7:
					x = i * line_height;
					y = 0;
					break;
				case 8:
					x = ((widget->width - (line_count * line_height)) / 2)
						+ i * line_height;
					y = 0;
					break;
				case 9:
					x = widget->width - ((line_count - i) * line_height);
					y = 0;
					break;
				default:
					dief("unknown alignment %hhd", widget_data->alignment);
					break;
			}
			imlib_context_set_cliprect(- up_x, - up_y,
					widget->width, widget->height); // FIXME
			imlib_text_draw(widget->x - up_x + x, widget->y - up_y + y, buf);
		}
	} else {
		dief("unknown orientation %hhd", widget_data->orientation);
	}
}

void render_widgets_on_drawable() {
	Imlib_Image buffer;
	Imlib_Updates update;
	widget_t *widget;
	DATA32 *imgdata;
	int i, up_x, up_y, up_w, up_h;

	updates = imlib_updates_merge_for_rendering(updates, width, height);

	for (update = updates; update; update = imlib_updates_get_next(update)) {
		imlib_updates_get_coordinates(update, &up_x, &up_y, &up_w, &up_h);

		buffer = imlib_create_image(up_w, up_h);
		imlib_context_set_image(buffer);
		imlib_image_set_has_alpha(1);

		imgdata = imlib_image_get_data();
		memset(imgdata, 0, up_w * up_h * sizeof(DATA32));
		imlib_image_put_back_data(imgdata);

		for (i=0; i < widget_list_len; i++) {
			widget = &(widget_list[i]);

			if (!((up_x + up_w > widget->x) && (widget->x + widget->width > up_x) &&
				(up_y + up_h > widget->y) && (widget->y + widget->height > up_y)))
					continue;

			imlib_context_set_image(buffer);
			imlib_context_set_cliprect(widget->x - up_x, widget->y - up_y,
					widget->width, widget->height);
			switch (widget->type) {
				case line_widget:
					render_line_widget(buffer, widget, up_x, up_y);
					break;
				case rectangle_widget:
					render_rectangle_widget(buffer, widget, up_x, up_y);
					break;
				case image_widget:
					render_image_widget(buffer, widget, up_x, up_y);
					break;
				case barchart_widget:
					render_barchart_widget(buffer, widget, up_x, up_y);
					break;
				case linechart_widget:
					render_linechart_widget(buffer, widget, up_x, up_y);
					break;
				case areachart_widget:
					render_areachart_widget(buffer, widget, up_x, up_y);
					break;
				case text_widget:
					render_text_widget(buffer, widget, up_x, up_y);
					break;
				default:
					dief("unknown widget type %hhd", widget->type);
					break;
			}
		}
		imlib_context_set_image(buffer);
		imlib_context_set_cliprect(0, 0, 0, 0);
		imlib_render_image_on_drawable(up_x, up_y);
		imlib_free_image();
	}
	if (updates) {
		imlib_updates_free(updates);
		updates = imlib_updates_init();
	}
}

/******************************************************************************/

static void update_barchart_widget_value(widget_t *widget) {
	barchart_widget_t *widget_data;
	var_t *var;
	unsigned int i;

	widget_data = (barchart_widget_t *) widget->data;

	for (i=0; i < widget->var_count; i++) {
		var = &(var_list[widget->var_id + i]);
		widget_data->data[i] = get_as_double(var->type, var->id, (*(var->func))(var));
	}
}

static void update_linechart_widget_value(widget_t *widget) {
	linechart_widget_t *widget_data;
	var_t *var;
	DATA32 *imgdata;
	color_t color;
	ImlibPolygon poly[widget->var_count];
	unsigned int i, j, index;
	double *data;
	double min = 0.0;
	double max = 0.0;
	int xval, yval;
	int widget_val_count;

	widget_data = (linechart_widget_t *) widget->data;

	if (widget_data->orientation == NORTH || widget_data->orientation == SOUTH)
		widget_val_count = widget->width;
	else
		widget_val_count = widget->height;

	imlib_context_set_image(widget_data->img);
	imgdata = imlib_image_get_data();
	memset(imgdata, 0, widget->width * widget->height * sizeof(DATA32));
	imlib_image_put_back_data(imgdata);

	index = widget_data->data_index;

	//TODO background

	for (i=0; i < widget->var_count; i++) {
		var = &(var_list[widget->var_id + i]);
		data = widget_data->data[i];
		data[index] = get_as_double(var->type, var->id, (*(var->func))(var));
	}

	if (!widget_data->static_min || !widget_data->static_max) {
		for (j=0; j < widget_val_count; j++) {
			double sum = 0.0;
			for (i=0; i < widget->var_count; i++) {
				data = widget_data->data[i];
				if (!isnan(data[j])) {
					sum += data[j];
					max = max(data[j], max);
				}
			}
			if (widget_data->add) {
				max = max(sum, max);
			}
		}
	}
	if (widget_data->static_min)
		min = widget_data->min;
	if (widget_data->static_max)
		max = widget_data->max;

	for (i=0; i < widget->var_count; i++) {
		poly[i] = imlib_polygon_new();
	}

	for (j=0; j < widget_val_count; j++) {
		double sum = 0.0;
		index = (index + 1) % widget_val_count;
		for (i=0; i < widget->var_count; i++) {
			data = widget_data->data[i];
			if (!isnan(data[index])) {
				if (widget_data->add)
					sum += data[index];
				else
					sum = data[index];
				if (widget_data->orientation == NORTH) {
					yval = widget->height - 1 -
							(int) (((sum - min) / (max - min)) *
							(double) (widget->height - 1));
					xval = j;
				} else if (widget_data->orientation == SOUTH) {
					yval = (int) (((sum - min) / (max - min)) *
							(double) (widget->height - 1));
					xval = j;
				} else if (widget_data->orientation == WEST) {
					xval = (int) (((sum - min) / (max - min)) *
							(double) (widget->width - 1));
					yval = j;
				} else if (widget_data->orientation == EAST) {
					xval = widget->width - 1 -
							(int) (((sum - min) / (max - min)) *
							(double) (widget->width - 1));
					yval = j;
				} else {
					dief("unknown orientation");
				}
				imlib_polygon_add_point(poly[i], xval, yval);
			}
		}
	}

	for (i=0; i < widget->var_count; i++) {
		color = widget_data->color[i];
		imlib_context_set_color(color.r, color.g, color.b, color.a);
		imlib_image_draw_polygon(poly[i], 0);
		imlib_polygon_free(poly[i]);
	}
}

static void update_areachart_widget_value(widget_t *widget) {
	areachart_widget_t *widget_data;
	var_t *var;
	DATA32 *imgdata;
	Imlib_Color_Range range;
	int widget_val_count;
	double *data;
	double min = 0.0;
	double max = 0.0;
	double angle;
	unsigned int i, j, index;
	int clip_x, clip_y, clip_w, clip_h;
	color_t color_min, color_max, color_top;

	widget_data = (areachart_widget_t *) widget->data;

	if (widget_data->orientation == NORTH || widget_data->orientation == SOUTH)
		widget_val_count = widget->width;
	else
		widget_val_count = widget->height;

	imlib_context_set_image(widget_data->img);
	imgdata = imlib_image_get_data();
	memset(imgdata, 0, widget->width * widget->height * sizeof(DATA32));
	imlib_image_put_back_data(imgdata);

	index = widget_data-> data_index;

	// TODO background
	// TODO redraw only last value if min/max not changed
	// TODO top_pixel/color_top

	for (i=0; i < widget->var_count; i++) {
		var = &(var_list[widget->var_id + i]);
		data = widget_data->data[i];
		data[index] = get_as_double(var->type, var->id, (*(var->func))(var));
	}

	if (!widget_data->static_min || !widget_data->static_max) {
		for (j=0; j < widget_val_count; j++) {
			double sum = 0.0;
			for (i=0; i < widget->var_count; i++) {
				data = widget_data->data[i];
				if (!isnan(data[j])) {
					sum += data[j];
					max = max(data[j], max);
				}
			}
			if (widget_data->add) {
				max = max(sum, max);
			}
		}
	}
	if (widget_data->static_min)
		min = widget_data->min;
	if (widget_data->static_max)
		max = widget_data->max;

	for (j=0; j < widget_val_count; j++) {
		int last = 0;
		index = (index + 1) % widget_val_count;
		for (i=0; i < widget->var_count; i++) {
			data = widget_data->data[i];
			if (!isnan(data[index])) {
				color_min = widget_data->color_min[i];
				color_max = widget_data->color_max[i];
				color_top = widget_data->color_top[i];
				range = imlib_create_color_range();
				imlib_context_set_color_range(range);
				imlib_context_set_color(color_max.r, color_max.g,
						color_max.b, color_max.a);
				imlib_add_color_to_color_range(0);
				imlib_context_set_color(color_min.r, color_min.g,
						color_min.b, color_min.a);

				if (widget_data->orientation == NORTH) {
					imlib_add_color_to_color_range(widget->height);
					angle = 0.0;
					clip_x = j;
					clip_w = 1;
					clip_h = round((((data[index] - min) / (max - min)) *
							(double) (widget->height)));
					clip_y = widget->height - clip_h;
					if (widget_data->add)
						clip_y -= last;
					last += clip_h;
				} else if (widget_data->orientation == SOUTH) {
					imlib_add_color_to_color_range(widget->height);
					angle = 180.0;
					clip_x = j;
					clip_w = 1;
					clip_h = round((((data[index] - min) / (max - min)) *
							(double) (widget->height)));
					clip_y = 0;
					if (widget_data->add)
						clip_y += last;
					last += clip_h;
				} else if (widget_data->orientation == WEST) {
					imlib_add_color_to_color_range(widget->width);
					angle = 90.0;
					clip_y = j;
					clip_h = 1;
					clip_w = round((((data[index] - min) / (max - min)) *
							(double) (widget->width)));
					clip_x = 0;
					if (widget_data->add)
						clip_x += last;
					last += clip_w;
				} else if (widget_data->orientation == EAST) {
					imlib_add_color_to_color_range(widget->width);
					angle = 270.0;
					clip_y = j;
					clip_h = 1;
					clip_w = round((((data[index] - min) / (max - min)) *
							(double) (widget->width)));
					clip_x = widget->width - clip_w;
					if (widget_data->add)
						clip_x -= last;
					last += clip_w;
				} else {
					dief("unknown orientation");
				}
				imlib_context_set_cliprect(clip_x, clip_y, clip_w, clip_h);
				if (clip_w > 0 && clip_h > 0)
					imlib_image_fill_color_range_rectangle(0, 0,
							widget->width, widget->height, angle);
				imlib_free_color_range();
			}
		}
	}
}

static void update_text_widget_value(widget_t *widget) {
	text_widget_t *widget_data;
	var_t *var;
	char bufi[buflen];
	char *str, *fmt, *buf = bufi;
	unsigned int i, len;

	widget_data = (text_widget_t *) widget->data;
	str = widget_data->string;
	fmt = widget_data->format;

	// copy until first '%'
	while (*fmt) {
		if (*fmt == '%') {
			if (fmt[1] == '%')
				*buf++ = *fmt++;
			else
				break;
		}
		*buf++ = *fmt++;
	}
	// no '%' found
	if (!*fmt) {
		strcpy(widget_data->string, bufi);
		return;
	}
	// copy first '%'
	*buf++ = *fmt++;
	// copy until second '%'
	while (*fmt) {
		if (*fmt == '%') {
			if (fmt[1] == '%')
				*buf++ = *fmt++;
			else
				break;
		}
		*buf++ = *fmt++;
	}
	*buf = '\0';

	for (i=0; i < widget->var_count; i++) {
		if (!*bufi)
			return;
		var = &(var_list[widget->var_id + i]);
		len = buflen - (str - widget_data->string);
		switch (var->type) {
			case string_type: {
				str += snprintf(str, len, bufi, (*(var->func))(var));
				break; }
			case char_type: {
				char val;
				val = *(char *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case uchar_type: {
				unsigned char val;
				val = *(unsigned char *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case short_type: {
				short val;
				val = *(short *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case ushort_type: {
				unsigned short val;
				val = *(unsigned short *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case int_type: {
				int val;
				val = *(int *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case uint_type: {
				unsigned int val;
				val = *(unsigned int *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case long_type: {
				long val;
				val = *(long *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case ulong_type: {
				unsigned long val;
				val = *(unsigned long *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case longlong_type: {
				long long val;
				val = *(long long *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case ulonglong_type: {
				unsigned long long val;
				val = *(unsigned long long *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case float_type: {
				float val;
				val = *(float *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case double_type: {
				double val;
				val = *(double *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			case longdouble_type: {
				long double val;
				val = *(long double *) (*(var->func))(var);
				val *= widget_data->factor[i];
				str += snprintf(str, len, bufi, val);
				break; }
			default:
				dief("Variable %u has unknown type %d", var->id, var->type);
				break;
		}
		buf = bufi;
		*buf++ = *fmt++;
		while (*fmt) {
			if (*fmt == '%') {
				if (fmt[1] == '%')
					*buf++ = *fmt++;
				else
					break;
			}
			*buf++ = *fmt++;
		}
		*buf = '\0';
	}
}

void update_widget_value(widget_t *widget) {
	updates = imlib_update_append_rect(updates, widget->x, widget->y, widget->width, widget->height);
	switch (widget->type) {
		case line_widget:
		case rectangle_widget:
		case image_widget:
			break;
		case barchart_widget:
			update_barchart_widget_value(widget);
			break;
		case linechart_widget:
			update_linechart_widget_value(widget);
			break;
		case areachart_widget:
			update_areachart_widget_value(widget);
			break;
		case text_widget:
			update_text_widget_value(widget);
			break;
		default:
			dief("unknown widget type: %hhu", widget->type);
			break;
	}
}

/******************************************************************************/

static void update_barchart_widget(widget_t *widget) {
	update_barchart_widget_value(widget);
}
static void update_linechart_widget(widget_t *widget) {
	linechart_widget_t *widget_data;

	widget_data = (linechart_widget_t *) widget->data;
	if (widget_data->orientation == EAST || widget_data->orientation == WEST)
		widget_data->data_index = (widget_data->data_index + 1) % widget->height;
	else
		widget_data->data_index = (widget_data->data_index + 1) % widget->width;
	update_linechart_widget_value(widget);
}

static void update_areachart_widget(widget_t *widget) {
	areachart_widget_t *widget_data;

	widget_data = (areachart_widget_t *) widget->data;
	if (widget_data->orientation == EAST || widget_data->orientation == WEST)
		widget_data->data_index = (widget_data->data_index + 1) % widget->height;
	else
		widget_data->data_index = (widget_data->data_index + 1) % widget->width;
	update_areachart_widget_value(widget);
}

static void update_text_widget(widget_t *widget) {
	update_text_widget_value(widget);
}

void update_widgets() {
	widget_t *widget;
	unsigned int i;

	for (i=0; i < widget_list_len; i++) {
		widget = &(widget_list[i]);
		if ((count % widget->mult) == 0) {
			updates = imlib_update_append_rect(updates, widget->x, widget->y,
					widget->width, widget->height);
			switch (widget->type) {
				case line_widget:
				case rectangle_widget:
				case image_widget:
					break;
				case barchart_widget:
					update_barchart_widget(widget);
					break;
				case linechart_widget:
					update_linechart_widget(widget);
					break;
				case areachart_widget:
					update_areachart_widget(widget);
					break;
				case text_widget:
					update_text_widget(widget);
					break;
				default:
					dief("unknown widget type: %hhu", widget->type);
					break;
			}
		}
	}
}

/******************************************************************************/

static void parse_line_widget(char *line, unsigned int var_id, unsigned int var_count, char **varlines) {
	line_widget_t *widget_data;
	widget_t *widget;
	int x1, y1, x2, y2;
	int x, y, width, height;
	int n;
	color_t color;

	n = sscanf(line, "%d %d %d %d #%2c%2hhx%2hhx%2hhx", &x1, &y1, &x2, &y2,
			&color.r, &color.g, &color.b, &color.a);

	if (n != 8)
		dief("cannot parse configuration line: line %s", line);

	x = min(x1, x2);
	y = min(y1, y2);
	width = max(x1, x2) - x;
	height = max(y1, y2) - y;

	widget_list_len++;
	widget_list = (widget_t *) srealloc((void *) widget_list, sizeof(widget_t) * widget_list_len);
	widget = &(widget_list[widget_list_len - 1]);

	widget->mult = -1;
	widget->x = x;
	widget->y = y;
	widget->width = width;
	widget->height = height;
	widget->type = line_widget;
	widget->var_id = var_id;
	widget->var_count = var_count;

	widget->data = (line_widget_t *) smalloc(sizeof(line_widget_t));
	widget_data = (line_widget_t *) widget->data;
	widget_data->color = color;
}

static void parse_rectangle_widget(char *line, unsigned int var_id, unsigned int var_count, char **varlines) {
	rectangle_widget_t *widget_data;
	widget_t *widget;
	int x, y, width, height;
	int n;
	color_t color;

	n = sscanf(line, "%d %d %d %d #%2hhx%2hhx%2hhx%2hhx", &x, &y, &width, &height,
			&color.r, &color.g, &color.b, &color.a);

	if (n != 8)
		dief("cannot parse configuration line: rectangle %s", line);

	widget_list_len++;
	widget_list = (widget_t *) srealloc((void *) widget_list, sizeof(widget_t) * widget_list_len);
	widget = &(widget_list[widget_list_len - 1]);

	widget->mult = -1;
	widget->x = x;
	widget->y = y;
	widget->width = width;
	widget->height = height;
	widget->type = rectangle_widget;
	widget->var_id = var_id;
	widget->var_count = var_count;

	widget->data = (rectangle_widget_t *) smalloc(sizeof(rectangle_widget_t));
	widget_data = (rectangle_widget_t *) widget->data;
	widget_data->color = color;
}

static void parse_image_widget(char *line, unsigned int var_id, unsigned int var_count, char **varlines) {
	image_widget_t *widget_data;
	widget_t *widget;
	int x, y, width, height;
	int n, m;
	char filename[buflen];

	n = sscanf(line, "%d %d %d %d %n", &x, &y, &width, &height, &m);

	if (n != 4)
		dief("cannot parse configuration line: image %s", line);

	if (!parse_quoted_string(line + m, filename))
		if ((sscanf(line + m, "%s", filename)) != 1)
			dief("cannot parse configuration line: image %s", line);

	widget_list_len++;
	widget_list = (widget_t *) srealloc((void *) widget_list, sizeof(widget_t) * widget_list_len);
	widget = &(widget_list[widget_list_len - 1]);

	widget->mult = -1;
	widget->x = x;
	widget->y = y;
	widget->width = width;
	widget->height = height;
	widget->type = image_widget;
	widget->var_id = var_id;
	widget->var_count = var_count;

	widget->data = (image_widget_t *) smalloc(sizeof(image_widget_t));
	widget_data = (image_widget_t *) widget->data;
	widget_data->filename = expand_filename(filename);
}

static void parse_barchart_widget(char *line, unsigned int var_id, unsigned int var_count, char **varlines) {
	barchart_widget_t *widget_data;
	widget_t *widget;
	orientation_t orientation;
	unsigned long long mult;
	int x, y, width, height;
	char orientation_char = 'N';
	boolean add = false;
	boolean static_min = false;
	boolean static_max = false;
	color_t color_min;
	color_t color_max;
	double min = NAN;
	double max = NAN;
	int n, m;
	int i;

	n = sscanf(line, "%llu %d %d %d %d %c %n", &mult, &x, &y, &width, &height, &orientation_char, &m);

	if (n < 6)
		dief("cannot parse configuration line: barchart %s", line);

	if ((n = string_starts_with(line + m, "+")))
		add = true;
	m += n;

	n = sscanf(line + m, "%lf %lf", &min, &max);

	if (n >= 1)
		static_min = true;
	if (n >= 2)
		static_max = true;

	orientation = parse_orientation(orientation_char);

	widget_list_len++;
	widget_list = (widget_t *) srealloc((void *) widget_list, sizeof(widget_t) * widget_list_len);
	widget = &(widget_list[widget_list_len - 1]);

	widget->mult = mult;
	widget->x = x;
	widget->y = y;
	widget->width = width;
	widget->height = height;
	widget->type = barchart_widget;
	widget->var_id = var_id;
	widget->var_count = var_count;

	widget->data = (barchart_widget_t *) smalloc(sizeof(barchart_widget_t));
	widget_data = (barchart_widget_t *) widget->data;
	widget_data->orientation = orientation;
	widget_data->add = add;
	widget_data->static_min = static_min;
	widget_data->static_max = static_max;
	widget_data->min = min;
	widget_data->max = max;

	widget_data->color_min = (color_t *) smalloc(sizeof(color_t) * var_count);
	widget_data->color_max = (color_t *) smalloc(sizeof(color_t) * var_count);
	widget_data->data = (double *) smalloc(sizeof(double) * var_count);

	for (i=0; i < var_count; i++) {
		n = sscanf(varlines[i], "#%2hhx%2hhx%2hhx%2hhx #%2hhx%2hhx%2hhx%2hhx",
					&color_min.r, &color_min.g, &color_min.b, &color_min.a,
					&color_max.r, &color_max.g, &color_max.b, &color_max.a);
		if (n < 4)
			dief("cannot parse barchart configuration: %s", varlines[i]);
		if (n < 8)
			color_max = color_min;
		widget_data->color_min[i] = color_min;
		widget_data->color_max[i] = color_max;
		widget_data->data[i] = NAN;
	}
}

static void parse_linechart_widget(char *line, unsigned int var_id, unsigned int var_count, char **varlines) {
	linechart_widget_t *widget_data;
	widget_t *widget;
	orientation_t orientation;
	unsigned long long mult;
	int x, y, width, height;
	char orientation_char = 'N';
	boolean add = false;
	boolean static_min = false;
	boolean static_max = false;
	color_t color;
	double min = NAN;
	double max = NAN;
	int n, m, o, p;
	int i, j;
	char filenamei[buflen];
	char *filename = filenamei;

	n = sscanf(line, "%llu %d %d %d %d %c %n", &mult, &x, &y, &width, &height, &orientation_char, &m);

	if (n < 6)
		dief("cannot parse configuration line: linechart %s", line);

	if ((n = string_starts_with(line + m, "+")))
		add = true;
	m += n;

	n = sscanf(line + m, "%lf %n %lf %n", &min, &o, &max, &p);

	if (n >= 1)
		static_min = true;
	if (n >= 2)
		static_max = true;

	if (static_min && !static_max)
		m += o;
	if (static_min && static_max)
		m += p;

	if (!parse_quoted_string(line + m, filename))
		if ((sscanf(line + m, "%s", filename)) != 1)
			filename = NULL;

	orientation = parse_orientation(orientation_char);

	widget_list_len++;
	widget_list = (widget_t *) srealloc((void *) widget_list, sizeof(widget_t) * widget_list_len);
	widget = &(widget_list[widget_list_len - 1]);

	widget->mult = mult;
	widget->x = x;
	widget->y = y;
	widget->width = width;
	widget->height = height;
	widget->type = linechart_widget;
	widget->var_id = var_id;
	widget->var_count = var_count;

	widget->data = (linechart_widget_t *) smalloc(sizeof(linechart_widget_t));
	widget_data = (linechart_widget_t *) widget->data;
	widget_data->orientation = orientation;
	widget_data->add = add;
	widget_data->static_min = static_min;
	widget_data->static_max = static_max;
	widget_data->min = min;
	widget_data->max = max;

	if (filename)
		widget_data->bgfile = expand_filename(filename);
	else
		widget_data->bgfile = filename;

	widget_data->img = imlib_create_image(width, height);
	widget_data->data_index = 0;

	imlib_context_set_image(widget_data->img);
	imlib_image_set_has_alpha(1);

	widget_data->color = (color_t *) smalloc(sizeof(color_t) * var_count);
	widget_data->data = (double **) smalloc(sizeof(double *) * var_count);

	for (i=0; i < var_count; i++) {
		n = sscanf(varlines[i], "#%2hhx%2hhx%2hhx%2hhx",
					&color.r, &color.g, &color.b, &color.a);
		if (n < 4)
			dief("cannot parse linechart configuration: %s", varlines[i]);
		widget_data->color[i] = color;
		if (orientation == WEST || orientation == EAST) {
			widget_data->data[i] = (double *) smalloc(sizeof(double) * height);
			for (j=0; j < height; j++)
				widget_data->data[i][j] = NAN;
		} else {
			widget_data->data[i] = (double *) smalloc(sizeof(double) * width);
			for (j=0; j < width; j++)
				widget_data->data[i][j] = NAN;
		}
	}
}

static void parse_areachart_widget(char *line, unsigned int var_id, unsigned int var_count, char **varlines) {
	areachart_widget_t *widget_data;
	widget_t *widget;
	orientation_t orientation;
	unsigned long long mult;
	int x, y, width, height;
	char orientation_char = 'N';
	boolean add = false;
	boolean static_min = false;
	boolean static_max = false;
	color_t color_min;
	color_t color_max;
	color_t color_top;
	unsigned int top_pixel;
	double min = NAN;
	double max = NAN;
	int n, m, o, p;
	int i, j;
	char filenamei[buflen];
	char *filename = filenamei;
	DATA32 *imgdata;

	n = sscanf(line, "%llu %d %d %d %d %c %n", &mult, &x, &y, &width, &height, &orientation_char, &m);

	if (n < 6)
		dief("cannot parse configuration line: areachart %s", line);

	if ((n = string_starts_with(line + m, "+")))
		add = true;
	m += n;

	n = sscanf(line + m, "%lf %n %lf %n", &min, &o, &max, &p);

	if (n >= 1)
		static_min = true;
	if (n >= 2)
		static_max = true;

	if (static_min && !static_max)
		m += o;
	if (static_min && static_max)
		m += p;

	if (!parse_quoted_string(line + m, filename))
		if ((sscanf(line + m, "%s", filename)) != 1)
			filename = NULL;

	orientation = parse_orientation(orientation_char);

	widget_list_len++;
	widget_list = (widget_t *) srealloc((void *) widget_list, sizeof(widget_t) * widget_list_len);
	widget = &(widget_list[widget_list_len - 1]);

	widget->mult = mult;
	widget->x = x;
	widget->y = y;
	widget->width = width;
	widget->height = height;
	widget->type = areachart_widget;
	widget->var_id = var_id;
	widget->var_count = var_count;

	widget->data = (areachart_widget_t *) smalloc(sizeof(areachart_widget_t));
	widget_data = (areachart_widget_t *) widget->data;
	widget_data->orientation = orientation;
	widget_data->add = add;
	widget_data->static_min = static_min;
	widget_data->static_max = static_max;
	widget_data->min = min;
	widget_data->max = max;

	if (filename)
		widget_data->bgfile = expand_filename(filename);
	else
		widget_data->bgfile = filename;

	widget_data->img = imlib_create_image(width, height);
	widget_data->data_index = 0;

	imlib_context_set_image(widget_data->img);
	imlib_image_set_has_alpha(1);
	imgdata = imlib_image_get_data();
	memset(imgdata, 0, width * height * sizeof(DATA32));
	imlib_image_put_back_data(imgdata);

	widget_data->color_min = (color_t *) smalloc(sizeof(color_t) * var_count);
	widget_data->color_max = (color_t *) smalloc(sizeof(color_t) * var_count);
	widget_data->color_top = (color_t *) smalloc(sizeof(color_t) * var_count);
	widget_data->top_pixel = (unsigned int *) smalloc(sizeof(unsigned int) * var_count);
	widget_data->data = (double **) smalloc(sizeof(double *) * var_count);

	for (i=0; i < var_count; i++) {
		n = sscanf(varlines[i], "#%2hhx%2hhx%2hhx%2hhx #%2hhx%2hhx%2hhx%2hhx "
					"#%2hhx%2hhx%2hhx%2hhx %u",
				&color_min.r, &color_min.g, &color_min.b, &color_min.a,
				&color_max.r, &color_max.g, &color_max.b, &color_max.a,
				&color_top.r, &color_top.g, &color_top.b, &color_top.a,
				&top_pixel);
		if (n < 4)
			dief("cannot parse areachart configuration: %s", varlines[i]);
		if (n < 8)
			color_max = color_min;
		if (n < 12)
			top_pixel = 0;
		widget_data->color_min[i] = color_min;
		widget_data->color_max[i] = color_max;
		widget_data->color_top[i] = color_top;
		widget_data->top_pixel[i] = top_pixel;
		if (orientation == WEST || orientation == EAST) {
			widget_data->data[i] = (double *) smalloc(sizeof(double) * height);
			for (j=0; j < height; j++)
				widget_data->data[i][j] = NAN;
		} else {
			widget_data->data[i] = (double *) smalloc(sizeof(double) * width);
			for (j=0; j < width; j++)
				widget_data->data[i][j] = NAN;
		}
	}
}

static void parse_text_widget(char *line, unsigned int var_id, unsigned int var_count, char **varlines) {
	text_widget_t *widget_data;
	widget_t *widget;
	orientation_t orientation;
	unsigned long long mult;
	int x, y, width, height;
	char orientation_char = 'N';
	char alignment_char = '7';
	color_t color;
	unsigned int i;
	int n, m;
	unsigned char alignment;
	char format[buflen];
	char font_string[buflen];
	double factor;

	n = sscanf(line, "%llu %d %d %d %d %c %c #%2hhx%2hhx%2hhx%2hhx %n", &mult, &x, &y,
			&width, &height, &orientation_char, &alignment_char,
			&color.r, &color.g, &color.b, &color.a, &m);

	if (n < 11)
		dief("cannot parse configuration line: text %s", line);

	if (!(n = parse_quoted_string(line + m, font_string)))
		if ((sscanf(line + m, "%s %n", font_string, &n)) != 1)
			dief("cannot parse configuration line: text %s", line);

	if (!parse_quoted_string(line + m + n, format))
		if ((sscanf(line + m + n, "%s", format)) != 1)
			dief("cannot parse configuration line: text %s", line);

	orientation = parse_orientation(orientation_char);

	switch (alignment_char) {
		case '1':
			alignment = 1;
			break;
		case '2':
			alignment = 2;
			break;
		case '3':
			alignment = 3;
			break;
		case '4':
			alignment = 4;
			break;
		case '5':
			alignment = 5;
			break;
		case '6':
			alignment = 6;
			break;
		case '7':
			alignment = 7;
			break;
		case '8':
			alignment = 8;
			break;
		case '9':
			alignment = 9;
			break;
		default:
			debugf("using default alignment for text: %s", line);
			alignment = 7;
			break;
	}

	widget_list_len++;
	widget_list = (widget_t *) srealloc((void *) widget_list, sizeof(widget_t) * widget_list_len);
	widget = &(widget_list[widget_list_len - 1]);

	widget->mult = mult;
	widget->x = x;
	widget->y = y;
	widget->width = width;
	widget->height = height;
	widget->type = text_widget;
	widget->var_id = var_id;
	widget->var_count = var_count;

	widget->data = (text_widget_t *) smalloc(sizeof(text_widget_t));
	widget_data = (text_widget_t *) widget->data;
	widget_data->orientation = orientation;
	widget_data->alignment = alignment;
	widget_data->color = color;

	if (!(widget_data->font = imlib_load_font(font_string)))
		dief("cannot load font: %s", font_string);

	widget_data->format = (char *) smalloc(strlen(format) + 1);
	strcpy(widget_data->format, format);

	widget_data->string = (char *) smalloc(buflen);

	widget_data->factor = (double *) smalloc(sizeof(double) * var_count);

	for (i=0; i < var_count; i++) {
		n = sscanf(varlines[i], "%lf", &factor);

		if (n < 1)
			factor = 1.0;

		widget_data->factor[i] = factor;
	}
}

void parse_widgets(char *buf) {
	// TODO
	if (false) {
		parse_line_widget(NULL, 0, 0, NULL);
		parse_rectangle_widget(NULL, 0, 0, NULL);
		parse_image_widget(NULL, 0, 0, NULL);
		parse_barchart_widget(NULL, 0, 0, NULL);
		parse_linechart_widget(NULL, 0, 0, NULL);
		parse_areachart_widget(NULL, 0, 0, NULL);
		parse_text_widget(NULL, 0, 0, NULL);
	}
}

/******************************************************************************/

void default_widgets() {

	var_list_len = 6;
	var_list = (var_t *) smalloc(sizeof(var_t) * var_list_len);

	if (!set_var(&(var_list[1]), 1, 1, "cpu_percents:kernel"))
		dief("cannot set var: cpu_percents:kernel");
	if (!set_var(&(var_list[2]), 2, 1, "cpu_percents:user"))
		dief("cannot set var: cpu_percents:user");
	if (!set_var(&(var_list[3]), 3, 1, "cpu_percents:iowait"))
		dief("cannot set var: cpu_percents:iowait");
	if (!set_var(&(var_list[4]), 4, 1, "cpu_percents:swap"))
		dief("cannot set var: cpu_percents:swap");
	if (!set_var(&(var_list[5]), 5, 1, "cpu_percents:nice"))
		dief("cannot set var: cpu_percents:nice");

	static char *varlines[] = {
		"#88000088 #FF0000FF",
		"#00880088 #00FF00FF",
		"#00008888 #0000FFFF",
		"#88008888 #FF00FFFF",
		"#88880088 #FFFF00FF"
	};

	parse_rectangle_widget("0 0 120 100 #000000FF", 0, 0, NULL);
	parse_areachart_widget("1 0 0 120 100 N + 0.0 100.0", 1, 5, varlines);
}

