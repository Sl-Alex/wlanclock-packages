include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=wlanclock-fonts
PKG_VERSION:=0.1
PKG_RELEASE:=1

FILES_DIR:=files

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/$(PKG_NAME)
	SECTION:=wlanclock
	CATEGORY:=wlanclock
	SUBMENU:=Utilities
	DEPENDS:=
	TITLE:=WlanClock fonts collection
endef

# Package description; a more verbose description on what our package does
define Package/$(PKG_NAME)/description
	WlanClock fonts collection
endef

# Package preparation instructions; create the build directory and copy the source code. 
# The last command is necessary to ensure our preparation instructions remain compatible with the patching system.
define Build/Prepare
	$(Build/Patch)
endef

# Package build instructions; invoke the target-specific compiler to first compile the source file, and then to link the file into the final executable
define Build/Compile
endef

# Package install instructions; create a directory inside the package to hold our executable, and then copy the executable we built previously into the folder
define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/share/fonts/wlanclock
	cp -r $(FILES_DIR)/*.ttf $(1)/usr/share/fonts/wlanclock
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,$(PKG_NAME)))
