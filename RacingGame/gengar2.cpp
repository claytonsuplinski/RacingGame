/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 2

Creates a bowl object (same process as creating a sphere, except 
only goes halfway through the process)
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "gengar2.h"

using namespace std;
using namespace glm;

//Creates the bowl object
Gengar2::Gengar2() : Object2()
{
	this->shader_index = 0;
}

void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<glm::vec2> &textures, vector<GLushort> &elements) {
  ifstream in(filename, ios::in);
  if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }
  string line;
  while (getline(in, line)) {
	if (line == ""){

	}
    else if (line.substr(0,2) == "v ") {
      istringstream s(line.substr(2));
      glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
      vertices.push_back(v);
    }  else if (line.substr(0,2) == "f ") {
        istringstream s(line.substr(2));
		GLuint a1,a2,a3,a4,a5,a6,a7,a8,a9;
		s >> a1; s>>a2; s >>a3; s>>a4; s >>a5;
		s>>a6; s >>a7; s>>a8; s >>a9;
		elements.push_back(a1);elements.push_back(a2);elements.push_back(a3);
		elements.push_back(a4);elements.push_back(a5);elements.push_back(a6);
		elements.push_back(a7);elements.push_back(a8);elements.push_back(a9);
    } else if (line.substr(0,2) == "vn") {
      istringstream s(line.substr(2));
      glm::vec3 r; s >> r.x; s >> r.y; s >> r.z;
      normals.push_back(r);
    } else if (line.substr(0,2) == "vt") {
      istringstream s(line.substr(2));
      glm::vec2 t; s >> t.x; s >> t.y;
      textures.push_back(t);
    }
    else if (line[0] == '#') { /* ignoring this line */ }
    else { /* ignoring this line */ }
  }
}

