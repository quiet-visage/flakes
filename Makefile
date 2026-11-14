CC=g++

MAKEFLAGS := --jobs=$(shell nproc)

INCLUDES = -Isrc -Ivendor -Ivendor/funchook/include

CFLAGS= -shared -fPIC -std=c++23 -g --no-gnu-unique -MMD -MP $(INCLUDES)

LDFLAGS= -l:libGLEW.so.2.1 -lSDL2 -lvulkan -lcurl vendor/funchook/libfunchook.a vendor/funchook/libdistorm.a -z execstack -g

OBJ_FILES =  src/main.cpp.o # Unity build
OBJ_FILES += vendor/libsigscan/libsigscan.c.o # Sigscan library
OBJ_FILES += vendor/imgui/imgui_tables.cpp.o vendor/imgui/imgui_draw.cpp.o vendor/imgui/imgui_impl_sdl2.cpp.o vendor/imgui/imgui_demo.cpp.o vendor/imgui/imgui_impl_opengl3.cpp.o vendor/imgui/imgui_impl_vulkan.cpp.o vendor/imgui/imgui_widgets.cpp.o vendor/imgui/imgui.cpp.o vendor/imgui/imgui_stdlib.cpp.o # GUI library and OpenGL/Vulkan wrappers
OBJ_FILES += vendor/micropather/micropather.cpp.o # Path solving library
OBJ_FILES += vendor/md5/md5.cpp.o # Checksum PRNG
# OBJ_FILES += bitbuf/bitbuf.cpp.o

OBJS = $(addprefix obj/, $(OBJ_FILES)) 

BIN = tf2.so

SRC_DIR = /var/home/anon/projects/Flakes
STEAM_DIR = /var/home/anon/.var/app/com.valvesoftware.Steam/TeamFortress2-Linux-Internal
TOOLBOX_NAME = fedora-toolbox-44
IN_TOOLBOX := $(or $(wildcard /.toolboxenv),$(wildcard /run/.toolboxenv),$(wildcard /run/.containerenv))

.PHONY: all clean debug sync inject logs

#-------------------------------------------------------------------------------

ifneq ($(IN_TOOLBOX),)
all: $(BIN)
else
all:
	toolbox run -c $(TOOLBOX_NAME) make -C $(SRC_DIR) $(BIN)
endif

clean:
	rm -rf obj
	rm -f $(BIN)

sync:
	chmod +x $(SRC_DIR)/scripts/inject.sh
	cp -f --remove-destination $(SRC_DIR)/tf2.so $(STEAM_DIR)/tf2.so
	cp -f --remove-destination $(SRC_DIR)/scripts/inject.sh $(STEAM_DIR)/inject.sh
	cp -f --remove-destination $(SRC_DIR)/Taskfile.yml $(STEAM_DIR)/Taskfile.yml
	cp -f --remove-destination $(SRC_DIR)/Makefile $(STEAM_DIR)/Makefile
	chmod +x $(STEAM_DIR)/inject.sh

inject: sync
	sudo nsenter -t "$$(podman inspect -f '{{.State.Pid}}' $(TOOLBOX_NAME))" -m $(STEAM_DIR)/inject.sh

logs:
	sudo tail -n +1 -f "/proc/$$(pgrep tf_linux64 | head -n 1)/root/tmp/tf2.log"

#-------------------------------------------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@if [ "$$(execstack -q $@ 2>/dev/null)" = "X $@" ]; then execstack -c $@; fi

obj/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(OBJS:.o=.d)
