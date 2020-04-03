
# define the directories
SRCDIR := src
INCDIR := include
BUILDDIR := build
BINDIR := bin

# define common variables
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -iname "*.$(SRCEXT)" )
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
INC := $(shell find $(INCDIR) -maxdepth 1 -type d -exec echo -I {}  \;)

# flags
#CFLAGS := -g -lz 

# define specific binaries to create
APP0 := wlanclock-input
TARGET := $(BINDIR)/$(APP0)

LIBEVDEV_CFLAGS:=$(shell $(PKG_CONFIG) --cflags libevdev)

all: info $(TARGET)

$(TARGET): $(OBJECTS) 
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -lubus -lubox -lblobmsg_json -ljson-c -lrt -levdev -luci -o $(TARGET)


# generic: build any object file required
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@echo "LIBEVDEV_PATH = $(LIBEVDEV_PATH)"
	$(CC) $(CFLAGS) $(LIBEVDEV_CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(BINDIR)

info:
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LIB: $(LIB)"
	@echo "INC: $(INC)"
	@echo "SOURCES: $(SOURCES)"
	@echo "OBJECTS: $(OBJECTS)"


.PHONY: clean
