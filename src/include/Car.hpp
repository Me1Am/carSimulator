#include "Vector2D.hpp"
#include <iostream>
#include <memory>
#include <cmath>
#include <array>

#define MAGNITUDE(v) (std::sqrt(v.x * v.x + v.y * v.y))

class Car {
    public:
        Car() {
			position = Vector2Df(0.f, 0.f);
		}
		/** Constructor 
		 * @param mass A float representing the mass of the car
		 * @param airResConst A float representing the air resistance constant
		 * @param rollResConst A float representing rolling resistance constant
		 * @param maxRPM An integer representing the max rpm of the engine
		 * @param minRPM An integer representing the minimum/idle rpm of the engine
		 * @param maxTorque An integer representing the max torque of the engine 
		 * @param reverseGears An integer representing the number of reverse gears
		 * @param hasHL A bool determening whether the engine has H-L gears
		 * @brief Constructor that sets all required variables
		 * @note 'hasHL' does not affect reverse gear count, H-L gears are sub gears
		 */
		Car(const float mass, const float wheelRadius, const float frontOffsetCG, const float rearOffsetCG, const float wheelBase, 
			const float groundOffsetCG, const float airResConst, const float rollResConst, const float wheelFrictCoef, 
			const int maxRPM, const int minRPM, const int maxTorque, const int reverseGears, const bool hasHL) {
			// Constant Car Variables
			this->mass = mass;
			this->wheelFrictCoef = wheelFrictCoef;
			this->frontOffsetCG = frontOffsetCG;
			this->rearOffsetCG = rearOffsetCG;
			this->wheelBase = wheelBase;
			this->groundOffsetCG = groundOffsetCG;
			this->wheelRadius = wheelRadius;
			this->airResConst = airResConst;
			this->rollResConst = rollResConst;

			// Constant Engine Variables
			this->maxRPM = maxRPM;
			this->minRPM = minRPM;
			this->maxTorque = maxTorque;
			this->reverseGears = reverseGears;
			this->hasHL = hasHL;
			this->transEff = 0.80f;
			this->diffRatio = 2.30f;

			position = Vector2Df(0.f, 0.f);
		}
		/**
		 * @brief Runs through one physics cycle
		 * @param deltaTime A float representing the time since last cycle
		 * @param airResConst A float representing the air resistance constant
		 * @param rollResConst A float representing the rolling resistance constant(30x 'airResConst')
		 * @param mass A float representing the total mass of the car
		 * @param wheelRadius A float representing the radius of the wheels, used in 'calcEngineForce'
		 * @param breakStatus A float representing the status of the breaks from 0-1 inclusive
		 */
		void runCycle(const float deltaTime, const float airResConst, const float rollResConst, const float mass, const float wheelRadius, const float breakStatus) {
			// Speed
			speed = MAGNITUDE(velocity);
			if(speed != speed) speed = 0;
			
			// Direction Vector
			Vector2Df directionVector = velocity / speed;	// The direction(unit) vector is found by dividing the vector by its magnitude
			if(directionVector.x != directionVector.x || directionVector.y != directionVector.y) directionVector = Vector2Df(0.f, 1.f);
			
			// Traction Force Vector
			Vector2Df traction = directionVector * calcEngineForce();	// Calculate traction, direction * engineforce
			if(traction.x != traction.x || traction.y != traction.y) traction = Vector2Df(directionVector.x * calcEngineForce(), directionVector.y * calcEngineForce());
			
			// Breaking Force Vector
			Vector2Df breaking;
			if(breakStatus >= 0.f) breaking = directionVector * -120.f;	// Calculate the breaking force, direction * break force
			else breaking = Vector2Df(0.f, 0.f);	// Car is not breaking, break force is 0
			if(breaking.x != breaking.x || breaking.y != breaking.y) breaking = Vector2Df(0.f, 0.f);

			// Air Resistance Force Vector
			Vector2Df drag = -airResConst * velocity * speed;	// Calculate the air resistance simply
			if(drag.x != drag.x || drag.y != drag.y) drag = Vector2Df(0.f, 0.f);
			
			// Rolling Resistance Force Vector
			Vector2Df rollDrag = -rollResConst * velocity;	// Calculate the rolling resistance(between wheel and ground)
			if(rollDrag.x != rollDrag.x || rollDrag.y != rollDrag.y) rollDrag = Vector2Df(0.f, 0.f);
			
			// Longitudual Force Vector
			Vector2Df longForce = traction + breaking + drag + rollDrag;	// Calculate the total longtitudinal force, when in a straight line at a constant speed this is zero
			
			// Acceleration Vector
			Vector2Df acceleration = Vector2Df(longForce.x / mass, longForce.y / mass);	// Calculate acceleration by the force on the car * its mass
			
			// Get maximum force on wheelsets
			std::array<float, 2> results = calcWheelMaxForce(acceleration);
			if(results[0] > MAGNITUDE(traction) || results[1] > MAGNITUDE(traction)){
				traction *= 0.75f;	// Wheels are spinning freely, decrease traction
				longForce = traction + breaking + drag + rollDrag;	// Update 'longForce'
				acceleration = Vector2Df(longForce.x / mass, longForce.y / mass); // Update 'acceleration'
			}

			// Integrate the acceleration to change velocity
			velocity += (deltaTime * acceleration);
			
			#ifdef DEBUG // Print out the calculations
				std::cout << (
					"RPM: " + std::to_string(rpm)
					+ "\nTorque: " + std::to_string(getTorque())
					+ "\nSpeed: " + std::to_string(speed)
					+ "\nEngine Force: " + std::to_string(calcEngineForce())
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
		}
		/** 
		 * @brief Calculates the force on each wheel
		 * @param acceleration A Vector2Df representing the acceleration of the car
		 * @return 
		 */
		std::array<float, 2> calcWheelMaxForce(const Vector2Df acceleration) {
			// Max friction
			float frontWeight;
			float rearWeight;

			// Weight distribution
			if(velocity.x == 0.f && velocity.y == 0.f){	// Stationary
				frontWeight = (rearOffsetCG / wheelBase) * mass * 0.98f;
				rearWeight = (frontOffsetCG / wheelBase) * mass * 0.98f;
			} else {	// Accelerating/Decelerating
				float accelerationMagnitude = MAGNITUDE(acceleration);
				frontWeight = ((rearOffsetCG / wheelBase) * mass * 0.98f) - ((groundOffsetCG * wheelBase) * mass * accelerationMagnitude);
				rearWeight = ((frontOffsetCG / wheelBase) * mass * 0.98f) + ((groundOffsetCG * wheelBase) * mass * accelerationMagnitude);
			}

			/* Calculate the maximum friction force for the wheel sets
			 * If the force being exerted on either wheelset is greater
			 * Then the wheels spinout, reducing traction and thus force
			 */
			float frontMaxFrict = wheelFrictCoef * frontWeight;
			float rearMaxFrict = wheelFrictCoef * rearWeight;
			return {frontMaxFrict, rearMaxFrict};
		}
		float getTorque() { return (rpm >= minRPM && rpm <= 1500.f) ? 2788.f : -rpm + 4745.f; }
		void setRPM(const float rpm) {
			this->rpm = rpm;
			if(this->rpm < minRPM) this->rpm = minRPM;
			else if(this->rpm > maxRPM) this->rpm = maxRPM;
		}
		/**
		 * @brief Calculates the force the engine is producing on the ground
		 * @return Returns the force the wheels are exerting
		*/
		float calcEngineForce() {
			float gearRatio = 15.76f;
			return getTorque() * gearRatio * diffRatio * transEff / wheelRadius;
		}
		Vector2Df position;
	private:
        // Car details
		float mass; 			// The mass of the car in kg
        float airResConst;		// The air resistance constant of the car
		float rollResConst;		// The rolling resistance constant of the car's wheels
		float wheelRadius;		// The radius of the wheels in m
		float wheelFrictCoef;	// The friction coefficient of the wheels
		float frontOffsetCG;	// The offset of the front wheels from the center of gravity
		float rearOffsetCG;		// The offset of the rear wheels from the center of gravity
		float wheelBase;		// The distance between the front and rear wheels
		float groundOffsetCG;	// The distance between the ground and the center of gravity
		
		// Engine Details
		int maxRPM;     // Maximum/Redline RPM
		int minRPM;     // Minimum/Idle RPM
		int maxTorque;  // Max torque in n*m
		
		// Transmission Details
		bool hasHL;			// If the transmission has sub-H/L gears
		int reverseGears;	// The number of reverse gears
		float transEff;		// The efficiency of the transmission
		float diffRatio;	// The differential's output ratio
		//float gears[];

		// Running Variables
		float rpm;					// The current rpm
		float speed;				// The current speed in m/s
		int currentGear;			// The current gear index
		bool rearWheelsSpinout;		// If the rear wheels are spun out
		bool frontWheelsSpinout;	// If the front wheels are spun out
		Vector2Df velocity;	// The current velocity
};