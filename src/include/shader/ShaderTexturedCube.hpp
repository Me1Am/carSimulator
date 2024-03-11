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
class ShaderTexturedCube : public Shader {
	public:
		/**
		 * @brief Deletes the shader program
		*/
		void freeProgram() {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
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
			GLfloat vertexData[] = {
				// Vertex Positions    // Normal Verts    // Texture
				-0.5f, -0.5f, -0.5f,   0.f,  0.f, -1.f,   0.f, 0.f,
				 0.5f, -0.5f, -0.5f,   0.f,  0.f, -1.f,   1.f, 0.f,
				 0.5f,  0.5f, -0.5f,   0.f,  0.f, -1.f,   1.f, 1.f,
				 0.5f,  0.5f, -0.5f,   0.f,  0.f, -1.f,   1.f, 1.f,
				-0.5f,  0.5f, -0.5f,   0.f,  0.f, -1.f,   0.f, 1.f,
				-0.5f, -0.5f, -0.5f,   0.f,  0.f, -1.f,   0.f, 0.f,

				-0.5f, -0.5f,  0.5f,   0.f,  0.f,  1.f,   0.f, 0.f,
				 0.5f, -0.5f,  0.5f,   0.f,  0.f,  1.f,   1.f, 0.f,
				 0.5f,  0.5f,  0.5f,   0.f,  0.f,  1.f,   1.f, 1.f,
				 0.5f,  0.5f,  0.5f,   0.f,  0.f,  1.f,   1.f, 1.f,
				-0.5f,  0.5f,  0.5f,   0.f,  0.f,  1.f,   0.f, 1.f,
				-0.5f, -0.5f,  0.5f,   0.f,  0.f,  1.f,   0.f, 0.f,

				-0.5f,  0.5f,  0.5f,  -1.f,  0.f,  0.f,   1.f, 0.f,
				-0.5f,  0.5f, -0.5f,  -1.f,  0.f,  0.f,   1.f, 1.f,
				-0.5f, -0.5f, -0.5f,  -1.f,  0.f,  0.f,   0.f, 1.f,
				-0.5f, -0.5f, -0.5f,  -1.f,  0.f,  0.f,   0.f, 1.f,
				-0.5f, -0.5f,  0.5f,  -1.f,  0.f,  0.f,   0.f, 0.f,
				-0.5f,  0.5f,  0.5f,  -1.f,  0.f,  0.f,   1.f, 0.f,

				 0.5f,  0.5f,  0.5f,   1.f,  0.f,  0.f,   1.f, 0.f,
				 0.5f,  0.5f, -0.5f,   1.f,  0.f,  0.f,   1.f, 1.f,
				 0.5f, -0.5f, -0.5f,   1.f,  0.f,  0.f,   0.f, 1.f,
				 0.5f, -0.5f, -0.5f,   1.f,  0.f,  0.f,   0.f, 1.f,
				 0.5f, -0.5f,  0.5f,   1.f,  0.f,  0.f,   0.f, 0.f,
				 0.5f,  0.5f,  0.5f,   1.f,  0.f,  0.f,   1.f, 0.f,

				-0.5f, -0.5f, -0.5f,   0.f, -1.f,  0.f,   0.f, 1.f,
				 0.5f, -0.5f, -0.5f,   0.f, -1.f,  0.f,   1.f, 1.f,
				 0.5f, -0.5f,  0.5f,   0.f, -1.f,  0.f,   1.f, 0.f,
				 0.5f, -0.5f,  0.5f,   0.f, -1.f,  0.f,   1.f, 0.f,
				-0.5f, -0.5f,  0.5f,   0.f, -1.f,  0.f,   0.f, 0.f,
				-0.5f, -0.5f, -0.5f,   0.f, -1.f,  0.f,   0.f, 1.f,

				-0.5f,  0.5f, -0.5f,   0.f,  1.f,  0.f,   0.f, 1.f,
				 0.5f,  0.5f, -0.5f,   0.f,  1.f,  0.f,   1.f, 1.f,
				 0.5f,  0.5f,  0.5f,   0.f,  1.f,  0.f,   1.f, 0.f,
				 0.5f,  0.5f,  0.5f,   0.f,  1.f,  0.f,   1.f, 0.f,
				-0.5f,  0.5f,  0.5f,   0.f,  1.f,  0.f,   0.f, 0.f,
				-0.5f,  0.5f, -0.5f,   0.f,  1.f,  0.f,   0.f, 1.f
			};

			// Create objects
			glGenVertexArrays(1, &vao);	// Create VAO
			glGenBuffers(1, &vbo);		// Create VBO
			glBindVertexArray(vao);		// Bind VAO to capture calls

			// VBO
			glBindBuffer(GL_ARRAY_BUFFER, vbo);	// Use the vbo
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);	// Set data

