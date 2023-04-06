#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2DCopy::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2DCopy::CreateCircle(const std::string& name, glm::vec3 centre, float radius, glm::vec3 color) {
	float a = radius;

	int range = 90;

	std::vector<VertexFormat> vertices = {
		VertexFormat(centre + glm::vec3(0, 0, 0), color) // 0
	};

	for (int i = 0; i < range + 1; ++i) {
		float angle = -M_PI / 2 + M_PI / 45 * i;
		vertices.push_back(VertexFormat(centre + glm::vec3(a * cos(angle), a * sin(angle), 0), color));
	}
	Mesh* balloon = new Mesh(name);
	
	std::vector<unsigned int> indices;
	for (int i = 1; i <= range; ++i) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(1 + i % range);
	}

	balloon->InitFromData(vertices, indices);
	return balloon;
}

Mesh* object2DCopy::CreateBody(const std::string& name, glm::vec3 color, float width, float height)
{
    std::vector<VertexFormat> vertices =
    {   
        VertexFormat(glm::vec3(0, 0, 0), color),
        VertexFormat(glm::vec3(width, height / 2, 0), color),
        VertexFormat(glm::vec3(0, height, 0), color),
        
    };

    Mesh* body = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2};


    body->InitFromData(vertices, indices);
    return body;
}

Mesh* object2DCopy::CeateNose(const std::string& name, glm::vec3 color, glm::vec3 starting_point, float width, float height)
{
     std::vector<VertexFormat> vertices =
    {   
        VertexFormat(starting_point, color),
        VertexFormat(starting_point + glm::vec3(width, 2 * height / 10, 0), color),
        VertexFormat(starting_point + glm::vec3(0, height, 0), color),
        
    };

    Mesh* nose = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2};


    nose->InitFromData(vertices, indices);
    return nose;
}

Mesh* object2DCopy::CreateWing(const std::string& name, glm::vec3 color, glm::vec3 starting_point, float width, float height)
{
    std::vector<VertexFormat> vertices =
    {   
        VertexFormat(starting_point, color),
        VertexFormat(starting_point + glm::vec3(0, height, 0), color),
        VertexFormat(starting_point + glm::vec3(-width, height, 0), color),
        
    };

    Mesh* wing = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2};


    wing->InitFromData(vertices, indices);
    return wing;
}

Mesh* object2DCopy::CreateRectangle(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float width,
    float height,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(width, 0, 0), color),
        VertexFormat(corner + glm::vec3(width, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}