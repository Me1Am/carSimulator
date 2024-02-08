#include "Vector2D.hpp"
#include <array>
#include <cmath>
#include <iostream>
#include <memory>

#define MAGNITUDE(v) (std::sqrt(v.x * v.x + v.y * v.y))
#define IS_NAN_VECT(v) (v.x != v.x || v.y != v.y)
#define IS_NAN(v) (v != v)

/** 
 * @brief A struct representing a car wheel
 * @note Holds constant varibles for radius, friction coefficient, and if its powered
 * @note Holds running variables for angular velocity and acceleration
 */
struct wheel {
	// Constants
	float mass;			// The mass of the wheel
	float radius;		// The radius of the wheel in meters
	float frictionCoef;	// The friction coeficient of the wheel
	float inertia;		// The inertia of the wheels on the same axle
	bool powered;		// Whether the wheel is powered by the engine or not
	
	// Running 
	float rpm = 0.f;				// Current rpm of the wheel
	float angVelocity = 0.f;		// Current angular velocity of the wheel
	float angAcceleration = 0.f;	// Current angular acceleration of the wheel
};

class Car {
	public:
		Car() : B({10.f, 12.f, 5.f, 4.f}), C({1.9, 2.3, 2.f, 2.f}), D({1.f, 0.82, 0.3, 0.1}), E({0.97, 1.f, 1.f, 1.f}) {
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
		Car(const float carConstants[], const float engineConstants[], const float wheelArgs[][4]) : 
			B({10.f, 12.f, 5.f, 4.f}), C({1.9, 2.3, 2.f, 2.f}), D({1.f, 0.82, 0.3, 0.1}), E({0.97, 1.f, 1.f, 1.f}) {
			// Constant Car Variables
			this->mass = carConstants[0];
			this->frontOffsetCG = carConstants[1];
			this->rearOffsetCG = carConstants[2];
			this->groundOffsetCG = carConstants[3];
			this->wheelBase = carConstants[4];
			this->airResConst = carConstants[5];
			this->rollResConst = carConstants[6];
			this->wheelCount = carConstants[7];

			// Constant Engine Variables
			this->maxRPM = engineConstants[0];
			this->minRPM = engineConstants[1];
			this->maxTorque = engineConstants[2];
			this->reverseGears = engineConstants[3];
			this->hasHL = (bool)engineConstants[4];
			this->transEff = engineConstants[5];
			this->diffRatio = engineConstants[6];

			// Setup wheels
			wheelArr = new wheel[wheelCount];
			for(int i = 0; i < wheelCount; i++) {
				wheelArr[i].mass = wheelArgs[i][0];			// Set mass
				wheelArr[i].radius = wheelArgs[i][1];		// Set radius
				wheelArr[i].frictionCoef = wheelArgs[i][2];	// Set friction coefficient
				wheelArr[i].powered = wheelArgs[i][3];		// Set if powered
				wheelArr[i].inertia = 2 * (wheelArgs[i][0] * (wheelArgs[i][1] * wheelArgs[i][1]) / 2);	// Multiplied by the wheels on the same axle(2)
			}

			// Set Running Variables
			rpm = 0.f;
			speed = 0.f;
			currentGear = 0;
			velocity = Vector2Df();
			position = Vector2Df();
			acceleration = Vector2Df();

		}
		~Car() { delete[] wheelArr; }
		/**
		 * @brief Runs through one physics cycle
		 * @param brakeStatus A float representing the status of the brakes from 0-1 inclusive
		 * @param deltaTime A float representing the time since last cycle
		 */
		void runCycle(const float brakeStatus, const float deltaTime) {
			// Speed
			speed = MAGNITUDE(velocity);
			if(IS_NAN(speed)) speed = 0;

			// Direction Vector
			Vector2Df directionVector = velocity / speed;	// The direction(unit) vector is found by dividing the vector by its magnitude
			if(IS_NAN_VECT(directionVector))
				directionVector = Vector2Df(0.f, 1.f);

			// Air Resistance Force Vector
			Vector2Df drag = -airResConst * velocity * speed;	// Calculate the air resistance simply
			if(IS_NAN_VECT(drag)) drag = Vector2Df(0.f, 0.f);

			// Rolling Resistance Force Vector
			Vector2Df rollDrag = -rollResConst * velocity;	// Calculate the rolling resistance(between wheel and ground)
			if(IS_NAN_VECT(rollDrag)) rollDrag = Vector2Df(0.f, 0.f);

			// Braking Torque
			// TODO Remove brake constant and calculate torque instead
			float brakeTorque;
			if(brakeStatus >= 0.f) brakeTorque = brakeStatus * -10.f;
			else brakeTorque = 0.f;	// Car is not braking, brake torque is 0
			if(IS_NAN(brakeTorque)) brakeTorque = 0.f;

			// Braking Force
			// TODO Add brake force calculation
			Vector2Df brakeForce = directionVector * brakeTorque;
			if(IS_NAN_VECT(brakeForce)) brakeForce = Vector2Df(0.f, 0.f);
			
			// TODO Add road condition selection
			roadCondition = 1;	// TEMP

			// TODO Check and alter traction forces on car to be accurate to the real world
			float tractionForce;
			for(int i = 0; i < wheelCount; i++) {
				tractionForce += calcWheelTractionForce(&wheelArr[i], brakeTorque, deltaTime);
			}

			// Net Longitudual Force Vector
			Vector2Df longForce = (directionVector * tractionForce) + brakeForce + drag + rollDrag;	// Calculate the net longtitudinal force on the car

			// Acceleration Vector
			acceleration = Vector2Df(longForce.x / mass, longForce.y / mass);	// Calculate acceleration

			// Integrate the acceleration to change velocity
			velocity += (deltaTime * acceleration);

			#ifdef DEBUG	// Print out the calculations
				std::cout << (
					"RPM: " + std::to_string(rpm) + 
					"\nEngine Torque: " + std::to_string(getTorque()) +
					"\nEngine Torque(on wheel): " + std::to_string(calcEngineTorque()) +
					"\nSpeed: " + std::to_string(speed) + 
					"\nDirection Vector: " + std::to_string(directionVector.x) + ", " + std::to_string(directionVector.y) +
					"\nNet Traction Force: " + std::to_string(tractionForce) +
					"\nAir Resistance: " + std::to_string(drag.x) + ", " + std::to_string(drag.y) +
					"\nRolling Resistance: " + std::to_string(rollDrag.x) + ", " + std::to_string(rollDrag.y) +
					"\nLongtitudinal Force: " + std::to_string(longForce.x) + ", " + std::to_string(longForce.y) +
					"\nAcceleration: " + std::to_string(acceleration.x) + ", " + std::to_string(acceleration.y) +
					"\nVelocity: " + std::to_string(velocity.x) + ", " + std::to_string(velocity.y) + "\n"
				) << std::endl;
			#endif
		}

		/**
		 * @brief Calculates a given wheels traction force
		 * @param wheel A wheel struct to be worked on
		 * @param brakeTorque A float representing the brake torque on the wheel
		 * @param deltaTime A float representing the time between last frame
		 * @return A float representing the wheel's traction force
		 */
		float calcWheelTractionForce(wheel* wheel, const float brakeTorque, const float deltaTime) {
			float driveTorque = calcEngineTorque();	// Get torque the engine is applying to the wheels
			
			float tractionForce;		// Force the wheel creates which pushes the car
			float tractionTorque;		// Torque opposing the wheel, the sum of the wheels on the same axle
			float netTorque;			// The net amount of torque on the wheel
			float slip;					// The ratio percent between wheel speed to car speed
			float fz;					// The vertical load on the wheels, weight distribution of car chassis

			wheel->angVelocity += wheel->angAcceleration * deltaTime;	// Update wheel velocity from previous frame's acceleration
			slip = ((wheel->angVelocity * wheel->radius) - speed) / abs(speed) * 100;	// Calculate the wheel slip ratio percent
			fz = calcWheelLoad(acceleration, false);	// Get the current vertical load on the wheel

			// Calculate longitudinal force using the Simplified Pacejka Magic Formula
			// F = Fz * D * sin(C * arctan(B * slip - E * (B * slip - arctan(B * slip))))
			tractionForce = 
				fz * D[roadCondition] * sin(C[roadCondition] * atan(
					B[roadCondition] * slip - E[roadCondition] * (B[roadCondition] * slip - atan(B[roadCondition] * slip))));

			// TODO Get traction torqure from the other wheel on the axel(if any) instead of multiplying by two
			tractionTorque = 2 * tractionForce * wheel->radius;	// Get opposing torque from wheel force

			// Update car variables
			float gearRatio = 15.76f;	// Temp
			setRPM(wheel->angVelocity * gearRatio * diffRatio * 60 / 2 * M_PI);	// Calculate and set new engine rpm

			// Update variabels for next frame
			netTorque = driveTorque - tractionTorque - brakeTorque;		// Update total torque on the wheel
			wheel->angAcceleration = netTorque / wheel->inertia;			// Update wheel acceleration

			return tractionForce;
		}
		/**
		 * @brief Calculates the vertical load on each wheel
		 * @param acceleration A Vector2Df representing the acceleration of the car
		 * @param isFrontAxle A bool representing whether the wheel is on the front or wheel axle
		 * @return The vertical load
		 */
		float calcWheelLoad(const Vector2Df acceleration, const bool isFrontAxle) {
			// Weight distribution
			if(velocity.x == 0.f && velocity.y == 0.f) {	// Stationary
				return isFrontAxle ? 
					(rearOffsetCG / wheelBase) * mass * 0.98f :
					(frontOffsetCG / wheelBase) * mass * 0.98f;
			} else {	// Accelerating/Decelerating
				float accelerationMagnitude = MAGNITUDE(acceleration);
				return isFrontAxle ? 
					((rearOffsetCG / wheelBase) * mass * 0.98f) - 
					((groundOffsetCG * wheelBase) * mass * accelerationMagnitude) :
					((frontOffsetCG / wheelBase) * mass * 0.98f) + 
					((groundOffsetCG * wheelBase) * mass * accelerationMagnitude);
			}
		}
		float calcWheelsMaxForce(const Vector2Df acceleration, const float frictionCoef, const bool isFrontAxle) {
			float wheelLoad = calcWheelLoad(acceleration, isFrontAxle);
			return wheelLoad * frictionCoef ;
		}
		
		/**
		 * @returns The torque of the engine
		 */
		float getTorque() { return (rpm >= minRPM && rpm <= 1500.f) ? 2788.f : -rpm + 4745.f; }
		void setRPM(const float rpm) {
			this->rpm = rpm;
			if(this->rpm < minRPM) this->rpm = minRPM;
			else if(this->rpm > maxRPM) this->rpm = maxRPM;
		}
		/**
		 * @brief Calculates the force the engine is producing
		 * @return Returns the force the engine is producing
		 */
		float calcEngineTorque() {
			float gearRatio = 15.76f;
			return getTorque() * gearRatio * diffRatio * transEff;
		}
		/**
		 *  @brief Returns the current position of the car
		 * @return Vector2Df representing position
		 */
		Vector2Df getPosition() { return position; }

	private:
		// Car details
		float mass;				// The mass of the car in kg
		float airResConst;		// The air resistance constant of the car
		float rollResConst;		// The rolling resistance constant of the car's wheels
		float frontOffsetCG;	// The offset of the front wheels from the center of gravity
		float rearOffsetCG;		// The offset of the rear wheels from the center of gravity
		float wheelBase;		// The distance between the front and rear wheels
		float groundOffsetCG;	// The distance between the ground and the center of gravity
		int wheelCount;			// The number of wheels
		wheel* wheelArr;		// An array of wheel structs

		// Engine Details
		int maxRPM;		// Maximum/Redline RPM
		int minRPM;		// Minimum/Idle RPM
		int maxTorque;	// Max torque in n*m

		// Transmission Details
		bool hasHL;			// If the transmission has sub-H/L gears
		int reverseGears;	// The number of reverse gears
		float transEff;		// The efficiency of the transmission
		float diffRatio;	// The differential's output ratio
		// float gears[];

		// Running Variables
		float rpm;					// The current rpm
		float speed;				// The current speed in m/s
		int currentGear;			// The current gear index
		Vector2Df position;			// The current position
		Vector2Df velocity;			// The current velocity
		Vector2Df acceleration;		// The acceleration of the car

		// Physics
		/** Simple Pacejka Magic Formula Constants
		 * Indexes:
		 * 	0: Dry road
		 * 	1: Wet road
		 * 	2: Snowy road
		 * 	3: Icy road
		*/
		const float B[4];	// Stiffness, usually 4-12
		const float C[4];	// Shape, usually 1-2
		const float D[4];	// Peak, usually 
		const float E[4];	// Curve, usually ~-10-1
		int roadCondition;	// The current road condition as an index of B, C, D, E
};
