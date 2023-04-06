#include "object3D.h"
#include <vector>
#include "core/engine.h"
#include "utils/gl_utils.h"
#include <iostream>
#include "Tema2.h"

Mesh* object3D::CreateMesh(const std::string& name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Create the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// Set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// Set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));


	// Set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	Mesh *mesh = new Mesh(name);
	mesh->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
	mesh->vertices = vertices;
	mesh->indices = indices;
	return mesh;
}

Mesh* object3D::CreateRaceTrack(
	const std::string& name,
	glm::vec3 color,
	float dist_int,
	float dist_out,
	std::vector<glm::vec3> track_mid_points)
{	
	glm::vec3 UP = glm::vec3(0, 1, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	float height = 0.01;
	int extra_points = 1000;

	std::vector<glm::vec3> extended_mid_points;
	for (int i = 0; i < track_mid_points.size() - 1; i++) {
		float deltaX = track_mid_points[i + 1].x - track_mid_points[i].x;
		float deltaZ = track_mid_points[i + 1].z - track_mid_points[i].z;

		extended_mid_points.push_back(track_mid_points[i]);
		for (int j = 1; j <= extra_points; j++) {
			float posX = track_mid_points[i].x + (deltaX / (extra_points + 1)) * j;
			float posZ = track_mid_points[i].z + (deltaZ / (extra_points + 1)) * j;
			glm::vec3 aux = glm::vec3(posX, height, posZ);
			
			extended_mid_points.push_back(aux);
		}
	}
	int size = track_mid_points.size();

	float deltaX = track_mid_points[0].x - track_mid_points[size - 1].x;
	float deltaZ = track_mid_points[0].z - track_mid_points[size - 1].z;
	
	extended_mid_points.push_back(track_mid_points[size - 1]);
	for (int j = 1; j <= extra_points; j++) {
		float posX = track_mid_points[size - 1].x + (deltaX / (extra_points + 1)) * j;
		float posZ = track_mid_points[size - 1].z + (deltaZ / (extra_points + 1)) * j;
		glm::vec3 aux = glm::vec3(posX, height, posZ);

		extended_mid_points.push_back(aux);
	}
	extended_mid_points.push_back(track_mid_points[0]);

	
	std::cout << extended_mid_points.size();
	std::vector<glm::vec3> track_out_points;
	std::vector<glm::vec3> track_in_points;
	
	for (int i = 0; i < extended_mid_points.size() - 1; i++) {
		glm::vec3 P1 = extended_mid_points[i];
		glm::vec3 P2 = extended_mid_points[i + 1];
		glm::vec3 D = glm::normalize(P2 - P1);
		glm::vec3 P = glm::cross(D, UP);
		glm::vec3 OUT = P1 + dist_out * P;
		glm::vec3 IN = P1 - dist_int * P;
		track_out_points.push_back(OUT);
		track_in_points.push_back(IN);
	}

	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i < extended_mid_points.size() - 1; i++) {
		vertices.push_back(VertexFormat(track_out_points[i], color));
		vertices.push_back(VertexFormat(track_in_points[i], color));
	}

	
	for (int i = 0; i <= vertices.size() - 4; i += 2) {
		// first triangle
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 3);
		// second triangle
		indices.push_back(i);
		indices.push_back(i + 3);
		indices.push_back(i + 2);
	}
	
	indices.push_back(vertices.size() - 2);
	indices.push_back(vertices.size() - 1);
	indices.push_back(1);
	indices.push_back(vertices.size() - 2);
	indices.push_back(1);
	indices.push_back(0);

	return CreateMesh(name, vertices, indices);
}

Mesh* object3D::CreateCar(const std::string& name, glm::vec3 color, float length, float height, float width, glm::vec3 leftBotttomCorner)
{	
	glm::vec3 corner = leftBotttomCorner;
	std::vector<VertexFormat> vertices
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(0, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, width), color),
		VertexFormat(corner + glm::vec3(0, 0, width) , color),
		VertexFormat(corner + glm::vec3(length, 0, width), color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, height, width), color),
		VertexFormat(corner + glm::vec3(length, height, 0),  color),
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2,	0, 2, 3,
		0, 5, 1,	5, 7, 1,
		0, 3, 4,	0, 4, 5,
		2, 3, 6,	3, 4, 6,
		4, 5, 7,	4, 7, 6,
		1, 2, 6,	6, 7, 1
	};

	return CreateMesh(name, vertices, indices);
}

