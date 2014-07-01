######################################################################
# Unit/regression tests, based on CxxTest.
# Use the 'test' target to run them.
# Edit TESTS and TESTLIBS to add more tests.
#
######################################################################
include config.mk

TESTS		:= $(srcdir)/test/common/*.h $(srcdir)/test/audio/*.h $(srcdir)/test/math/*.h
TEST_LIBS = $(OBJS)
TEST_LIBS	:= engines/libengines.a gui/libgui.a image/libimage.a
TEST_LIBS	+= backends/libbackends.a video/libvideo.a graphics/libgraphics.a
TEST_LIBS	+= audio/libaudio.a math/libmath.a base/libbase.a common/libcommon.a
# Included again to make it link...
TEST_LIBS	+= engines/libengines.a audio/libaudio.a backends/libbackends.a common/libcommon.a

### We have to include the backend too...
ifdef SDL_BACKEND
ifdef POSIX
TEST_LIBS	+= backends/platform/sdl/posix/posix.o backends/saves/posix/posix-saves.o
TEST_LIBS	+= backends/fs/posix/posix-fs.o backends/fs/posix/posix-fs-factory.o
else
ifdef WIN32
TEST_LIBS	+= backends/platform/sdl/win32/win32.o
else
ifdef MACOSX
TEST_LIBS	+= backends/platform/sdl/macosx/macosx.o
else
	### FIXME
endif
endif
endif
### All SDL platforms get this
TEST_LIBS	+= backends/platform/sdl/sdl.o backends/graphics/surfacesdl/surfacesdl-graphics.o
TEST_LIBS	+= backends/graphics/sdl/sdl-graphics.o backends/events/sdl/sdl-events.o
TEST_LIBS	+= backends/mixer/sdl/sdl-mixer.o backends/mixer/doublebuffersdl/doublebuffersdl-mixer.o
TEST_LIBS	+= backends/timer/sdl/sdl-timer.o backends/mutex/sdl/sdl-mutex.o backends/audiocd/sdl/sdl-audiocd.o
else
	### FIXME
endif

### Everyone gets these!
TEST_LIBS	+= backends/log/log.o backends/saves/default/default-saves.o backends/saves/savefile.o
TEST_LIBS	+= backends/fs/abstract-fs.o backends/fs/stdiostream.o backends/events/default/default-events.o
TEST_LIBS	+= backends/timer/default/default-timer.o backends/audiocd/default/default-audiocd.o
TEST_LIBS	+= backends/base-backend.o backends/modular-backend.o

### Make optional tests based on the configuration
ifdef ENABLE_MONKEY4
TESTS		+= $(srcdir)/test/engines/grim/*.h
TEST_LIBS	:= engines/grim/libgrim.a $(TEST_LIBS) engines/grim/libgrim.a audio/libaudio.a engines/libengines.a common/libcommon.a
endif

ifdef ENABLE_MYST3
TEST_LIBS	:= engines/myst3/libmyst3.a $(TEST_LIBS) engines/myst3/libmyst3.a audio/libaudio.a engines/libengines.a common/libcommon.a
endif


#
TEST_FLAGS   := --runner=StdioPrinter --no-std --no-eh --include=$(srcdir)/test/cxxtest_mingw.h
TEST_CFLAGS  := -I$(srcdir)/test/cxxtest
TEST_LDFLAGS := $(LIBS)
TEST_CXXFLAGS := $(filter-out -Wglobal-constructors,$(CXXFLAGS))

ifdef HAVE_GCC3
# In test/common/str.h, we test a zero length format string. This causes GCC
# to generate a warning which in turn poses a problem when building with -Werror.
# To work around this, we disable -Wformat here.
TEST_CFLAGS  +=  -Wno-format
endif

# Enable this to get an X11 GUI for the error reporter.
#TEST_FLAGS   += --gui=X11Gui
#TEST_LDFLAGS += -L/usr/X11R6/lib -lX11


test: test/runner
	./test/runner
test/runner: test/runner.cpp $(TEST_LIBS)
	$(QUIET_LINK)$(CXX) $(TEST_CXXFLAGS) $(CPPFLAGS) $(TEST_CFLAGS) -o $@ $+ $(TEST_LDFLAGS)
test/runner.cpp: $(TESTS)
	@mkdir -p test
	$(srcdir)/test/cxxtest/cxxtestgen.py $(TEST_FLAGS) -o $@ $+


clean: clean-test
clean-test:
	-$(RM) test/runner.cpp test/runner

.PHONY: test clean-test
