// Assignment number 2, 159.709, 2018 S1
// Euinton, Dale, 14026002,
// Explain what the program is doing . . .


// ----------------------------------------------------------------------
// 			WORKLOG
// ----------------------------------------------------------------------

// Tomorrow focus on actually extracting data and getting it working!!




// ----------------------------------------------------------------------

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "utils.h"
#include "shader.h"
#include "OBJFileReader.h"
#include "glm_virtual_camera.h"
#include "image.h"

#define PI 3.14159
using namespace std;

GLMVirtualCamera vcam;
GLMVirtualCamera modelMatController;

double currentTime;
double previousTime;
double elapsedTime;

struct GLObject {
	std::vector<GLfloat> vertices;
	GLuint vao; 					//vertex array object
	GLuint vbo; 					//vertex buffer object
	GLuint ebo; 					//element buffer object
	GLuint sp; 						//shader program
	Material* material;
};

// ----------------------------------------------------------------------------------------

// Called on Error Event
void onError(int error, const char *description) {
	// Print Error message
	std::cerr << "Error: " << error << " : " << description << std::endl;
}

// Called on Window Close Event
void onWindowClose(GLFWwindow *window) {
	// Nothing to do right now
	// Do not call glfwDestroyWindow from here
}

// Called on Window Size Event
void onFramebufferSize(GLFWwindow *window, int width, int height) {
	// Set-up the window/screen coordinates
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	glfwMakeContextCurrent(NULL);
}

// ----------------------------------------------------------------------------------------

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	float cameraSpeed = 100.0f;
	float rps = PI;

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, 1);
		}
		// if (key == GLFW_KEY_W) {vcam.moveForwards(elapsedTime * cameraSpeed);}
		// if (key == GLFW_KEY_A) {vcam.moveLeft(elapsedTime * cameraSpeed);}
		// if (key == GLFW_KEY_S) {vcam.moveBackwards(elapsedTime * cameraSpeed);}
		// if (key == GLFW_KEY_D) {vcam.moveRight(elapsedTime * cameraSpeed);}
		// if (key == GLFW_KEY_LEFT) {vcam.lookLeft(elapsedTime * rps);}
		// if (key == GLFW_KEY_RIGHT) {vcam.lookRight(elapsedTime * rps);}
		// if (key == GLFW_KEY_UP) {vcam.lookUp(elapsedTime * rps);}
		// if (key == GLFW_KEY_DOWN) {vcam.lookDown(elapsedTime * rps);}
		// if (key == GLFW_KEY_R) {vcam.moveUp(elapsedTime * cameraSpeed);}
		// if (key == GLFW_KEY_F) {vcam.moveDown(elapsedTime * cameraSpeed);}
		if (key == GLFW_KEY_P) {vcam.printPosition();}

		// if (key == GLFW_KEY_J) {modelMatController.lookLeft(elapsedTime * rps);}
		// if (key == GLFW_KEY_L) {modelMatController.lookRight(elapsedTime * rps);}
		// if (key == GLFW_KEY_I) {modelMatController.lookUp(elapsedTime * rps);}
		// if (key == GLFW_KEY_K) {modelMatController.lookDown(elapsedTime * rps);}
	}
}



