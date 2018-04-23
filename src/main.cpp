// Assignment number 2, 159.709, 2018 S1
// Euinton, Dale, 14026002,
// Explain what the program is doing . . .


// ----------------------------------------------------------------------
// 			WORKLOG
// ----------------------------------------------------------------------

// Tomorrow focus on actually extracting data and getting it working!!




// ----------------------------------------------------------------------

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>

using namespace std;

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

glm::vec3 getVertex(string &str) {
	string trimmed = str.substr(2, string::npos);
	stringstream stream;
	stream.str(trimmed);
	string str1, str2, str3;
	GLfloat num1, num2, num3;
	num1 = getAndConvertToFloat(stream, str1);
	num2 = getAndConvertToFloat(stream, str2);
	num3 = getAndConvertToFloat(stream, str3);

	cout << "(" << num1 << ", " << num2 << ", " << num3 << ")" << endl;

	glm::vec3 vec(num1, num2, num3);
	return vec;
}

int main() {
	const char *filename = "test.obj";
	// const char *filename = "old_man/muro.obj";
	stringstream *file = readFileIntoBuffer(filename);
	
	stringstream linestream;
	string line;

	while(getline(*file, line)) {
		cout << line << endl;

		if (line[0] == '#') {
			// Do nothing
			cout << "Doing nothing" << endl;
		} else if (line [0] == 'v') {
			cout << "Vertex" << endl;
			getVertex(line);
		}
	}




	delete file;
	return 0;
}


