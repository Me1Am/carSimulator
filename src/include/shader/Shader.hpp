#pragma once

#include <GL/glew.h>
#include <GL/glu.h>

#include <iostream>
#include <string>

/**
 * @brief Shader program encapsulation class
*/
class Shader {
	public:
		/**
		 * @brief Default constructor
		 * @brief Sets programID to NULL
		*/
		Shader() {
			programID = NULL;
		}
		/**
		 * @brief Calls freeProgram()
		*/
		virtual ~Shader() {
			freeProgram();
		}
		/**
		 * @brief Loads the shader program
		 * @return a bool if loading worked or not 
		 * @note Purely virtual function, requires subclass implementation
		*/
		virtual bool loadProgram() = 0;
		/**
		 * @brief Loads the shader program
		 * @param vertPath A string representing the path to the vertex shader
		 * @param fragPath A string representing the path to the fragment shader
		 * @return a bool if loading worked or not 
		 * @note Purely virtual function, requires subclass implementation
		*/
		virtual bool loadProgram(const std::string vertPath, const std::string fragPath) = 0;
		/**
		 * @brief Deletes the shader program
		*/
		virtual void freeProgram() {
			glDeleteProgram(programID);
		}
		/**
		 * @brief Sets this shader as the current program
		 * @return a bool if the binding worked or not
		*/
		bool bind() {
			glUseProgram(programID);

			// Check for errors
			GLenum error = glGetError();
			if(error != GL_NO_ERROR) {
				std::cout << "Error binding shader: " << std::endl;
				printProgramLog( programID );
				return false;
			}

			return true;
		}
		/**
		 * @brief Unbinds the shader
		*/
		void unbind() {
			glUseProgram(0);	// Unbind
		}
		/**
		 * @brief Sets a boolean uniform variable's value
		 * @param field The name of the variable
		 * @param value The value to set
		 */
		void setBool(const std::string &field, const bool value) {
			glUniform1i(glGetUniformLocation(programID, field.c_str()), (int)value); 
		}
		/**
		 * @brief Sets an int uniform variable's value
		 * @param field The name of the variable
		 * @param value The value to set
		 */
		void setInt(const std::string &field, const int value) {
			glUniform1i(glGetUniformLocation(programID, field.c_str()), value); 
		}
		/**
		 * @brief Sets a float uniform variable's value
		 * @param field The name of the variable
		 * @param value The value to set
		 */
		void setFloat(const std::string &field, const float value) {
			glUniform1f(glGetUniformLocation(programID, field.c_str()), value);
		}
		/**
		 * @brief Gets the shader program's ID
		 * @return The GLuint representing the ID
		*/
		GLuint getProgramID() {
			return programID;
		}

	protected:
		/**
		 *  @brief Prints the log for the given shader
		 */
		void printShaderLog(const GLuint shader) {
			if(!glIsShader(shader)) return;	// Check if its a shader
			int infoLogLength = 0;
			int maxLength = 0;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);	// Get the info string length
			char* infoLog = new char[maxLength];	// Allocate
			
			glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);	// Get info log
			if(infoLogLength > 0) std::cout << infoLog << std::endl;
			
			delete[] infoLog;
		}
		/**
		 *  @brief Prints the log for the given program
		 */
		void printProgramLog(const GLuint program) {
			if(!glIsProgram(program)) return;	// Check if its a program
			int infoLogLength = 0;
			int maxLength = 0;
			
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);	// Get the info string length
			char* infoLog = new char[maxLength];	// Allocate
			
			glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);	// Get info log
			if(infoLogLength > 0) std::cout << infoLog << std::endl;
			
			delete[] infoLog;
		}

		// Program ID
		GLuint programID;
};
