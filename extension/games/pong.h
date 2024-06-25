#ifndef PONG_H
#define PONG_H

#include "./../game.h"

#define PONG_TILESIZE 20
#define PONG_PADDLE_SPEED 300
#define PONG_BALL_SPEED 500
#define PONG_WIN_SCORE 10

typedef struct {
	Vec2f position;
	Vec2f size;
	Vec2f velocity;
	Sprite sprite;
} PongBall;

typedef struct {
	int playerNum;
	Vec2f position;
	Vec2f size;
	Vec2f velocity;
	Sprite sprite;
} PongPlayer;

// define state
typedef struct {
	enum GameState gameState;
	int score[2];
	PongBall ball;
	PongPlayer player1;
	PongPlayer player2;
	Texture ballTexture;
	Texture paddleTexture;
} PongState;

void initPong(Game* game, void** state);
void updatePong(Game* game, void* state);
void renderPong(Game* game, void* state);
void exitPong(Game* game, void* state);

#endif
