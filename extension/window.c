#include "window.h"

#include <stdio.h>
#include <stdlib.h>

bool initialiseGLFW(void) {
	// initiliase glfw
	if (!glfwInit()) {
		// failed
		fprintf(stderr, "Error - Could not initialise GLFW.\n");
		return false;
	}

	// set window metdata
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// add MacOS exclusive properties
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	return true;
}

bool initialiseGLEW(void) {
	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Error - Could not initalise GLEW\n");
		return false;
	}

	return true;
}

bool createWindow(Window* win) {
	win->width = INITIAL_WIDTH;
	win->height = INITIAL_HEIGHT;
	win->windowHandle = glfwCreateWindow(win->width, win->height, WINDOW_TITLE, NULL, NULL);

	if (win->windowHandle == NULL) {
		fprintf(stderr, "Error - failed to create window\n");
		return false;
	}

	// make window the current opengl context
	glfwMakeContextCurrent(win->windowHandle);

	return true;
}

void initialiseWindow(Window* win) {
	bool result;

	// initialise glfw
	result = initialiseGLFW();

	if (!result) {
		exit(1);
	}

	// create window
	createWindow(win);

	if (!result) {
		exit(1);
	}

	// initialise glew
	initialiseGLEW();

	if (!result) {
		exit(1);
	}

	// set window running to true
	win->running = true;
}

void pollWindowEvents(Window* win) {
	// poll events
	glfwPollEvents();

	// check if escape key was pressed
	if (glfwGetKey(win->windowHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		win->running = false;
	}
}

void freeWindow(Window* win) {
	glfwDestroyWindow(win->windowHandle);
}
