#include "OBJFileReader.h"

void printVec(vector<glm::vec3> &vec) {
	for (auto v : vec) {
		cout << glm::to_string(v) << endl;
	}
}

void printVec(vector<glm::vec2> &vec) {
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
	// for (int i = 0; i < 457; i++) {
	// 	cout << vec[i] << endl;
	// }
}

void printVec(vector<GLfloat> &vec) {
	cout << "Printing!" << endl;
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

void sortVerticesTCsAndNormals(vector<GLfloat> &output, vector<glm::vec3> &vertices, vector<glm::vec2> &texcoords, vector<glm::vec3> &normals,
								vector<GLuint> &vIndices, vector<GLuint> &tcIndices, vector<GLuint> &nIndices) {
	if (vIndices.size() != tcIndices.size() || vIndices.size() != nIndices.size() || nIndices.size() != tcIndices.size()) {
		cout << "Failed to sort as the indices vectors are different sizes." << endl;
		return;
	}

	for (int i = 0; i < vIndices.size(); i++) {

		// ptrdiff_t pos = find(vIndices.begin(), vIndices.end(), i) - vIndices.begin();

		output.push_back(vertices[vIndices[i]].x);
		output.push_back(vertices[vIndices[i]].y);
		output.push_back(vertices[vIndices[i]].z);

		output.push_back(texcoords[tcIndices[i]].x);
		output.push_back(texcoords[tcIndices[i]].y);

		output.push_back(normals[nIndices[i]].x);
		output.push_back(normals[nIndices[i]].y);
		output.push_back(normals[nIndices[i]].z);	
	}

	// for (int i = 0; i < vertices.size(); i++) {

	// 	ptrdiff_t pos = find(vIndices.begin(), vIndices.end(), i) - vIndices.begin();

	// 	output.push_back(vertices[i].x);
	// 	output.push_back(vertices[i].y);
	// 	output.push_back(vertices[i].z);

	// 	output.push_back(texcoords[tcIndices[(int) pos]].x);
	// 	output.push_back(texcoords[tcIndices[(int) pos]].y);

	// 	output.push_back(normals[nIndices[(int) pos]].x);
	// 	output.push_back(normals[nIndices[(int) pos]].y);
	// 	output.push_back(normals[nIndices[(int) pos]].z);	
	// }
}

vector<Material*>* readMaterialsLibrary(stringstream &materialsLibrarySS) {
	vector<Material*> *output = new vector<Material*>;
	string line;
	int index = -1;
	while (getline(materialsLibrarySS, line)) {
		vector<string> words = getWords(line);
		string identifier = words[0];
		int mlCase = getMLCase(identifier);
		

		// cout << "Beginning" << endl;
		// printVec(words);

		switch(mlCase) {
			case NEW_MTL: {
				// cout << "New Mat" << endl;
				index++;
				Material *another = new Material();
				output->push_back(another);
				output->at(index)->name = words[1];
				break;
			}
			case KD: {
				// cout << "Kd" << endl;
				glm::vec3 nums = getVertex(line);
				// cout << "Kd: " << glm::to_string(nums);
				// printVec(words);
				glm::vec4 lightingVec = glm::vec4(nums.x, nums.y, nums.z, 1.0f);
				// cout << "Just before accessing. " << index << " size is " << output->size() << endl;
				output->at(index)->Kd = lightingVec;
				break;
			}
			case KA: {
				// cout << "Ka" << endl;
				glm::vec3 nums = getVertex(line);
				// cout << "Ka: " << glm::to_string(nums);
				// printVec(words);
				glm::vec4 lightingVec = glm::vec4(nums.x, nums.y, nums.z, 1.0f);
				output->at(index)->Ka = lightingVec;
				break;
			}
			case KS: {
				// cout << "Ks" << endl;
				glm::vec3 nums = getVertex(line);
				// cout << "Ks: " << glm::to_string(nums);
				// printVec(words);
				glm::vec4 lightingVec = glm::vec4(nums.x, nums.y, nums.z, 1.0f);
				output->at(index)->Ks = lightingVec;
				break;
			}
			case MAP_KD: {
				// cout << "Map Kd" << endl;
				output->at(index)->diffuseMap = words[1];
				break;
			}
			case MAP_KS: {
				// cout << "Map Ks" << endl;
				output->at(index)->specularMap = words[1];
				break;
			}
			case BUMP_MAP: {
				// cout << "Bump Map" << endl;
				output->at(index)->bumpMap = words[1];
				break;
			}
		}
	}

	return output; 
}

int getMLCase(string &identifier) {
	int output = -1;

	if (identifier.compare("newmtl") == 0) {
		output = NEW_MTL;
	} else if (identifier.compare("illum") == 0) {
		output = ILLUM;
	} else if (identifier.compare("Kd") == 0) {
		output = KD;
	} else if (identifier.compare("Ka") == 0) {
		output = KA;
	} else if (identifier.compare("Tf") == 0) {
		output = TF;
	} else if (identifier.compare("map_Kd") == 0) {
		output = MAP_KD;
	} else if (identifier.compare("bump") == 0) {
		output = BUMP_MAP;
	} else if (identifier.compare("Ni") == 0) {
		output = NI;
	} else if (identifier.compare("Ks") == 0) {
		output = KS;
	} else if (identifier.compare("map_Ks") == 0) {
		output = MAP_KS;
	} else {
		// cout << "Unknown: " << identifier << endl;
	}

	return output;
}