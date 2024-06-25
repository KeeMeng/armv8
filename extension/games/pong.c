#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "pong.h"

#define PADDLE_SPRITE_WIDTH 32
#define PADDLE_SPRITE_HEIGHT 160
#define BALL_SPRITE_SIZE 144
#define MAX_BALL_DEVIATION 150
#define CHAR_WIDTH 32
#define CHAR_HEIGHT 32

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

// load media
static void loadMediaPong(void* state) {
	PongState* pState = state;

	// load player (paddle) texture
	loadTexture(&pState->paddleTexture, "./textures/Paddle.png");

	// load ball texture
	loadTexture(&pState->ballTexture, "./textures/Ball.png");
}

// initialise players
static void initialisePlayer(PongState* state, int playerNum) {
	PongPlayer* player = NULL;
	if (playerNum == 1) {
		player = &state->player1;
		player->position.x = 0;
		player->position.y = 0;
	} else if (playerNum == 2) {
		player = &state->player2;
		player->position.x = INITIAL_WIDTH - PADDLE_SPRITE_WIDTH;
		player->position.y = 0;
	} else {
		fprintf(stderr, "Error initialising player. playerNum param must be either 1 or 2.");
	}

	player->size.x = PADDLE_SPRITE_WIDTH;
	player->size.y = PADDLE_SPRITE_HEIGHT;
	player->velocity.x = 0;
	player->velocity.y = 0;
	player->sprite.size = player->size;
	player->sprite.texture = &state->paddleTexture;
	player->sprite.texCoords.left = 0;
	player->sprite.texCoords.right = PADDLE_SPRITE_WIDTH;
	player->sprite.texCoords.bottom = 0;
	player->sprite.texCoords.top = PADDLE_SPRITE_HEIGHT;
	player->sprite.origin.x = 0;
	player->sprite.origin.y = 0;
	player->sprite.colour.x = 1.0;
	player->sprite.colour.y = 1.0;
	player->sprite.colour.z = 1.0;
	player->sprite.scale.x = 1.0;
	player->sprite.scale.y = 1.0;
	player->sprite.angle = 0.0;
}

// initialise ball
static void initialiseBall(PongState* state) {
	PongBall* b = &state->ball;
	b->position.x = INITIAL_WIDTH / 2;
	b->position.y = INITIAL_HEIGHT / 2;
	b->size.x = 15;
	b->size.y = 15;

	// initialise speed and direction of ball
	float angle;
	b->velocity.x = 0;

	// adjust angle if too steep
	while (fabsf(b->velocity.x) < PONG_BALL_SPEED / 3) {
		angle = randFloat(3) * 2 * M_PI;
		b->velocity.x = PONG_BALL_SPEED * cos(angle);
		b->velocity.y = PONG_BALL_SPEED * sin(angle);
	}

	b->sprite.size = b->size;
	b->sprite.texture = &state->ballTexture;
	b->sprite.texCoords.left = 0;
	b->sprite.texCoords.right = BALL_SPRITE_SIZE;
	b->sprite.texCoords.bottom = 0;
	b->sprite.texCoords.top = BALL_SPRITE_SIZE;
	b->sprite.origin.x = 0;
	b->sprite.origin.y = 0;
	b->sprite.colour.x = 1.0;
	b->sprite.colour.y = 1.0;
	b->sprite.colour.z = 1.0;
	b->sprite.scale.x = 1.0;
	b->sprite.scale.y = 1.0;
	b->sprite.angle = 0.0;
}

// start game
static void startGame(PongState* pState) {
	pState->score[0] = 0;
	pState->score[1] = 0;

	initialisePlayer(pState, 1);
	initialisePlayer(pState, 2);

	initialiseBall(pState);

	pState->gameState = GAME_PLAY;
}

// initialise function
void initPong(Game* game, void** state) {
	// allocate state
	SAFE_MALLOC(*state, sizeof(PongState));
	PongState* pState = *state;

	loadMediaPong(pState);
	pState->gameState = GAME_MENU;
}

// check for collision between player and window
static void checkCollideWindowPlayer(PongPlayer* p) {
	int py = p->position.y;
	int pv = p->velocity.y;
	int ph = p->size.y;

	if (py > INITIAL_HEIGHT - ph && pv > 0) {
		p->velocity.y = 0;
	} else if (py < 0 && pv < 0) {
		p->velocity.y = 0;
	}
}

// update player
static void updatePlayers(Game* game, PongState* state) {
	PongPlayer* player1 = &state->player1;
	PongPlayer* player2 = &state->player2;

	// check keys
	if (game->keysDown[GLFW_KEY_W]) {
		// set velocity to up
		player1->velocity.y = PONG_PADDLE_SPEED;
	} else if (game->keysDown[GLFW_KEY_S]) {
		// set velocity to down
		player1->velocity.y = -PONG_PADDLE_SPEED;
	} else {
		player1->velocity.y = 0;
	}

	if (game->keysDown[GLFW_KEY_UP]) {
		player2->velocity.y = PONG_PADDLE_SPEED;
	} else if (game->keysDown[GLFW_KEY_DOWN]) {
		player2->velocity.y = -PONG_PADDLE_SPEED;
	} else {
		player2->velocity.y = 0;
	}

	// check for collision with floor and ceiling
	checkCollideWindowPlayer(player1);
	checkCollideWindowPlayer(player2);

	player1->position.y += player1->velocity.y * game->deltaTime;
	player2->position.y += player2->velocity.y * game->deltaTime;
}

