CC=g++

MAKEFLAGS := --jobs=$(shell nproc)

CFLAGS= -shared -fPIC -std=c++23 -g --no-gnu-unique -MMD -MP

LDFLAGS= -l:libGLEW.so.2.1 -lSDL2 -lvulkan -lcurl libs/funchook/libfunchook.a libs/funchook/libdistorm.a -z execstack -g

OBJ_FILES =  main.cpp.o # Unity build
OBJ_FILES += libsigscan/libsigscan.c.o # Sigscan library
OBJ_FILES += imgui/imgui_tables.cpp.o imgui/imgui_draw.cpp.o imgui/imgui_impl_sdl2.cpp.o imgui/imgui_demo.cpp.o imgui/imgui_impl_opengl3.cpp.o imgui/imgui_impl_vulkan.cpp.o imgui/imgui_widgets.cpp.o imgui/imgui.cpp.o imgui/imgui_stdlib.cpp.o # GUI library and OpenGL wrapper
OBJ_FILES += hacks/navbot/micropather/micropather.cpp.o # Path solving library
# OBJ_FILES += bitbuf/bitbuf.cpp.o

OBJS = $(addprefix obj/, $(OBJ_FILES)) 

BIN = tf2.so

STEAM_DIR = /var/home/anon/.var/app/com.valvesoftware.Steam/TeamFortress2-Linux-Internal
TOOLBOX_NAME = fedora-toolbox-44

.PHONY: all clean debug sync inject logs

#-------------------------------------------------------------------------------

all: $(BIN)
	@if [ "$$(execstack -q $(BIN) 2>/dev/null)" = "X $(BIN)" ]; then execstack -c $(BIN); fi

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

sync:
	chmod +x /var/home/anon/projects/Flakes/inject.sh
	cp -f --remove-destination /var/home/anon/projects/Flakes/tf2.so $(STEAM_DIR)/tf2.so
	cp -f --remove-destination /var/home/anon/projects/Flakes/inject.sh $(STEAM_DIR)/inject.sh
	cp -f --remove-destination /var/home/anon/projects/Flakes/Taskfile.yml $(STEAM_DIR)/Taskfile.yml
	cp -f --remove-destination /var/home/anon/projects/Flakes/Makefile $(STEAM_DIR)/Makefile
	chmod +x $(STEAM_DIR)/inject.sh

inject: sync
	sudo nsenter -t "$$(podman inspect -f '{{.State.Pid}}' $(TOOLBOX_NAME))" -m $(STEAM_DIR)/inject.sh

logs:
	sudo tail -n +1 -f "/proc/$$(pgrep tf_linux64 | head -n 1)/root/tmp/tf2.log"

#-------------------------------------------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(OBJS:.o=.d)
