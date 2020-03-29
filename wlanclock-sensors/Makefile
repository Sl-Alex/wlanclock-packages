include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=wlanclock-sensors
PKG_VERSION:=0.1
PKG_RELEASE:=1

# Source settings (i.e. where to find the source codes)
# This is a custom variable, used below
SOURCE_DIR:=files

TARGET_CFLAGS += " -I$(PKG_BUILD_DIR)/include"

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/$(PKG_NAME)
	SECTION:=wlanclock
	CATEGORY:=wlanclock
	SUBMENU:=Utilities
	#VVV ,librt,libpthread
	DEPENDS:=+libstdcpp
	TITLE:=WlanClock sensors reader
endef

# Package description; a more verbose description on what our package does
define Package/$(PKG_NAME)/description
	WlanClock sensors reader
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
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/src/main.o            -c $(PKG_BUILD_DIR)/src/main.c
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/src/i2c.o             -c $(PKG_BUILD_DIR)/src/i2c.c
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/src/bme280_wrapper.o  -c $(PKG_BUILD_DIR)/src/bme280_wrapper.c
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/src/bme280.o          -c $(PKG_BUILD_DIR)/src/bme280.c
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/src/bh1750.o          -c $(PKG_BUILD_DIR)/src/bh1750.c
	$(TARGET_CC) $(TARGET_LDFLAGS) -o $(PKG_BUILD_DIR)/$1 \
        $(PKG_BUILD_DIR)/src/main.o \
        $(PKG_BUILD_DIR)/src/i2c.o \
        $(PKG_BUILD_DIR)/src/bme280_wrapper.o \
        $(PKG_BUILD_DIR)/src/bme280.o \
        $(PKG_BUILD_DIR)/src/bh1750.o \
endef

# Package install instructions; create a directory inside the package to hold our executable, and then copy the executable we built previously into the folder
define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,$(PKG_NAME)))
