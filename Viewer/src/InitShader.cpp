#include <glad/glad.h>
#include "InitShader.h"
#include <iostream>
#include <fstream>
#include <sstream>

// get line that takes line endings into account. The stack overflow thread:
// https://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
std::istream& safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case std::streambuf::traits_type::eof():
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

// Again from example in:
// https://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
string ReadShaderSource(const string& shaderFile)
{
	std::string path = shaderFile;  // insert path to test file here
	std::ifstream ifs(path.c_str());
	if (!ifs) {
		std::cerr << "Failed to read " << path << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string line;
	std::stringstream ss;
	while (!safeGetline(ifs, line).eof())
	{
		ss << line << std::endl;
	}
	return ss.str();
}

// Create a GLSL program object from vertex and fragment shader files
GLuint
InitShader(const string& vShaderFile, const string& fShaderFile)
{
	struct Shader {
		string			filename;
		GLenum			type;
		const GLchar* source;
	}
	shaders[2] = {
					{ vShaderFile, GL_VERTEX_SHADER, NULL },
					{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
	};

	GLuint program = glCreateProgram();

	for (int i = 0; i < 2; ++i) {
		Shader& s = shaders[i];
		// looks unnecessary, but makes sure opengl doesn't get dangling pointers
		// https://stackoverflow.com/questions/10877386/opengl-shader-compilation-errors-unexpected-undefined-at-token-undefined
		string contents = ReadShaderSource(s.filename);
		s.source = contents.c_str();

		GLuint shader = glCreateShader(s.type);
		glShaderSource(shader, 1, (const GLchar**)&s.source, NULL);
		glCompileShader(shader);

		GLint  compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			std::cerr << s.filename << " failed to compile:" << std::endl;
			GLint  logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader, logSize, NULL, logMsg);
			std::cerr << logMsg << std::endl;
			delete[] logMsg;

			exit(EXIT_FAILURE);
		}

		//delete [] s.source;

		glAttachShader(program, shader);
	}

	/* link  and error check */
	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;

		exit(EXIT_FAILURE);
	}

	/* use program object */
	glUseProgram(program);

	return program;
}