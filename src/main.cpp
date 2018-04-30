// Assignment number 2, 159.709, 2018 S1
// Euinton, Dale, 14026002,
// This program loads up the obj file muro.obj unless specified in the arguments in the terminal. 
// Should one want to load another object, they should run the program with the path to the folder 
// as the first argument and the obj file in the folder as the second argument. 

// MOVEMENT 
// J - rotate the object left
// L - rotate the object right
// W, A, S, D - virtual camera movement
// R, F = virtual camera height adjustment
// UP, DOWN, LEFT, RIGHT - virtual camera look direction

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

string folderWithOBJ = "./old_man/";
string file = "muro.obj";

struct GLObject {
	vector<GLfloat> vertices;
	GLuint vao; 					//vertex array object
	GLuint vbo; 					//vertex buffer object
	GLuint ebo; 					//element buffer object
	GLuint sp;						//shader program
	string materialName ;						
	Material* material;
	vector<GLuint> vIndices;
	vector<GLuint> tcIndices;
	vector<GLuint> nIndices;
	GLint modelMatLocation;
	GLint viewMatLocation;
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
		if (key == GLFW_KEY_P) {vcam.printPosition();}
	}
}

void bindAndSetBuffers(GLObject *object) {
	object->sp = loadProgram("./shader/basic.vert.glsl", NULL, NULL, NULL, "./shader/basic.frag.glsl");
	// cout << "The program number is : " << object->sp << endl;

	glGenVertexArrays(1, &object->vao);
	glGenBuffers(1, &object->vbo);

	glBindVertexArray(object->vao);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);

	glBufferData(GL_ARRAY_BUFFER, object->vertices.size() * sizeof(GLfloat), object->vertices.data(), GL_STATIC_DRAW);

	GLuint vertLoc = glGetAttribLocation(object->sp, "vertex");
	GLuint texLoc = glGetAttribLocation(object->sp, "texcoord");
	GLuint normalLoc = glGetAttribLocation(object->sp, "normal");

	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5*sizeof(GLfloat)));

	glEnableVertexAttribArray(vertLoc);
	glEnableVertexAttribArray(texLoc);
	glEnableVertexAttribArray(normalLoc);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	cout << "Made it to the end of bindAndSetBuffers" << endl;
}

