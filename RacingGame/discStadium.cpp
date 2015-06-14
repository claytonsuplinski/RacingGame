/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates the UFO arena
*/

#include <iostream>
#include "discStadium.h"

using namespace std;
using namespace glm;

#define HALF_ANG2RAD 3.14159265358979323846/360.0

//Create the arena
DiscStadium::DiscStadium() : Object()
{
}

DiscStadium::~DiscStadium(){
	this->TakeDown();
}

//Initialize all the elements of the arena
bool DiscStadium::Initialize()
{
	if (this->GLReturnedError("DiscStadium::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	this->floor = new Square2();this->innerWall= new Cylinder2();this->outerWall= new Cylinder2();
	this->triLight = new Triangle();this->triLight2 = new Triangle();this->naboo = new Gengar();
	this->building2 = new Gengar();this->building3 = new Gengar();this->bridge = new Gengar();
	this->sc4 = new Gengar();this->building = new Gengar();this->building1 = new Gengar();
	this->building10 = new Gengar();this->sc1 = new Gengar();this->sc19 = new Gengar();
	this->sc21 = new Gengar();this->bush = new Gengar();this->hill = new Gengar();this->plant = new Gengar();
	this->grass = new Bowl2();
	
	this->floor->texID = 1;
	this->floor->Initialize(600.0f, 100,"./textures/cobblestone.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->innerWall->texID = 444;
	this->innerWall->Initialize(70, 2.0f, 200.0f, 200.0f,"./models/wall.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->outerWall->texID = 445;
	this->outerWall->Initialize(70, 2.0f, 250.0f, 250.0f,"./models/wall.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->triLight->color = vec3(0.95f, 0.95f, 0.05f);
	this->triLight->Initialize(1.0, "phong.vert", "phong.frag");

	this->triLight2->color = vec3(1.0f, 1.0f, 1.0f);
	this->triLight2->Initialize(1.0, "phong.vert", "phong.frag");

	this->naboo->texID = 1200;this->naboo->order = 1;
	this->naboo->Initialize("./models/naboo.obj", "./models/naboo.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->building2->texID = 1201;this->building2->order = 1;
	this->building2->Initialize("./models/building2.obj", "./models/building2.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->bridge->texID = 1202;this->bridge->order = 1;
	this->bridge->Initialize("./models/bridgeWooden.obj", "./models/bridgeWooden.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->building3->texID = 1203;this->building3->order = 1;
	this->building3->Initialize("./models/building3.obj", "./models/building3.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->sc4->texID = 1204;this->sc4->order = 1;
	this->sc4->Initialize("./models/skyscraper4.obj", "./models/skyscraper4.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->building->texID = 1205;this->building->order = 1;
	this->building->Initialize("./models/building.obj", "./models/building.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->building1->texID = 1206;this->building1->order = 1;
	this->building1->Initialize("./models/building1.obj", "./models/building1.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->building10->texID = 1207;this->building10->order = 1;
	this->building10->Initialize("./models/building10.obj", "./models/building10.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->sc1->texID = 1208;this->sc1->order = 1;
	this->sc1->Initialize("./models/skyscraper1.obj", "./models/skyscraper1.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->sc19->texID = 1209;this->sc19->order = 1;
	this->sc19->Initialize("./models/skyscraper19.obj", "./models/skyscraper19.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->sc21->texID = 1210;this->sc21->order = 1;
	this->sc21->Initialize("./models/skyscraper21.obj", "./models/skyscraper21.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->bush->texID = 1211;this->bush->order = 1;
	this->bush->Initialize("./models/bush.obj", "./models/bush.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->hill->texID = 1212;this->hill->order = 1;
	this->hill->Initialize("./models/mtnPass.obj", "./models/mtnPass.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->grass->texID = 1213;
	this->grass->Initialize(15, 200.0f, "./textures/grass.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	bushX = new int[10];bushY = new int[10];
	bushX[0] = 175.0f;bushY[0] = 20.0f;
	bushX[1] = 175.0f;bushY[1] = 200.0f;
	bushX[2] = 170.0f;bushY[2] = 100.0f;
	bushX[3] = -185.0f;bushY[3] = 270.0f;
	bushX[4] = -185.0f;bushY[4] = 150.0f;
	bushX[5] = -185.0f;bushY[5] = 75.0f;
	bushX[6] = 175.0f;bushY[6] = 300.0f;
	bushX[7] = -180.0f;bushY[7] = 82.0f;
	bushX[8] = -180.0f;bushY[8] = 83.0f;
	bushX[9] = -170.0f;bushY[9] = 237.0f;

	grassX = new int[20];grassY = new int[20];
	for(int i=0; i<20; i++){
		grassX[i] = rand() % 5 + 50;grassY[i] = i * 360/20;
	}

	if (this->GLReturnedError("DiscStadium::Initialize - on exit"))
		return false;

	return true;
}

void DiscStadium::StepShader(){}

//Delete the DiscStadium object
void DiscStadium::TakeDown()
{
	this->floor = NULL;
	this->innerWall = NULL;
	this->outerWall = NULL;
	this->triLight = NULL;
	this->triLight2 = NULL;
	this->naboo = NULL;
	this->building2 = NULL;
	this->building3 = NULL;
	this->building = NULL;
	this->building1 = NULL;
	this->building10 = NULL;
	this->bridge = NULL;
	this->sc4 = NULL;
	this->sc1 = NULL;
	this->sc19 = NULL;
	this->sc21 = NULL;
	this->bush = NULL;
	this->hill = NULL;
	this->plant = NULL;
	this->vertices.clear();
	this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}




void DiscStadium::setCamInternals(float angle, float ratio, float nearD, float farD){
	// half of the the horizontal field of view
	float angleX;
	// store the information
	this->ratio = ratio;
	this->nearD = nearD;
	this->farD = farD;

	tang = (float)tan(HALF_ANG2RAD * angle) ;
	height = nearD * tang;
	width = height * ratio;

	angle *= HALF_ANG2RAD;
	// compute width and height of the near and far plane sections
	tang = tan(angle);
	sphereFactorY = 1.0/cos(angle);

	// compute half of the the horizontal field of view and sphereFactorX
	float anglex = atan(tang*ratio);
	sphereFactorX = 1.0/cos(anglex);

	X = vec3(1, 0, 0);
	Y = vec3(0, 1, 0);
	Z = vec3(0, 0, 1);	
}


bool DiscStadium::shouldIDrawThis(vec3 objectPosition, float radius) {

	float d;
	float az,ax,ay;
	bool result = true;
	//int result2 = 0;
	cc = vec3(userX, 0, userZ);

	cout << "objectPosition = " << objectPosition.x << ", " << objectPosition.y << ", " << objectPosition.z << ";" << endl;
	cout << "cc = " << cc.x << ", " << cc.y << ", " << cc.z << ";" << endl;

	vec3 v = objectPosition-cc;

	az = v.x * -Z.x + v.y * -Z.y + v.z * -Z.z; 
	if (az > farD + radius || az < nearD-radius){
		cout << "false" << endl;
 		return false;
	}

 	//if (az > farD - radius || az < nearD+radius)
 		//result2 = -99999;

 	ay = v.x * Y.x + v.y * Y.y + v.z * Y.z;
 	d = sphereFactorY * radius;
 	az *= tang;
 	if (ay > az+d || ay < -az-d){
		cout << "false" << endl;
 		return false;
	}

 	//if (ay > az-d || ay < -az+d)
 		//result2 = -99999;

 	ax = v.x * X.x + v.y * X.y + v.z * X.z;
 	az *= ratio;
 	d = sphereFactorX * radius;
 	if (ax > az+d || ax < -az-d){
		cout << "false" << endl;
 		return false;
	}

 	//if (ax > az-d || ax < -az+d)
		//result2 = -99999;

	cout << "true" << endl;
	return true;
}














bool DiscStadium::shouldDisplay(float objX, float objZ, float radius, bool print){

	float a = objX - userX;
	float b = objZ - userZ;
	float c = nextX - userX;
	float d = nextZ - userZ;
	float c2 = nextX2 - userX;
	float d2 = nextZ2 - userZ;
	float c3 = nextX3 - userX;
	float d3 = nextZ3 - userZ;

	float a4 = objX - userX;
	float b4 = objZ - userZ;
	float c4 = nextX - userX;
	float d4 = nextZ - userZ;

	float dot = a * c + b * d;
	float len_sq = c * c + d * d;
	float param = dot / len_sq;

	float dot2 = a * c2 + b * d2;
	float len_sq2 = c2 * c2 + d2 * d2;
	float param2 = dot2 / len_sq2;

	float dot3 = a * c3 + b * d3;
	float len_sq3 = c3 * c3 + d3 * d3;
	float param3 = dot3 / len_sq3;

	float xx,yy, xx2, yy2, xx3, yy3;//the coordinates of the point on the line segment closest to the test point

	/*if(param < 0 && param2 < 0 && param3 < 0){
		cout << "false" << endl;
		return false;
	}*/

	float diffX = userX - objX;float diffY = userZ - objZ;
	float dist = sqrt(diffX*diffX + diffY*diffY);

	if(param < 0 && dist > 50){
		if(print){cout << "false - behind user" << endl;}
		return false;
	}

	if(param > 1){
		xx = nextX; yy = nextZ;
	}
	else if(param < 0){
		xx = userX;yy = userZ;
	}
	else{
		xx = userX + param * c;
		yy = userZ + param * d;
	}

	diffX = xx - objX;diffY = yy - objZ;
	dist = sqrt(diffX*diffX + diffY*diffY);


	if(param2 < 0){
		xx2 = userX;yy2 = userZ;		
	}
	else if(param2 > 1){
		xx2 = nextX2; yy2 = nextZ2;
	}
	else{
		xx2 = userX + param2 * c2;
		yy2 = userZ + param2 * d2;
	}

	diffX = xx2 - objX;diffY = yy2 - objZ;
	float dist2 = sqrt(diffX*diffX + diffY*diffY);


	if(param3 < 0){
		xx3 = userX;yy3 = userZ;		
	}
	else if(param3 > 1){
		xx3 = nextX3; yy3 = nextZ3;
	}
	else{
		xx3 = userX + param3 * c3;
		yy3 = userZ + param3 * d3;
	}

	diffX = xx3 - objX;diffY = yy3 - objZ;
	float dist3 = sqrt(diffX*diffX + diffY*diffY);


	if(dist < radius || dist2 < radius || dist3 < radius){
		if(print){cout << "true" << endl;}
		return true;
	}
	
	if(print){cout << "false - all lines are too far away" << endl;}
	return false;
}

//Draw and position the elements of the DiscStadium
void DiscStadium::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("DiscStadium::Draw - on entry"))
		return;

	glEnable(GL_DEPTH_TEST);

	mat4 another, scaler;
	ivec2 light = ivec2(0,0);


	////////////////////X is vertical on track (-x is starting line; +x is bridge)
	////////////////////Z is horizontal on track (-z is second half of track; +z is first half of track)


	//cout << "userX = " << userX << "; userY = " << userY << "; userZ = " << userZ << ";" << endl;

	//cout << "start walls" << endl;
	//center
	if(shouldDisplay(0, 0, 1000, false) || shouldDisplay(0, -150, 1000, false) || shouldDisplay(-300, -15, 600, false)){
	//if(shouldIDrawThis(vec3(0, 0, 0), 200)){
	another = modelview;
	another = translate(modelview, vec3(80.0f,-4.5f,-100.0f));
	another = scale(another, vec3(2.25f, 0.5f, 2.0f));
	this->naboo->Draw(projection, another, size, time);
	}

	/*for(int i=0; i<20; i++){
		another = translate(modelview, vec3(grassX[i]*cos((float)(grassY[i])), -5.0f,grassX[i]*sin((float)(grassY[i]))));
		another = rotate(another, 50.0f, vec3(0,1,0));
		another = scale(another, vec3(15.5f, 3.0f, 15.5f));
		this->plant->Draw(projection, another, size, 0);
	}
	*/
	/*another = translate(modelview, vec3(-20.0f, -1.0f,0.0f));
	another = rotate(another, 50.0f, vec3(0,1,0));
	another = scale(another, vec3(15.5f, 1.0f, 15.5f));
	this->hill->Draw(projection, another, size, 0);*/

	/*another = translate(modelview, vec3(0.0f, -3.45f,0.0f));
	another = rotate(another, 50.0f, vec3(0,1,0));
	another = scale(another, vec3(0.635f, 0.015f, 0.635f));
	this->bush->Draw(projection, another, size, 0);*/

	another = translate(modelview, vec3(-10.0f, 0.0f,0.0f));
	another = scale(another, vec3(1.0f, 0.01f, 1.0f));
	this->grass->Draw(projection, another, size, 0);

	
	
		for(int i=0; i<10; i++){
			another = translate(modelview, vec3(bushX[i]*cos((float)(bushY[i])), -4.0f,bushX[i]*sin((float)(bushY[i]))));
			//if(shouldDisplay(bushX[i]*cos((float)(bushY[i])), bushX[i]*sin((float)(bushY[i])), 1000, true)){
			another = rotate(another, float(bushY[i]), vec3(0,1,0));
			another = scale(another, vec3(0.065f, 0.035f, 0.065f));
			
				this->bush->Draw(projection, another, size, 0);
			//}
		}
	

	another = translate(modelview, vec3(-135.0f,-5.0f,-140.0f));
	another = rotate(another, 50.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.035f, 0.035f));
	this->bush->Draw(projection, another, size, 0);
	

	//first half of lap (outer)

	//if(userX > -100){


	if(shouldDisplay(340, 60, 1000, false) || shouldDisplay(340, 90, 1000, false) || shouldDisplay(340, -30, 1000, false)){
		another = translate(modelview, vec3(-340.0f, -5.0f,-60.0f));
		another = rotate(another, 50.0f, vec3(0,1,0));
		another = scale(another, vec3(5.5f, 5.0f, 5.5f));
		this->hill->Draw(projection, another, size, 0);
	}

	if(shouldDisplay(230, 20, 1000, true)  || shouldDisplay(200, 150, 1000, true)){
	another = translate(modelview, vec3(-230.0f, 20.0f,-130.0f));
	another = rotate(another, 60.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.02f, 0.035f));
	this->sc21->Draw(projection, another, size, 0);
	}
	//}

	another = translate(modelview, vec3(-185.0f, 28.5f,-195.0f));
	another = rotate(another, 45.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.025f, 0.035f));
	this->sc4->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-135.0f, 30.0f,-225.0f));
	another = rotate(another, 30.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.02f, 0.035f));
	this->sc19->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-85.0f, 30.0f,-245.0f));
	another = rotate(another, 10.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.02f, 0.035f));
	this->sc19->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-35.0f, -1.5f,-273.0f));
	another = rotate(another, 10.0f, vec3(0,1,0));
	another = scale(another, vec3(2.25f, 2.2f, 2.25f));
	this->sc1->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(25.0f, 20.0f,-250.0f));
	another = rotate(another, -10.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->building3->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(80.0f, 25.0f,-235.0f));
	another = rotate(another, -25.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->building2->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(200.0f, 13.5f,-180.0f));
	another = rotate(another, -170.0f, vec3(0,1,0));
	another = scale(another, vec3(0.0005f, 0.00035f, 0.0005f));
	this->building1->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(130.0f, 22.0f,-240.0f));
	another = rotate(another, -130.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->sc4->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(180.0f, 0.0f,-200.0f));
	another = rotate(another, -150.0f, vec3(0,1,0));
	this->building->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(240.0f, 20.0f,-175.0f));
	another = rotate(another, -140.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.02f, 0.035f));
	this->sc21->Draw(projection, another, size, 0);

	//2nd half of lap (outer)

	if(userX > -100){
	another = translate(modelview, vec3(-285.0f, 25.0f,40.0f));
	another = rotate(another, 5.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->building2->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-272.0f, 0.0f,58.0f));
	another = rotate(another, 12.0f, vec3(0,1,0));
	this->building->Draw(projection, another, size, 0);
	}

	another = translate(modelview, vec3(-258.0f, 22.0f,118.0f));
	another = rotate(another, 20.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->sc4->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-215.0f, 20.0f,230.0f));
	another = rotate(another, 40.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.02f, 0.035f));
	this->sc21->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-170.0f, -1.5f,235.0f));
	another = scale(another, vec3(2.25f, 2.2f, 2.25f));
	this->sc1->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-95.0f, 30.0f,270.0f));
	another = rotate(another, 60.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.02f, 0.035f));
	this->sc19->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-60.0f, 13.5f,265.0f));
	another = rotate(another, 70.0f, vec3(0,1,0));
	another = scale(another, vec3(0.0005f, 0.00035f, 0.0005f));
	this->building1->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(-5.0f, 25.0f,280.0f));
	another = rotate(another, 85.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->building2->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(50.0f, 20.0f,330.0f));
	another = rotate(another, 5.0f, vec3(0,1,0));
	another = scale(another, vec3(0.027f, 0.02f, 0.027f));
	this->sc21->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(100.0f, 20.0f,230.0f));
	another = rotate(another, -150.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->building3->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(157.0f, -1.5f,230.0f));
	another = rotate(another, 60.0f, vec3(0,1,0));
	another = scale(another, vec3(2.25f, 2.2f, 2.25f));
	this->sc1->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(185.0f, 0.0f,185.0f));
	another = rotate(another, 40.0f, vec3(0,1,0));
	this->building->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(233.0f, 22.0f,165.0f));
	another = rotate(another, 60.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->sc4->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(255.0f, 22.0f,120.0f));
	another = rotate(another, 65.0f, vec3(0,1,0));
	another = scale(another, vec3(0.025f, 0.02f, 0.025f));
	this->sc4->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(253.0f, 30.0f,88.0f));
	another = rotate(another, -10.0f, vec3(0,1,0));
	another = scale(another, vec3(0.035f, 0.02f, 0.035f));
	this->sc19->Draw(projection, another, size, 0);

	another = translate(modelview, vec3(260.0f, -1.5f,20.0f));
	another = scale(another, vec3(2.2f, 2.4f, 2.2f));
	this->sc1->Draw(projection, another, size, 0);

	//half way through lap

	if(userX > -100){
	another = translate(modelview, vec3(-235.0f, 0.0f,-35.0f));
	another = scale(another, vec3(25.0f, 25.0f, 15.0f));
	this->bridge->Draw(projection, another, size, time);
	}

	another = translate(modelview, vec3(-10.0f,0.0f,0.0f));

	mat4 center = another;


	glCullFace(GL_FRONT);
	this->innerWall->Draw(projection, another, size, time);
	this->outerWall->Draw(projection, another, size, time);
	center = scale(center, vec3(0.005f, 1.0f, 0.005f));
	this->innerWall->Draw(projection, center, size, time);
	glCullFace(GL_BACK);
	this->innerWall->Draw(projection, another, size, time);
	this->outerWall->Draw(projection, another, size, time);
	this->innerWall->Draw(projection, center, size, time);
	//cout << "end walls / start floor" << endl;
	another = translate(modelview, vec3(-300.0f,0,-300.0f));
	another = rotate(another, 90.0f, vec3(1,0,0));
	this->floor->Draw(projection, another, size, 0);

	//cout << "end floors" << endl;

	if (this->GLReturnedError("DiscStadium::Draw - on exit"))
		return;
}
