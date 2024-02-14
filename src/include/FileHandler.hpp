#include <GL/glew.h>
#include <GL/glu.h>


#include <iostream>
#include <fstream>
#include <string>

class FileHandler {
    public:
        /** Load shader file from a given path
		 * @param path The path to the shader file
		 * @return A null-terminated string representing the source
		*/
		static std::string getShaderFromFile(const std::string path) {
			std::ifstream fileStream(path, std::ios::in);	// Create a filestream to the given path
			std::string source;	// Resulting file data

			if(!fileStream.is_open()) {
				std::cerr << "Could not read file: " << path << std::endl;
				return NULL;
			}

			std::string line = "";	// Current line
			while(!fileStream.eof()) {
				std::getline(fileStream, line);	// Load into 'line'
				source.append(line + "\n");
			}

			source.append("\0");	// Make null-terminated
			fileStream.close();

			return source;	// Return the source string
		}

		/** Compile and return a shader from the given path
		 * @param path The path to the shader file
		 * @return a GLuint representing the compiled shader
		 * @note REQUIRES that the given path uses the following file extensions
		 * @note .vert - a vertex shader
		 * @note .tesc - a tessellation control shader
		 * @note .tese - a tessellation evaluation shader
		 * @note .geom - a geometry shader
		 * @note .frag - a fragment shader
		 * @note .comp - a compute shader
		*/
		static GLuint compileShader(const std::string path) {
			GLuint shader;
			// Determine shader type
			if(path.substr(path.length()-4, 4).compare("vert") == 0){
				shader = glCreateShader(GL_VERTEX_SHADER);
			} else if(path.substr(path.length()-4, 4).compare("tesc") == 0) {
				shader = glCreateShader(GL_TESS_CONTROL_SHADER);
			} else if(path.substr(path.length()-4, 4).compare("tese") == 0) {
				shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
			} else if(path.substr(path.length()-4, 4).compare("geom") == 0) {
				shader = glCreateShader(GL_GEOMETRY_SHADER);
			} else if(path.substr(path.length()-4, 4).compare("frag") == 0) {
				shader = glCreateShader(GL_FRAGMENT_SHADER);
			} else if(path.substr(path.length()-4, 4).compare("comp") == 0) {
				shader = glCreateShader(GL_COMPUTE_SHADER);
			} else {
				throw std::logic_error("FileHandler::compileShader(): Unknown shader file extension");
				return -1;
			}

			// Load and convert the shader file into a const GLchar**
			std::string shaderSourceStr = FileHandler::getShaderFromFile(path);	// Load shader
			const GLchar* shaderSourceArr[] = {shaderSourceStr.c_str()};	// Convert to GLchar array

			// Compile
			glShaderSource(shader, 1, shaderSourceArr, NULL);
			glCompileShader(shader);
			
			// Error check
			GLint status;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if(status != GL_TRUE){
				GLint length;	// Length of info buffer
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);	// Get length

				std::string log(length, ' ');	// Create string
				glGetShaderInfoLog(shader, length, &length, &log[0]);	// Write into string
				throw std::logic_error(log);	// Throw logic error
				return 0;
			}

			return shader;
		}
};