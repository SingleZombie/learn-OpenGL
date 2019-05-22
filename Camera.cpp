#include "Camera.h"
#include <iostream>

const float Camera::DEFAULT_MOVE_SPEED = 2.5f;
const float Camera::DEFAULT_MOUSE_SENSITIVITY = 0.1f;
const float Camera::DEFAULT_ZOOM = 45.0f;

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 worldUp) : _pos(pos), _front(glm::normalize(front)), _worldUp(worldUp),
_moveSpeed(DEFAULT_MOVE_SPEED), _mouseSensitivity(DEFAULT_MOUSE_SENSITIVITY), _zoom(DEFAULT_ZOOM)
{
	frontToEulerAngle(_front, _pitch, _yaw);
	calUpRight();
}

Camera::Camera(glm::vec3 pos, float pitch, float yaw, glm::vec3 worldUp) : _pos(pos), _pitch(pitch), _yaw(yaw), _worldUp(worldUp),
_moveSpeed(DEFAULT_MOVE_SPEED), _mouseSensitivity(DEFAULT_MOUSE_SENSITIVITY), _zoom(DEFAULT_ZOOM)
{
	standardizeEulerAngle();
	eulerAngleToFront(_pitch, _yaw, _front);
	calUpRight();
}

void Camera::standardizeEulerAngle(bool constrainPitch)
{
	if (constrainPitch)
	{
		_pitch = glm::clamp(_pitch, -89.0f, 89.0f);
	}
	//if (abs(_yaw) > 360.0f)
	//{
	//	// fmod 360
	//	_yaw -= 360.0f * floor(_yaw / 360.0f);
	//}
}

void Camera::processKeyboard(MoveDirection direction, float dt)
{
	float v = _moveSpeed * dt;
	glm::vec3 dirF = _front, dirR = _right;
	dirF.y = dirR.y = 0;;
	dirF = glm::normalize(dirF);
	dirR = glm::normalize(dirR);
	switch (direction)
	{
	case FORWARD:
		_pos += dirF * v;
		break;
	case BACKWARD:
		_pos -= dirF * v;
		break;
	case LEFT:
		_pos -= dirR * v;
		break;
	case RIGHT:
		_pos += dirR * v;
		break;
	default:
		std::cout << "keyboard type error" << std::endl;
	}
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
	xOffset *= _mouseSensitivity;
	yOffset *= _mouseSensitivity;

	_yaw += xOffset;
	_pitch += yOffset;

	standardizeEulerAngle(constrainPitch);
	eulerAngleToFront(_pitch, _yaw, _front);
	calUpRight();
}

void Camera::processMouseScroll(float yOffset)
{
	_zoom -= yOffset;
	_zoom = glm::clamp(_zoom, 1.0f, 45.0f);
}

void Camera::frontToEulerAngle(const glm::vec3& front, float& pitch, float& yaw)
{
	// Make sure the front has been normalized
	assert(abs(glm::length(front) - 1.0f) < 1e-6);

	pitch = asin(front.y);
	yaw = acos(front.x / cos(pitch));
	pitch = glm::degrees(pitch);
	yaw = glm::degrees(yaw);
}
void Camera::eulerAngleToFront(float pitch, float yaw, glm::vec3& front)
{
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(front);
}
void Camera::calUpRight()
{
	_right = glm::normalize(glm::cross(_front, _worldUp));
	_up = glm::normalize(glm::cross(_right, _front));
}