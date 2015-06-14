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

class Cpus : public Object
{
public:
	Cpus();
	~Cpus();

	std::vector<glm::vec3> cpuPositions;

	
private:

};