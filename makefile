.PHONY: default so fast debug size verbose clean

CC = clang
CFLAGS = $(INCLUDES) -Wall -Wextra -Wpedantic 
OPTIMIZE ?= O1

FILE = main
C_OUT = c-out
SRC_DIR = src

MAIN ?=  $(SRC_DIR)/$(FILE).c

BIN = $(FILE)
INCL_DIR = include

INCLUDES = $$(find include -type d | sed 's/^/-I.\//')

define MKDIRS
/bin/sh -c ' \
if [ ! -d $(C_OUT) ]; then \
    mkdir $(C_OUT) $(C_OUT)/bin $(C_OUT)/lib $(C_OUT)/dbg; \
else \
    if [ ! -d $(C_OUT)/bin ]; then mkdir $(C_OUT)/bin; fi; \
    if [ ! -d $(C_OUT)/lib ]; then mkdir $(C_OUT)/lib; fi; \
    if [ ! -d $(C_OUT)/dbg ]; then mkdir $(C_OUT)/dbg; fi; \
fi'
endef

define REPLACE_FILES
/bin/sh -c ' \
if [ -d $(C_OUT)/dbg/$(BIN).dSYM/ ]; then \
    rm -r $(C_OUT)/dbg/$(BIN).dSYM/; \
fi; \
\
if [ -d $(C_OUT)/bin/$(BIN).dSYM ]; then \
    mv $(C_OUT)/bin/$(BIN).dSYM $(C_OUT)/dbg/; \
fi'
endef

PRUNE = $(SHELL) -c 'find $(C_OUT) -type d -empty -delete'

CCPFX = $(CC) $$(find $(SRC_DIR) -type f -name '*.c') $$(find lib ! -name lib) -o $(C_OUT)/bin/$(BIN) $(CFLAGS)

default: $(MAIN)
	$(MKDIRS)
	$(CCPFX) -$(OPTIMIZE)
	$(REPLACE_FILES)
	$(PRUNE)

so: $(MAIN)
	$(MKDIRS)
	# can also compile with -L./lib -lheap
	for dir in $(SRC_DIR)/*/; do dir=$${dir%*/}; clang -shared -undefined dynamic_lookup $$(find $$dir -type f -name '*.c') -o $(C_OUT)/lib/lib$${dir##*/}.so $(CFLAGS) -Wl,-install_name,lib/lib$${dir##*/}.so; done
	$(REPLACE_FILES)
	$(PRUNE)

all:
	$(MKDIRS)
	$(CCPFX) -$(OPTIMIZE)
	for dir in $(SRC_DIR)/*/; do dir=$${dir%*/}; clang -shared -undefined dynamic_lookup $$(find $$dir -type f -name '*.c') -o $(C_OUT)/lib/lib$${dir##*/}.so $(CFLAGS) -Wl,-install_name,lib/lib$${dir##*/}.so; done
	$(REPLACE_FILES)
	$(PRUNE)

fast: $(MAIN)
	$(MKDIRS)
	$(CCPFX) -Ofast
	$(REPLACE_FILES)
	$(PRUNE)

debug: $(MAIN)
	$(MKDIRS)
	$(CCPFX) -g
	$(REPLACE_FILES)
	$(PRUNE)

size: $(MAIN)
	$(MKDIRS)
	$(CCPFX) -Os
	$(REPLACE_FILES)
	$(PRUNE)

verbose: $(MAIN)
	$(MKDIRS)
	$(CCPFX) -$(OPTIMIZE) -v
	$(REPLACE_FILES)
	$(PRUNE)

clean: $(C_OUT)
	rm -r $(C_OUT) compile_commands.json
