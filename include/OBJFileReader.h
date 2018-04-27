#ifndef _FILEREADING_
#define _FILEREADING_

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>

#include "utils.h"

using namespace std;

const int COMMENT = 0;
const int NEW_LINE = 8;

const int GROUP = 1;
const int SMOOTHING_GROUP = 2;

const int VERTEX = 3;
const int TEX_COORD = 4;
const int NORMAL = 5;
const int FACE = 6;

const int MATERIAL_LIBRARY = 7;
const int USE_MTL = 9;

const int NEW_MTL = 10;
const int ILLUM = 11;
const int KD = 12;
const int KA = 13;
const int KS = 14;
const int TF = 15;
const int MAP_KD = 16;
const int BUMP_MAP = 17;
const int NI = 18;
const int MAP_KS = 19;



struct Material {
	string name;
	glm::vec4 Ka;
	glm::vec4 Kd;
	glm::vec4 Ks;
	string diffuseMap;
	GLuint diffuseTex;
	string bumpMap;
	GLuint bumpTex;
	string specularMap;
	GLuint specularTex;

	void print() {
		cout << endl;
		cout << "Material: " << name << endl;
		cout << "Ka: " << glm::to_string(Ka) << endl; 
		cout << "Kd: " << glm::to_string(Kd) << endl; 
		cout << "Ks: " << glm::to_string(Ks) << endl; 
		cout << "Diffuse Map: " << diffuseTex << " " << diffuseMap << endl;
		cout << "Bump Map: " << bumpTex << " " << bumpMap << endl;
		cout << "Specular Map: " << specularTex << " " << specularMap << endl;
	}
};

void printVec(vector<glm::vec3> &vec);
void printVec(vector<glm::vec2> &vec);
void printVec(vector<string> &vec);
void printVec(vector<GLuint> &vec);
void printVec(vector<GLfloat> &vec);
stringstream* readFileIntoBuffer(const char *filename);
GLfloat getAndConvertToFloat(stringstream &ss, string &str);
vector<string> getWords(string &line);
int sumSentence(string &line);
GLfloat* getFloats(string &str);
glm::vec3 getVertex(string &line);
glm::vec2 getTexCoords(string &line);
int getIdentifier(string &line);
int countWords(stringstream &ss);
int countWords(string &line);
vector<GLuint> seperateFaceIndices(string &word);
void getFaceIndices(vector<string> &faceWords, vector<GLuint> &vIndices, vector<GLuint> &vTexCoords, vector<GLuint> &vNormals);
void sortVerticesTCsAndNormals(vector<GLfloat> &output, vector<glm::vec3> &vertices, vector<glm::vec2> &texcoords, vector<glm::vec3> &normals,
								vector<GLuint> &vIndices, vector<GLuint> &tcIndices, vector<GLuint> &nIndices);
vector<Material*>* readMaterialsLibrary(stringstream &materialsLibrarySS);
int getMLCase(string &identifier);

#endif