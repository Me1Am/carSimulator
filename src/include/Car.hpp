#include "Engine.hpp"
//#include "Vector2D.hpp"
#include <iostream>
#include <memory>

class Car {
    public:
        Car() {
			engine.reset(new Engine());
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
		Car(const float mass, const float airResConst, const float rollResConst, const int maxRPM, const int minRPM, const int maxTorque, const int reverseGears, const bool hasHL) {
			engine.reset(new Engine(maxRPM, minRPM, maxTorque, reverseGears, hasHL));
			this->mass = mass;
			this->airResConst = airResConst;
			this->rollResConst = rollResConst;
			position = Vector2Df(0.f, 0.f);
		}
        void cycle(const float deltaTime) {
			std::array<Vector2Df, 2> results = engine.get()->runCycle(deltaTime, airResConst, rollResConst, mass, 0.40f, false);
			position = results[0];
			calcWeightDistribution(results[0], results[1]);
		}
		void calcWeightDistribution(const Vector2Df velocity, const Vector2Df acceleration) {
			/* Max friction, idk why this is useful just seen in the resource
			float wheelFrictCoef = 10.f;
			Vector2Df maxFrict = Vector2Df(wheelFrictCoef * mass * 0.98f, wheelFrictCoef * mass * 0.98f);
			*/
			const float frontOffsetCG = 1.5f;
			const float rearOffsetCG = 5.5f;
			const float wheelBase = 7.f;
			const float groundOffsetCG = 2.f;

			// Weight distribution
			if(velocity.x == 0.f && velocity.y == 0.f){	// Stationary
				float frontWeight = (rearOffsetCG / wheelBase) * mass * 0.98f;
				float rearWeight = (frontOffsetCG / wheelBase) * mass * 0.98f;
			} else {	// Accelerating/Decelerating
				float accelerationMagnitude = std::sqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y);
				float frontWeight = ((rearOffsetCG / wheelBase) * mass * 0.98f) - ((groundOffsetCG * wheelBase) * mass * accelerationMagnitude);
				float rearWeight = ((frontOffsetCG / wheelBase) * mass * 0.98f) + ((groundOffsetCG * wheelBase) * mass * accelerationMagnitude);
			}
		}
		Vector2Df position;
		void setRPM(const float rpm) { engine.get() -> setRPM(rpm); }
	private:
        float mass; // The mass of the car in kg
        float airResConst;
		float rollResConst;	// Should be ~30x that of 'airResConst' because @30m/s(100kph) they're equal

        std::unique_ptr<Engine> engine; // Pointer to the engine
};