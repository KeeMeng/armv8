#ifndef SNAKE_H
#define SNAKE_H

#include "./../game.h"

#define SNAKE_BOARD_WIDTH 16
#define SNAKE_BOARD_HEIGHT 12
#define SNAKE_MOVE_INTERVAL 0.25
#define SNAKE_GAME_PRE_START -1
#define SNAKE_NUM_OBSTACLES 10

enum SnakeDirection { UP, DOWN, LEFT, RIGHT };

typedef struct SnakePlayerBody* SnakePlayerBody;
struct SnakePlayerBody {
	enum SnakeDirection direction;
	Vec2f position;
	Sprite sprite;
	SnakePlayerBody prev;
	SnakePlayerBody next;
};

typedef struct {
	enum SnakeDirection direction;
	int length;
	SnakePlayerBody head;
	SnakePlayerBody tail;
	Texture texture;
} SnakePlayer;

typedef struct {
	Vec2f position;
	Sprite sprite;
} SnakeFood;

typedef struct {
	Vec2f position;
	Sprite sprite;
} SnakeObstacle;

typedef struct {
	enum GameState gameState;
	int score;
	int numObstacles;
	double moveInterval;
	double timeSinceLastMove;
	SnakePlayer player;
	SnakeFood food;
	SnakeObstacle** obstacles;
	Sprite** tiles;
	Sprite** walls;
	Texture emptyTexture1;
	Texture emptyTexture2;
	Texture foodTexture;
	Texture obstacleTexture;
	Texture wallTexture;
} SnakeState;

void initSnake(Game* game, void** state);
void updateSnake(Game* game, void* state);
void renderSnake(Game* game, void* state);
void exitSnake(Game* game, void* state);

#endif