int main() {

	// // const char *filename = "test.obj";
	const char *filename = "old_man/muro.obj";
	stringstream *file = readFileIntoBuffer(filename);
	stringstream *materialLibrary;
	
	string folderWithOBJ = "./old_man/";
	string line;
	vector<glm::vec3> vertices, normals;
	vector<glm::vec2> texturecoords;
	vector<GLuint> indices, vTC, vNormals;
	vector<Material*> *materials;

	int coms=0, gs=0, vs=0, vts=0, vns=0, sgs=0, fs=0, mtl=0, usemtls=0, unknown=0;

	bool limiter = true;

	while(getline(*file, line) && limiter) {

		int identifier = getIdentifier(line);

		switch (identifier) {
			case COMMENT:
			coms++;
			break;

			case GROUP:
			gs++;
			if (gs >= 3) {
				limiter = false;
			}
			break;

			case SMOOTHING_GROUP:
			sgs++;
			break;

			case VERTEX: {
				vs++;
				glm::vec3 vec;
				vec = getVertex(line);
				vertices.push_back(vec);
				break;
			}					

			case TEX_COORD: {
				vts++;
				glm::vec2 vec;
				vec = getTexCoords(line);
				texturecoords.push_back(vec);
				break;
			}
			

			case NORMAL: {
				vns++;
				glm::vec3 vec;
				vec = getVertex(line);
				normals.push_back(vec); 
				break;
			}
			

			case FACE: {
				fs++;
				vector<string> vec = getWords(line);
				getFaceIndices(vec, indices, vTC, vNormals);
			}
			break;

			case MATERIAL_LIBRARY: {
				mtl++;
				vector<string> words = getWords(line);
				string filename = folderWithOBJ + words[1];
				materialLibrary = readFileIntoBuffer(filename.c_str());
				materials = readMaterialsLibrary(*materialLibrary);				
			}
			
			break;

			case USE_MTL: {
				usemtls++;
			}
			break;

			case -1:
			unknown++;
			break;
		}
	}
	
	// cout << "Comments: " << coms << endl;
	// cout << "Groups: " << gs << endl;
	// cout << "Shading groups: " << sgs << endl;
	// cout << "Vertices: " << vs << endl;
	// cout << "Tex Coords: " << vts << endl;
	// cout << "Normals: " << vns << endl;
	// cout << "Faces: " << fs << endl;
	// cout << "Material libraries: " << mtl << endl;
	// cout << "Use materials: " << usemtls << endl;
	// cout << "Unknown: " << unknown << endl;

	// cout << endl;
	// cout << "Size: " << vertices.size() <<endl;
	// cout << "vIndices Size: " << indices.size() <<endl;
	// cout << "vTexCoords Size: " << vTC.size() <<endl;
	// cout << "vNormals Size: " << vNormals.size() <<endl;

	// ------------------------------------------------------------

	
	// ------------------------------------------------------------

	vector<GLfloat> fullVertices;

	sortVerticesTCsAndNormals(fullVertices, vertices, texturecoords, normals, indices, vTC, vNormals);

	// --------------------------------------------------
	// Open GL Stuff
	// --------------------------------------------------

	glfwSetErrorCallback(onError);

	if (!glfwInit()) {
		return 1;
	}

	GLFWwindow *window = createWindow(600, 600, "Object Loader", 3, 2);

	if (window == NULL) {
		cout << "ERROR: Window didn't get created properly." << endl;
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, onFramebufferSize);
	glfwSetWindowCloseCallback(window, onWindowClose);
	glfwSetKeyCallback(window, key_callback);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLuint basicProgram = loadProgram("./shader/basic.vert.glsl", NULL, NULL, NULL, "./shader/basic.frag.glsl");

	GLuint vao, vbo, ebo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, fullVertices.size() * sizeof(GLfloat), fullVertices.data(), GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, testArraySize * sizeof(GLfloat), triangle, GL_STATIC_DRAW);

	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesTest.size() * sizeof(GLuint), indicesTest.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, testIndicesSize * sizeof(GLuint), triangleIndices, GL_STATIC_DRAW);

	GLuint vertLoc = glGetAttribLocation(basicProgram, "vertex");
	GLuint texLoc = glGetAttribLocation(basicProgram, "texcoord");
	GLuint normalLoc = glGetAttribLocation(basicProgram, "normal");

	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5*sizeof(GLfloat)));

	glEnableVertexAttribArray(vertLoc);
	glEnableVertexAttribArray(texLoc);
	glEnableVertexAttribArray(normalLoc);

	// ------------------------------------------------------------
	// ------------------------------------------------------------
	// ------------------------------------------------------------

	int x, y, random;
	unsigned char *image = loadImage("./old_man/Muro_head_dm.tga", x, y, random);

	if (image == NULL) {
		cout << "ERROR" << endl;
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // No mip-mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configure Texture Coordinate Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Delete image data
	delete[] image;
	image = NULL;

	cout << "Texture: " << texture << endl;
	// ------------------------------------------------------------
	// ------------------------------------------------------------
	// ------------------------------------------------------------

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(basicProgram);

	glm::mat4 projectionMat = glm::perspective(glm::radians(67.0f), 1.0f, 0.2f, 1000.0f);

	GLint projectionLoc = glGetUniformLocation(basicProgram, "u_projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMat));
	
	GLint viewLoc = glGetUniformLocation(basicProgram, "u_view");
	
	glm::vec3 initialPos = glm::vec3(0.0f, 165.5, 30.5);
	vcam.setPosition(initialPos);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(*vcam.getInverseViewMatrix()));
	
	glm::mat4 modelMat;

	GLint modelLoc = glGetUniformLocation(basicProgram, "u_model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

	GLuint textureLoc = glGetAttribLocation(basicProgram, "u_texture");
	glUniform1i(textureLoc, 0);

	currentTime = 0.0;
	previousTime = glfwGetTime();
	elapsedTime = 0.0;

	int iteration = 0;
	int counter = 3;

	while (!glfwWindowShouldClose(window)) {
		float cameraSpeed = 100.0f;

		currentTime = glfwGetTime();
		elapsedTime = currentTime - previousTime;
		previousTime = currentTime;

		glfwMakeContextCurrent(window);

		glClearColor(0.4f,0.4f,0.4f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(basicProgram);

		glBindVertexArray(vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(*vcam.getInverseViewMatrix()));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(*modelMatController.getViewMatrix()));

		// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
		// glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, NULL);
		// glDrawArrays(GL_LINES, 0, fullVertices.size());
		glDrawArrays(GL_TRIANGLES, 0, fullVertices.size());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			modelMatController.lookLeft(elapsedTime*PI);
		}
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			modelMatController.lookRight(elapsedTime*PI);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.moveForwards(elapsedTime*cameraSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.moveBackwards(elapsedTime*cameraSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.moveUp(elapsedTime*cameraSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.moveDown(elapsedTime*cameraSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.moveLeft(elapsedTime*cameraSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.moveRight(elapsedTime*cameraSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.lookUp(elapsedTime*PI);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.lookDown(elapsedTime*PI);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.lookLeft(elapsedTime*PI);
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			vcam.lookRight(elapsedTime*PI);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// delete file;
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


