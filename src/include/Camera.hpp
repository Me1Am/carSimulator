#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

class Camera {
	public:
		/**
		 * @brief Caclulates the view matrix
		 * @return A glm::mat4 representing the camera's view
		*/
		glm::mat4 calcCameraView() {
			glm::vec3 cameraTarget = cameraPos + cameraFront;

			glm::vec3 cameraRight = glm::normalize(glm::cross(WORLD_UP, cameraFront));
			cameraUp = glm::cross(cameraFront, cameraRight);

			// LookAt matrix which allows easy camera manipulation
			glm::mat4 view = glm::mat4(1.f);
			//view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

			return view;
		}
		/**
		 * @brief Updates the camera's position
		 * @param forward A float representing if the camera should move forward
		 * @param backward A float representing if the camera should move backwards
		 * @param left A float representing if the camera should strafe left
		 * @param right A float representing if the camera should strafe right
		 * @param deltaTime A float representing the time since last frame in ms
		*/
		void updateCameraPosition(const bool forward, const bool backward, const bool left, const bool right, const float deltaTime) {
			float frameSpeed = cameraSpeed * deltaTime/1000;

			if(forward)
				cameraPos += cameraFront * frameSpeed;
			if(backward)
				cameraPos -= cameraFront * frameSpeed;
			if(left)
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * frameSpeed;
			if(right)
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * frameSpeed;
		}
	private:
		float cameraSpeed = 2.5f;
		glm::vec3 cameraPos 	= glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraFront 	= glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp 		= glm::vec3(0.0f, 1.0f, 0.0f);
		const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
};