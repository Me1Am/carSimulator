#include <iostream>

#include "LShaderProgram.hpp"
#include "../FileHandler.hpp"

class LPlainPolygonProgram2D : public LShaderProgram {
	public:
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

			glClearColor(0.f, 0.f, 0.f, 0.f);	// Initialize clear color
			GLfloat vertexData[] = {	// VBO data
				-0.5, -0.5, 0.0, 
				0.5, -0.5, 0.0, 
				0.5, 0.5, 0.0, 
				-0.5, 0.5, 0.0
			};
			GLuint indexData[] = {0, 1, 2, 3};	// IBO data
						
			// Create VBO
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);	
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
						
			// Create IBO
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

			// Create quad
			vertex2DPos = glGetAttribLocation(programID, "LVertexPos2D");	// Get attribute to send it vertex data
			if(vertex2DPos == -1){
				std::cout << "LVertexPos2D is not a valid glsl program variable" << std::endl;
				return false;
			}

			return true;
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
		GLuint getIBO() {
			return ibo;
		}
	
		/**
		 * @brief Gets the shader's vertex position object
		 * @return The GLuint representing the 2D vertex position
		*/
		GLuint getVertex2DPos() {
			return vertex2DPos;
		}
	private:
		GLuint vbo = 0;			// OpenGL vertex buffer object
		GLuint ibo = 0;			// OpenGL index buffer object, order to draw VBOs
		GLint vertex2DPos = -1;	// 2D vertex position object
};