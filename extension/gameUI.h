#ifndef GAMEUI_H
#define GAMEUI_H

#include "linearAlgebra.h"
#include "game.h"

#define LABEL_LEN 512

typedef struct {
	char str[LABEL_LEN];
	Vec2f position;
	Vec3f colour;
	Vec2f size;
	bool alive;
} TextLabel;

typedef struct {
	char str[LABEL_LEN];
	Vec2f position;
	Vec2f textSize;
	Vec2f size;
	Vec3f colour;
	Vec3f hoverColour;
	void (*clickFunc)(Game* game);
	bool hover;
	bool alive;
} Button;

// text label functions
void initTextLabel(TextLabel* tl, const char* text, double x, double y, double width, double height);
void updateTextLabelText(TextLabel* tl, const char* text);
void renderTextLabel(Renderer* r, TextLabel* tl);
void destroyTextLabel(TextLabel* tl);

// button functions
void initButton(Button* b, const char* text, void (*clickFunc)(Game* game), double x, double y, double width, double height, double cWidth, double cHeight);
void updateButton(Game* game, Button* b);
void renderButton(Renderer* r, Button* b);

#endif
