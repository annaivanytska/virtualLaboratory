#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<stdexcept>
#include<cstring>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>


#include "../include/glm/glm.hpp"
#include "../include/glm/ext/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp" 

std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();

	void setMat4(const std::string &name, const glm::mat4 &mat) const;
};
#endif