void loadAndBindOneTexture(string texname, GLuint &tex) {
	int x, y, n;
	string filename = folderWithOBJ + texname;
	unsigned char* image = loadImage(filename.c_str(), x, y, n);

	if (image == NULL) {
		cout << "ERROR: Didn't load an image." << endl;
		return;
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // No mip-mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] image;
	image = NULL;
}

void loadAndBindObjectTextures(GLObject *object) {
	if (object->material != NULL) {
		if (object->material->diffuseMap.length() ==0) {
			cout << "Didn't load an image because " << object->materialName << " does not have a diffuseMap." << endl;
			return;
		}

		loadAndBindOneTexture(object->material->diffuseMap, object->material->diffuseTex);

		if (object->material->specularMap.length() == 0) {
			cout << "Didn't load an image because " << object->materialName << " does not have a specularMap." << endl;
			return;
		}

		loadAndBindOneTexture(object->material->specularMap, object->material->specularTex);

	}
	cout << "Made it to the end of loadAndBindObjectTextures" << endl;
}

void setUniforms(GLObject *object) {
	cout << "Shader program number " << object->sp << endl;

	glUseProgram(object->sp);

	glm::mat4 projectionMat = glm::perspective(glm::radians(67.0f), 1.0f, 0.2f, 1000.0f);
	GLint projectionLoc = glGetUniformLocation(object->sp, "u_projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMat));

	object->viewMatLocation = glGetUniformLocation(object->sp, "u_view");
	glm::vec3 initialPos = glm::vec3(0.0f, 120.0, 175.0);
	vcam.setPosition(initialPos);
	glUniformMatrix4fv(object->viewMatLocation, 1, GL_FALSE, glm::value_ptr(*vcam.getInverseViewMatrix()));
	
	object->modelMatLocation = glGetUniformLocation(object->sp, "u_model");
	glUniformMatrix4fv(object->modelMatLocation, 1, GL_FALSE, glm::value_ptr(*modelMatController.getViewMatrix()));
	
	GLuint textureLoc = glGetUniformLocation(object->sp, "u_texture");	
	glUniform1i(textureLoc, 0);

	GLuint specMapLoc = glGetUniformLocation(object->sp, "u_specMap");	
	glUniform1i(specMapLoc, 0);

	GLuint useTexLoc = glGetUniformLocation(object->sp, "useTexture");	
	cout << endl;
	cout << object->materialName << endl;
	cout << object->material->diffuseMap << " length: " << object->material->diffuseMap.length() << endl;
	cout << endl;
	int useTexValue = 0;
	if (object->material->diffuseMap.length() == 0) {
		cout << "NO TEX" << endl;
		// glUniform1i(useTexLoc, 0);
		useTexValue = 0;
	} else {
		cout << "Use the texture" << endl;
		// glUniform1i(useTexLoc, 1);	
		useTexValue = 1;
	}

	glUniform1i(useTexLoc, useTexValue);


	GLuint useSpecLoc = glGetAttribLocation(object->sp, "useSpecMap");	
	// cout << endl;
	// cout << object->materialName << endl;
	// cout << object->material->specularMap << " length: " << object->material->specularMap.length() << endl;
	// cout << endl;
	if (object->material->specularMap.length() == 0) {
		// cout << "True" << endl;
		glUniform1i(useSpecLoc, 0);
	} else {
		// cout << "False" << endl;
		glUniform1i(useSpecLoc, 1);
	}

	if (object->material != NULL) {

		cout << endl;
		cout << object->materialName << endl;
		cout << "HERE IT IS -------------" << endl;
		cout << endl;

		glm::vec4 vector;

		vector = object->material->Ka;
		GLuint kaLoc = glGetUniformLocation(object->sp, "Ka");
		// glUniform4fv(kaLoc, 3, glm::value_ptr(object->material->Ka));
		glUniform4f(kaLoc, vector.x, vector.y, vector.z, vector.w);

		cout << glm::to_string(vector) << endl;

		vector = object->material->Kd;
		GLuint kdLoc = glGetUniformLocation(object->sp, "Kd");
		// glUniform4fv(kdLoc, 3, glm::value_ptr(object->material->Kd));
		glUniform4f(kdLoc, vector.x, vector.y, vector.z, vector.w);

		cout << glm::to_string(vector) << endl;

		vector = object->material->Ks;
		GLuint ksLoc = glGetUniformLocation(object->sp, "Ks");
		// glUniformMatrix4fv(ksLoc, 1, GL_FALSE, glm::value_ptr(object->material->Ks));
		// glUniform4f(ksLoc, object->material->Ks.x, object->material->Ks.y, object->material->Ks.z, object->material->Ks.w);
		glUniform4f(ksLoc, vector.x, vector.y, vector.z, vector.w);

		cout << glm::to_string(vector) << endl;

		GLuint niLoc = glGetUniformLocation(object->sp, "a");
		glUniform1f(niLoc, object->material->Ns);
	}

	glUseProgram(0);
	cout << "Made it to the end of setUniforms" << endl;
}

void draw(GLObject *object) {
	// cout << "Printing! " << endl;
	// printVec(object->vertices);
	glUseProgram(object->sp);
	glBindVertexArray(object->vao);

	

	if (object->material->diffuseTex != 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object->material->diffuseTex);
	} 
	if (object->material->specularTex != 0) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, object->material->specularTex);
	}
	glUniformMatrix4fv(object->viewMatLocation, 1, GL_FALSE, glm::value_ptr(*vcam.getInverseViewMatrix()));
	glUniformMatrix4fv(object->modelMatLocation, 1, GL_FALSE, glm::value_ptr(*modelMatController.getViewMatrix()));
	glDrawArrays(GL_TRIANGLES, 0, object->vertices.size());
	// glDrawArrays(GL_LINES, 0, object->vertices.size());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void deleteBuffersAndProgram(GLObject* object) {
	glDeleteVertexArrays(1, &object->vao);
	glDeleteBuffers(1, &object->vbo);
	glDeleteProgram(object->sp);
}

void deleteObject(GLObject* object) {
	if (object->material != NULL) {
		// delete object->material;
		// object->material = NULL;
	}
	
	delete object;
	object = NULL;
}

