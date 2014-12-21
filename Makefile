CC=gcc
SHELL:=$(shell which bash) -e -o pipefail
UNAME_S:=$(shell uname -s)
_CFLAGS=-Wall -Wextra -Werror -Wshadow -Winline -D_GNU_SOURCE -std=c99 -g

BIN_DIR=$(CURDIR)/bin
SRC_DIR=$(CURDIR)/src
DATA_DIR=$(CURDIR)/etc/data
SCRIPT_DIR=$(CURDIR)/scripts
ROUTER_DIR=$(SRC_DIR)/router
LIB_DIR=$(SRC_DIR)/lib

# All header files in libraries
INCLUDES_ROUTER=-I$(SRC_DIR) -I$(CURDIR) \
	        -I$(CURDIR)/deps/uthash \
	        -I$(ROUTER_DIR) \
	        -I$(LIB_DIR) \
	        -lpthread \
		-lpcap \

# All .c files in src folder
_SOURCES=$(shell find $(SRC_DIR) -type f -iname '*.[c]')
_SOURCES_ROUTER=$(shell find $(ROUTER_DIR) -type f -iname '*.[c]')
_SOURCES_LIB=$(shell find $(LIB_DIR) -type f -iname '*.[c]')

# Build
build_router:
	    -mkdir -p $(BIN_DIR)
	    $(CC) -o $(BIN_DIR)/router $(_SOURCES_LIB) $(_SOURCES_ROUTER) $(INCLUDES_ROUTER)

clean:
	@echo "Deleting bin folder"
	rm -rf $(BIN_DIR)/*
