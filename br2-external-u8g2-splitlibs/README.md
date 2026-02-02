# Buildroot u8g2 split-libs external tree

This br2-external tree adds two Buildroot packages:

- **u8g2-core**  -> `/usr/lib/libu8g2.so*` (NO built-in font data)
- **u8g2-fonts** -> `/usr/lib/libu8g2font.so*` (only selected fonts you provide)

## 1) Vendor upstream u8g2 sources

Put upstream u8g2 repository into:

`package/u8g2-core/u8g2/`

It must contain at least the `csrc/` directory.

## 2) Put selected fonts

Put only the font `.c` files you need into:

`package/u8g2-fonts/fonts/`

## 3) Enable packages

From your Buildroot tree:

```
make BR2_EXTERNAL=/abs/path/to/br2-external-u8g2-splitlibs menuconfig
```

Enable:
- u8g2-core
- u8g2-fonts

Then `make`.

## 4) Link your app

Link with:

`-lu8g2 -lu8g2font`

Or in pkg-config style:

`pkg-config --cflags --libs u8g2 u8g2font`
