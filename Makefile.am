include extra/prelude.am

ACLOCAL_AMFLAGS		+= -I m4
AUTOMAKE_OPTIONS	+= -Wno-portability -Wno-override

BUILT_SOURCES		= # empty

EXTRA_DIST		= # empty

AM_CFLAGS		= # empty
AM_CPPFLAGS		= # empty
AM_CXXFLAGS		= # empty
AM_LDFLAGS		= # empty
AM_YFLAGS		= # empty

TESTS			= # empty
XFAIL_TESTS		= # empty

# Standard includes
AM_CPPFLAGS		+= -I$(top_srcdir) -I$(top_srcdir)/include
AM_CPPFLAGS		+= -I$(top_builddir) -I$(top_builddir)/include

AM_YFLAGS += -d --verbose
AM_CPPFLAGS += -ggdb3
#AM_CPPFLAGS += -include stdafx/stdafx.h
#AM_CPPFLAGS += -include stdafx.h
#AM_CPPFLAGS += -include-pch stdafx.h.pch
#AM_CPPFLAGS += -D_GLIBCXX_DEBUG
AM_CPPFLAGS += -DVISIBLE='__attribute__ ((__visibility__ ("default")))'
#AM_CXXFLAGS += -fnon-call-exceptions
AM_CXXFLAGS += -fvisibility=hidden
AM_CXXFLAGS += -DM4DIR='"$(m4dir)"'
AM_CXXFLAGS += -DPKGDATADIR='"$(pkgdatadir)"'
AM_CXXFLAGS += -DRULESDIR='"$(rulesdir)"'

AM_CXXFLAGS += -O3
AM_CXXFLAGS += -march=native
#AM_CXXFLAGS += -fopenmp -DOPENMP
#AM_CXXFLAGS += -fweb
#AM_CXXFLAGS += -ffast-math
#AM_CXXFLAGS += -fprofile-generate
#AM_CXXFLAGS += -fprofile-use
#AM_CXXFLAGS += -fno-inline
#AM_CXXFLAGS += -finline-small-functions
#AM_CXXFLAGS += -fno-unroll-loops

COMLIB = -lboost_filesystem-mt -lboost_regex-mt -lboost_system-mt $(PROTOBUF_LIBS)

CLEANFILES =		\
	nodes.bin	\
	nodes.txt	\
	nodes.xml

include extra/Rules.am
include include/Rules.am
include src/Rules.am
include testsuite/Rules.am
