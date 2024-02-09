#include "include/Car.hpp"
#include <iostream>
#include <Window.hpp>

int main() {
	std::cout << "start" << std::endl;

	Window* win = new Window(640, 480);
	win->mainLoop();
	delete win;
	
	std::cout << "end" <<std::endl;
	return 0;



	float wheelArgs[4][4] = {{25, 0.5, 1.0, true}, {25, 0.5, 1.0, true},
							 {25, 0.5, 1.0, false}, {25, 0.5, 1.0, false}};
	float constCar[] = {9075, 0.4, 1.5, 7.0, 5.5, 0.4257, 12.8, 4};
	float constEngine[] = {2100, 1200, 2730, 2, false, 0.80, 2.30};

	Car car(constCar, constEngine, wheelArgs);

	int count = 0;
	while(true) {
		if(count >= 1000) break;
		car.setRPM(1201);
		std::cout << "Pos: " + std::to_string(car.getPosition().x) + ", " + std::to_string(car.getPosition().y) << std::endl;
		
		count++;
	}

	return 0;
}