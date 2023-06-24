#pragma once
#include <string>
using std::string;

string ReadShaderSource(const string& shaderFile);
GLuint InitShader(const string& vShaderFile, const string& fShaderFile);