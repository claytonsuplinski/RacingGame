/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates the UFO arena
*/

#pragma once
#include "object.h"
#include "shader.h"
#include "disc.h"
#include "ufoTop.h"
#include "triangle.h"
#include "square2.h"
#include "cylinder2.h"
#include "gengar.h"
#include "bowl2.h"

class DiscStadium : public Object
{
public:
	DiscStadium();
	~DiscStadium();
	virtual bool Initialize();
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);
	void TakeDown();
	void StepShader();
	Shader shader;
	Shader solid_color;
	Square2 * floor;
	Bowl2 * grass;
	Cylinder2 * innerWall;
	Cylinder2 * outerWall;
	Triangle * triLight;
	Triangle * triLight2;
	Gengar * naboo, * building2, * bridge, * building3, * sc4,
		* building, * building1, * building10, * sc1, * sc19, * sc21, 
		* bush, * hill, * plant;
	int* bushX, *bushY;
	int* grassX, *grassY;

	float nearD, farD, width, height, tang, ratio;
	float sphereFactorX, sphereFactorY;

	glm::vec3 cc;
	glm::vec3 X, Y, Z;
	float userX, userZ, nextX, nextZ, nextX2, nextZ2, nextX3, nextZ3, rotation;

	void setCamInternals(float angle, float ratio, float nearD, float farD);
	bool shouldIDrawThis(glm::vec3 objectPosition, float radius);
	bool shouldDisplay(float objX, float objZ, float radius, bool print);
private:
	void BuildNormalVisualizationGeometry();
	std::vector<VertexAttributesPCN> vertices;
	typedef Object super;
};