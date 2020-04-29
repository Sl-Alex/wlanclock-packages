include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=wlanclock-app
PKG_VERSION:=0.1
PKG_RELEASE:=1

# Source settings (i.e. where to find the source codes)
# This is a custom variable, used below
SOURCE_DIR:=files
CONF_DIR:=config

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/$(PKG_NAME)
	SECTION:=wlanclock
	CATEGORY:=wlanclock
	SUBMENU:=Utilities
	#VVV ,librt,libpthread
	DEPENDS:=+libstdcpp +libubus +libubox +libuci +libfreetype +libopenssl +libcurl +libjson-c +libmbedtls +wlanclock-assets
	TITLE:=WlanClock main application
endef

# Package description; a more verbose description on what our package does
define Package/$(PKG_NAME)/description
	WlanClock main application
endef

# Configuration files
define Package/$(PKG_NAME)/conffiles
/etc/config/wlanclock
endef

# Package preparation instructions; create the build directory and copy the source code. 
# The last command is necessary to ensure our preparation instructions remain compatible with the patching system.
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -r $(SOURCE_DIR)/* $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

# Package build instructions; invoke the target-specific compiler to first compile the source file, and then to link the file into the final executable
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	CC="$(TARGET_CC)" \
	CXX="$(TARGET_CXX)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

# Package install instructions; create a directory inside the package to hold our executable, and then copy the executable we built previously into the folder
define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	# install the binary
	$(INSTALL_BIN) $(CONF_DIR)/init.d/wlanclock.sh $(1)/etc/init.d/wlanclock
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/$(PKG_NAME) $(1)/usr/bin
	$(INSTALL_CONF) $(CONF_DIR)/wlanclock $(1)/etc/config/wlanclock
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,$(PKG_NAME)))
