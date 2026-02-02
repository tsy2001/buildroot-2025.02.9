# u8g2-core (libu8g2.so)

This package builds **libu8g2.so** from upstream u8g2 `csrc/` sources, but
**excludes the built-in font data** compilation units:

- `csrc/u8g2_fonts.c`
- `csrc/u8g2_font_*.c` (if present)

You must vendor the upstream u8g2 repository at:

`package/u8g2-core/u8g2/`

so that it contains at least `csrc/`.

Runtime installs to `/usr/lib`.
Headers install to staging: `/usr/include/u8g2/`.
A `pkg-config` file is installed to staging: `/usr/lib/pkgconfig/u8g2.pc`.
