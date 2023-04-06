#pragma once

#include <string>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace object3D
{
	Mesh* CreateRaceTrack(const std::string& name, glm::vec3 color, float dist_int, float dist_out, std::vector<glm::vec3> track_mid_points);
	Mesh* CreateGround(const std::string& name, glm::vec3 color);
	Mesh* CreateCar(const std::string& name, glm::vec3 color, float length, float height, float width, glm::vec3 leftBotttomCorner);
	Mesh* CreateRectangle(const std::string& name, glm::vec3 color, float length, float height, float width, glm::vec3 leftBotttomCorner);
    Mesh* CreateMesh(const std::string& name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
	Mesh* CreateDottedLine(const char* name);
}