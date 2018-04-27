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

struct Material {
	string name;
	glm::vec4 Ka;
	glm::vec4 Kd;
	glm::vec4 Ks;
	string diffuse_map;
	GLuint diffuse_tex;
	string bump_map;
	GLuint bump_tex;
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


	// ofstream vertexFile;
	// vertexFile.open("verticesRecord.txt", std::ofstream::trunc);
	// vertexFile.close();
	// vertexFile.open("verticesRecord.txt", std::ofstream::out | std::ofstream::app);

	int coms=0, gs=0, vs=0, vts=0, vns=0, sgs=0, fs=0, mtl=0, usemtls=0, unknown=0;

	bool limiter = true;

	while(getline(*file, line) && limiter) {

		int identifier = getIdentifier(line);

		switch (identifier) {
			case COMMENT:
			coms++;
			// cout << coms << ": " << line << endl;
			break;

			case GROUP:
			gs++;
			if (gs >= 3) {
				limiter = false;
			}
			// cout << "Words: " << countWords(line) << endl;
			break;

			case SMOOTHING_GROUP:
			sgs++;
			break;

			case VERTEX: {
				vs++;
				glm::vec3 vec;
				vec = getVertex(line);
				
				// vertexFile << glm::to_string(vec) << "\n";
				
				vertices.push_back(vec);
				// printVec(vertices);
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
				// // printVec(vec);
				getFaceIndices(vec, indices, vTC, vNormals);
			}
			break;

			case MATERIAL_LIBRARY: {
				mtl++;
				vector<string> words = getWords(line);
				printVec(words);
				string filename = folderWithOBJ + words[1];
				materialLibrary = readFileIntoBuffer(filename.c_str());
				
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
	// vertexFile.close();

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

	// ptrdiff_t pos = find(indices.begin(), indices.end(), 0) - indices.begin();
	// cout << "Position of 0 is : " << pos << endl;
	// cout << indices.at(456) << endl;

	string readbufferthing;
	while (getline(*materialLibrary, readbufferthing)) {
		cout << "A line: " << readbufferthing << endl;
	}
	

	// ------------------------------------------------------------

	// printVec(texturecoords);

	// cout << "Loop started" << endl;
	// for (auto v: vertices) {
	// 	if (v.y < 5.0f) {
	// 		cout << v.y << endl;
	// 	}
	// }
	// cout << "Loop finished" << endl;

	// vector<GLuint> indicesTest(indices.size());

	// indicesTest.at(0) = (indices[0]);
	// indicesTest.at(1) = (indices[1]);
	// indicesTest.at(2) = (indices[2]);

	// printVec(vertices);

	vector<GLfloat> fullVertices;
	// printVec(indices);

	sortVerticesTCsAndNormals(fullVertices, vertices, texturecoords, normals, indices, vTC, vNormals);

	// ofstream checkFullVs;
	// checkFullVs.open("checkingFullVec.txt", ofstream::trunc);
	// checkFullVs.close();
	// checkFullVs.open("checkingFullVec.txt", ofstream::app | ofstream::out);
	// int count = 0;
	// int count2 = 0;
	// while (count < fullVertices.size()) {

	// 	checkFullVs << "Vector: " << ++count2 << "\n";
	// 	checkFullVs << fullVertices[count] << " " << fullVertices[count+1] << " " << fullVertices[count+2] << "\n";
	// 	checkFullVs << fullVertices[count+3] << " " << fullVertices[count+4] << "\n";
	// 	checkFullVs << fullVertices[count+5] << " " << fullVertices[count+6] << " " << fullVertices[count+7] << "\n";

	// 	count += 8;

	// }
	// checkFullVs.close();

	// cout << "Size: " << fullVertices.size() << endl;
	// cout << "480: " << fullVertices[480] << endl;
	// cout << "481: " << fullVertices[481] << endl;
	// cout << "482: " << fullVertices[482] << endl;

	// --------------------------------------------------
	// Open GL Stuff
	// --------------------------------------------------

	glfwSetErrorCallback(onError);

	// GLfloat triangle[9] = {-0.5, -0.5f, 0.0f,
	// 						0.5f, -0.5f, 0.0f,
	// 						0.0f, 0.75f, 0.0f};

	// const int testArraySize = 12;
	// const int testIndicesSize = 6;

	// GLfloat triangle[testArraySize] = {4.610062, 170.562805, 8.466225, //21
	// 					   3.658489, 170.652985, 8.805237, //101
	// 						3.675125, 169.875290, 8.671654, //104
	// 						4.382637, 169.833160, 8.441437 //9
	// 					};


	// GLuint triangleIndices[testIndicesSize] = {0, 1, 2,
	// 											2, 3, 0};

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
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // No mip-mapping
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // No mip-mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configure Texture Coordinate Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Delete image data
	delete[] image;
	image = NULL;

	cout << "Texture: " << texture << endl;
	// ------------------------------------------------------------
	// ------------------------------------------------------------
	// ------------------------------------------------------------
	int jkl = 0;
	while (jkl < fullVertices.size()) {
		if (fullVertices[jkl] < 0.0 && fullVertices[jkl] > -1.4) {
			if (fullVertices[jkl+2] > 160) {
				// if (fullVertices[jkl+2] < 0) {
					cout << "Vertex" << endl;
					cout << fullVertices[jkl] << " "
					<< fullVertices[jkl+1] << " "
					<< fullVertices[jkl+2]<< " "
					// << fullVertices[jkl+3]<< " "
					// << fullVertices[jkl+4]<< " "
					// << fullVertices[jkl+5]<< " "
					// << fullVertices[jkl+6]<< " "
					/*<< fullVertices[jkl+7]*/ << endl;;
				// }
			}
		}
// if (fullVertices[jkl+3] < 0.52 && fullVertices[jkl+3] > 0.48) {
// 			if (fullVertices[jkl+4] < 0.52 && fullVertices[jkl+4] > 0.48) {
// 				if (fullVertices[jkl+2] < 0) {
// 					cout << "Vertex" << endl;
// 					cout << fullVertices[jkl] << " "
// 					<< fullVertices[jkl+1] << " "
// 					<< fullVertices[jkl+2]<< " "
// 					// << fullVertices[jkl+3]<< " "
// 					// << fullVertices[jkl+4]<< " "
// 					// << fullVertices[jkl+5]<< " "
// 					// << fullVertices[jkl+6]<< " "
// 					/*<< fullVertices[jkl+7]*/ << endl;;
// 				}
// 			}
// 		}

		jkl += 8;
	}


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(basicProgram);

	glm::mat4 projectionMat = glm::perspective(glm::radians(67.0f), 1.0f, 0.2f, 1000.0f);

	GLint projectionLoc = glGetUniformLocation(basicProgram, "u_projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMat));
	// ----------------------------------------------------------------------
	// glm::mat4 viewMat;
	// glm::vec3 viewPosition(4.0f,  170.0f,  10.0f);
	// glm::vec3 viewUp      (0.0f,  1.0f,  0.0f);
	// glm::vec3 viewForward (0.0f,  0.0f, -1.0f);

	// viewUp      = glm::normalize(viewUp);
	// viewForward = glm::normalize(viewForward);

	// viewMat = glm::lookAt(viewPosition, viewPosition + viewForward, viewUp);

	GLint viewLoc = glGetUniformLocation(basicProgram, "u_view");
	// glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

	glm::vec3 initialPos = glm::vec3(0.0f, 165.5, 30.5);
	vcam.setPosition(initialPos);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(*vcam.getInverseViewMatrix()));
	
	// ----------------------------------------------------------------------

	glm::mat4 modelMat;

	GLint modelLoc = glGetUniformLocation(basicProgram, "u_model");
	// glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(*modelMatController.getViewMatrix()));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

	// glClearColor(0.9f,0.2f,0.2f,0.0f);

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
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, NULL);

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
		// glDrawElements(GL_LINES, indices.size() * 3, GL_UNSIGNED_INT, NULL);
		// glDrawElements(GL_TRIANGLES, testIndicesSize, GL_UNSIGNED_INT, NULL);

		// if (iteration > 1975) {

		// 	getchar();

		// 	cout << "Iteration: " << ++iteration << endl;

		// 	indicesTest.at(counter) = (indices[counter]); counter++;
		// 	indicesTest.at(counter) = (indices[counter]); counter++;
		// 	indicesTest.at(counter) = (indices[counter]); 
		// 	cout << "Inidices ---------------" << endl;
		// 	cout << indicesTest.at(counter - 2) << ": " << glm::to_string(vertices[indicesTest.at(counter - 2) - 1]) << endl;	
		// 	cout << indicesTest.at(counter - 1) << ": " << glm::to_string(vertices[indicesTest.at(counter - 1) - 1]) << endl;	
		// 	cout << indicesTest.at(counter) << ": " << glm::to_string(vertices[indicesTest.at(counter) - 1]) << endl;	
		// 	cout << "---------------" <<endl;
		// 	counter++;
		// 	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesTest.size()*sizeof(GLuint), indicesTest.data());
		// } else if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		// 	cout << "Iteration: " << ++iteration << endl;

		// 	indicesTest.at(counter) = (indices[counter]); counter++;
		// 	indicesTest.at(counter) = (indices[counter]); counter++;
		// 	indicesTest.at(counter) = (indices[counter]); 
		// 	if (iteration > 1975) {
		// 		cout << "Inidices ---------------" << endl;
		// 		cout << indicesTest.at(counter - 2) << ": " << glm::to_string(vertices[indicesTest.at(counter - 2) - 1]) << endl;	
		// 		cout << indicesTest.at(counter - 1) << ": " << glm::to_string(vertices[indicesTest.at(counter - 1) - 1]) << endl;	
		// 		cout << indicesTest.at(counter) << ": " << glm::to_string(vertices[indicesTest.at(counter) - 1]) << endl;	
		// 		cout << "---------------" <<endl;
		// 	}
		// 	counter++;
		// 	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesTest.size()*sizeof(GLuint), indicesTest.data());
		// 	// printVec(indicesTest);
		// }


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// delete file;
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


