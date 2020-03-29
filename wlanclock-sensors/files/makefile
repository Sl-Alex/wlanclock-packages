
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
APP0 := wlanclock-sensors
TARGET := $(BINDIR)/$(APP0)



all: info $(TARGET)

$(TARGET): $(OBJECTS) 
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(TARGET)


# generic: build any object file required
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

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
