/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates the UFO arena
*/

#include <iostream>
#include "cpus.h"

using namespace std;
using namespace glm;

Cpus::Cpus() : Object()
{
}

Cpus::~Cpus(){
	this->TakeDown();
}

