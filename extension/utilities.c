#include "utilities.h"

// generate random float between 0 and 1 to "n" dp
// to get a random number between 0 and x:
//    - get random float "f" using randFloat
//    - multiply by x to get float between 0 and x (exlusive)
float randFloat(int n) {
	assert(n >= 1);
	int degree = pow(10, n);
	return ((float) (rand() % degree) / degree);
}

// check if left of sprite 1 collides with sprite 2
bool collideLeft(Sprite s1, Sprite s2) {
	float s1left = s1.position.x;
	return (s2.position.x < s1left) && (s1left < s2.position.x + s2.size.x);
}

// check if right of sprite 1 collides with sprite 2
bool collideRight(Sprite s1, Sprite s2) {
	float s1right = s1.position.x + s1.size.x;
	return (s2.position.x < s1right) && (s1right < s2.position.x + s2.size.x);
}

// check if bottom of sprite 1 collides with sprite 2
bool collideBottom(Sprite s1, Sprite s2) {
	float s1bottom = s1.position.y;
	return (s2.position.y < s1bottom) && (s1bottom < s2.position.y + s2.size.y);
}

// check if top of sprite 1 collides with sprite 2
bool collideTop(Sprite s1, Sprite s2) {
	float s1top = s1.position.y + s1.size.y;
	return (s2.position.y < s1top) && (s1top < s2.position.y + s2.size.y);
}

// for converting hues, saturations and brightness to rgb
void hsv_to_rgb(float h, float s, float v, float* r, float* g, float* b) {
	int i = (int) (h * 6);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i % 6) {
	case 0: *r = v; *g = t; *b = p; break;
	case 1: *r = q; *g = v; *b = p; break;
	case 2: *r = p; *g = v; *b = t; break;
	case 3: *r = p; *g = q; *b = v; break;
	case 4: *r = t; *g = p; *b = v; break;
	case 5: *r = v; *g = p; *b = q; break;
	}
}
