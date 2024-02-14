#include <iostream>
#include <fstream>
#include <string>

class FileHandler {
    public:
        /** Load shader file from a given path
		 * @param path The path to the shader file
		 * @return A null-terminated string representing the source
		 * @note .vert - a vertex shader
		 * @note .tesc - a tessellation control shader
		 * @note .tese - a tessellation evaluation shader
		 * @note .geom - a geometry shader
		 * @note .frag - a fragment shader
		 * @note .comp - a compute shader
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

};