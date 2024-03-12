#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <cmath>

#include "include/shader/ShaderTexturedCube.hpp"
#include "include/shader/ShaderLightSource.hpp"
#include "include/Camera.hpp"

class Window {
	public:
		Window(const int width, const int height) {
			this->width = width;
			this->height = height;
		}
		~Window() {
			cube.freeProgram();
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
			// Enable VSync
			SDL_GL_SetSwapInterval(1);
			if(SDL_GL_GetSwapInterval() != 1){
				std::cout << "Warning: Unable to enable VSync, SDL_Error: " << SDL_GetError() << std::endl;
			}

			// Initialize OpenGL
			if(!initOpenGL()){
				std::cout << "Unable to initialize OpenGL" << std::endl;
				return false;
			}
			
			keyboard = SDL_GetKeyboardState(NULL);	// Get pointer to internal keyboard state

			paused = true;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_ShowCursor(SDL_TRUE);

			return true;	// Success
		}
		/// Initialize OpenGL components
		bool initOpenGL() {
			GLint status = GL_FALSE;

			glViewport(0, 0, width, height);
			glEnable(GL_DEPTH_TEST);

			// Load shaders
			if(!cube.loadProgram()){
				printf( "Unable to load cube shader!\n" );
				return false;
			}

			if(!lightSource.loadProgram()){
				printf( "Unable to load light shader!\n" );
				return false;
			}
			
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
										cube.freeProgram();
										SDL_DestroyWindow(window);

										// Push quit message
										event.type = SDL_QUIT;
										SDL_PushEvent(&event);
										break;
									case SDL_WINDOWEVENT_EXPOSED:
										//render();	// Render
										//SDL_GL_SwapWindow(window);	// Update
										break;
									case SDL_WINDOWEVENT_RESIZED:
										resize();
									default:
										break;
								}
							}
							break;
						} case SDL_KEYDOWN: {
							// Toggle mouse visibility and capture state with escape key
							if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
								paused = !paused;
								SDL_SetRelativeMouseMode((SDL_bool)(!paused));
								SDL_ShowCursor((SDL_bool)(paused));
							}
							break;
						} case SDL_MOUSEMOTION: {
							if(paused) break;
							float offsetX = event.motion.xrel;
							float offsetY = event.motion.yrel;

							// Adjust for sensitivity
							offsetX *= SENSITIVITY;
							offsetY *= SENSITIVITY;

							camera.incYaw(offsetX);
							camera.incPitch(-offsetY);
						} case SDL_MOUSEBUTTONDOWN: {
							mouseButtonState = SDL_GetMouseState(NULL, NULL);	// Get buttons

							switch(mouseButtonState) {
								case SDL_BUTTON(1):
									break;
								case SDL_BUTTON(2):
									break;
								case SDL_BUTTON(3):
									break;
							}
							break;

						} case SDL_MOUSEWHEEL: {
							if(paused) break;
							
							camera.incFOV(-event.wheel.y * 2.5f);
							break;
						} 
						case SDL_QUIT:	// Quit window
							SDL_Quit();	// Cleanup subsystems
							return;	// Exit loop
						default:
							break;
					}
				}
				/* Constant Logic */
				// Update roll
				if(!paused){
					if(keyboard[SDL_SCANCODE_E]){
						camera.incRoll(1.5f * deltaTime / 1000);	// Roll right(increase)
					} else if(keyboard[SDL_SCANCODE_Q]) {
						camera.incRoll(-1.5f * deltaTime / 1000);	// Roll left(decrease)
					}
				}

				render();	// Render

				// Framerate Handling
				Uint32 currentTime = SDL_GetTicks();
				deltaTime = currentTime - prevTime;
				if(SDL_GL_GetSwapInterval() != 1){
					SDL_Delay((deltaTime < MIN_FRAME_TIME) ? MIN_FRAME_TIME - deltaTime : 0);
					std::cout << "Frametime: " << deltaTime << 
								" | Limited Frametime: " << SDL_GetTicks() - prevTime << 
								" | Delay Time: " << ((deltaTime < MIN_FRAME_TIME) ? MIN_FRAME_TIME - deltaTime : 0) << std::endl;
				}
				prevTime = SDL_GetTicks();
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
			glClearColor(0.02f, 0.02f, 0.02f, 0.f);	// Set clear color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Only update the camera if its not paused
			if(!paused){
				camera.updateCameraPosition(	// Update camera position for view calculations
					keyboard[SDL_SCANCODE_W], 
					keyboard[SDL_SCANCODE_S], 
					keyboard[SDL_SCANCODE_A], 
					keyboard[SDL_SCANCODE_D], 
					keyboard[SDL_SCANCODE_SPACE], 
					keyboard[SDL_SCANCODE_LCTRL], 
					deltaTime
				);
				camera.updateCameraDirection();
			}

			glm::vec3 cubePositions[] = {
				glm::vec3( 0.0f,  0.0f,  0.0f),
				glm::vec3( 2.0f,  5.0f, -15.0f),
				glm::vec3(-1.5f, -2.2f, -2.5f),
				glm::vec3(-3.8f, -2.0f, -12.3f),
				glm::vec3( 2.4f, -0.4f, -3.5f),
				glm::vec3(-1.7f,  3.0f, -7.5f),
				glm::vec3( 1.3f, -2.0f, -2.5f),
				glm::vec3( 1.5f,  2.0f, -2.5f),
				glm::vec3( 1.5f,  0.2f, -1.5f),
				glm::vec3(-1.3f,  1.0f, -1.5f)
			};
			glm::vec3 pointLightPositions[] = {
				glm::vec3( 0.7f,  0.2f,  2.0f),
				glm::vec3( 2.3f, -3.3f, -4.0f),
				glm::vec3(-4.0f,  2.0f, -12.0f),
				glm::vec3( 0.0f,  0.0f, -3.0f)
			};

			// Light Source
			lightSource.bind();
			glBindVertexArray(lightSource.getVAO());

			lightSource.setFloat3("color", 0.494f, 0.493f, 0.425f);
			lightSource.setScale(0.2f, 0.2f, 0.2f);
			for(int i = 0; i < 10; i++) {
				glm::mat4 model = glm::mat4(1.f);
				model = glm::translate(model, pointLightPositions[i]);
				model = glm::rotate(model, glm::radians(0.f), glm::vec3(1.f, 0.3f, 0.5f));

				lightSource.setMat4("model", model);

				lightSource.setPos(pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
				lightSource.perspective(
					camera.calcCameraView(), 
					camera.getFOV()
				);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			// Cube
			cube.bind();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cube.getTexture(1));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, cube.getTexture(2));
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, cube.getTexture(3));

			cube.setVec3("cameraPos", camera.getPos());

			// Material Struct
			cube.setInt("material.baseTexture", 0);	// Base texture
			cube.setInt("material.specMap", 1);		// Specular Map
			cube.setInt("material.decal", 2);		// Decal
			cube.setFloat3("material.ambient", 0.1f, 0.1f, 0.1f);
			cube.setFloat("material.shininess", 32.f);
			cube.setFloat("material.decalBias", 0.f);	// Set to 0.f to remove it
			
			// Directional light
			cube.setFloat3("dirLight.direction", -0.2f, -1.f, -0.3f);
			cube.setFloat3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
			cube.setFloat3("dirLight.diffuse", 0.f, 0.f, 0.f);	// Set to zero to turn off
			cube.setFloat3("dirLight.specular", 1.f, 1.f, 1.f);

			// Point Light
			for(int i = 0; i < 4; i++) {
				cube.setVec3("pointLights["+std::to_string(i)+"].position", pointLightPositions[i]);
				cube.setFloat3("pointLights["+std::to_string(i)+"].ambient", 0.1f, 0.1f, 0.1f);
				cube.setFloat3("pointLights["+std::to_string(i)+"].diffuse", 0.15f, 0.15f, 0.15f);
				cube.setFloat3("pointLights["+std::to_string(i)+"].specular", 0.8f, 0.8f, 0.8f);
				cube.setFloat("pointLights["+std::to_string(i)+"].constant", 1.f);
				cube.setFloat("pointLights["+std::to_string(i)+"].linear", 0.09f);
				cube.setFloat("pointLights["+std::to_string(i)+"].quadratic", 0.032f);
			}

			// Spotlight
			//cube.setVec3("spotlights[0].position", camera.getPos());
			//cube.setVec3("spotlights[0].direction", camera.getDir());
			//cube.setFloat3("spotlights[0].ambient", 0.1f, 0.1f, 0.1f);
			//cube.setFloat3("spotlights[0].diffuse", 0.8f, 0.8f, 0.8f);
			//cube.setFloat3("spotlights[0].specular", 1.f, 1.f, 1.f);
			//cube.setFloat("spotlights[0].constant", 1.f);
			//cube.setFloat("spotlights[0].linear", 0.09f);
			//cube.setFloat("spotlights[0].quadratic", 0.032f);
			//cube.setFloat("spotlights[0].cutOff", glm::cos(glm::radians(12.5f)));
			//cube.setFloat("spotlights[0].outerCutOff", glm::cos(glm::radians(17.5f)));

			// Update cube
			//cube.setRotation(SDL_GetTicks()/1000.f, 0.5f, 1.f, 0.f);
			cube.setScale(0.5f, 0.5f, 0.5f);
			cube.setScale(5.f, 5.f, 5.f);
			cube.perspective(
				camera.calcCameraView(), 
				camera.getFOV()
			);

			glBindVertexArray(cube.getVAO());
			for(int i = 0; i < 10; i++) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions[i]);
				model = glm::rotate(model, glm::radians(20.f * i), glm::vec3(1.0f, 0.3f, 0.5f));
				
				cube.setMat4("model", model);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glUseProgram(0);	// Unbind
			
			glFlush();
			SDL_GL_SwapWindow(window);	// Update
			glFinish();
		}
	private:
		SDL_Window* window = NULL;		// The window
		SDL_Renderer* renderer = NULL;	// The renderer for the window, uses hardware acceleration
		SDL_GLContext gContext = NULL;	// The OpenGL context

		// Running Window Variables
		int width;			// The running drawable window width
		int height;			// The running drawable window width
		Uint32 deltaTime;	// The running time between frames
		Uint32 prevTime;	// The running time from init last frame was
		bool paused;

		// Running Mouse Variables
		Uint32 mouseButtonState;	// Mouse buttons state

		// Constants
		const Uint8* keyboard;						// The running state of the keyboard
		const float SENSITIVITY = 0.1f;				// Mouse sensitivity
		const float MIN_FRAME_TIME = 16.66666667;	// Minimum frame time in ms

		// Objects
		ShaderTexturedCube cube;
		ShaderLightSource lightSource;
		Camera camera;
};