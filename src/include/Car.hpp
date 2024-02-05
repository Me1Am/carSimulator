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
	float radius;
	float frictionCoef;
	bool powered;
	
	// Running 
	Vector2Df angVelocity;
	Vector2Df angAcceleration;
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
	Car(const float carConstants[], const float engineConstants[], const float wheelArgs[][3]) {	
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
			wheelArr[i].radius = wheelArgs[i][0];		// Set radius
			wheelArr[i].frictionCoef = wheelArgs[i][1];	// Set friction coefficient
			wheelArr[i].powered = wheelArgs[i][2];		// Set if powered
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
		
		Vector2Df traction[4];
		for(int i = 0; i < wheelCount; i++) {
			traction[i] = calcWheelTractionForce(breakForce, directionVector, speed);
		}

		// Net Longitudual Force Vector
		// Using the average traction force right now for simplification
		Vector2Df tractionAvg;
		for(Vector2Df tract : traction) tractionAvg + tract;
		tractionAvg = tractionAvg / (float)wheelCount;	// Calculate average
		Vector2Df longForce = tractionAvg + breakForce + drag + rollDrag;	// Calculate the net longtitudinal force on the car

		// Acceleration Vector
		acceleration = Vector2Df(longForce.x / mass, longForce.y / mass);	// Calculate acceleration

		// Integrate the acceleration to change velocity
		velocity += (deltaTime * acceleration);

		#ifdef DEBUG	// Print out the calculations
			std::cout << (
				"RPM: " + std::to_string(rpm) + "\nTorque: " + std::to_string(getTorque()) +
				"\nSpeed: " + std::to_string(speed) + "\nEngine Force: " + std::to_string(calcEngineForce()) +
				"\nDirection Vector: " + std::to_string(directionVector.x) + ", " + std::to_string(directionVector.y) +
				"\nTraction: " + std::to_string(tractionAvg.x) + ", " + std::to_string(tractionAvg.y) +
				"\nAir Resistance: " + std::to_string(drag.x) + ", " + std::to_string(drag.y) +
				"\nRolling Resistance: " + std::to_string(rollDrag.x) + ", " + std::to_string(rollDrag.y) +
				"\nLongtitudinal Force: " + std::to_string(longForce.x) + ", " + std::to_string(longForce.y) +
				"\nAcceleration: " + std::to_string(acceleration.x) + ", " + std::to_string(acceleration.y) +
				"\nVelocity: " + std::to_string(velocity.x) + ", " + std::to_string(velocity.y) + "\n"
			) << std::endl;
		#endif
	}
	Vector2Df calcWheelTractionForce(Vector2Df breakForce, Vector2Df directionVector, float speed) {
		// Traction Force Vector
		Vector2Df tractionForce = directionVector * calcEngineForce() / wheelRadius;	// Calculate traction of the wheels
		if(IS_NAN_VECT(tractionForce))
			tractionForce = Vector2Df(directionVector.x * calcEngineForce(), directionVector.y * calcEngineForce());

		// Slip ratio
		float slipRatio = (wheelRadius - speed) / std::abs(speed);

		std::array<float, 2> maxForces = calcWheelsMaxForce(acceleration);
		if(maxForces[0] > MAGNITUDE(tractionForce) || maxForces[1] > MAGNITUDE(tractionForce))
			tractionForce *= 0.80f;	// Wheels are spinning freely, decrease traction force
		
		return tractionForce;
	}
	/**
	 * @brief Calculates the force on each wheel
	 * @param acceleration A Vector2Df representing the acceleration of the car
	 * @return A float array with the front wheels force first
	 */
	std::array<float, 2> calcWheelsMaxForce(const Vector2Df acceleration) {
		// Max friction
		float frontWeight;
		float rearWeight;

		// Weight distribution
		if(velocity.x == 0.f && velocity.y == 0.f) {	// Stationary
			frontWeight = (rearOffsetCG / wheelBase) * mass * 0.98f;
			rearWeight = (frontOffsetCG / wheelBase) * mass * 0.98f;
		} else {	// Accelerating/Decelerating
			float accelerationMagnitude = MAGNITUDE(acceleration);
			frontWeight =
				((rearOffsetCG / wheelBase) * mass * 0.98f) - 
				((groundOffsetCG * wheelBase) * mass * accelerationMagnitude);
			rearWeight =
				((frontOffsetCG / wheelBase) * mass * 0.98f) + 
				((groundOffsetCG * wheelBase) * mass * accelerationMagnitude);
		}

		/* Calculate the maximum friction force for the wheel sets
		 * If the force being exerted on either wheelset is greater
		 * Then the wheels spinout, reducing tractionForce and thus force
		 */
		float frontMaxFrict = wheelFrictCoef * frontWeight;
		float rearMaxFrict = wheelFrictCoef * rearWeight;
		return {frontMaxFrict, rearMaxFrict};
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
	float calcEngineForce() {
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
		float wheelRadius;		// The radius of the wheels in m
		float wheelFrictCoef;	// The friction coefficient of the wheels
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