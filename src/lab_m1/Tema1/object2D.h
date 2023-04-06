#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2DCopy
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateRectangle(const std::string &name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);
    Mesh* CreateCircle(const std::string& name, glm::vec3 centre, float radius, glm::vec3 color);
    Mesh* CreateTriangle(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateBody(const std::string& name, glm::vec3 color, float width, float height);
    Mesh* CreateWing(const std::string& name, glm::vec3 color, glm::vec3 starting_point, float width, float height);
    Mesh* CeateNose(const std::string& name, glm::vec3 color, glm::vec3 starting_point, float width, float height);
}
