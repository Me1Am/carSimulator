#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.hpp"
#include "../FileHandler.hpp"

/**
 * @brief Shader program which holds a textured quad
 * @extends Shader
*/
class ShaderTexturedQuad : public Shader {
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
			GLuint vertexShader = FileHandler::compileShader("../shaders/texture.vert");
			glAttachShader(programID, vertexShader);	// Attach shader to the program

			// Fragment Shader
			GLuint fragmentShader = FileHandler::compileShader("../shaders/texture.frag");
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
				0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,	// Top right
				0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,	// Bottom right
				-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,	// Bottom left
				-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f 	// Top left 
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

			// Set vertex position
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
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);	// Enable the attribute at index 0
			// Set color
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);	// Enable the attribute at index 1
			// Set texture position
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);	// Enable the attribute at index 1

			// Texture 1
			glGenTextures(1, &texture1);	// Create one texture and assign to 'texture'
			glBindTexture(GL_TEXTURE_2D, texture1);	// Bind

			// Texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);			// Map to X(S)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);			// Map to Y(T)
			// Filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);				// Nearest pixel match for downscale
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				// Bileanar for upscale
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear mipmap for downscale

			if(!FileHandler::loadImage("../assets/wall.jpg", GL_RGB)){
				std::cout << "Failed to create texture" << std::endl;
				return false;
			}

			// Texture 2
			glGenTextures(1, &texture2);	// Create one texture and assign to 'texture'
			glBindTexture(GL_TEXTURE_2D, texture2);	// Bind

			// Texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);			// Map to X(S)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);			// Map to Y(T)
			// Filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);				// Nearest pixel match for downscale
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				// Bileanar for upscale
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear mipmap for downscale

			if(!FileHandler::loadImage("../assets/awesomeface.png", GL_RGBA)){
				std::cout << "Failed to create texture" << std::endl;
				return false;
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbind VBO

			return true;
		}
		/**
		 * @brief Rotates the object by the given degrees, on the given axis
		 * @param degrees Float representing the amount to rotate by
		 * @param xAxis Float representing the x axis value for applying the rotation
		 * @param yAxis Float representing the y axis value for applying the rotation
		 * @param zAxis Float representing the z axis value for applying the rotation
		 */
		void rotate(const float degrees, const float xAxis, const float yAxis, const float zAxis) {
			glm::mat4 matrix = glm::mat4(1.0f);
			matrix = glm::rotate(matrix, glm::radians(degrees), glm::vec3(xAxis, yAxis, zAxis));

			// Apply rotation
			glUniformMatrix4fv(glGetUniformLocation(programID, "rotate"), 1, GL_FALSE, glm::value_ptr(matrix));	
		}
		/**
		 * @brief Scales the object by the given values
		 * @param xScale Float representing the x scale factor
		 * @param yScale Float representing the y scale factor
		 * @param zScale Float representing the z scale factor
		 */
		void scale(const float xScale, const float yScale, const float zScale) {
			glm::mat4 matrix = glm::mat4(1.0f);
			matrix = glm::scale(matrix, glm::vec3(xScale, yScale, zScale));

			// Apply scale
			glUniformMatrix4fv(glGetUniformLocation(programID, "scale"), 1, GL_FALSE, glm::value_ptr(matrix));
		}
		/**
		 * @brief Applies the appropriate transforms to show perspective or not
		 * @param hasPerspective A bool representing whether the quad should be given perspective
		*/
		void perspective(const bool hasPerspective) {
			glm::mat4 model 		= glm::mat4(1.f);
			glm::mat4 view 			= glm::mat4(1.f);
			glm::mat4 projection 	= glm::mat4(1.f);

			if(hasPerspective){
				model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
				projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
			}

			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
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
		/**
		 * @brief Gets the shader's texture
		 * @param textureIndex The texture number
		 * @return The GLuint representing the texture
		 * @note If the given index is invalid it returns 0
		*/
		GLuint getTexture(const int textureIndex) {
			switch(textureIndex) {
				case 1:
					return texture1;
				case 2:
					return texture2;
				default:
					return 0;
			}
		}

	private:
		GLuint vao = 0;		// OpenGL vertex array object, stores vertex attrib calls
		GLuint vbo = 0;		// OpenGL vertex buffer object
		GLuint ebo = 0;		// OpenGL element buffer object, 
		GLuint texture1;	// OpenGL texture object
		GLuint texture2;	// OpenGL texture object
};