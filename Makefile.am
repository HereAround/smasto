## Process this file with automake to produce Makefile.in

bin_PROGRAMS = \
	sms-adjoin \
	sms-blockechelon \
	sms-info \
	sms-norm \
	sms-random \
	sms-randminor \
	sms-reordcols \
	sms-reordrows \
	sms-rescale \
	sms-shrink \
	sms-transpose \
	sms-to-svg \
	sms-wellknown

man_MANS = \
	man/sms-adjoin.1 \
	man/sms-info.1 \
	man/sms-norm.1 \
	man/sms-randminor.1 \
	man/sms-random.1 \
	man/sms-reordcols.1 \
	man/sms-reordrows.1 \
	man/sms-rescale.1 \
	man/sms-shrink.1 \
	man/sms-to-svg.1 \
	man/sms-transpose.1 \
	man/sms-wellknown.1

sms_adjoin_SOURCES = src/sms-adjoin.cpp
sms_blockechelon_SOURCES = src/sms-blockechelon.cpp
sms_info_SOURCES = src/sms-info.cpp
sms_norm_SOURCES = src/sms-norm.cpp
sms_random_SOURCES = src/sms-random.cpp
sms_randminor_SOURCES = src/sms-randminor.cpp
sms_reordcols_SOURCES = src/sms-reordcols.cpp
sms_reordrows_SOURCES = src/sms-reordrows.cpp
sms_rescale_SOURCES = src/sms-rescale.cpp
sms_shrink_SOURCES = src/sms-shrink.cpp
sms_transpose_SOURCES = src/sms-transpose.cpp
sms_to_svg_SOURCES = src/sms-to-svg.cpp
sms_wellknown_SOURCES = src/sms-wellknown.cpp


ACLOCAL_AMFLAGS = -I build-aux/m4
AM_CPPFLAGS = -I$(srcdir) -I$(top_srcdir) $(BOOST_CPPFLAGS)

