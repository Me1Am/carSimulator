#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <iostream>
#include <string>

class Window {
	public:
		Window(const int width, const int height) : SCREEN_WIDTH(width), SCREEN_HEIGHT(height) {
			if(SDL_Init(SDL_INIT_VIDEO) < 0){
				 std::cout << "Unable to initialize SDL, SDL_Error: %s\n" << SDL_GetError() << std::endl;
			} else {
				// Create window
				window = SDL_CreateWindow(
					"Physics Engine", 
					SDL_WINDOWPOS_UNDEFINED, 
					SDL_WINDOWPOS_UNDEFINED, 
					SCREEN_WIDTH, 
					SCREEN_HEIGHT, 
					SDL_WINDOW_SHOWN
				);
				// Error check
				if(window == NULL){
					std::cout << "Unable to create window, SDL_Error: %s\n" << SDL_GetError() << std::endl;
					return;
				}
				windowSurface = SDL_GetWindowSurface(window);	// Set window surface
			}
		}
		~Window() {
		//Destroy window
		SDL_DestroyWindow(window);

		//Quit SDL subsystems
		SDL_Quit();
	}
		void mainLoop() {
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
										// TODO Add code to cleanup and cleanly exit 

										// Push quit message
										event.type = SDL_QUIT;
										SDL_PushEvent(&event);
										break;
									case SDL_WINDOWEVENT_EXPOSED:
										// Draw
										SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface->format, 255, 255, 255));
										break;
								}
							}
							break;
						}
						case SDL_QUIT:	// Quit window
							return;	// Exit loop
							break;
						default:
							break;
					}
					SDL_UpdateWindowSurface(window);
				}
			}
		}
	private:
		SDL_Window* window = NULL;          // The render window
		SDL_Surface* windowSurface = NULL;  // The surface within the window
		
		// Constants
		const int SCREEN_WIDTH;
		const int SCREEN_HEIGHT;

};