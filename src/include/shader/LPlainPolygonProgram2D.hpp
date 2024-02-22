#include <iostream>

#include "LShaderProgram.hpp"
#include "../FileHandler.hpp"

class LPlainPolygonProgram2D : public LShaderProgram {
	public:
		/**
		 * @brief Deletes the shader program
		*/
		void freeProgram() {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &ebo);
			glDeleteProgram(programID);
		}
		/**
		 * @brief Loads the shader program
		 * @return a bool if loading worked or not 
		*/
		bool loadProgram() {
			GLint programSuccess = GL_TRUE;	// Success flag
			programID = glCreateProgram();	// Create program

			// Vertex Shader
			GLuint vertexShader = FileHandler::compileShader("../shaders/vertexShader.vert");
			glAttachShader(programID, vertexShader);	// Attach shader to the program

			// Fragment Shader
			GLuint fragmentShader = FileHandler::compileShader("../shaders/fragmentShader.frag");
			glAttachShader(programID, fragmentShader);
			
			glLinkProgram(programID);	// Link
			// Error check
			glGetProgramiv(programID, GL_LINK_STATUS, &programSuccess);
			if(programSuccess != GL_TRUE){
				std::cout << "Error linking program, program ID: " << programID << std::endl;
				printProgramLog(programID);
				return false;
			}

			// Cleanup
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			glClearColor(0.f, 0.f, 0.f, 0.f);	// Initialize clear color
			GLfloat vertexData[] = {	// VBO data
				0.5f,  0.5f, 0.0f,  // Top right
				0.5f, -0.5f, 0.0f,  // Bottom right
				-0.5f, -0.5f, 0.0f, // Bottom left
				-0.5f,  0.5f, 0.0f	// Top left 
			};
			GLuint indexData[] = { 
				0, 1, 3, 	// Triangle 1
				1, 2, 3		// Triangle 2
			};

			// Create objects
			glGenVertexArrays(1, &vao);	// Create VAO
			glGenBuffers(1, &vbo);		// Create VBO
			glGenBuffers(1, &ebo);		// Create VBO
			glBindVertexArray(vao);		// Bind VAO to capture calls

			// VBO
			glBindBuffer(GL_ARRAY_BUFFER, vbo);	// Use the vbo
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);	// Set data

			// EBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);	// Use the ebo
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);	// Set data

			/* Tell OpenGL how to interpret the data
			 * 	1: Which vertex attribute to configure, ie. its location
			 *  2: The size of the attribute(vec3 has 3 values)
			 *  3: The data type of the attribute(vec3 is a float)
			 *  4: If the data should be normalized
			 * 		For (int, byte) the integer data is normalized 
			 * 		to 0(or -1 for signed) and 1 when converted to float
			 *  5: The space between consecuative attributes(3 because vec3 is 3 floats)
			 *  6: The offset of where the position data begins in the buffer
			 */
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);	// Enable the attribute at index 0
			glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbind VBO

			return true;
		}
		/**
		 * @brief Sets a Vec4 uniform variable's value
		 * @param field The name of the variable
		 * @param value The first value of the Vec4
		 * @param value The second value of the Vec4
		 * @param value The third value of the Vec4
		 * @param value The fourth value of the Vec4
		 */
		void setVec4(const std::string &field, const float value1, const float value2, const float value3, const float value4) {
			glUniform4f(glGetUniformLocation(programID, field.c_str()), value1, value2, value3, value4);
		}
		/**
		 * @brief Gets the shader's VAO
		 * @return The GLuint representing the VAO
		*/
		GLuint getVAO() {
			return vao;
		}

		/**
		 * @brief Gets the shader's VBO
		 * @return The GLuint representing the VBO
		*/
		GLuint getVBO() {
			return vbo;
		}

		/**
		 * @brief Gets the shader's IBO
		 * @return The GLuint representing the IBO
		*/
		GLuint getEBO() {
			return ebo;
		}
	
	private:
		GLuint vao = 0;			// OpenGL vertex array object, stores vertex attrib calls
		GLuint vbo = 0;			// OpenGL vertex buffer object
		GLuint ebo = 0;			// OpenGL element buffer object, 
};