			// Set vertex position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);	// Enable the attribute at index 0
			// Set normals
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);	// Enable the attribute at index 1
			// Set texture position
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);	// Enable the attribute at index 1

			// Base Texture
			glGenTextures(1, &baseTexture);	// Create one texture and assign to 'texture'
			glBindTexture(GL_TEXTURE_2D, baseTexture);	// Bind

			if(!FileHandler::loadImage("../assets/container.png", GL_RGBA)){
				std::cout << "Failed to create base texture" << std::endl;
				return false;
			}

			// Specular Map
			glGenTextures(1, &specMap);
			glBindTexture(GL_TEXTURE_2D, specMap);

			if(!FileHandler::loadImage("../assets/containerSpecMap.png", GL_RGBA)){
				std::cout << "Failed to create specular map" << std::endl;
				return false;
			}

			// Decal
			glGenTextures(1, &decal);
			glBindTexture(GL_TEXTURE_2D, decal);

			if(!FileHandler::loadImage("../assets/awesomeface.png", GL_RGBA)){
				std::cout << "Failed to create decal" << std::endl;
				return false;
			}

			return true;
		}
		/**
		 * @brief Applies the appropriate transforms to show perspective or not
		 * @param hasPerspective A bool representing whether the quad should be given perspective
		*/
		void perspective(const glm::mat4 cameraView, const float fov) {
			glm::mat4 model = glm::mat4(1.f);
			glm::mat4 view 	= glm::mat4(1.f);
			glm::mat4 projection = glm::mat4(1.f);

			model = glm::translate(model, pos);						// Set position
			model = glm::scale(model, scale);						// Set scale
			model = glm::rotate(model, rotationDeg, rotationAxis);	// Set rotation
			//model = glm::rotate(model, glm::radians(-55.f), glm::vec3(1.f, 0.3f, 0.5f));
			projection = glm::perspective(glm::radians(fov), 640.f / 480.f, 0.1f, 100.f);
			view = cameraView;

			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		}
		/**
		 * @brief Sets the rotation of the object by the given degrees, on the given axis
		 * @param degrees Float representing the amount to rotate by
		 * @param xAxis Float representing the x axis value for applying the rotation
		 * @param yAxis Float representing the y axis value for applying the rotation
		 * @param zAxis Float representing the z axis value for applying the rotation
		 */
		void setRotation(const float degrees, const float xAxis, const float yAxis, const float zAxis) {
			rotationDeg = degrees;
			rotationAxis = glm::vec3(xAxis, yAxis, zAxis);	
		}
		/**
		 * @brief Sets the scale ratio for the object
		 * @param xScale Float representing the x scale factor
		 * @param yScale Float representing the y scale factor
		 * @param zScale Float representing the z scale factor
		 */
		void setScale(const float xScale, const float yScale, const float zScale) {
			scale = glm::vec3(xScale, yScale, zScale);
		}
		/**
		 * @brief Sets a Vec4 uniform variable's value
		 * @param field The name of the variable
		 * @param value1 The first value of the Vec4
		 * @param value2 The second value of the Vec4
		 * @param value3 The third value of the Vec4
		 * @param value4 The fourth value of the Vec4
		 */
		void setFloat4(const std::string &field, const float value1, const float value2, const float value3, const float value4) {
			glUniform4f(glGetUniformLocation(programID, field.c_str()), value1, value2, value3, value4);
		}
		/**
		 * @brief Sets a Vec3 uniform variable's value
		 * @param field The name of the variable
		 * @param value1 The first value of the Vec3
		 * @param value2 The second value of the Vec3
		 * @param value3 The third value of the Vec3
		 */
		void setFloat3(const std::string &field, const float value1, const float value2, const float value3) {
			glUniform3f(glGetUniformLocation(programID, field.c_str()), value1, value2, value3);
		}
		/**
		 * @brief Sets a Vec3 uniform variable's value
		 * @param field The name of the variable
		 * @param value1 The first value of the Vec3
		 * @param value2 The second value of the Vec3
		 * @param value3 The third value of the Vec3
		 */
		void setFloat3(const std::string &field, const glm::vec3 &vec3) {
			glUniform3f(glGetUniformLocation(programID, field.c_str()), vec3.x, vec3.y, vec3.z);
		}
		/**
		 * @brief Gets the position of the cube
		 * @return The GLuint representing the position
		*/
		glm::vec3 getPos() {
			return pos;
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
		 * @brief Gets the shader's texture
		 * @param textureIndex The texture number
		 * @return The GLuint representing the texture
		 * @note If the given index is invalid it returns 0
		*/
		GLuint getTexture(const int textureIndex) {
			switch(textureIndex) {
				case 1:
					return baseTexture;
				case 2:
					return specMap;
				case 3:
					return decal;
				default:
					return 0;
			}
		}

	private:
		GLuint vao = 0;		// OpenGL vertex array object, stores vertex attrib calls
		GLuint vbo = 0;		// OpenGL vertex buffer object
		GLuint baseTexture;	// Base texture
		GLuint specMap;		// Specular map for base texture
		GLuint decal;		// Decal texture(applied over base)

		GLfloat rotationDeg = 0.f;

		glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 rotationAxis = glm::vec3(0.f, 0.f, 0.f);
};