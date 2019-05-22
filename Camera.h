#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	enum MoveDirection
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	Camera(glm::vec3 pos, glm::vec3 front = { 0.0f, 0.0f, -1.0f }, glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f });
	Camera(glm::vec3 pos, float pitch, float yaw, glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f });

	static void frontToEulerAngle(const glm::vec3& front, float& pitch, float& yaw);
	static void eulerAngleToFront(float pitch, float yaw, glm::vec3& front);

	// Funtions that only modify the object itself
	void standardizeEulerAngle(bool constrainPitch = true);
	
	// Functions that interact with other objects
	glm::mat4 getViewMatrix() { return glm::lookAt(_pos, _pos + _front, _up); }
	
	void processKeyboard(MoveDirection direction, float dt);
	void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
	void processMouseScroll(float yOffset);


	// getters and setters
	const glm::vec3& getPosition() const { return _pos; }
	const glm::vec3& getFront() const { return _front; }
	const glm::vec3& getWorldUp() const { return _worldUp; }
	float getPitch() const { return _pitch; }
	float getYaw() const {return _yaw;}
	void setMoveSpeed(float spd) { _moveSpeed = spd; }
	void setMouseSensitivity(float sensitivity) { _mouseSensitivity = sensitivity; }
	float getZoom() const { return _zoom; }
	void setZoom(float zoom) { _zoom = zoom; }

	// constants
	
	static const float DEFAULT_MOVE_SPEED;
	static const float DEFAULT_MOUSE_SENSITIVITY;
	static const float DEFAULT_ZOOM;
private:
	glm::vec3 _pos;
	glm::vec3 _front;
	glm::vec3 _right;
	glm::vec3 _up;
	glm::vec3 _worldUp;

	float _moveSpeed, _mouseSensitivity, _zoom;
	float _pitch, _yaw;

	void calUpRight();
};

#endif;