static void updateBall(Game* game, PongState* state) {
	PongBall* ball = &state->ball;
	PongPlayer* player1 = &state->player1;
	PongPlayer* player2 = &state->player2;

	// move ball
	ball->position.x += ball->velocity.x * game->deltaTime;
	ball->position.y += ball->velocity.y * game->deltaTime;

	assert(ball->velocity.x != 0);
	bool collision = 0;
	// check for collision when going left
	if (ball->velocity.x < 0) {
		collision = (
			collideLeft(ball->sprite, player1->sprite) && (collideBottom(ball->sprite, player1->sprite) || collideTop(ball->sprite, player1->sprite))
		);
	}
	// check for collision when going right
	else {
		collision = (
			collideRight(ball->sprite, player2->sprite) && (collideBottom(ball->sprite, player2->sprite) || collideTop(ball->sprite, player2->sprite))
		);
	}
	if (collision) {
		// switch horizontal direction and apply deviation to ball
		ball->velocity.x *= -1;
		ball->velocity.x += (randFloat(3) - 0.5) * MAX_BALL_DEVIATION;
		ball->velocity.y += (randFloat(3) - 0.5) * MAX_BALL_DEVIATION;
	}

	// check for collision with ceiling and floor
	if ((ball->position.y + ball->size.y >= INITIAL_HEIGHT) || (ball->position.y <= 0)) {
		ball->velocity.y *= -1;
		ball->position.y += ball->velocity.y * game->deltaTime;
	}
}

// check if a player has scored
static void checkScore(PongState* state) {
	PongBall* ball = &state->ball;
	if (ball->position.x <= 0) {
		state->score[1] += 1;
		initialiseBall(state);
	} else if (ball->position.x + ball->size.x >= INITIAL_WIDTH) {
		state->score[0] += 1;
		initialiseBall(state);
	}
}

// update game
void updatePong(Game* game, void* state) {
	PongState* p = state;

	if (p->gameState == GAME_MENU) {
		// update menu logic
		// check for spacebar
		if (game->keysDown[GLFW_KEY_SPACE] == GLFW_PRESS && game->keyTime[GLFW_KEY_SPACE] == 0.0) {
			// start game
			startGame(p);
		}
	} else if (p->gameState == GAME_PLAY) {
		updatePlayers(game, p);
		updateBall(game, p);
		checkScore(p);
	} else if (p->gameState == GAME_OVER) {
		// update game over logic
		// check for spacebar
		if (game->keysDown[GLFW_KEY_SPACE] == GLFW_PRESS && game->keyTime[GLFW_KEY_SPACE] == 0.0) {
			// start game
			startGame(p);
		}
	}
}

// render game
void renderPong(Game* game, void* state) {
	PongState* p = state;

	if (p->gameState == GAME_MENU) {
		// render menu text
		RENDER_MENU("Pong", "Press SPACE to start or M to return to menu");
	} else if (p->gameState == GAME_PLAY) {
		// render and draw players
		PongPlayer* p1 = &(p->player1);
		PongPlayer* p2 = &(p->player2);
		PongBall* b = &(p->ball);
		p1->sprite.position = p1->position;
		p2->sprite.position = p2->position;
		b->sprite.position = b->position;

		renderSprite(&game->renderer, &p1->sprite);
		renderSprite(&game->renderer, &p2->sprite);
		renderSprite(&game->renderer, &b->sprite);

		char scoreString[32];
		if (p->score[0] < PONG_WIN_SCORE && p->score[1] < PONG_WIN_SCORE) {
			sprintf(scoreString, "%d  |  %d", (p->score)[0], (p->score)[1]);
			renderStringCentered(&game->renderer, scoreString, INITIAL_HEIGHT - 2 * CHAR_HEIGHT, CHAR_HEIGHT);
		} else {
			p->gameState = GAME_OVER;
		}
	} else if (p->gameState == GAME_OVER) {
		// render game over text

		// check scores
		if (p->score[1] > p->score[0]) {
			// player 2 wins
			renderStringCentered(&game->renderer, "Player 2 wins", 400, 32);
		} else {
			renderStringCentered(&game->renderer, "Player 1 wins", 400, 32);
		}

		char scoreString[16];
		sprintf(scoreString, "Score: %d : %d", p->score[0], p->score[1]);
		RENDER_MENU(scoreString, "Press SPACE to restart or M to return to menu");
	}
}

// exit pong
void exitPong(Game* game, void* state) {
	PongState* p = state;
	free(p->ballTexture.pixels);
	free(p->paddleTexture.pixels);
	free(state);
}
