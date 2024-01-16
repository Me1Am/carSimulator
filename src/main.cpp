#include "include/Car.hpp"
#include <iostream>

int main() {
	std::cout << "start" << std::endl;
	Car car(9075, 0.4257, 12.8, 2100, 1200, 2730, 2, true);

	int count = 0;
	while(true) {
		if(count >= 1000) break;
		car.setRPM(1201);
		std::cout << "Pos: " + std::to_string(car.position.x) + ", " + std::to_string(car.position.y) << std::endl;
		car.cycle(0.033);
		count++;
	}

	return 0;
}