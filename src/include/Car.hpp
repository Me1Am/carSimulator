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
	float mass;
	float radius;
	float frictionCoef;
	bool powered;
	
	// Running 
	float rpm = 0.f;
	float angVelocity = 0.f;
	float angAcceleration = 0.f;
};

class Car {
	public:
	Car() { position = Vector2Df(0.f, 0.f); }
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
	Car(const float carConstants[], const float engineConstants[], const float wheelArgs[][4]) {	
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
		}

		// Set Running Variables
		rpm = 0.f;
		speed = 0.f;
		currentGear = 0;
		velocity = Vector2Df();
		position = Vector2Df();
		acceleration = Vector2Df();

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
	~Car() { delete[] wheelArr; }
	void runCycle(const float deltaTime, const float airResConst, const float rollResConst, 
				  const float mass, const float wheelRadius, const float breakStatus) {
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

		// Breaking Force Vector
		Vector2Df breakForce;
		if(breakStatus >= 0.f) breakForce = directionVector * -120.f;	// Calculate the breaking force
		else breakForce = Vector2Df(0.f, 0.f);	// Car is not breaking, break force is 0
		if(IS_NAN_VECT(breakForce)) breakForce = Vector2Df(0.f, 0.f);
		
		float traction[4];
		for(int i = 0; i < wheelCount; i++) {
			traction[i] = calcWheelTractionForce(breakForce, directionVector, &wheelArr[i], deltaTime);
		}

		// Net Longitudual Force Vector
		// Using the average traction force right now for simplification
		float tractionAvg;
		for(float tract : traction) tractionAvg + tract;
		tractionAvg = tractionAvg / (float)wheelCount;	// Calculate average
		Vector2Df longForce = (directionVector * tractionAvg) + breakForce + drag + rollDrag;	// Calculate the net longtitudinal force on the car

		// Acceleration Vector
		acceleration = Vector2Df(longForce.x / mass, longForce.y / mass);	// Calculate acceleration

		// Integrate the acceleration to change velocity
		velocity += (deltaTime * acceleration);

		#ifdef DEBUG	// Print out the calculations
			std::cout << (
				"RPM: " + std::to_string(rpm) + "\nEngine Torque: " + std::to_string(getTorque()) +
				"\nSpeed: " + std::to_string(speed) + "\nEngine Torque(on wheel): " + std::to_string(calcEngineTorque()) +
				"\nDirection Vector: " + std::to_string(directionVector.x) + ", " + std::to_string(directionVector.y) +
				"\nTraction: " + std::to_string(tractionAvg) +
				"\nAir Resistance: " + std::to_string(drag.x) + ", " + std::to_string(drag.y) +
				"\nRolling Resistance: " + std::to_string(rollDrag.x) + ", " + std::to_string(rollDrag.y) +
				"\nLongtitudinal Force: " + std::to_string(longForce.x) + ", " + std::to_string(longForce.y) +
				"\nAcceleration: " + std::to_string(acceleration.x) + ", " + std::to_string(acceleration.y) +
				"\nVelocity: " + std::to_string(velocity.x) + ", " + std::to_string(velocity.y) + "\n"
			) << std::endl;
		#endif
	}
	float calcWheelTractionForce(Vector2Df breakForce, Vector2Df directionVector, wheel* wheel, const float deltaTime) {
		float driveTorque = calcEngineTorque(); 
		
		// Drive Force Vector
		// The longtitudinal force that the wheels exert
		float driveForce = driveTorque / wheel->radius;	// Calculate force of the wheels
	
		float maxForces = calcWheelsMaxForce(acceleration, wheel->frictionCoef, false);
		if(maxForces > driveForce || maxForces > driveForce)
			driveForce *= 0.85f;	// Wheels are spinning freely, decrease traction force
		
		float tractionForce;		// Force the wheel creates which pushes the car
		float tractionTorque;		// Torque opposing the wheel, the sum of the wheels on the same axle
		float netTorque;			// The net amount of torque on the wheel
		float brakeTorque;			// The beaking torque on the wheel
		float inertia;				// The inertia of the wheels on the same axle
		float slipRatio;			// The ratio between wheel speed to car speed

		inertia = 2 * (wheel->mass * (wheel->radius * wheel->radius) / 2);	// Multiplied by the wheels on the same axle
		netTorque = driveTorque + tractionTorque + brakeTorque;	// Total torque on the wheel
		wheel->angAcceleration = netTorque / inertia;
		wheel->angVelocity += wheel->angAcceleration * deltaTime;
		slipRatio = ((wheel->angVelocity * wheel->radius) - speed) / abs(speed);

		float verticalLoad = calcWheelLoad(acceleration, false);

		// Update car variables
		float gearRatio = 15.76f;	// Temp
		rpm = wheel->angVelocity * gearRatio * diffRatio * 60 / 2 * M_PI;	// Calculate new engine rpm

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
};