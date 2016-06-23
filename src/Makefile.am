## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gdmap

gdmap_SOURCES = \
	main.c l_i18n.h \
	utils.c utils.h \
	gui_support.c gui_support.h \
	gui_main.c gui_main.h \
	colors.c colors.h \
	preferences.c preferences.h \
	about.c about.h \
	tree.c tree.h

localedir = $(datadir)/locale
gdmap_CFLAGS = \
	-DGDMAP_LOCALE_DIR=\""$(localedir)"\" -DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" @UI_CFLAGS@ @LIBXML_CFLAGS@

gdmap_LDADD = @UI_LIBS@ @LIBXML_LIBS@ -lm

