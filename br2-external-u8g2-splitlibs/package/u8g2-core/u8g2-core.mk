################################################################################
#
# u8g2-core (split build: core .so without font data)
#
################################################################################

U8G2_CORE_VERSION = 1.0
U8G2_CORE_SITE = $(BR2_EXTERNAL_U8G2SPLIT_PATH)/package/u8g2-core/src
U8G2_CORE_SITE_METHOD = local
U8G2_CORE_INSTALL_STAGING = YES

U8G2_CORE_LICENSE = BSD-2-Clause
# Upstream u8g2 has a LICENSE file in repo root; we don't ship it here because
# you vendor the upstream repo into package/u8g2-core/u8g2.
# U8G2_CORE_LICENSE_FILES = LICENSE

U8G2_CORE_DEPENDENCIES =

define U8G2_CORE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" \
		AR="$(TARGET_AR)" \
		RANLIB="$(TARGET_RANLIB)" \
		STRIP="$(TARGET_STRIP)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		U8G2_UPSTREAM_DIR="$(BR2_EXTERNAL_U8G2SPLIT_PATH)/package/u8g2-core/u8g2" \
		PREFIX="/usr"
endef

define U8G2_CORE_INSTALL_STAGING_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install-staging \
		DESTDIR="$(STAGING_DIR)" \
		U8G2_UPSTREAM_DIR="$(BR2_EXTERNAL_U8G2SPLIT_PATH)/package/u8g2-core/u8g2" \
		PREFIX="/usr"
endef

define U8G2_CORE_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install-target \
		DESTDIR="$(TARGET_DIR)" \
		U8G2_UPSTREAM_DIR="$(BR2_EXTERNAL_U8G2SPLIT_PATH)/package/u8g2-core/u8g2" \
		PREFIX="/usr"
endef

$(eval $(generic-package))
