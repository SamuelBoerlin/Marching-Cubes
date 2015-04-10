//Author: Samuel Börlin

#ifndef VOXEL_H
#define VOXEL_H

#include <glm\vec3.hpp>

struct Voxel {
	Voxel(float density, short int material, glm::vec3 normal) {
		this->density = density;
		this->material = material;
		this->normal = normal;
	}
	float density;
	glm::vec3 normal;
	short int material;
};

#endif