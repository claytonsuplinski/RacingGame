/*	Perry Kivolowitz - University of Wisconsin - Madison 
	Computer Sciences Department

	A sample hello world like program demonstrating modern
	OpenGL techniques. 

	Created:	2/25/13
	Updates:
*/

#pragma once
#include "object2.h"
#include "shader2.h"
#include "texturedshader.h"
#include "TextureManager.h"

/*	This is a concrete instanciation of the abstract Object() class.
*/

class Gengar2 : public Object2
{
public:
	Gengar2();
	virtual bool Initialize(const char * objName, const char * jpgName, char* v, char* f);
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);
	void TakeDown();
	void StepShader();
	char * filename;
	unsigned int texID;
	int order;

	std::vector<glm::vec4> gengar2_vertices;
	std::vector<glm::vec3> gengar2_normals;
	std::vector<glm::vec2> gengar2_textures;
	std::vector<GLushort> gengar2_faces;

private:
	typedef Object2 super;
	int shader_index;

	Shader2 pattern_shader;
	Shader2 basic_shader;
	Shader2 colored_shader;
	Shader2 play_shader;
	TexturedShader basic_texture_shader;
	std::vector<Shader2 *> shaders;
};