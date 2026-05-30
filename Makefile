CXX := g++
CC := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -DNDEBUG -Ofast -Iinclude -Iinclude/imgui -Iinclude/imgui/backends
CFLAGS := -Iinclude
LOG := events.log
SRC_DIR := src

FORMAT_DIR := src
CLANG_FORMAT := clang-format
FORMAT_STYLE := file

ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
    TARGET    := FlowEngine.exe
    LDFLAGS   := -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lole32 -lpthread
    RM        := rm -f
    MKDIR     := mkdir
    COPY_DLLS := ldd $(TARGET) | grep mingw | awk '{print $$3}' | xargs -I{} cp {} .
else
    PLATFORM  := Linux
    TARGET    := FlowEngine.app
    LDFLAGS   := -lglfw -lGL -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
    RM        := rm -f
    MKDIR     := mkdir -p
endif

CPP_SRCS := $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/core/app.cpp \
	$(SRC_DIR)/core/window.cpp \
	$(SRC_DIR)/gfx/render.cpp \
	$(SRC_DIR)/gui/gui.cpp \
	$(SRC_DIR)/net/network.cpp \
	$(SRC_DIR)/utils/logger.cpp

IMGUI_SRCS := include/imgui/imgui.cpp \
              include/imgui/imgui_demo.cpp \
              include/imgui/imgui_draw.cpp \
              include/imgui/imgui_tables.cpp \
              include/imgui/imgui_widgets.cpp \
              include/imgui/backends/imgui_impl_glfw.cpp \
              include/imgui/backends/imgui_impl_opengl3.cpp

C_SRCS := $(SRC_DIR)/glad/glad.c

OBJS := $(CPP_SRCS:.cpp=.o) $(IMGUI_SRCS:.cpp=.o)
C_OBJS := $(C_SRCS:.c=.o)

$(TARGET): $(OBJS) $(C_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
ifeq ($(OS),Windows_NT)
	-@$(COPY_DLLS) 2>/dev/null || true
endif

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

include/imgui/%.o: include/imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

include/imgui/backends/%.o: include/imgui/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

format:
	find $(FORMAT_DIR) -type f \( -name "*.cpp" -o -name "*.h" \) \
	    -exec $(CLANG_FORMAT) -i -style=$(FORMAT_STYLE) {} +

clean:
	$(RM) $(TARGET) $(OBJS) $(C_OBJS) $(LOG)

fast:
	$(MAKE) -j$(shell nproc 2>/dev/null || echo 4)

.PHONY: format clean fast
