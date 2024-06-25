#include "gameUI.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

// text label functions
void initTextLabel(TextLabel* tl, const char* text, double x, double y, double width, double height) {
	strncpy(tl->str, text, LABEL_LEN);
	tl->position.x = x;
	tl->position.y = y;
	tl->size.x = width;
	tl->size.y = height;
	tl->colour.x = 1.0;
	tl->colour.y = 1.0;
	tl->colour.z = 1.0;
	tl->alive = true;
}

void updateTextLabelText(TextLabel* tl, const char* text) {
	strncpy(tl->str, text, LABEL_LEN);
}

void renderTextLabel(Renderer* r, TextLabel* tl) {
	renderString(r, tl->str, tl->position.x, tl->position.y, tl->size.x, tl->size.y);
}

void destroyTextLabel(TextLabel* tl) {
	tl->alive = false;
}

// button functions
void initButton(Button* b, const char* text, void (*clickFunc)(Game* game), double x, double y, double width, double height, double cWidth, double cHeight) {
	strncpy(b->str, text, LABEL_LEN);
	b->clickFunc = clickFunc;
	b->position.x = x;
	b->position.y = y;
	b->size.x = width;
	b->size.y = height;
	b->textSize.x = cWidth;
	b->textSize.y = cHeight;
	b->colour.x = 0.1;
	b->colour.y = 0.1;
	b->colour.z = 0.1;
	b->hoverColour.x = 0.25;
	b->hoverColour.y = 0.25;
	b->hoverColour.z = 0.25;
	b->hover = false;
	b->alive = true;
}

void updateButton(Game* game, Button* b) {
	// check hover
	if (fabs(game->mouseX - b->position.x) < b->size.x / 2 && fabs(game->mouseY - b->position.y) < b->size.y / 2) {
		b->hover = true;

		if (game->mousePressed && game->mouseTimer == 0) {
			(*b->clickFunc)(game);
		}
	} else {
		b->hover = false;
	}
}

void renderButton(Renderer* r, Button* b) {
	// create sprite
	Sprite rectangle;
	rectangle.position.x = b->position.x;
	rectangle.position.y = b->position.y;
	rectangle.size.x = b->size.x;
	rectangle.size.y = b->size.y;
	rectangle.texture = NULL;
	rectangle.angle = 0;
	rectangle.origin.x = b->size.x / 2;
	rectangle.origin.y = b->size.y / 2;
	rectangle.scale.x = 1;
	rectangle.scale.y = 1;
	rectangle.texCoords.left = 0;
	rectangle.texCoords.right = 1.0;
	rectangle.texCoords.top = 1.0;
	rectangle.texCoords.bottom = 0;

	if (b->hover) {
		rectangle.colour = b->hoverColour;
	} else {
		rectangle.colour = b->colour;
	}

	renderSprite(r, &rectangle);
	renderString(r, b->str, b->position.x - strlen(b->str) * b->textSize.x / 2, b->position.y - b->textSize.y / 2, b->textSize.x, b->textSize.y);
}
