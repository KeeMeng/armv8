#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <assert.h>
#include "stb_image.h"
#include "utilities.h"

// load text file
static const char* loadShaderFile(const char* srcName) {
	// attempt to open file
	FILE* inFile = fopen(srcName, "r");

	// check if file opened correctly
	if (inFile != NULL) {
		// get file size
		fseek(inFile, 0, SEEK_END);
		int fileSize = ftell(inFile);
		rewind(inFile);

		// read data
		char* data;
		SAFE_MALLOC(data, fileSize * sizeof(char));

		int bytesRead = 0;

		while (bytesRead < fileSize) {
			bytesRead += fread(data + bytesRead, sizeof(char), fileSize - bytesRead, inFile);
		}

		// terminate string
		data[fileSize - 1] = '\0';

		// close file
		fclose(inFile);

		return (const char*) data;
	} else {
		fprintf(stderr, "Error - failed to open file %s\n", srcName);
	}
	return (const char*) NULL;
}

static unsigned int buildShader(const char* path, GLint64 type) {
	// load shader source
	const char* src = loadShaderFile(path);

	// create shader object
	unsigned int shader = glCreateShader(type);

	// attach shader source code
	glShaderSource(shader, 1, &src, NULL);

	// attempt to compile shader
	glCompileShader(shader);

	// check for compile errors
	int success;
	char infoLog[512] = "hello world\0";

	printf("Source: %s\n", src);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		// get info log and report error
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		fprintf(stderr, "Error - shader %s failed to load: %s\n", path, infoLog);
		exit(1);
	}

	// free shader source
	free((char*) src);

	return shader;
}

void initialiseShaderProgram(Renderer* r) {
	unsigned int vertexShader = buildShader("./shaders/vertex.glsl", GL_VERTEX_SHADER);
	unsigned int fragmentShader = buildShader("./shaders/fragment.glsl", GL_FRAGMENT_SHADER);

	r->shaderProgram = glCreateProgram();
	glAttachShader(r->shaderProgram, vertexShader);
	glAttachShader(r->shaderProgram, fragmentShader);

	// link program
	glLinkProgram(r->shaderProgram);

	// check for failure
	int success;
	glGetProgramiv(r->shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(r->shaderProgram, 512, NULL, infoLog);
		fprintf(stderr, "Error - faileed to link shader program: %s\n", infoLog);
		exit(1);
	}

	// delete now unneeded shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	printf("Initialised shader program %d\n", r->shaderProgram);
}

// sprite renderer
// this needs to draw textured quads that may be transformed by a translation enlargement and rotation

void initialiseRendererSprite(RendererSprite* r) {
	// set coordinates

	// top left
	r->coords[0] = 0.0; // pos x
	r->coords[1] = 1.0; // pos y
	r->coords[2] = 0.0; // tex x
	r->coords[3] = 1.0; // tex y

	// bottom right
	r->coords[4] = 1.0; // pos x
	r->coords[5] = 0.0; // pos y
	r->coords[6] = 1.0; // tex x
	r->coords[7] = 0.0; // tex y

	// bottom left
	r->coords[8] = 0.0; // pos x
	r->coords[9] = 0.0; // pos y
	r->coords[10] = 0.0; // tex x
	r->coords[11] = 0.0; // tex y

	// top left
	r->coords[12] = 0.0; // pos x
	r->coords[13] = 1.0; // pos y
	r->coords[14] = 0.0; // tex x
	r->coords[15] = 1.0; // tex y

	// top right
	r->coords[16] = 1.0; // pos x
	r->coords[17] = 1.0; // pos y
	r->coords[18] = 1.0; // tex x
	r->coords[19] = 1.0; // tex y

	// bottom right
	r->coords[20] = 1.0; // pos x
	r->coords[21] = 0.0; // pos y
	r->coords[22] = 1.0; // tex x
	r->coords[23] = 0.0; // tex y

	// create vertex buffer object
	glGenBuffers(1, &r->vbo);
	glGenVertexArrays(1, &r->vao);

	// select the vbo and set its data to be the vertices
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(r->coords), r->coords, GL_STATIC_DRAW);

	// select the vao and set its vertex attribute pointers
	glBindVertexArray(r->vao);

	// specify location of vertex position in buffer data
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	// specify location of texture coordinate in buffer data
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// deselect vertex array and buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// initialise renderer
void initialiseRenderer(Renderer* r) {
	// enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// create shader program
	initialiseShaderProgram(r);

	// initialise font
	initialiseFont(r);

	// create sprite renderer
	initialiseRendererSprite(&r->spriteRenderer);
}

