#include "Vector2D.hpp"
#include <array>
#include <cmath>
#include <iostream>

class Engine {
	public:
		Engine() {}
		/** Constructor 
		 * @param maxRPM An integer representing the max rpm of the engine
		 * @param minRPM An integer representing the minimum/idle rpm of the engine
		 * @param maxTorque An integer representing the max torque of the engine 
		 * @param reverseGears An integer representing the number of reverse gears
		 * @param hasHL A bool determening whether the engine has H-L gears
		 * @brief Constructor that sets all required variables
		 * @note 'hasHL' does not affect reverse gear count, H-L gears are sub gears
		 */
		Engine(const int maxRPM, const int minRPM, const int maxTorque, const int reverseGears, const bool hasHL) {
			this->maxRPM = maxRPM;
			this->minRPM = minRPM;
			this->maxTorque = maxTorque;
			this->reverseGears = reverseGears;
			this->hasHL = hasHL;
		}
		float getTorque(const float rpm) { return (rpm > minRPM && rpm < 1500.f) ? 2788.f : -rpm + 4745.f; }
		void setRPM(const float rpm) {
			this->rpm = rpm;
			if(this->rpm < minRPM) this->rpm = minRPM;
			else if(this->rpm > maxRPM) this->rpm = maxRPM;
		}
		Vector2Df runCycle(const float deltaTime, const float airResConst, const float rollResConst, const float mass, const bool isBreaking) {
			// Speed
			speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
			if(speed != speed) speed = 0;
			
			// Direction Vector
			Vector2Df directionVector = velocity / speed;	// The direction(unit) vector is found by dividing the vector by its magnitude
			if(directionVector.x != directionVector.x || directionVector.y != directionVector.y) directionVector = Vector2Df(0.f, 1.f);
			
			// Traction Force Vector
			Vector2Df traction = directionVector * getTorque(rpm);	// Calculate traction, direction * engineforce(torque)
			if(traction.x != traction.x || traction.y != traction.y) traction = Vector2Df(directionVector.x * getTorque(rpm), directionVector.y * getTorque(rpm));
			
			// Breaking Force Vector
			Vector2Df breaking;
			if(isBreaking) breaking = directionVector * -120.f;	// Calculate the breaking force, direction * break force
			else breaking = Vector2Df(0.f, 0.f);	// Car is not breaking, break force is 0

			if(breaking.x != breaking.x || breaking.y != breaking.y) std::cerr << "Breaking force is NaN" << std::endl;

			// Air Resistance Force Vector
			Vector2Df drag = -airResConst * velocity * speed;	// Calculate the air resistance simply
			if(drag.x != drag.x || drag.y != drag.y) drag = Vector2Df(0.f, 0.f);
			
			// Rolling Resistance Force Vector
			Vector2Df rollDrag = -rollResConst * velocity;	// Calculate the rolling resistance(between wheel and ground)
			if(rollDrag.x != rollDrag.x || rollDrag.y != rollDrag.y) rollDrag = Vector2Df(0.f, 0.f);
			
			// Longitudual Force Vector
			Vector2Df longForce = traction + breaking + drag + rollDrag;	// Calculate the total longtitudinal force, when in a straight line at a constant speed this is zero
			
			// Acceleration Vector
			Vector2Df acceleration =  Vector2Df(longForce.x / mass, longForce.y / mass);	// Calculate acceleration by the force on the car * its mass
			
			// Integrate the acceleration to change velocity
			velocity += (deltaTime * acceleration);
			#ifdef DEBUG
				std::cout << (
					"RPM: " + std::to_string(rpm)
					+ "\nTorque: " + std::to_string(getTorque(rpm))
					+ "\nSpeed: " + std::to_string(speed)
					+ "\nDirection Vector: " + std::to_string(directionVector.x) + ", " + std::to_string(directionVector.y)
					+ "\nTraction: " + std::to_string(traction.x) + ", " + std::to_string(traction.y)
					+ "\nAir Resistance: " + std::to_string(drag.x) + ", " + std::to_string(drag.y)
					+ "\nRolling Resistance: " + std::to_string(rollDrag.x) + ", " + std::to_string(rollDrag.y)
					+ "\nLongtitudinal Force: " + std::to_string(longForce.x) + ", " + std::to_string(longForce.y)
					+ "\nAcceleration: " + std::to_string(acceleration.x) + ", " + std::to_string(acceleration.y)
					+ "\nVelocity: " + std::to_string(velocity.x) + ", " + std::to_string(velocity.y)
					+ "\n"
				) << std::endl;
			#endif
			return deltaTime * velocity;
		}
	private:
		// Engine Details
		int maxRPM;     // Redline RPM
		int minRPM;     // Idle RPM
		int maxTorque;  // Max torque in n*m
		
		// Transmission Details
		bool hasHL;
		int reverseGears;
		//float gears[];

		// Running Variables
		float rpm;			// The current rpm
		float speed;		// The current speed in m/s
		int currentGear;	// The current gear index
		Vector2Df velocity;	// The current velocity
};
