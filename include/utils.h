#ifndef UTILS_H
#define UTILS_H

// System Headers
#include <iostream>
#include <fstream>

// OpenGL Headers
#if defined(_WIN32)
	#include <GL/glew.h>
	#if defined(GLEW_EGL)
		#include <GL/eglew.h>
	#elif defined(GLEW_OSMESA)
		#define GLAPI extern
		#include <GL/osmesa.h>
	#elif defined(_WIN32)
		#include <GL/wglew.h>
	#elif !defined(__APPLE__) && !defined(__HAIKU__) || defined(GLEW_APPLE_GLX)
		#include <GL/glxew.h>
	#endif

	// OpenGL Headers
	#define GLFW_INCLUDE_GLCOREARB
	#include <GLFW/glfw3.h>
#elif defined(__APPLE__)
	#define GLFW_INCLUDE_GLCOREARB
	#include <GLFW/glfw3.h>
	#include <OpenGL/gl3.h>
#endif

// --------------------------------------------------------------------------------
// GLFW Functions
// --------------------------------------------------------------------------------

// Create a GLFW Window
GLFWwindow* createWindow(int width, int height, const char *title, int major = 3, int minor = 2, GLFWmonitor *monitor = NULL, GLFWwindow *share = NULL);

#endif // UTILS_H