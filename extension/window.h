#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define INITIAL_WIDTH 800
#define INITIAL_HEIGHT 600
#define WINDOW_TITLE "Arcade Anthology"

typedef struct {
	GLFWwindow* windowHandle;
	unsigned int width;
	unsigned int height;
	bool running;
} Window;

// declare functions
bool initialiseGLFW(void);
bool initialiseGLEW(void);
bool createWindow(Window* win);
void initialiseWindow(Window* win);
void pollWindowEvents(Window* win);
void freeWindow(Window* win);

#endif
