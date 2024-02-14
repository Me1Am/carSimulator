#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <iostream>
#include <string>

#include "include/FileHandler.hpp"


class Window {
	public:
		Window(const int width, const int height) : SCREEN_WIDTH(width), SCREEN_HEIGHT(height) {}
		~Window() {
			//Destroy window
			SDL_DestroyWindow(window);

			//Quit SDL subsystems
			SDL_Quit();
		}

		/**
		 * @brief Initializes subsystems and creates the window and other resources
		 * @return A bool whether the creation was successful or not
		 */		
		bool create() {
			/* Initialize Subsystems */
			// Init Video and Events Subsystems
			if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0){
				std::cout << "Unable to initialize SDL_VIDEO or SDL_EVENTS, SDL_Error: " << SDL_GetError() << std::endl;
				return false;
			}
			
			// Use OpenGL 3.1 core
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			
			// Other OpenGL settings
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

			// Create window 
			window = SDL_CreateWindow(
				"Physics Engine", 
				SDL_WINDOWPOS_UNDEFINED, 
				SDL_WINDOWPOS_UNDEFINED, 
				SCREEN_WIDTH, 
				SCREEN_HEIGHT, 
				SDL_WINDOW_SHOWN |
				SDL_WINDOW_OPENGL
			);
			if(window == NULL){
				std::cout << "Unable to create window, SDL_Error: " << SDL_GetError() << std::endl;
				return false;
			}

			/* OpenGL and GLEW Initialization */
			// Create OpenGL context
			gContext = SDL_GL_CreateContext(window);
			if(gContext == NULL){
				std::cout << "Unable to create OpenGL context, SDL_Error: " << SDL_GetError() << std::endl;
				return false;
			}
			
			// Initialize Glew
			glewExperimental = GL_TRUE;
			GLenum glewStatus = glewInit();
			if (glewStatus != GLEW_OK){
				std::cout << "Unable to initialize GLEW, GLEW Error: " << glewGetErrorString(glewStatus) << std::endl;
				return false;
			}

			// TODO Creat program args function to enable vsync
			//// Enable VSync
			//SDL_GL_SetSwapInterval(1);
			//if(SDL_GL_GetSwapInterval() != 1){
			//	std::cout << "Warning: Unable to enable VSync, SDL_Error: " << SDL_GetError() << std::endl;
			//}

			// Initialize OpenGL
			if(!initOpenGL()){
				std::cout << "Unable to initialize OpenGL" << std::endl;
				return false;
			}

			return true;	// Success
		}
		/// Initialize OpenGL components
		bool initOpenGL() {
			GLint status = GL_FALSE;

			gProgramID = glCreateProgram();	// Create OpenGL shader program and get it's ID

			// Vertex Shader
			GLuint vertexShader = FileHandler::compileShader("../shaders/vertexShader.vert");
			glAttachShader(gProgramID, vertexShader);	// Attach shader to the program
			
			// Fragment Shader
			GLuint fragmentShader = FileHandler::compileShader("../shaders/fragmentShader.frag");
			glAttachShader(gProgramID, fragmentShader);

			glLinkProgram(gProgramID);	// Link the OpenGL program
			status = GL_TRUE;
			glGetProgramiv(gProgramID, GL_LINK_STATUS, &status);
			if(status != GL_TRUE){
				std::cout << "Unable to link OpenGL program, program ID: " << gProgramID << std::endl;
				printProgramLog(gProgramID);
				return false;
			}
			// Detach shaders when done linking
			glDetachShader(gProgramID, vertexShader);
			glDetachShader(gProgramID, fragmentShader);

			// Get attribute from the shader to send it vertex data
			gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
			if(gVertexPos2DLocation == -1){
				std::cout << "LVertexPos2D is not a valid glsl program variable" << std::endl;
				return false;
			}

			glClearColor(0.f, 0.f, 0.f, 0.f);	// Initialize clear color
			GLfloat vertexData[] = {	// VBO data
				-0.5, -0.5, 
				0.5, -0.5, 
				0.5, 0.5, 
				-0.5, 0.5, 
			};
			GLuint indexData[] = {0, 1, 2, 3};	// IBO data

			// Create VBO
			glGenBuffers(1, &gVBO);
			glBindBuffer(GL_ARRAY_BUFFER, gVBO);
			glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
			
			// Create IBO
			glGenBuffers( 1, &gIBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW );
			
			return true;
		}
		/// Prints the log for the given shader
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
		/// Prints the log for the given program
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
		/// Main loop
		void loop() {
			SDL_Event event;
			Uint32 windowID = SDL_GetWindowID(window);

			// Main Event Loop
			while(true) {
				while(SDL_PollEvent(&event)) {
					// Main Event Handler
					switch(event.type) {
						case SDL_WINDOWEVENT: {
							// Window event is for this window(useful if application uses more than one window)
							if(event.window.windowID == windowID){
								switch(event.window.event) {
									case SDL_WINDOWEVENT_CLOSE:	// Window receives close command
										// Destroy objects
										glDeleteProgram(gProgramID);
										SDL_DestroyWindow(window);

										// Push quit message
										event.type = SDL_QUIT;
										SDL_PushEvent(&event);
										break;
									case SDL_WINDOWEVENT_EXPOSED:
										break;
								}
							}
							break;
						}
						case SDL_QUIT:	// Quit window
							SDL_Quit();	// Cleanup subsystems
							return;	// Exit loop
						default:
							break;
					}
					
				}
				render();	// Render
				SDL_GL_SwapWindow(window);	// Update screen
			}
		}
		// Render
		void render() {
			glClear(GL_COLOR_BUFFER_BIT);	// Clear color buffer

			glUseProgram(gProgramID);	// Bind program
			glEnableVertexAttribArray(gVertexPos2DLocation);	// Enable vertex position
			// Set vertex data
			glBindBuffer(GL_ARRAY_BUFFER, gVBO);
			glVertexAttribPointer(gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );	// Set index data
			
			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);	// Draw
			glDisableVertexAttribArray( gVertexPos2DLocation );	// Disable vertex position
			
			glUseProgram(NULL);	// Unbind
		}

	private:
		SDL_Window* window = NULL;          // The window
		SDL_Renderer* renderer = NULL;		// The renderer for the window, uses hardware acceleration
		
		SDL_GLContext gContext = NULL;		// The OpenGL context

		SDL_Surface* lettuce_sur;
		SDL_Texture* lettuce_tex;

		GLuint gVBO = 0;					// OpenGL vertex buffer object
		GLuint gIBO = 0;					// OpenGL index buffer object, order to draw VBOs
		GLuint gProgramID = 0;				// ID of the OpenGL shader program
		GLint gVertexPos2DLocation = -1;	// 2D vertex position object

		// Constants
		const int SCREEN_WIDTH;
		const int SCREEN_HEIGHT;

};