//Create a bowl object with the specified parameters
bool Gengar2::Initialize(const char * objName, const char * jpgName, char* v, char* f)
{
	//load_obj("gengar.obj", gengar_vertices, gengar_normals, gengar_textures, gengar_faces);
	if (this->GLReturnedError("Gengar2::Initialize - on entry"))
		return false;

	load_obj(objName, gengar2_vertices, gengar2_normals, gengar2_textures, gengar2_faces);

	#pragma region Shader code should not be here for better O.O.
	//	Nothing shader related ought to be here for better O.O.

	if (!this->basic_texture_shader.Initialize(v,f))
		return false;

	this->shaders.push_back(&this->basic_texture_shader);
#pragma endregion

#pragma region Code to make FreeImage work
	//	The picture too, ought not to be here.

	if (!TextureManager::Inst()->LoadTexture(jpgName, texID))
		return false;

	//	The current "TextureManager" that comes with FreeImage is quite dumb.
	glTexEnvf(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);
#pragma endregion
	if(gengar2_normals.size() == 0){
		//Loop,  for 3 gengar_faces at a time (9 vector values); 1st index is index for vertex, 2nd index is normal, 3rd index is texture	
		for(int i=0; i< (int)(gengar2_faces.size()/6); i++){
			cout << "Here"<<endl;
				VertexAttributes cur_vertex , nxt_vertex, dwn_vertex;
				cur_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i)-1).z);
				cur_vertex.normal = vec3(1,0,0);
				cur_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+1)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+1)-1).y);

				dwn_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i+2)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i+2)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i+2)-1).z);
				dwn_vertex.normal = vec3(1,0,0);
				dwn_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+3)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+3)-1).y);
		
				nxt_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i+4)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i+4)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i+4)-1).z);
				nxt_vertex.normal = vec3(1,0,0);
				nxt_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+5)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+5)-1).y);
			
				this->vertices.push_back(nxt_vertex);
				this->vertices.push_back(dwn_vertex);			
				this->vertices.push_back(cur_vertex);
	
				this->vertex_indices.push_back(this->vertices.size() - 1);
				this->vertex_indices.push_back(this->vertices.size() - 2);
				this->vertex_indices.push_back(this->vertices.size() - 3);
		}
	}
	else if(this->order == 1){
		//Loop,  for 3 gengar2_faces at a time (9 vector values); 1st index is index for vertex, 2nd index is normal, 3rd index is texture	
		for(int i=0; i< (int)(gengar2_faces.size()/9); i++){
			cout << "Here"<<endl;
				VertexAttributes cur_vertex , nxt_vertex, dwn_vertex;
				cur_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i)-1).z);
				cur_vertex.normal = vec3(gengar2_normals.at(gengar2_faces.at(9*i+2)-1).x, gengar2_normals.at(gengar2_faces.at(9*i+2)-1).y,
					gengar2_normals.at(gengar2_faces.at(9*i+2)-1).z);
				cur_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+1)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+1)-1).y);

				dwn_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i+3)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i+3)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i+3)-1).z);
				dwn_vertex.normal = vec3(gengar2_normals.at(gengar2_faces.at(9*i+5)-1).x, gengar2_normals.at(gengar2_faces.at(9*i+5)-1).y,
					gengar2_normals.at(gengar2_faces.at(9*i+5)-1).z);
				dwn_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+4)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+4)-1).y);
		
				nxt_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i+6)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i+6)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i+6)-1).z);
				nxt_vertex.normal = vec3(gengar2_normals.at(gengar2_faces.at(9*i+8)-1).x, gengar2_normals.at(gengar2_faces.at(9*i+8)-1).y,
					gengar2_normals.at(gengar2_faces.at(9*i+8)-1).z);
				nxt_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+7)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+7)-1).y);
			
				this->vertices.push_back(nxt_vertex);
				this->vertices.push_back(dwn_vertex);			
				this->vertices.push_back(cur_vertex);
	
				this->vertex_indices.push_back(this->vertices.size() - 1);
				this->vertex_indices.push_back(this->vertices.size() - 2);
				this->vertex_indices.push_back(this->vertices.size() - 3);
		}
	}
	else{
		//Loop,  for 3 gengar_faces at a time (9 vector values); 1st index is index for vertex, 2nd index is normal, 3rd index is texture	
		for(int i=0; i< (int)(gengar2_faces.size()/9); i++){
				VertexAttributes cur_vertex , nxt_vertex, dwn_vertex;
				cur_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i)-1).z);
				cur_vertex.normal = vec3(gengar2_normals.at(gengar2_faces.at(9*i+1)-1).x, gengar2_normals.at(gengar2_faces.at(9*i+1)-1).y,
					gengar2_normals.at(gengar2_faces.at(9*i+1)-1).z);
				cur_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+2)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+2)-1).y);

				dwn_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i+3)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i+3)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i+3)-1).z);
				dwn_vertex.normal = vec3(gengar2_normals.at(gengar2_faces.at(9*i+4)-1).x, gengar2_normals.at(gengar2_faces.at(9*i+4)-1).y,
					gengar2_normals.at(gengar2_faces.at(9*i+4)-1).z);
				dwn_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+5)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+5)-1).y);
		
				nxt_vertex.position = vec3(gengar2_vertices.at(gengar2_faces.at(9*i+6)-1).x, gengar2_vertices.at(gengar2_faces.at(9*i+6)-1).y,
					gengar2_vertices.at(gengar2_faces.at(9*i+6)-1).z);
				nxt_vertex.normal = vec3(gengar2_normals.at(gengar2_faces.at(9*i+7)-1).x, gengar2_normals.at(gengar2_faces.at(9*i+7)-1).y,
					gengar2_normals.at(gengar2_faces.at(9*i+7)-1).z);
				nxt_vertex.texture_coordinate = vec2(gengar2_textures.at(gengar2_faces.at(9*i+8)-1).x, gengar2_textures.at(gengar2_faces.at(9*i+8)-1).y);
			
				this->vertices.push_back(nxt_vertex);
				this->vertices.push_back(dwn_vertex);			
				this->vertices.push_back(cur_vertex);
	
				this->vertex_indices.push_back(this->vertices.size() - 1);
				this->vertex_indices.push_back(this->vertices.size() - 2);
				this->vertex_indices.push_back(this->vertices.size() - 3);
		}
	}

if (!this->PostGLInitialize(&this->vertex_array_handle, &this->vertex_coordinate_handle, this->vertices.size() * sizeof(VertexAttributes), &this->vertices[0]))
		return false;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), (GLvoid *) (sizeof(vec3)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), (GLvoid *) (sizeof(vec3) * 2));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), (GLvoid *) (sizeof(vec3) * 3));

	//	Each of the attributes to be used must be enabled.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	if (this->GLReturnedError("Gengar2::Initialize - on exit"))
		return false;

	return true;
}

//Delete the bowl object
void Gengar2::TakeDown()
{
	super::TakeDown();
}

//Draws the bowl object with respect to the lighting
void Gengar2::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Gengar2::Draw - on entry"))
		return;

	mat4 mvp = projection * modelview;
	mat3 nm = inverse(transpose(mat3(modelview)));

	this->shaders[this->shader_index]->Use();
	TextureManager::Inst()->BindTexture(texID, 0);
	this->shaders[this->shader_index]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	this->shaders[this->shader_index]->CustomSetup();

	glBindVertexArray(this->vertex_array_handle);
#ifdef DRAW_POINTS
	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, this->vertex_indices.size());
#else
	glDrawElements(GL_TRIANGLES , this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
#endif

	glUseProgram(0);
	glBindVertexArray(0);

	if (this->GLReturnedError("Gengar2::Draw - on exit"))
		return;
}