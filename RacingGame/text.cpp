/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 2

Creates a tiger object out of cylinders, discs, and spheres.
*/

#include <iostream>
#include "text.h"

using namespace std;
using namespace glm;

//Create a tiger object
Text::Text() : Object()
{
}

Text::~Text()
{
	this->TakeDown();
}

//Initialize the elements of the tiger object
bool Text::Initialize()
{
	if (this->GLReturnedError("Text::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	/* Initialize the FreeType2 library */
        if(FT_Init_FreeType(&ft)) {
                fprintf(stderr, "Could not init freetype library\n");
                return 0;
        }

        /* Load a font */
        if(FT_New_Face(ft, fontfilename, 0, &face)) {
                fprintf(stderr, "Could not open font %s\n", fontfilename);
                return 0;
        }

        /* Compile and link the shader program */
        GLint link_ok = GL_FALSE;

        GLuint vs, fs;
        if ((vs = create_shader("text.vert", GL_VERTEX_SHADER))        == 0) return 0;
        if ((fs = create_shader("text.frag", GL_FRAGMENT_SHADER)) == 0) return 0;

        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
        if (!link_ok) {
                fprintf(stderr, "glLinkProgram:");
                print_log(program);
                return 0;
        }

        const char* attribute_name;
        attribute_name = "coord";
        attribute_coord = glGetAttribLocation(program, attribute_name);
        if (attribute_coord == -1) {
                fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
                return 0;
        }

        const char* uniform_name;
        uniform_name = "tex";
        uniform_tex = glGetUniformLocation(program, uniform_name);
        if (uniform_tex == -1) {
                fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
                return 0;
        }

        uniform_name = "color";
        uniform_color = glGetUniformLocation(program, uniform_name);
        if (uniform_color == -1) {
                fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
                return 0;
        }

        // Create the vertex buffer object
        glGenBuffers(1, &vbo);

	if (this->GLReturnedError("Text::Initialize - on exit"))
		return false;

	return true;
}

void Text::render_text(const char *text, float x, float y, float sx, float sy) {
        const char *p;
        FT_GlyphSlot g = face->glyph;

        /* Create a texture that will be used to hold one "glyph" */
        GLuint tex;
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(uniform_tex, 0);

        /* We require 1 byte alignment when uploading texture data */
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        /* Clamping to edges is important to prevent artifacts when scaling */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        /* Linear filtering usually looks best for text */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /* Set up the VBO for our vertex data */
        glEnableVertexAttribArray(attribute_coord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

        /* Loop through all characters */
        for(p = text; *p; p++) {
                /* Try to load and render the character */
                if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
                        continue;

                /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
                glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

                /* Calculate the vertex and texture coordinates */
                float x2 = x + g->bitmap_left * sx;
                float y2 = -y - g->bitmap_top * sy;
                float w = g->bitmap.width * sx;
                float h = g->bitmap.rows * sy;

                point box[4] = {
                        {x2,     -y2    , 0, 0},
                        {x2 + w, -y2    , 1, 0},
                        {x2,     -y2 - h, 0, 1},
                        {x2 + w, -y2 - h, 1, 1},
                };

                /* Draw the character on the screen */
                glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                /* Advance the cursor to the start of the next character */
                x += (g->advance.x >> 6) * sx;
                y += (g->advance.y >> 6) * sy;
        }

        glDisableVertexAttribArray(attribute_coord);
        glDeleteTextures(1, &tex);
}

void Text::StepShader(){
}

//Delete the elements of the tiger
void Text::TakeDown()
{
	glDeleteProgram(program);
	this->vertices.clear();
	this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}

//Draw a tiger object
void Text::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float rotY)
{
	if (this->GLReturnedError("Text::Draw - on entry"))
		return;

	glEnable(GL_DEPTH_TEST);

	float sx = 2.0 / glutGet(GLUT_WINDOW_WIDTH);
    float sy = 2.0 / glutGet(GLUT_WINDOW_HEIGHT);

    glUseProgram(program);

    /* White background */
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Enable blending, necessary for our alpha texture */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat black[4] = {0, 0, 0, 1};
    GLfloat red[4] = {1, 0, 0, 1};
    GLfloat transparent_green[4] = {0, 1, 0, 0.5};

    /* Set font size to 48 pixels, color to black */
    FT_Set_Pixel_Sizes(face, 0, 48);
    glUniform4fv(uniform_color, 1, black);

    /* Effects of alignment */
    render_text("The Quick Brown Fox Jumps Over The Lazy Dog",          -1 + 8 * sx,   1 - 50 * sy,    sx, sy);
    render_text("The Misaligned Fox Jumps Over The Lazy Dog",           -1 + 8.5 * sx, 1 - 100.5 * sy, sx, sy);

    /* Scaling the texture versus changing the font size */
    render_text("The Small Texture Scaled Fox Jumps Over The Lazy Dog", -1 + 8 * sx,   1 - 175 * sy,   sx * 0.5, sy * 0.5);
    FT_Set_Pixel_Sizes(face, 0, 24);
    render_text("The Small Font Sized Fox Jumps Over The Lazy Dog",     -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
    FT_Set_Pixel_Sizes(face, 0, 48);
    render_text("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog",  -1 + 8 * sx,   1 - 235 * sy,   sx * 0.25, sy * 0.25);
    FT_Set_Pixel_Sizes(face, 0, 12);
    render_text("The Tiny Font Sized Fox Jumps Over The Lazy Dog",      -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
    FT_Set_Pixel_Sizes(face, 0, 48);

    /* Colors and transparency */
    render_text("The Solid Black Fox Jumps Over The Lazy Dog",          -1 + 8 * sx,   1 - 430 * sy,   sx, sy);

    glUniform4fv(uniform_color, 1, red);
    render_text("The Solid Red Fox Jumps Over The Lazy Dog",            -1 + 8 * sx,   1 - 330 * sy,   sx, sy);
    render_text("The Solid Red Fox Jumps Over The Lazy Dog",            -1 + 28 * sx,  1 - 450 * sy,   sx, sy);

    glUniform4fv(uniform_color, 1, transparent_green);
    render_text("The Transparent Green Fox Jumps Over The Lazy Dog",    -1 + 8 * sx,   1 - 380 * sy,   sx, sy);
    render_text("The Transparent Green Fox Jumps Over The Lazy Dog",    -1 + 18 * sx,  1 - 440 * sy,   sx, sy);

	if (this->GLReturnedError("Text::Draw - on exit"))
		return;
}
