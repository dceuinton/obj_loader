#ifndef SHADER_H
#define SHADER_H

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
// Shader Functions
// --------------------------------------------------------------------------------

// Read file contents
char* readFile(const char *filename);

// Check the status of a shader
GLuint checkShader(GLuint shader);

// Check the status of a program
GLuint checkProgram(GLuint program);

// Load and compile shader from source file
GLuint loadShader(GLuint type, const char *filename);

// Load and compiler program from source files
GLuint loadProgram(const char *vert_file, const char *ctrl_file, const char *eval_file, const char *geom_file, const char *frag_file);

// --------------------------------------------------------------------------------

#endif // SHADER_H