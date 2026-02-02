################################################################################
#
# u8g2-fonts (split build: fonts .so)
#
################################################################################

U8G2_FONTS_VERSION = 1.0
U8G2_FONTS_SITE = $(BR2_EXTERNAL_U8G2SPLIT_PATH)/package/u8g2-fonts/src
U8G2_FONTS_SITE_METHOD = local
U8G2_FONTS_INSTALL_STAGING = YES

U8G2_FONTS_LICENSE = BSD-2-Clause

U8G2_FONTS_DEPENDENCIES = u8g2-core

define U8G2_FONTS_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" \
		AR="$(TARGET_AR)" \
		RANLIB="$(TARGET_RANLIB)" \
		STRIP="$(TARGET_STRIP)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PREFIX="/usr" \
		FONT_DIR="$(BR2_EXTERNAL_U8G2SPLIT_PATH)/package/u8g2-fonts/fonts"
endef

define U8G2_FONTS_INSTALL_STAGING_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install-staging \
		DESTDIR="$(STAGING_DIR)" \
		PREFIX="/usr" \
		FONT_DIR="$(BR2_EXTERNAL_U8G2SPLIT_PATH)/package/u8g2-fonts/fonts"
endef

define U8G2_FONTS_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install-target \
		DESTDIR="$(TARGET_DIR)" \
		PREFIX="/usr" \
		FONT_DIR="$(BR2_EXTERNAL_U8G2SPLIT_PATH)/package/u8g2-fonts/fonts"
endef

$(eval $(generic-package))
