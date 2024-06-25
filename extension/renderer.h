#ifndef RENDERER_H
#define RENDERER_H

#include "linearAlgebra.h"
#include <stdbool.h>
#include <stdarg.h>

#define CHARACTER_WIDTH 7
#define CHARACTER_HEIGHT 9


#define RENDER_TITLE(title) renderStringCentered(&game->renderer, title, 500, 48);
#define RENDER_SUBTITLE(subtitle) renderStringCentered(&game->renderer, subtitle, 100, 20);

#define RENDER_MENU(title, subtitle) RENDER_TITLE(title); RENDER_SUBTITLE(subtitle);

typedef struct {
	float left;
	float right;
	float top;
	float bottom;
} Rectanglef;

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
} Pixel;

typedef struct {
	int width;
	int height;
	int channels;
	Pixel* pixels;
	unsigned int textureId;
} Texture;

typedef struct {
	Vec2f position;
	Vec2f size;
	float angle;
	Vec2f origin;
	Rectanglef texCoords;
	Vec2f scale;
	Vec3f colour;
	Texture* texture;
} Sprite;

typedef struct {
	float coords[24];
	unsigned int vbo;
	unsigned int vao;
} RendererSprite;

typedef struct {
	unsigned int shaderProgram;
	RendererSprite spriteRenderer;
	Texture tex_font;
	Sprite spr_font;
} Renderer;

// declare functions
void initialiseShaderProgram(Renderer* r);
void initialiseRendererSprite(RendererSprite* r);
void initialiseRenderer(Renderer* r);
void renderSprite(Renderer* r, const Sprite* spr);
bool loadTexture(Texture* tex, const char* path);
void initialiseFont(Renderer* r);
void renderCharacter(Renderer* r, char c, float x, float y, float width, float height);
void renderString(Renderer* r, const char* str, double x, double y, double width, double height);
void renderStringLeft(Renderer* r, const char* str, double left, double y, double size);
void renderStringCentered(Renderer* r, const char* str, double y, double size);
void renderStringRight(Renderer* r, const char* str, double right, double y, double size);
void freeRenderer(Renderer* r);

#endif
