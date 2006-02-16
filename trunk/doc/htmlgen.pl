#!/usr/bin/env perl

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

use warnings;
use strict;
use POSIX qw(strftime);

my ($version, $id, $title, $time);

open(VERSION, $ARGV[2]) || die('cannot read VERSION file');
$version = <VERSION>;
close(VERSION);

$id = '$Id$';
$title = $ARGV[0];
$time = strftime('%F %T', localtime);

if (!$ARGV[0] || !$ARGV[1] || ($title ne 'Home' && $title ne 'Examples'
			&& $title ne 'Configuration' && $title ne 'Variables'
			&& $title ne 'Daemons') || $ARGV[1] ne 'htmlgen.pl') {
	die('please dont run this script directly - use make!');
}


# copied from http://www.oac.uci.edu/indiv/ehood/tar/perl2html
my @perl_keywords = qw(
	-A -B -C -M -O -R -S -T -W -X -b -c -d -e -f -g -k -l -o -p
	-r -s -t -u -w -x -z
	ARGV DATA ENV SIG STDERR STDIN STDOUT
	atan2
	bind binmode bless
	caller chdir chmod chomp chop chown chr chroot close closedir
	cmp connect continue cos crypt
	dbmclose dbmopen defined delete die do dump
	each else elsif endgrent endhostent endnetent endprotoent
	endpwent endservent eof eq eval exec exists exit exp
	fcntl fileno flock for foreach fork format formline
	ge getc getgrent getgrgid getgrnam gethostbyaddr gethostbyname
	gethostent getlogin getnetbyaddr getnetbyname getnetent getpeername
	getpgrp getppid getpriority getprotobyname getprotobynumber
	getprotoent getpwent getpwnam getpwuid getservbyname getservbyport
	getservent getsockname getsockopt glob gmtime goto grep gt
	hex
	if
	import index int ioctl
	join
	keys kill
	last lc lcfirst le length link listen local localtime log lstat lt
	m map mkdir msgctl msgget msgrcv msgsnd my
	ne next no
	oct open opendir ord
	pack package pipe pop pos print printf push
	q qq quotemeta qw qx
	rand read readdir readlink recv redo ref rename require reset
	return reverse rewinddir rindex rmdir
	s scalar seek seekdir select semctl semget semop send setgrent
	sethostent setnetent setpgrp setpriority setprotoent setpwent
	setservent setsockopt shift shmctl shmget shmread shmwrite shutdown
	sin sleep socket socketpair sort splice split sprintf sqrt srand
	stat study sub substr symlink syscall sysopen sysread system
	syswrite
	tell telldir tie tied time times tr truncate
	uc ucfirst umask undef unless unlink unpack unshift untie until
	use utime
	values vec wait
	waitpid wantarray warn while write
	y
);
my %perl_keywords;
@perl_keywords{@perl_keywords} = (1) x scalar(@perl_keywords);

sub highlight_perl_keyword {
	my $word = shift;
	if ($word eq '<') {
		return '&lt;';
	}
	if ($word eq '>') {
		return '&gt;';
	}
	if ($word eq '&') {
		return '&amp;';
	}
	if ($perl_keywords{$word}) {
		return qq(<div class="perl_keyword">$word</div>);
	}
	$word;
}