Mesh* object3D::CreateRectangle(const std::string& name, glm::vec3 color, float length, float height, float width, glm::vec3 leftBotttomCorner)
{
	glm::vec3 corner = leftBotttomCorner;
	std::vector<VertexFormat> vertices
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(0, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, width), color),
		VertexFormat(corner + glm::vec3(0, 0, width) , color),
		VertexFormat(corner + glm::vec3(length, 0, width), color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, height, width), color),
		VertexFormat(corner + glm::vec3(length, height, 0),  color),
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2,	0, 2, 3,
		0, 5, 1,	5, 7, 1,
		0, 3, 4,	0, 4, 5,
		2, 3, 6,	3, 4, 6,
		4, 5, 7,	4, 7, 6,
		1, 2, 6,	6, 7, 1
	};

	return CreateMesh(name, vertices, indices);
}

Mesh* object3D::CreateDottedLine(const char* name) 
{
	glm::vec3 color = glm::vec3(0.9f, 0.9f, 0.9f);
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

        float height = 0.01;

	std::vector<glm::vec3> originalPoints =
	{
		glm::vec3(16, height, 0),
        glm::vec3(15.5, height, 2),
        glm::vec3(14, height, 4.5),
        glm::vec3(10, height, 6),
        glm::vec3(7, height, 5),
        glm::vec3(4, height, 4.8),
        glm::vec3(0, height, 5),
        glm::vec3(-6, height, 7),
        glm::vec3(-10, height, 8),
        glm::vec3(-14, height, 7),
        glm::vec3(-16.9, height, 4),
        glm::vec3(-18, height, 0),
        glm::vec3(-16.9, height, -4),
        glm::vec3(-14, height, -7),
        glm::vec3(-10, height, -8),
        glm::vec3(-6, height, -7),
        glm::vec3(-4.5, height, -5.8),
        glm::vec3(0, height, -4),
        glm::vec3(4, height, -4),
        glm::vec3(7.5, height, -5),
        glm::vec3(10, height, -6),
        glm::vec3(14, height, -4),
		glm::vec3(16, height, 0),

	};

	float distR = 0.02f, distA = 0.02f;
	glm::vec3 D, P, UP = glm::vec3(0, 1, 0), curr1, curr2;
	std::vector<glm::vec3>::iterator iter;
	int numVertices = 0;

	for (iter = originalPoints.begin(); iter + 1 != originalPoints.end(); ++iter) {
		D = *(iter + 1) - *iter;
		P = glm::cross(D, UP);
		curr1 = *iter + distR * P;
		curr2 = *iter - distA * P;
		vertices.push_back(VertexFormat(curr1, color));
		vertices.push_back(VertexFormat(curr2, color));
		numVertices += 2;
	}

	for (int i = 0; i < numVertices / 2 - 2; i++) {
		// pt r1, a1, a2
		indices.push_back(2 * i);
		indices.push_back(2 * i + 1);
		indices.push_back(2 * i + 3);

		// pt r1, a2, r2
		indices.push_back(2 * i);
		indices.push_back(2 * i + 3);
		indices.push_back(2 * i + 2);
	}

	// pt rlast, alast, a0
	indices.push_back((numVertices / 2 - 3) * 2);
	indices.push_back((numVertices / 2 - 3) * 2 + 1);
	indices.push_back(1);

	// pt rlast, a0, r0
	indices.push_back((numVertices / 2 - 3) * 2);
	indices.push_back(1);
	indices.push_back(0);

	return CreateMesh(name, vertices, indices);
}

Mesh* object3D::CreateGround(const std::string& name, glm::vec3 color)
{	
	std::vector<VertexFormat> vertices;

	std::vector<unsigned int> indices;
	int a = 0, b = 0;
	for (float i = -100; i <= 100; i += 2) {
		for (float j = -100; j <= 100; j += 2) {
			VertexFormat v = VertexFormat(glm::vec3(i, 0, j), color);
			if (a % 2 == 0 && b % 2 == 0) {
				v.text_coord = glm::vec2(0, 0);
			}
			else if (a % 2 == 0 && b % 2 == 1) {
				v.text_coord = glm::vec2(0, 1);
			}
			else if (a % 2 == 1 && b % 2 == 0) {
				v.text_coord = glm::vec2(1, 0);
			}
			else if (a % 2 == 1 && b % 2 == 1) {
				v.text_coord = glm::vec2(1, 1);
			}

			vertices.push_back(v);
			b++;
		}
		a++;
	}
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			indices.push_back((i + 1) * 100 + j);
			indices.push_back((i + 1) * 100 + j + 1);
			indices.push_back(i * 100 + j + 1);

			indices.push_back(i * 100 + j);
			indices.push_back((i + 1) * 100 + j);
			indices.push_back(i * 100 + j + 1);
		}
	}

	return CreateMesh(name, vertices, indices);
}

