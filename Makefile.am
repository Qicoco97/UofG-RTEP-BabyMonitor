NULL =

AM_CXXFLAGS = $(ARCH_CXXFLAGS) $(WARN_CXXFLAGS) -Wno-error=inline -Wno-inline -O3 \
	-funsafe-math-optimizations \
	-fno-trapping-math \
	-fno-signaling-nans \
	-pthread

AM_LDFLAGS = -lrt

bin_PROGRAMS = babymonitor

babymonitor_SOURCES = \
    src/VideoSource.hpp \
    src/CommandLine.hpp \
    src/RieszTransform.hpp \
    src/ComplexMat.hpp \
    src/Butterworth.hpp \
    src/WorkerThread.hpp \
    src/INIReader.h \
    src/ini.h \
    src/MotionDetection.hpp \
    src/CommandLine.cpp \
    src/Butterworth.cpp \
    src/INIReader.cpp \
    src/ini.c \
    src/MotionDetection.cpp \
    src/RieszTransform.cpp \
    src/main.cpp \
    src/VideoSource.cpp \
    $(NULL)

babymonitor_CXXFLAGS = $(AM_CXXFLAGS) $(DEPS_CFLAGS)
babymonitor_LDFLAGS = $(AM_LDFLAGS) $(DEPS_LIBS)

dist_doc_DATA = \
    docs/index.md \
    docs/setup/hw-setup.md \
    docs/setup/sw-setup.md

configdir = $(sysconfdir)/babymonitor

dist_config_DATA = config.ini

systemdsystemunitdir = $(sysconfdir)/systemd/system
systemdsystemunit_DATA = babymonitor.service

babymonitor.service: babymonitor.service.in
	sed -e 's|[@]bindir@|$(bindir)|g' -e 's|[@]sysconfdir@|$(sysconfdir)|g' $< > $@

CLEANFILES = babymonitor.service

GITIGNOREFILES = \
	aclocal.m4 \
	build-aux \
	config.h.in \
	m4/libtool.m4 \
	m4/ltoptions.m4 \
	m4/ltsugar.m4 \
	m4/ltversion.m4 \
	m4/lt~obsolete.m4

-include $(top_srcdir)/git.mk