sub print_perl {
	my $file = shift;
	my ($code, $comment);
	open(PERL, $file) || die("cannot read file: $file");
	while (<PERL>) {
		($code, $comment) = split(/#/, $_, 2);
		if ($code ne '') {
			$code =~ s/(<|>|&|\b\w+\b|-\w\b)/highlight_perl_keyword($1)/ge;
			print $code;
		}
		if ($comment ne '') {
			print qq(<div class="perl_comment">#$comment</div>);
		}
	}
}


print <<"EOF";
<?xml version="1.0" encoding="iso-8859-1" ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>xsysguard - $title</title>
<link rel="icon" type="image/png" href="http://xsysguard.berlios.de/favicon.ico" />
<style type="text/css">
<!--
body {
	margin: 10px;
	padding: 0px;
	background: #FFF;
	color: #000;
	font-family: Arial, Helvetica, sans-serif;
	font-size: 90%;
}
img {
	border: 0px;
}
table {
	border: 0px;
	padding: 0px;
	margin: 0px;
	width: 100%;
}
.code {
	padding: 10px;
	margin: 0px;
	border: 1px solid #888;
	background: #EEE;
}
td {
	padding: 0px;
	margin: 0px;
	vertical-align: top;
}
pre {
	margin: 0px;
	padding: 0px;
	font-size: 90%;
}
.prot {
	width: 100%;
	border-bottom: 1px dashed #BBB;
	border-top: 1px dashed #BBB;
	font-weight: bold;
}
.sub {
	width: 100%;
	margin-top: 4px;
	margin-bottom: 16px;
}
.small {
	width: 1px;
	color: #BBB;
	font-size: 85%;
}
.perl_comment {
	color: #888;
	font-style: italic;
}
.perl_keyword {
	color: #88F;
	font-weight: bold;
}
.conf {
	color: #222;
}
.conf_comment {
	font-style: italic;
	color: #888;
}
.conf_var {
	color: #88F;
}
.conf_opt {
	color: #FB8;
}
.conf_ret {
	color: #8A8;
	font-style: italic;
}
.footer {
	color: #AAA;
	font-style: italic;
	font-size: 90%;
	text-decoration: none;
}
-->
</style>
</head>
<body>
<table class="menu">
<tr>
<td>
<a href="index.html">Home</a> |
<a href="examples.html">Examples</a> |
<a href="configuration.html">Configuration</a> |
<a href="variables.html">Variables</a> |
<a href="daemons.html">Daemons</a> |
<a href="http://developer.berlios.de/project/showfiles.php?group_id=5348">Downloads</a>
</td>
<td style="text-align: right;">
<a href="http://developer.berlios.de/projects/xsysguard/">
<img src="http://developer.berlios.de/bslogo.php?group_id=5348" 
	width="124" height="32" alt="BerliOS" /></a>
</td>
</tr>
</table>
EOF
if ($title eq 'Home') {
	print "<h3>FIXME</h3>\n";
} elsif ($title eq 'Examples') {
	print "<h3>FIXME</h3>\n";
} elsif ($title eq 'Configuration') {
	my $file = $ARGV[3];
	open(FILE, $file) || die("cannot read file: $file");
	my @data = <FILE>;
	close(FILE);
	chomp(@data);
	print qq(<table class="code">\n);
	for (my $i = 0; $i < scalar(@data);) {
		if ($data[$i] =~ /^#/) {
			print q(<tr><td colspan="2" ><pre class="conf_comment">);
			while ($data[$i] =~ /^#/) {
				$data[$i] =~ s/&/&amp;/g;
				$data[$i] =~ s/<([^>]*)>/<span class=\"conf_var\">&lt;$1&gt;<\/span>/g;
				$data[$i] =~ s/\[([^]]*)\]/<span class=\"conf_opt\">\[$1\]<\/span>/g;
				print $data[$i] . "\n";
				if (++$i >= scalar(@data)) {
					last;
				}
			}
			print qq(</pre></td></tr>\n);
		} else {
			my ($name, $rest) =  split(/ /, $data[$i], 2);
			print q(<tr><td><pre class="conf">);
			while (!($data[$i] =~ /^#/)) {
				$data[$i] =~ s/&/&amp;/g;
				$data[$i] =~ s/</&lt;/g;
				$data[$i] =~ s/>/&gt;/g;
				print $data[$i] . "\n";
				if (++$i >= scalar(@data)) {
					last;
				}
			}
			print q(</pre></td><td style="text-align:right;">);
			if (-e "html/widgets/$name.png") {
				print qq(<img src="widgets/$name.png" width="120" height="80" alt="" />);
			}
			print qq(</td></tr>\n);
		}
	}
	print qq(</table>\n);
} elsif ($title eq 'Variables') {
	my $file = $ARGV[3];
	open(FILE, $file) || die("cannot read file: $file");
	my @data = <FILE>;
	close(FILE);
	chomp(@data);
	print qq(<table class="code">\n);
	for (my $i = 0; $i < scalar(@data); $i++) {
		if ($data[$i] =~ /^#/) {
			$data[$i] =~ s/&/&amp;/g;
			$data[$i] =~ s/<([^>]*)>/<span class=\"conf_var\">&lt;$1&gt;<\/span>/g;
			$data[$i] =~ s/\[([^]]*)\]/<span class=\"conf_opt\">\[$1\]<\/span>/g;
			$data[$i] =~ s/(returns .*)$/<span class=\"conf_ret\">$1<\/span>/g;
			print qq(<tr><td><pre class="conf_comment">);
			print $data[$i];
			print qq(</pre></td></tr>\n);
		} else {
			$data[$i] =~ s/&/&amp;/g;
			$data[$i] =~ s/</&lt;/g;
			$data[$i] =~ s/>/&gt;/g;
			print qq(<tr><td><pre class="conf">);
			print $data[$i] . "<br />";
			print qq(</pre></td></tr>\n);
		}
	}
	print qq(</table>\n);
} elsif ($title eq 'Daemons') {
	print "<h3>FIXME</h3>\n";
}

print <<"EOF";
<hr class="footer" />
<table class="footer">
<tr>
<td>Generated $time for xsysguard $version with $id</td>
<td style="text-align: right;">&copy;
<a style="text-decoration: none; color: #AAA;" href="mailto:wessel\@users.berlios.de">Sascha Wessel</a></td>
</tr>
</table>
</body>
</html>
EOF


