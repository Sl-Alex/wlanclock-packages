
# define the directories
SRCDIR := src
INCDIR := include
BUILDDIR := build
BINDIR := bin

# define common variables
SRCEXT_CPP  := cpp
SOURCES_CPP := $(shell find $(SRCDIR) -type f -iname "*.$(SRCEXT_CPP)" )
OBJECTS_CPP := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES_CPP:.$(SRCEXT_CPP)=.opp))

SRCEXT_C  := c
SOURCES_C := $(shell find $(SRCDIR) -type f -iname "*.$(SRCEXT_C)" )
OBJECTS_C := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES_C:.$(SRCEXT_C)=.o))

INC := $(shell find $(INCDIR) -maxdepth 1 -type d -exec echo -I {}  \;)

# flags
#CFLAGS := -g -lz 

# define specific binaries to create
APP0 := wlanclock-app
TARGET := $(BINDIR)/$(APP0)

all: info $(TARGET)

$(TARGET): $(OBJECTS_C) $(OBJECTS_CPP)
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CXX) $^ $(CFLAGS) $(LDFLAGS) -lubox -lubus -luci -lfreetype -o $(TARGET)

# generic: build any object file required
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT_C)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/%.opp: $(SRCDIR)/%.$(SRCEXT_CPP)
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(BINDIR)

info:
	@echo "CC: $(CC)"
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LIB: $(LIB)"
	@echo "INC: $(INC)"
	@echo "SOURCES_C: $(SOURCES_C)"
	@echo "SOURCES_CPP: $(SOURCES_CPP)"
	@echo "OBJECTS_C: $(OBJECTS_C)"
	@echo "OBJECTS_CPP: $(OBJECTS_CPP)"


.PHONY: clean
