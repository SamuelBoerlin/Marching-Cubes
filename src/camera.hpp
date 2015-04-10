//Author: Benny Bobaganoosh, Samuel Börlin

#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Camera
{
public:
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar) {
		this->m_position = pos;
		this->m_forwardVec = glm::vec3(0.0f, 0.0f, 1.0f);
		this->m_upVec = glm::vec3(0.0f, 1.0f, 0.0f);
		this->m_projectionMatrix = glm::perspective(fov, aspect, zNear, zFar);
		this->m_rotYaw = 0.0f;
		this->m_rotPitch = 0.0f;
	}

	inline const glm::mat4 GetViewProjection() {
		return m_projectionMatrix * glm::lookAt(m_position, m_position + m_forwardVec, m_upVec);
	}

	void RotatePitch(float angle) {
		m_rotPitch += angle;

		glm::vec3 right = glm::normalize(glm::cross(m_upVec, m_forwardVec));

		m_forwardVec = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(m_forwardVec, 0.0)));
		m_upVec = glm::normalize(glm::cross(m_forwardVec, right));
	}

	void RotateYaw(float angle) {
		m_rotYaw += angle;

		static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

		glm::mat4 rotation = glm::rotate(angle, UP);

		m_forwardVec = glm::vec3(glm::normalize(rotation * glm::vec4(m_forwardVec, 0.0)));
		m_upVec = glm::vec3(glm::normalize(rotation * glm::vec4(m_upVec, 0.0)));
	}

	void SetRotationYaw(float angle) {
		RotateYaw(-m_rotYaw + angle);
	}

	void SetRotationPitch(float angle) {
		RotatePitch(-m_rotPitch + angle);
	}

	void SetRotation(float yaw, float pitch) {
		RotateYaw(-m_rotYaw + yaw);
		RotatePitch(-m_rotPitch + pitch);
	}

	void LookAt(glm::vec3 loc) {
		glm::vec3 diff = loc - m_position;
		double dist = sqrt(diff.x*diff.x+diff.z*diff.z);
		float rad = 180.0f/3.141592653589793f;
		SetRotationPitch(-atan(diff.y/dist)*rad);
		float yaw = 0.0f;
		if(diff.z >= 0.0) {
			yaw = -atan(diff.x / diff.z)*rad;
		} else if(diff.x >= 0.0) {
			yaw = atan(diff.z / diff.x)*rad - 90.0f;
		} else if(diff.x <= 0.0) {
			yaw = atan(diff.z / diff.x)*rad + 90.0f;
		}
		SetRotationYaw(-yaw);
	}

	inline void SetPos(glm::vec3 newPos) { m_position = newPos; }
	inline void SetUp(glm::vec3 newUp) { m_upVec = newUp; }
	inline void SetForward(glm::vec3 newForward) { m_forwardVec = newForward; }
	inline glm::vec3 GetPos() { return m_position; }
	inline glm::vec3 GetUpVec() {return m_upVec; }
	inline glm::vec3 GetForwardVec() { return m_forwardVec; }
	inline float GetRotationYaw() { return m_rotYaw; }
	inline float GetRotationPitch() { return m_rotPitch; }
protected:
private:
	glm::mat4 m_projectionMatrix;
	glm::vec3 m_position;
	glm::vec3 m_forwardVec;
	glm::vec3 m_upVec;
	float m_rotYaw;
	float m_rotPitch;
};

#endif
