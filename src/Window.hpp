#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <iostream>
#include <string>

//#include "include/FileHandler.hpp"
#include "include/shader/LPlainPolygonProgram2D.hpp"


class Window {
	public:
		Window(const int width, const int height) {
			this->width = width;
			this->height = height;
		}
		~Window() {
			quadProgram.freeProgram();
			SDL_DestroyWindow(window);		// Delete window

			SDL_Quit();	// Quit SDL
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
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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
				width, 
				height, 
				SDL_WINDOW_SHOWN |
				SDL_WINDOW_RESIZABLE |
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

			glViewport(0, 0, width, height);

			// Load basic shader program
			if(!quadProgram.loadProgram()){
				printf( "Unable to load basic shader!\n" );
				return false;
			}

			// Bind shader program
			quadProgram.bind();
			
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
			Uint32 nextTime;	// The tick that the next frame should take place on

			// Main Event Loop
			while(true) {				
				nextTime = SDL_GetTicks() + MIN_FRAME_TIME;

				while(SDL_PollEvent(&event)) {
					// Main Event Handler
					switch(event.type) {
						case SDL_WINDOWEVENT: {
							// Window event is for this window(useful if application uses more than one window)
							if(event.window.windowID == windowID){
								switch(event.window.event) {
									case SDL_WINDOWEVENT_CLOSE:	// Window receives close command
										// Destroy objects
										quadProgram.freeProgram();
										SDL_DestroyWindow(window);

										// Push quit message
										event.type = SDL_QUIT;
										SDL_PushEvent(&event);
										break;
									case SDL_WINDOWEVENT_EXPOSED:
										render();	// Render
										SDL_GL_SwapWindow(window);	// Update
										break;
									case SDL_WINDOWEVENT_RESIZED:
										resize();
									default:
										break;
								}
							}
							break;
						}
						case SDL_KEYDOWN:
							// Quit with escape key
							if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
								quadProgram.freeProgram();
								SDL_DestroyWindow(window);

								event.type = SDL_QUIT;
								SDL_PushEvent(&event);
							}
							break;
						case SDL_QUIT:	// Quit window
							SDL_Quit();	// Cleanup subsystems
							return;	// Exit loop
						default:
							break;
					}
					
				}

				Uint32 currentTime = SDL_GetTicks();
				SDL_Delay((nextTime <= currentTime) ? 0 : nextTime - currentTime);
				std::cout << "frametime: " << nextTime-currentTime << std::endl;
        		nextTime += MIN_FRAME_TIME;
			}
		}
		/// Resize Window
		// TODO Implement real resizing/keep ratio of drawable items
		void resize() {
			SDL_GL_GetDrawableSize(window, &width, &height);	// Set 'width' and 'height'

			glViewport(0, 0, width, height);	// Update OpenGL viewport
		}
		/// Render
		void render() {
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// Set clear color
			glClear(GL_COLOR_BUFFER_BIT);			// Clear window with the color given by glClearColor()

			glUseProgram(quadProgram.getProgramID());
			glBindVertexArray(quadProgram.getVAO());	// Use the VAO which sets up the VBO
			
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Wireframe
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// Draw
			
			glUseProgram(0);	// Unbind
		}

	private:
		SDL_Window* window = NULL;		// The window
		SDL_Renderer* renderer = NULL;	// The renderer for the window, uses hardware acceleration
		SDL_GLContext gContext = NULL;	// The OpenGL context

		int width;	// The running drawable window width
		int height;	// The running drawable window width
		
		const float MIN_FRAME_TIME = 16.66666667;	// Minimum frame time in ms

		LPlainPolygonProgram2D quadProgram;
};