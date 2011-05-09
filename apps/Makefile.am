bin_PROGRAMS = splay @XSPLAY@
EXTRA_PROGRAMS =xsplay
splay_SOURCES =  common.cc splay.cc splay.h
xsplay_SOURCES = xsplay.cc functions.cc \
	multipleselect.cc fileselect.cc panel.cc \
	optionpanel.cc \
	moc_multi.cc moc_file.cc moc_panel.cc \
	moc_optionp.cc common.cc

noinst_HEADERS		= multipleselect.h fileselect.h \
			  panel.h optionpanel.h xsplay.h



LDADD= ../libs/libmpeg.a

INCLUDES = -I../libs  -I/usr/include/qt
xsplay_LDADD=-lqt $(LDADD)

man_MANS = splay.1 xsplay.1
EXTRA_DIST = $(man_MANS)




LIBS			= @LIBS@

moc_multi.cc : multipleselect.h
	moc -o moc_multi.cc	multipleselect.h
moc_file.cc  : fileselect.h
	moc -o moc_file.cc	fileselect.h
moc_panel.cc : panel.h
	moc -o moc_panel.cc	panel.h
moc_optionp.cc : optionpanel.h
	moc -o moc_optionp.cc	optionpanel.h

