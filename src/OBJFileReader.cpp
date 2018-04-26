#include "OBJFileReader.h"

void printVec(vector<glm::vec3> &vec) {
	for (auto v : vec) {
		cout << glm::to_string(v) << endl;
	}
}

void printVec(vector<string> &vec) {
	for (auto v : vec) {
		cout << v << endl;
	}
}

void printVec(vector<GLuint> &vec) {
	for (auto v : vec) {
		cout << v << endl;
	}
}

stringstream* readFileIntoBuffer(const char *filename) {
	ifstream infile(filename);
	stringstream *buffer = new stringstream();
	*buffer << infile.rdbuf();
	// cout << buffer->str() << endl;
	infile.close();
	return buffer;
}

GLfloat getAndConvertToFloat(stringstream &ss, string &str) {
	getline(ss, str, ' ');
	return atof(str.c_str());
}

vector<string> getWords(string &line) {
	vector<string> output;
	stringstream ss;
	ss.str(line);
	string ph;
	while (getline(ss, ph, ' ')) {
		output.push_back(ph);
	}
	return output;
}

int sumSentence(string &line) {
	vector<string> vec = getWords(line);
	return vec.size();
}

GLfloat* getFloats(string &str) {
	vector<string> words;
	words = getWords(str);

	GLfloat *nums = new GLfloat[words.size()-1];
	// printVec(words);
	for (int i = 1; i < words.size(); i++)
	{
		nums[i-1] = atof(words[i].c_str());
	}

	return nums;
}

glm::vec3 getVertex(string &line) {
	GLfloat* nums = getFloats(line);
	glm::vec3 vec = glm::vec3(nums[0], nums[1], nums[2]);
	delete nums;
	return vec;
}

glm::vec2 getTexCoords(string &line) {
	GLfloat *nums = getFloats(line);
	glm::vec2 vec = glm::vec2(nums[0], nums[1]);
	delete nums;
	return vec;
}

int getCase(string &identifier) {
	int output = -1;

	if (identifier.compare("g") == 0) {
		output = GROUP;
	} else if (identifier.compare("#") == 0) {
		output = COMMENT;
	} else if (identifier.compare("v") == 0) {
		output = VERTEX;
	} else if (identifier.compare("s") == 0) {
		output = SMOOTHING_GROUP;
	} else if (identifier.compare("vt") == 0) {
		output = TEX_COORD;
	} else if (identifier.compare("vn") == 0) {
		output = NORMAL;
	} else if (identifier.compare("f") == 0) {
		output = FACE;
	} else if (identifier.compare("mtllib") == 0) {
		output = MATERIAL_LIBRARY;
	} else if (identifier.compare("\n") == 0) {
		output = NEW_LINE;
	} else if (identifier.compare("usemtl") == 0) {
		output = USE_MTL;
	} else {
		// cout << "Unknown: " << identifier << endl;
	}

	return output;
}

int getIdentifier(string &line) {
	stringstream linestream;
	linestream.str(line);
	string identifier;
	getline(linestream, identifier, ' ');
	// cout << identifier << endl;
	return getCase(identifier);
}

int countWords(stringstream &ss) {
	string str;
	int i = 0;
	while (getline(ss, str)) {
		i++;
	}
	return i;
}

int countWords(string &line) {
	stringstream ss;
	string str;
	int i = 0;
	ss.str(line);
	while (getline(ss, str, ' ')) {
		i++;
	}
	return i;
}

vector<GLuint> seperateFaceIndices(string &word) {
	stringstream ss;
	ss.str(word);
	string ph;
	vector<GLuint> indices;

	while(getline(ss, ph, '/')) {
		indices.push_back((GLuint) atof(ph.c_str()));
	}

	return indices;
}

void getFaceIndices(vector<string> &faceWords, vector<GLuint> &vIndices, vector<GLuint> &vTexCoords, vector<GLuint> &vNormals) {
	vector<vector<GLuint>> face;

	if (faceWords.size() == 5) {
		for (int i = 1; i < faceWords.size(); i++)
		{
			vector<GLuint> indices = seperateFaceIndices(faceWords[i]);
			face.push_back(indices);
			// cout << indices[0] << " " << indices[1] << " " << indices[2] << " " <<endl;
		}
		if (face.size() == 4) {
		for (int i = 0; i < 3; i++) {
					vIndices.push_back(face[i][0] - 1);
					vTexCoords.push_back(face[i][1] - 1);
					vNormals.push_back(face[i][2] - 1);
			}
			for (int i = 0; i < 3; i++) {
				int index = (i + 2) % face.size(); // This just allows me to get the indices 2, 3, 0 
					vIndices.push_back(face[index][0] - 1);
					vTexCoords.push_back(face[index][1] - 1);
					vNormals.push_back(face[index][2] - 1);
			}
		} else {
			cout << "ERROR: check getFaceIndices." << endl;
		}
	} else if (faceWords.size() == 4) {
		for (int i = 1; i < faceWords.size(); i++)
		{
			vector<GLuint> indices = seperateFaceIndices(faceWords[i]);
			face.push_back(indices);
			// cout << indices[0] << " " << indices[1] << " " << indices[2] << " " <<endl;
		}
		for (int i = 0; i < face.size(); i++) {
				vIndices.push_back(face[i][0] - 1);
				vTexCoords.push_back(face[i][1] - 1);
				vNormals.push_back(face[i][2] - 1);
		}
	}
}