void loadObject() {

	const char *filename = (folderWithOBJ + file).c_str();
	cout << filename << endl;
	stringstream *file = readFileIntoBuffer(filename);
	stringstream *materialLibrary;	
	
	string line;

	vector<glm::vec3> vertices, normals;
	vector<glm::vec2> texturecoords;

	vector<Material*> *materials;
	vector<GLObject*> *objects = new vector<GLObject*>();

	bool limiter = true;

	int usemtls = 0;

	while(getline(*file, line)) {

		int identifier = getIdentifier(line);

		switch (identifier) {

			case VERTEX: {
				glm::vec3 vec;
				vec = getVertex(line);
				vertices.push_back(vec);
				break;
			}					

			case TEX_COORD: {
				glm::vec2 vec;
				vec = getTexCoords(line);
				texturecoords.push_back(vec);
				break;
			}			

			case NORMAL: {
				glm::vec3 vec;
				vec = getVertex(line);
				normals.push_back(vec); 
				break;
			}			

			case FACE: {
				vector<string> vec = getWords(line);
				getFaceIndices(vec, objects->back()->vIndices, objects->back()->tcIndices, objects->back()->nIndices);
				break;
			}
			

			case MATERIAL_LIBRARY: {
				vector<string> words = getWords(line);
				string filename = folderWithOBJ + words[1];
				cout << filename << endl;
				string testing;
				// ifstream checkFile(filename);
				// cout << "Exists: " << checkFile.good() <<endl;
				// getline(checkFile, testing);
				// cout << testing << endl;
				// getline(checkFile, testing);
				// cout << testing << endl;
				// getline(checkFile, testing);
				// cout << testing << endl;
				// getline(checkFile, testing);
				// cout << testing << endl;
				// getline(checkFile, testing);
				// cout << testing << endl;
				// checkFile.close();
				// const char* realFileName = filename.c_str();
				// materialLibrary = readFileIntoBuffer(realFileName);
				materialLibrary = readFileIntoBuffer(filename.c_str());
				// materialLibrary = readFileIntoBuffer("./Baymax/Bigmax_White_OBJ.mtl");
				materials = readMaterialsLibrary(*materialLibrary);				
			}
			
			break;

			case USE_MTL: {
				GLObject *object = new GLObject();
				vector<string> words = getWords(line);
				cout << words[1] << endl;
				object->materialName = words[1];
				objects->push_back(object);
				break;
			}			

			case -1:
				break;
		}
	}
	delete file;
	file = NULL;
	delete materialLibrary;
	materialLibrary = NULL;

	cout << "Made it past the file loading!" << endl;
	cout << "Objects has: " << objects->size() << endl;
	cout << "Materials has: " << materials->size() << endl;

	if (materials->size() == 3) {
		Material* mat = materials->back();
		cout << mat->name << endl;
		cout << "Kd: " << glm::to_string(mat->Kd) << endl;
		cout << "Ka: " << glm::to_string(mat->Ka) << endl;
		cout << "Ks: " << glm::to_string(mat->Ks) << endl;
	}
	
	// Get the materials for each object
	for (auto o: *objects) {
		string material = o->materialName;
		for (auto m: *materials) {
			if (m->name.compare(material) == 0) {
				cout << "Matched " << m->name << endl;
				o->material = m;
			}
		}
	}

	// --------------------------------------------------
	// Open GL Stuff
	// --------------------------------------------------

	glfwSetErrorCallback(onError);

	if (!glfwInit()) {
		return;
	}

	GLFWwindow *window = createWindow(600, 600, "Object Loader", 3, 2);

	if (window == NULL) {
		cout << "ERROR: Window didn't get created properly." << endl;
		return;
	}

	glfwSetFramebufferSizeCallback(window, onFramebufferSize);
	glfwSetWindowCloseCallback(window, onWindowClose);
	glfwSetKeyCallback(window, key_callback);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// printVec(vertices);
	// cout << "Size: " << vertices.size() << endl;

	// sortVerticesTCsAndNormals(objects->back()->vertices, vertices, texturecoords, normals, objects->back()->vIndices, objects->back()->tcIndices, objects->back()->nIndices);

	for (GLObject* o: *objects) {
		cout << o->materialName << endl;
		sortVerticesTCsAndNormals(o->vertices, vertices, texturecoords, normals, o->vIndices, o->tcIndices, o->nIndices);
		cout << "Got past sort vertices and normals" << endl;
		bindAndSetBuffers(o);
		loadAndBindObjectTextures(o);
		setUniforms(o);
	}

	cout << "Made it to after object initialisation" << endl;

	currentTime = 0.0;
	previousTime = glfwGetTime();
	elapsedTime = 0.0;

	int iteration = 0;
	int counter = 3;

	while (!glfwWindowShouldClose(window)) {

		// cout << "Window starting soon" <<endl;
		float cameraSpeed = 100.0f;

		currentTime = glfwGetTime();
		elapsedTime = currentTime - previousTime;
		previousTime = currentTime;

		glfwMakeContextCurrent(window);

		glClearColor(0.4f,0.4f,0.4f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (GLObject* o: *objects) {
			draw(o);
		}

		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
			modelMatController.lookLeft(elapsedTime*PI);
		}
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT) {
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

	delete materials;

	for (auto o: *objects) {
		deleteBuffersAndProgram(o);
		deleteObject(o);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

int main(int argc, const char* argv[]) {
	if (argc > 2) {
		folderWithOBJ = argv[1];
		folderWithOBJ = "./" + folderWithOBJ;
		file = argv[2];
	}
	loadObject();
}


