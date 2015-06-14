/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates the UFO arena
*/

#include <iostream>
#include "items.h"

using namespace std;
using namespace glm;

Items::Items() : Object()
{
}

Items::~Items(){
	this->TakeDown();
}