// render sprite
void renderSprite(Renderer* r, const Sprite* spr) {
	// create transformation matrix
	Mat4x4f transform = matEnlarge3d4x4f(spr->size.x, spr->size.y, 1.0, 1.0);
	transform = matMultiply4x4f(mat3dTranslation4x4f(-spr->origin.x, -spr->origin.y, 0.0), transform);
	transform = matMultiply4x4f(matEnlarge3d4x4f(spr->scale.x, spr->scale.y, 1.0, 1.0), transform);
	transform = matMultiply4x4f(mat3dRotationZ4x4f(spr->angle), transform);
	transform = matMultiply4x4f(mat3dTranslation4x4f(spr->position.x, spr->position.y, 0.0), transform);

	Mat4x4f projection = pixelCoordToNDC2d4x4f(800, 600);

	// compute matrix for transforming tex coordinates
	// (0,0) -> (left, bottom)
	// (1,1) -> (right, top)
	// x -> left + (right - left) * x
	// y -> bottom + (top - bottom) * y

	Rectanglef texCoords = spr->texCoords;

	if (spr->texture != NULL) {
		texCoords.left /= spr->texture->width;
		texCoords.right /= spr->texture->width;
		texCoords.bottom /= spr->texture->height;
		texCoords.top /= spr->texture->height;
	}

	Mat3x3f texCoordTransform = matEnlarge2d3x3f(texCoords.right - texCoords.left, texCoords.top - texCoords.bottom);
	texCoordTransform = matMultiply3x3f(mat3dTranslation3x3f(texCoords.left, texCoords.bottom, 0.0), texCoordTransform);

	// get uniform location
	int projLoc = glGetUniformLocation(r->shaderProgram, "project");
	int modLoc = glGetUniformLocation(r->shaderProgram, "model");
	int coordLoc = glGetUniformLocation(r->shaderProgram, "texCoordsTransform");
	int colLoc = glGetUniformLocation(r->shaderProgram, "spriteColour");
	int useTextureLoc = glGetUniformLocation(r->shaderProgram, "useTexture");

	if (spr->texture != NULL) {
		glBindTexture(GL_TEXTURE_2D, spr->texture->textureId);
	}

	// use shader
	glUseProgram(r->shaderProgram);

	/*
		We need to set the following uniforms:
		Vertex Shader:
		mat4 projection - converts world coords to NDC
		mat4 model - transforms rectangle according to world coords

		Fragment Shader:
		sampler2D image
		vec3 spriteColour
	*/

	// set transform uniform
	glUniformMatrix4fv(projLoc, 1, 1, (float*) projection.data);
	glUniformMatrix4fv(modLoc, 1, 1, (float*) transform.data);
	glUniformMatrix3fv(coordLoc, 1, 1, (float*) texCoordTransform.data);
	glUniform3fv(colLoc, 1, (const GLfloat*) &spr->colour);

	if (spr->texture != NULL) {
		glUniform1i(useTextureLoc, true);
	} else {
		glUniform1i(useTextureLoc, false);
	}

	// bind current texture
	// select vao for rectangle
	glBindVertexArray(r->spriteRenderer.vao);

	// draw arrays
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// deselect vao
	glBindVertexArray(0);
}

// load texture
bool loadTexture(Texture* tex, const char* path) {
	// load image
	// we use 4 as the default number of channels - RGBA
	tex->pixels = (Pixel*) stbi_load(path, &tex->width, &tex->height, &tex->channels, 4);

	if (tex->pixels == NULL) {
		return false;
	}

	// create opengl texture
	glGenTextures(1, &tex->textureId);

	// bind (select) current texture
	glBindTexture(GL_TEXTURE_2D, tex->textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// link opengl texture object to bitmap data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) tex->pixels);

	// create mipmap for currently bound texture
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

void initialiseFont(Renderer* r) {
	// load font text
	loadTexture(&r->tex_font, "./textures/main_font.png");

	// initialise sprite
	r->spr_font.origin.x = 0;
	r->spr_font.origin.y = 0;
	r->spr_font.texture = &r->tex_font;
	r->spr_font.angle = 0;
}

double pixelXToNDCTexture(Texture* t, int x) {
	return (double) x / (double) t->width;
}

double pixelYToNDCTexture(Texture* t, int y) {
	return (double) y / (double) t->height;
}

void renderCharacter(Renderer* r, char c, float x, float y, float width, float height) {
	// set sprite
	r->spr_font.position.x = x;
	r->spr_font.position.y = y;

	r->spr_font.size.x = width;
	r->spr_font.size.y = height;

	r->spr_font.scale.x = 1;
	r->spr_font.scale.y = 1;

	r->spr_font.colour.x = 1.0;
	r->spr_font.colour.y = 1.0;
	r->spr_font.colour.z = 1.0;

	int pixelX = ((c - 32) % 18) * CHARACTER_WIDTH;
	int pixelY = ((c - 32) / 18) * CHARACTER_HEIGHT;

	// cut out character a
	r->spr_font.texCoords.left = pixelX;
	r->spr_font.texCoords.bottom = pixelY;
	r->spr_font.texCoords.right = pixelX + CHARACTER_WIDTH;
	r->spr_font.texCoords.top = pixelY + CHARACTER_HEIGHT;

	renderSprite(r, &r->spr_font);
}

void renderString(Renderer* r, const char* str, double x, double y, double width, double height) {
	int len = strlen(str);

	for (int i = 0; i < len; i++) {
		// draw renderer
		renderCharacter(r, str[i], i * width + x, y, width, height);
	}
}

void renderStringLeft(Renderer* r, const char* str, double left, double y, double size) {
	double width = size * CHARACTER_WIDTH / CHARACTER_HEIGHT;
	renderString(r, str, left, y, width, size);
}

void renderStringCentered(Renderer* r, const char* str, double y, double size) {
	int len = strlen(str);
	double width = size * CHARACTER_WIDTH / CHARACTER_HEIGHT;
	renderString(r, str, (INITIAL_WIDTH - len * width) / 2, y, width, size);
}

void renderStringRight(Renderer* r, const char* str, double right, double y, double size) {
	int len = strlen(str);
	double width = size * CHARACTER_WIDTH / CHARACTER_HEIGHT;
	renderString(r, str, INITIAL_WIDTH - right - len * width, y, width, size);
}

void freeRenderer(Renderer* r) {
	free(r->tex_font.pixels);
}
