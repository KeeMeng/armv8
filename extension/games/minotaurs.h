#ifndef MINOTAURS_H
#define MINOTAURS_H

#include "./../game.h"
#include "./../gameUI.h"

#define MINOTAURS_TILESIZE 20
#define MINOTAURS_MAZEWIDTH (800 / MINOTAURS_TILESIZE)
#define MINOTAURS_MAZEHEIGHT (600 / MINOTAURS_TILESIZE)
#define MINOTAURS_NUM_TILES (MINOTAURS_MAZEWIDTH * MINOTAURS_MAZEHEIGHT)
#define MINOTAURS_NUM_SCORES 10
#define MINOTAURS_PLAYER_SPEED 150
#define MINOTAURS_ENEMY_SPEED 75.0

typedef struct {
	int dir;
	float speed;
	float frameTime;
	Vec2f position;
	Vec2f size;
	Vec2f velocity;
	Sprite sprite;
} MinotaurPlayer;

typedef struct {
	bool alive;
	float speed;
	int type;
	int target;
	Vec2f position;
	Vec2f size;
	Vec2f velocity;
	Sprite sprite;
} MinotaurEnemy;

typedef struct {
	bool alive;
	Vec2f position;
	Vec2f size;
	Vec2f velocity;
	Sprite sprite;
} MinotaurPickup;

// define state
typedef struct {
	enum GameState gameState;
	int score;
	int lives;
	int gameRound;
	unsigned int highScores[MINOTAURS_NUM_SCORES];
	MinotaurPlayer player;
	MinotaurEnemy enemies[4];
	MinotaurPickup pickups[MINOTAURS_NUM_TILES];
	Button startButton;
	Button menuButton;
	Texture coinTexture;
	Texture playerTexture;
	Texture wallTexture;
} MinotaurState;

void initMinotaurs(Game* game, void** state);
void updateMinotaurs(Game* game, void* state);
void renderMinotaurs(Game* game, void* state);
void exitMinotaurs(Game* game, void* state);

#endif
