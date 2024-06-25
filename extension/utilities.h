#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include "game.h"
#include "renderer.h"

#define SAFE_MALLOC(ptr, size) { \
	ptr = malloc(size); \
	if (ptr == NULL) { \
		fprintf(stderr, "Error: malloc failed.\n"); \
		exit(1); \
	} \
}

float randFloat(int n);
bool collideLeft(Sprite s1, Sprite s2);
bool collideRight(Sprite s1, Sprite s2);
bool collideBottom(Sprite s1, Sprite s2);
bool collideTop(Sprite s1, Sprite s2);
void hsv_to_rgb(float h, float s, float v, float* r, float* g, float* b);
