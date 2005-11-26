# This file is part of xsysguard <http://xsysguard.berlios.de>
# Copyright 2005 Sascha Wessel <wessel@users.berlios.de>
#
# Xsysguard is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Xsysguard is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with xsysguard; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


CC      := gcc
LDLIBS  := -L. -lstatgrab
LDFLAGS := -I. -Wall -Werror -ggdb
IMLIB2  := $(shell imlib2-config --cflags --libs)
X11SRC  := x11.c x11.h widgets.c widgets.h
SRC     := util.c util.h common.c common.h libsg.c libsg.h

OSTYPE  := $(shell uname -s)
ifeq (${OSTYPE},SunOS)
LDLIBS  += -lkstat -lsocket -ldevinfo
endif

all: xsysguardd xsysguard

xsysguard: $(SRC) xsysguard.c $(X11SRC)
	$(CC) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LDLIBS) $(IMLIB2)

xsysguardd: $(SRC) xsysguardd.c
	$(CC) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LDLIBS)

clean distclean:
	rm -f xsysguardd
	rm -f xsysguard

install:
	cp xsysguardd $(DESTDIR)/bin
	cp xsysguard $(DESTDIR)/bin

.PHONY: all clean distclean install

