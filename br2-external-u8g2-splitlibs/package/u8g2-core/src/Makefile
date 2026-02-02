\
# Build wrapper for u8g2 core as shared library (without font data)
#
# Expected upstream layout:
#   $(U8G2_UPSTREAM_DIR)/csrc/*.c and *.h
#
# This Makefile is used by Buildroot package/u8g2-core/u8g2-core.mk, but
# can also be used standalone.

PREFIX ?= /usr
DESTDIR ?=
U8G2_UPSTREAM_DIR ?= ../u8g2

CSRCDIR := $(U8G2_UPSTREAM_DIR)/csrc

LIBNAME := u8g2
SONAME  := lib$(LIBNAME).so.1
REALSO  := lib$(LIBNAME).so.1.0.0

# Toolchain
CC     ?= gcc
AR     ?= ar
RANLIB ?= ranlib
STRIP  ?= strip

# Flags
CFLAGS   ?= -O2
CPPFLAGS ?=
LDFLAGS  ?=

# Always build PIC objects for shared lib
CFLAGS   += -D__ARM_LINUX__ -D__GNUC__ -fPIC -std=c99 
CPPFLAGS += -I$(CSRCDIR)

# u8g2 contains a huge built-in font compilation unit:
#   csrc/u8g2_fonts.c  (very large)
# and (depending on how you vendor sources) possibly generated per-font units:
#   csrc/u8g2_font_*.c
# We exclude those from libu8g2.so; they go into libu8g2font.so.
SRCS := $(shell find "$(CSRCDIR)" -maxdepth 1 -type f -name '*.c' \
            ! -name 'u8g2_fonts.c' \
            ! -name 'u8g2_font_*.c' )

OBJS := $(patsubst $(CSRCDIR)/%.c,build/%.o,$(SRCS))

.PHONY: all clean install-staging install-target

all: $(REALSO) symlinks

build:
	mkdir -p build

build/%.o: $(CSRCDIR)/%.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(REALSO): $(OBJS)
	$(CC) -shared -Wl,-soname,$(SONAME) $(LDFLAGS) -o $@ $^

symlinks:
	ln -sf $(REALSO) $(SONAME)
	ln -sf $(SONAME) lib$(LIBNAME).so

# Install headers + pkg-config + libs into staging
install-staging: all
	install -d $(DESTDIR)$(PREFIX)/lib
	install -m 0755 $(REALSO) $(DESTDIR)$(PREFIX)/lib/
	ln -sf $(REALSO) $(DESTDIR)$(PREFIX)/lib/$(SONAME)
	ln -sf $(SONAME) $(DESTDIR)$(PREFIX)/lib/lib$(LIBNAME).so
	install -d $(DESTDIR)$(PREFIX)/include/u8g2
	install -m 0644 $(CSRCDIR)/*.h $(DESTDIR)$(PREFIX)/include/u8g2/
	install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	sed -e 's|@PREFIX@|$(PREFIX)|g' -e 's|@VERSION@|1.0|g' u8g2.pc.in > $(DESTDIR)$(PREFIX)/lib/pkgconfig/u8g2.pc

# Only install runtime libs into target
install-target: all
	install -d $(DESTDIR)$(PREFIX)/lib
	install -m 0755 $(REALSO) $(DESTDIR)$(PREFIX)/lib/
	ln -sf $(REALSO) $(DESTDIR)$(PREFIX)/lib/$(SONAME)
	ln -sf $(SONAME) $(DESTDIR)$(PREFIX)/lib/lib$(LIBNAME).so

clean:
	rm -rf build lib$(LIBNAME).so*
