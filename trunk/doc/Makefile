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

DESTDIR ?= /usr/local
DOCDIR  := $(DESTDIR)/share/doc/xsysguard
PERL    ?= /usr/bin/env perl
HTMLGEN := htmlgen.pl
VERSION := ../VERSION
HTML    := html/index.html \
	   html/examples.html \
	   html/configuration.html \
	   html/variables.html \
	   html/daemons.html
TXT     := configuration.txt \
           variables.txt \
	   daemons.txt

all: $(HTML)

html/index.html: $(HTMLGEN) $(VERSION)
	$(PERL) $(HTMLGEN) Home $^ > $@

html/examples.html: $(HTMLGEN) $(VERSION)
	$(PERL) $(HTMLGEN) Examples $^ > $@

html/configuration.html: $(HTMLGEN) $(VERSION) configuration.txt
	$(PERL) $(HTMLGEN) Configuration $^ > $@

html/variables.html: $(HTMLGEN) $(VERSION) variables.txt
	$(PERL) $(HTMLGEN) Variables $^ > $@

html/daemons.html: $(HTMLGEN) $(VERSION) daemons.txt
	$(PERL) $(HTMLGEN) Daemons $^ > $@

clean:
	rm -f $(HTML)

distclean: clean

install: all
	mkdir -p $(DOCDIR)
	mkdir -p $(DOCDIR)/html
	mkdir -p $(DOCDIR)/html/widgets
	mkdir -p $(DOCDIR)/html/examples
	cp $(TXT) $(DOCDIR)/
	cp $(HTML) $(DOCDIR)/html/
	cp html/widgets/*.png $(DOCDIR)/html/widgets/
	cp html/examples/*.png $(DOCDIR)/html/examples/

.PHONY: all clean distclean install

