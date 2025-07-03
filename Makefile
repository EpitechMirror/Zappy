# Simple Zappy Makefile
DIST_DIR = dist
SERVER_DIR = server
GUI_DIR = gui
AI_DIR = ai
NPROC := $(shell nproc 2>/dev/null || echo 1)

.PHONY: all build build-server build-gui help

all: build

help:
	@echo "Zappy Build System"
	@echo "Targets: help build build-server build-gui"

build: build-server build-gui
	@echo "All components built"

build-server:
	@echo "Building server..."
	@mkdir -p $(DIST_DIR)
	@if [ -d "$(SERVER_DIR)" ]; then \
		mkdir -p $(SERVER_DIR)/build; \
		cd $(SERVER_DIR)/build && cmake .. && make -j$(NPROC); \
		if [ -f zappy_server ]; then \
			cp zappy_server ../../$(DIST_DIR)/; \
		fi; \
	fi

build-gui:
	@echo "Building GUI..."
	@mkdir -p $(DIST_DIR)
	@if [ -d "$(GUI_DIR)" ]; then \
		mkdir -p $(GUI_DIR)/build; \
		cd $(GUI_DIR)/build && cmake .. && make -j$(NPROC); \
		if [ -f ../zappy_gui ]; then \
			cp ../zappy_gui ../../$(DIST_DIR)/; \
		fi; \
	fi
