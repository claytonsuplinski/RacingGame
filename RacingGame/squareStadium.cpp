/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates the square arena
*/

#include <iostream>
#include "squareStadium.h"

using namespace std;
using namespace glm;

//Create the SquareStadium object
SquareStadium::SquareStadium() : Object()
{
}

SquareStadium::~SquareStadium(){
	this->TakeDown();
}

//Initialize all the elements of the SquareStadium
bool SquareStadium::Initialize(int weatherIndex)
{
	if (this->GLReturnedError("SquareStadium::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	this->wall = new Square3();this->floor = new Square2();this->bleacher = new Bleacher();
	this->grass = new Cylinder();this->bird = new Bird();this->bird2 = new Bird();
	this->bird3 = new Bird();this->bird4 = new Bird();this->jb = new Jumbotron();
	this->zubat = new Gengar();this->charizard = new Gengar();this->blades = new Gengar();
	this->mtns = new Gengar();this->trees = new Gengar();

	this->jb->Initialize();
	
	if(weatherIndex == 0){
		this->floor->texID = 28;
		this->floor->Initialize(1000.0f, 200, "./textures/grass.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	}
	else if(weatherIndex == 1){
		this->floor->texID = 500;
		this->floor->Initialize(1000.0f, 200, "./textures/grassRain.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	}
	else{
		this->floor->texID = 501;
		this->floor->Initialize(1000.0f, 200, "./textures/grassWinter.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	}

	this->wall->texID = 10;
	this->wall->Initialize(12,105.6f, "./textures/clothTexture.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->grass->color = vec3(0.05f, 0.65f, 0.2f);
	this->grass->Initialize(1,0.75f, 0.03f, 0.03f, "phong.vert", "phong.frag");

	this->zubat->texID = 23;
	this->zubat->Initialize("./models/zubat.obj", "./models/zubat.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->charizard->texID = 24;
	this->charizard->Initialize("./models/charizard.obj", "./models/charizard.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	if(weatherIndex < 2){
		this->trees->texID = 49;
		this->trees->Initialize("./models/GumBough.obj", "./models/GumBough.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	}
	else{
		this->trees->texID = 490;
		this->trees->Initialize("./models/GumBough.obj", "./models/GumBoughWnt.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	}

	vec3 yellow = vec3(0.8, 0.8, 0.1);vec3 red = vec3(0.8, 0.1, 0.1);vec3 black = vec3(0.0, 0.0, 0.0);
	vec3 white = vec3(1.0, 1.0, 1.0);vec3 blue = vec3(0.1, 0.1, 0.75);vec3 green = vec3(0.1, 0.45, 0.1);
	this->bird->Initialize(yellow, red, black, red);this->bird2->Initialize(red, yellow, green, blue);this->bird3->Initialize(white, blue, white, white);
	this->bird4->Initialize(yellow, black, black, black);

	this->bleacher->Initialize();

	numBirds = 12; //Give birds initial positions
	birdX = new int[numBirds];birdY = new int[numBirds];birdType = new int[numBirds];birdOffset = new int[numBirds];
	for(int i=0; i<numBirds; i++){
		birdX[i] = rand() % 106 - 106;birdY[i] = rand() % 15 + 5;
		birdType[i] = rand() % 4 + 1;birdOffset[i] = rand() % 360 + 1;
	}

	if (this->GLReturnedError("SquareStadium::Initialize - on exit"))
		return false;

	return true;
}

void SquareStadium::StepShader(){
	this->floor->StepShader();this->bleacher->StepShader();this->grass->StepShader();
	this->bird->StepShader();this->bird2->StepShader();this->bird3->StepShader();
	this->bird4->StepShader();this->jb->StepShader();
}

//Delete the SquareStadium
void SquareStadium::TakeDown()
{
	this->wall = NULL;this->floor = NULL;this->bleacher = NULL;this->grass = NULL;
	this->bird = NULL;this->bird2 = NULL;this->bird3 = NULL;this->bird4 = NULL;
	this->jb = NULL;this->zubat = NULL;this->charizard = NULL;this->blades = NULL;
	this->mtns = NULL;this->trees = NULL;this->vertices.clear();this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}

//Draw and position the elements of the SquareStadium
void SquareStadium::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("SquareStadium::Draw - on entry"))
		return;

	glEnable(GL_DEPTH_TEST);

	mat4 another, scaler;

	float t = float(glutGet(GLUT_ELAPSED_TIME) / 5000.0f);
	float tOffset = float(glutGet(GLUT_ELAPSED_TIME) / 1000.0f) - 1000;

	

	//Elements of the arena.
	another = modelview;
	another = translate(modelview, vec3(-500.0f, 0.0f, -500.0f));
	another = rotate(another, 90.0f, vec3(1.0f, 0.0f, 0.0f));	
	this->floor->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-25.0f, -0.30f, 4.0f));
	this->trees->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-55.0f, 0.0f, 3.0f));
	another = rotate(another, 180.0f, vec3(0, 1, 0));
	this->jb->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-95.0f, -0.5f, 2.0f));
	another = rotate(another, 45.0f, vec3(0, 1, 0));
	this->trees->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-15.0f, 0.0f, -111.0f));
	this->trees->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-32.0f, 0.0f, -107.0f));
	this->jb->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-55.0f, -0.5f, -107.0f));
	another = rotate(another, 75.0f, vec3(0, 1, 0));
	this->trees->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-78.0f, 0.0f, -107.0f));
	this->jb->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-105.0f, 0.0f, -118.0f));
	another = rotate(another, 145.0f, vec3(0, 1, 0));
	this->trees->Draw(projection, another, size, 0);

	scaler = rotate(modelview, 90.0f, vec3(1.0f, 0.0f, 0.0f));
	scaler = scale(scaler, vec3(1.0f, 1.0f, 0.02f));	
	this->wall->Draw(projection, scaler, size, 0);

	scaler = translate(modelview, vec3(0.0f, 0.0f, -105.6f));
	scaler = rotate(scaler, 90.0f, vec3(1.0f, 0.0f, 0.0f));
	scaler = scale(scaler, vec3(1.0f, 1.0f, 0.02f));	
	this->wall->Draw(projection, scaler, size, 0);

	scaler = translate(modelview, vec3(1.0f, 0.0f, -1.0f));
	scaler = rotate(scaler, -90.0f, vec3(0.0f, 1.0f, 0.0f));
	scaler = rotate(scaler, 90.0f, vec3(1.0f, 0.0f, 0.0f));	
	scaler = scale(scaler, vec3(1.0f, 1.0f, 0.02f));	
	this->wall->Draw(projection, scaler, size, 0);

	scaler = translate(modelview, vec3(-104.6f, 0.0f, -1.0f));
	scaler = rotate(scaler, -90.0f, vec3(0.0f, 1.0f, 0.0f));
	scaler = rotate(scaler, 90.0f, vec3(1.0f, 0.0f, 0.0f));	
	scaler = scale(scaler, vec3(1.0f, 1.0f, 0.02f));	
	this->wall->Draw(projection, scaler, size, 0);	

	if (this->GLReturnedError("SquareStadium::Draw - on exit"))
		return;
}
