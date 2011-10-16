## Process this file with automake to produce Makefile.in

bin_PROGRAMS = sms-info sms-norm sms-random sms-randminor \
	sms-reordcols sms-reordrows sms-rescale sms-shrink sms-transpose
man1_MANS = sms-info.1 sms-norm.1 sms-random.1 sms-randminor.1 \
	sms-reordcols.1	sms-reordrows.1 sms-rescale.1 sms-shrink.1 sms-transpose.1

sms_info_SOURCES = sms-info.cpp
sms_norm_SOURCES = sms-norm.cpp
sms_random_SOURCES = sms-random.cpp
sms_randminor_SOURCES = sms-randminor.cpp
sms_reordcols_SOURCES = sms-reordcols.cpp
sms_reordrows_SOURCES = sms-reordrows.cpp
sms_rescale_SOURCES = sms-rescale.cpp
sms_shrink_SOURCES = sms-shrink.cpp
sms_transpose_SOURCES = sms-transpose.cpp


ACLOCAL_AMFLAGS = -I build-aux/m4
AM_CPPFLAGS = -I$(srcdir) -I$(top_srcdir) $(BOOST_CPPFLAGS)
