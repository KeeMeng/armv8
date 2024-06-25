CC = gcc
CFLAGS = -Wall -g -std=c17 -D_POSIX_SOURCE -D_DEFAULT_SOURCE
# -Wextra
LIBS = -lm -lGL -lGLU -lGLEW -lglfw -lX11
MACLIBS = -lglfw -lGLEW
DEPENDENCIES = libx11-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxrandr-dev libxext-dev libxcursor-dev libxinerama-dev libxi-dev libglfw3 libglfw3-dev libglew2.2 libglew-dev
TARGET = game

SRCS = main.c window.c renderer.c linearAlgebra.c mainMenu.c game.c utilities.c ./games/minotaurs.c gameUI.c ./games/pong.c ./games/snake.c ./games/snakesAndLadders.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(TARGET)

debug:
	$(CC) $(SRCS) $(CFLAGS) -fsanitize=address,undefined,leak -Werror -pedantic -O1 $(LIBS) -o $(TARGET)

mac:
	$(CC) $(SRCS) $(CFLAGS) $(MACLIBS) -framework OpenGL -o $(TARGET)

clean:
	rm -r -f *.o

install:
	apt update
	apt upgrade
	apt install $(DEPENDENCIES)

install-mac:
	brew update
	brew install glfw
	brew install glew
