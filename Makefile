CC=g++

.DEFAULT_GOAL := all

MAKEFLAGS := --jobs=$(shell nproc)

INCLUDES = -Isrc -Ivendor -Ivendor/funchook/include

CFLAGS= -shared -fPIC -std=c++23 -fmodules -g --no-gnu-unique $(INCLUDES)

C_CC=gcc
C_CFLAGS= -fPIC -g $(INCLUDES)

FUNCHOOK_DIR = vendor/funchook
FUNCHOOK_LIBS = $(FUNCHOOK_DIR)/libfunchook.a $(FUNCHOOK_DIR)/libdistorm.a

LDFLAGS= -Lvendor -l:libGLEW.so.2.1 -lSDL2 -lvulkan -lcurl $(FUNCHOOK_LIBS) -z execstack -g

include modules.mk

VENDOR_OBJS = \
  obj/vendor/libsigscan/libsigscan.c.o \
  obj/vendor/imgui/imgui_tables.cpp.o \
  obj/vendor/imgui/imgui_draw.cpp.o \
  obj/vendor/imgui/imgui_impl_sdl2.cpp.o \
  obj/vendor/imgui/imgui_demo.cpp.o \
  obj/vendor/imgui/imgui_impl_opengl3.cpp.o \
  obj/vendor/imgui/imgui_impl_vulkan.cpp.o \
  obj/vendor/imgui/imgui_widgets.cpp.o \
  obj/vendor/imgui/imgui.cpp.o \
  obj/vendor/imgui/imgui_stdlib.cpp.o \
  obj/vendor/micropather/micropather.cpp.o \
  obj/vendor/md5/md5.cpp.o

OBJS = $(MODULE_OBJS) $(VENDOR_OBJS)

BIN = tf2.so

SRC_DIR = /var/home/anon/projects/Flakes
STEAM_DIR = /var/home/anon/.var/app/com.valvesoftware.Steam/TeamFortress2-Linux-Internal
TOOLBOX_NAME ?= $(shell toolbox list -c 2>/dev/null | awk 'NR>1 {print $$2; exit}')
ifeq ($(TOOLBOX_NAME),)
TOOLBOX_NAME = fedora-toolbox-45
endif
IN_TOOLBOX := $(or $(wildcard /.toolboxenv),$(wildcard /run/.toolboxenv),$(wildcard /run/.containerenv))
PODMAN ?= $(shell if command -v podman >/dev/null 2>&1; then echo podman; elif command -v flatpak-spawn >/dev/null 2>&1; then echo "flatpak-spawn --host podman"; else echo podman; fi)
SUDO ?= $(shell if [ -f /.toolboxenv ] || [ -f /run/.toolboxenv ]; then echo sudo; elif command -v podman >/dev/null 2>&1; then echo sudo; elif command -v flatpak-spawn >/dev/null 2>&1; then echo "flatpak-spawn --host sudo"; else echo sudo; fi)

.PHONY: all clean debug sync inject logs funchook-clean distclean

#-------------------------------------------------------------------------------

ifneq ($(IN_TOOLBOX),)
all: $(BIN)
else
all:
	toolbox run -c $(TOOLBOX_NAME) make -C $(SRC_DIR) $(BIN)
endif

GLEW_SNIPER := $(firstword $(wildcard $(HOME)/.var/app/com.valvesoftware.Steam/.local/share/Steam/steamapps/common/SteamLinuxRuntime_sniper/*/files/lib/x86_64-linux-gnu/libGLEW.so.2.1.0))

vendor/libGLEW.so.2.1:
	@mkdir -p vendor
	@if [ -n "$(GLEW_SNIPER)" ] && [ -f "$(GLEW_SNIPER)" ]; then \
		ln -sf "$(GLEW_SNIPER)" $@; \
	fi

$(FUNCHOOK_LIBS):
	@mkdir -p vendor
	@if [ ! -d "$(FUNCHOOK_DIR)" ]; then \
		git clone https://github.com/Doctor-Coomer/funchook.git $(FUNCHOOK_DIR); \
	fi
	@if [ ! -f "$(FUNCHOOK_DIR)/libfunchook.a" ] || [ ! -f "$(FUNCHOOK_DIR)/libdistorm.a" ]; then \
		mkdir -p $(FUNCHOOK_DIR)/build && \
		cd $(FUNCHOOK_DIR) && \
		cmake -DCMAKE_BUILD_TYPE=Release . && \
		$(MAKE); \
	fi

clean:
	rm -rf obj gcm.cache
	rm -f $(BIN)

funchook-clean:
	rm -rf $(FUNCHOOK_DIR)

distclean: clean funchook-clean

sync: all
	@mkdir -p $(STEAM_DIR)
	chmod +x $(SRC_DIR)/scripts/inject.sh $(SRC_DIR)/scripts/debug.sh
	cp -f --remove-destination $(SRC_DIR)/tf2.so $(STEAM_DIR)/tf2.so
	cp -f --remove-destination $(SRC_DIR)/scripts/inject.sh $(STEAM_DIR)/inject.sh
	cp -f --remove-destination $(SRC_DIR)/scripts/debug.sh $(STEAM_DIR)/debug.sh
	rm -f $(STEAM_DIR)/build.sh
	cp -f --remove-destination $(SRC_DIR)/Taskfile.yml $(STEAM_DIR)/Taskfile.yml
	cp -f --remove-destination $(SRC_DIR)/Makefile $(STEAM_DIR)/Makefile
	cp -f --remove-destination $(SRC_DIR)/modules.mk $(STEAM_DIR)/modules.mk
	chmod +x $(STEAM_DIR)/inject.sh $(STEAM_DIR)/debug.sh
	@echo "Sync completed: $(STEAM_DIR)"

inject: sync
	@if [ -n "$(IN_TOOLBOX)" ]; then \
		$(SUDO) $(STEAM_DIR)/inject.sh; \
	else \
		TPID=$$($(PODMAN) inspect -f '{{.State.Pid}}' $(TOOLBOX_NAME) 2>/dev/null); \
		if [ -z "$$TPID" ] || [ "$$TPID" = "0" ]; then \
			echo "Error: Container $(TOOLBOX_NAME) is not running or PID not found." >&2; \
			exit 1; \
		fi; \
		$(SUDO) nsenter -t "$$TPID" -m $(STEAM_DIR)/inject.sh; \
	fi

logs:
	@PROCID=$$(pgrep tf_linux64 | head -n 1); \
	if [ -z "$$PROCID" ]; then \
		echo "Failed to find tf_linux64 process" >&2; \
		exit 1; \
	fi; \
	$(SUDO) tail -n +1 -f "/proc/$$PROCID/root/tmp/tf2.log"

#-------------------------------------------------------------------------------

$(BIN): $(OBJS) $(FUNCHOOK_LIBS) | vendor/libGLEW.so.2.1
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@if [ "$$(execstack -q $@ 2>/dev/null)" = "X $@" ]; then execstack -c $@; fi

obj/vendor/%.cpp.o: vendor/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/vendor/%.c.o: vendor/%.c
	@mkdir -p $(dir $@)
	$(C_CC) $(C_CFLAGS) -c -o $@ $<
