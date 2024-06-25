#include "snake.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define AVAILABLE_SPACES ((SNAKE_BOARD_WIDTH - 2) * (SNAKE_BOARD_HEIGHT - 2) - state->numObstacles + state->player.length)
#define SNAKE_SAFE_DISTANCE 1
#define SNAKE_TILESIZE (INITIAL_WIDTH / SNAKE_BOARD_WIDTH)
#define SNAKE_TEXTURE_SIZE 64

// helper functions for collision detection
static bool collidePlayer(int x, int y, const SnakePlayer* player) {
	for (SnakePlayerBody current = player->head; current != NULL; current = current->next) {
		if (current->position.x == x && current->position.y == y) {
			return true;
		}
	}
	return false;
}

static bool collideSafeDistance(int x, int y, const SnakePlayer* player) {
	for (SnakePlayerBody current = player->head; current != NULL; current = current->next) {
		if (fabsf(x - player->head->position.x) <= SNAKE_SAFE_DISTANCE && fabs(y - player->head->position.y) <= SNAKE_SAFE_DISTANCE) {
			return true;
		}
	}
	return false;
}

static bool collideFood(int x, int y, const SnakeFood* food) {
	return (food->position.x == x && food->position.y == y);
}

static bool collideObstacles(int x, int y, const SnakeState* state) {
	for (int i = 0; i < state->numObstacles; i++) {
		if (state->obstacles[i]->position.x == x && state->obstacles[i]->position.y == y) {
			return true;
		}
	}
	return false;
}

static bool collideWalls(int x, int y) {
	return (x == 0 || x == SNAKE_BOARD_WIDTH - 1 || y == 0 || y == SNAKE_BOARD_HEIGHT - 1);
}

// helper functions for scaling relative position on board to screen position
static Vec2f scale(Vec2f v) {
	return scalarVec2f(INITIAL_WIDTH / SNAKE_BOARD_WIDTH, v);
}

// helper function to set body texture based on direction
static void setBodyTexture(SnakePlayerBody body, enum SnakeDirection newDirection, bool head, bool tail) {
	// set old head texture based on direction
	int shift = 0;

	if (tail) {
		body->direction = newDirection;
	}

	switch (body->direction) {
	case UP:
		if (tail) { shift = 10; break; }
		switch (newDirection) {
		case UP: shift = (head) ? 7 : 0; break;
		case DOWN: fprintf(stderr, "Error: invalid direction\n"); break;
		case LEFT: shift = 5; break;
		case RIGHT: shift = 4; break;
		}
		break;
	case DOWN:
		if (tail) { shift = 11; break; }
		switch (newDirection) {
		case UP: fprintf(stderr, "Error: invalid direction\n"); break;
		case DOWN: shift = (head) ? 6 : 0; break;
		case LEFT: shift = 2; break;
		case RIGHT: shift = 3; break;
		}
		break;
	case LEFT:
		if (tail) { shift = 12; break; }
		switch (newDirection) {
		case UP: shift = 3; break;
		case DOWN: shift = 4; break;
		case LEFT: shift = (head) ? 9 : 1; break;
		case RIGHT: fprintf(stderr, "Error: invalid direction\n"); break;
		}
		break;
	case RIGHT:
		if (tail) { shift = 13; break; }
		switch (newDirection) {
		case UP: shift = 2; break;
		case DOWN: shift = 5; break;
		case LEFT: fprintf(stderr, "Error: invalid direction\n"); break;
		case RIGHT: shift = (head) ? 8 : 1; break;
		}
		break;
	}

	body->sprite.texCoords.left = shift * SNAKE_TEXTURE_SIZE;
	body->sprite.texCoords.right = body->sprite.texCoords.left + SNAKE_TEXTURE_SIZE;
}

// helper function to find empty position on board
Vec2f emptyPosition(const SnakeState* state, bool avoidPlayer) {
	// check if there can be any empty positions
	if (AVAILABLE_SPACES == 0) {
		return (Vec2f) { -1, -1 };
	}

	int x, y;
	do {
		x = rand() % SNAKE_BOARD_WIDTH;
		y = rand() % SNAKE_BOARD_HEIGHT;
	} while (collideWalls(x, y) || collideObstacles(x, y, state) || collidePlayer(x, y, &state->player) || (avoidPlayer && collideSafeDistance(x, y, &state->player)));
	return (Vec2f) { x, y };
}

// helper functions for double ended linked list
static void insertHead(SnakePlayer* player, int x, int y) {
	SnakePlayerBody old_head = player->head;

	SAFE_MALLOC(player->head, sizeof(struct SnakePlayerBody));
	player->head->position = (Vec2f) { x, y };
	player->head->sprite = (Sprite) {
		.position = scale(player->head->position),
		.size = { SNAKE_TILESIZE, SNAKE_TILESIZE },
		.angle = 0,
		.origin = { 0, 0 },
		.texCoords = { .top = SNAKE_TEXTURE_SIZE, .bottom = 0 },
		.texture = &player->texture,
		.scale = { 1, 1 },
		.colour = { 1, 1, 1 },
	};
	player->head->direction = player->direction;

	setBodyTexture(player->head, player->direction, true, false);

	player->head->next = old_head;
	player->head->prev = NULL;

	if (old_head != NULL) {
		old_head->prev = player->head;
	} else {
		player->tail = player->head;
	}
}

static void popTail(SnakePlayer* player) {
	player->tail = player->tail->prev;
	free(player->tail->next);
	player->tail->next = NULL;
}

// initialisation functions
static void initPlayer(SnakeState* state) {
	state->player.head = NULL;
	state->player.tail = NULL;
	state->player.direction = RIGHT;
	state->player.length = 2;

	insertHead(&state->player, 1, SNAKE_BOARD_HEIGHT / 2);
	setBodyTexture(state->player.head, state->player.direction, false, true);

	insertHead(&state->player, 2, SNAKE_BOARD_HEIGHT / 2);
	setBodyTexture(state->player.head, state->player.direction, false, false);

	insertHead(&state->player, 3, SNAKE_BOARD_HEIGHT / 2);
	setBodyTexture(state->player.head, state->player.direction, true, false);
}

static void initFood(SnakeState* state) {
	state->food.sprite = (Sprite) {
		.size = { SNAKE_TILESIZE, SNAKE_TILESIZE },
		.angle = 0,
		.origin = { 0, 0 },
		.texCoords = { .left = 0, .right = SNAKE_TEXTURE_SIZE, .top = SNAKE_TEXTURE_SIZE, .bottom = 0 },
		.scale = { 1, 1 },
		.colour = { 1, 1, 1 },
	};
	state->food.sprite.texture = &state->foodTexture;

	state->food.position = emptyPosition(state, true);
	state->food.sprite.position = scale(state->food.position);
}

static void initObstacles(SnakeState* state) {
	SAFE_MALLOC(state->obstacles, sizeof(SnakeObstacle*) * state->numObstacles);

	// create obstacles and render each
	for (int i = 0; i < state->numObstacles; i++) {
		SAFE_MALLOC(state->obstacles[i], sizeof(SnakeObstacle));
		state->obstacles[i]->position = (Vec2f) { 0, 0 };
		state->obstacles[i]->sprite = (Sprite) {
			.size = { SNAKE_TILESIZE, SNAKE_TILESIZE },
			.angle = 0,
			.origin = { 0, 0 },
			.texCoords = { .left = 0, .right = SNAKE_TEXTURE_SIZE, .top = SNAKE_TEXTURE_SIZE, .bottom = 0 },
			.texture = &state->obstacleTexture,
			.scale = { 1, 1 },
			.colour = { 1, 1, 1 },
		};
	}

	for (int i = 0; i < state->numObstacles; i++) {
		// randomise obstacle position and render
		state->obstacles[i]->position = emptyPosition(state, true);
		state->obstacles[i]->sprite.position = scale(state->obstacles[i]->position);
	}
}

static void initWalls(SnakeState* state) {
	SAFE_MALLOC(state->walls, sizeof(Sprite*) * (SNAKE_BOARD_WIDTH + SNAKE_BOARD_HEIGHT - 2) * 2);

	int count = 0;
	// create walls and render each
	for (int i = 0; i < SNAKE_BOARD_WIDTH; i++) {
		for (int j = 0; j < SNAKE_BOARD_HEIGHT; j++) {
			if (collideWalls(i, j)) {
				SAFE_MALLOC(state->walls[count], sizeof(Sprite));
				*state->walls[count] = (Sprite) {
					.position = scale((Vec2f) { i, j }),
					.size = { SNAKE_TILESIZE, SNAKE_TILESIZE },
					.angle = 0,
					.origin = { 0, 0 },
					.texCoords = { .left = 0, .right = SNAKE_TEXTURE_SIZE, .top = SNAKE_TEXTURE_SIZE, .bottom = 0 },
					.texture = &state->wallTexture,
					.scale = { 1, 1 },
					.colour = { 1, 1, 1 },
				};

				count++;
			}
		}
	}
}

static void initTiles(SnakeState* state) {
	SAFE_MALLOC(state->tiles, sizeof(Sprite*) * (SNAKE_BOARD_WIDTH - 2) * (SNAKE_BOARD_HEIGHT - 2));

	// create tiles and render each
	for (int i = 1; i < SNAKE_BOARD_WIDTH - 1; i++) {
		for (int j = 1; j < SNAKE_BOARD_HEIGHT - 1; j++) {
			SAFE_MALLOC(state->tiles[(i - 1) * (SNAKE_BOARD_HEIGHT - 2) + (j - 1)], sizeof(Sprite));
			*state->tiles[(i - 1) * (SNAKE_BOARD_HEIGHT - 2) + (j - 1)] = (Sprite) {
				.position = scale((Vec2f) { i, j }),
				.size = { SNAKE_TILESIZE, SNAKE_TILESIZE },
				.angle = 0,
				.origin = { 0, 0 },
				.texCoords = { .left = 0, .right = SNAKE_TEXTURE_SIZE, .top = SNAKE_TEXTURE_SIZE, .bottom = 0 },
				.texture = ((i + j) % 2 == 0) ? &state->emptyTexture1 : &state->emptyTexture2,
				.scale = { 1, 1 },
				.colour = { 1, 1, 1 },
			};
		}
	}
}

static void initTextures(SnakeState* state) {
	loadTexture(&state->foodTexture, "./textures/snakeFood.png");
	loadTexture(&state->player.texture, "./textures/snakePlayer.png");
	loadTexture(&state->obstacleTexture, "./textures/snakeObstacle.png");
	loadTexture(&state->wallTexture, "./textures/snakeWall.png");
	loadTexture(&state->emptyTexture1, "./textures/snakeEmpty1.png");
	loadTexture(&state->emptyTexture2, "./textures/snakeEmpty2.png");
}

static void startGame(SnakeState* state) {
	state->gameState = GAME_PLAY;
	state->score = 0;
	state->timeSinceLastMove = SNAKE_GAME_PRE_START;

	initPlayer(state);
	initObstacles(state);
	initFood(state);
}

static void restartGame(SnakeState* state) {
	SnakePlayerBody current = state->player.head;
	for (SnakePlayerBody next = current->next; current != NULL; current = next) {
		next = current->next;
		free(current);
	}

	for (int i = 0; i < state->numObstacles; i++) {
		free(state->obstacles[i]);
	}
	free(state->obstacles);

	startGame(state);
}

// game function
void initSnake(Game* game, void** state) {
	// allocate state
	SAFE_MALLOC(*state, sizeof(SnakeState));
	SnakeState* snakeState = *state;

	// initialise state
	snakeState->numObstacles = SNAKE_NUM_OBSTACLES;
	snakeState->moveInterval = SNAKE_MOVE_INTERVAL;

	// textures and walls and tiles only need to be initialised once
	initTextures(snakeState);
	initWalls(snakeState);
	initTiles(snakeState);

	startGame(snakeState);
	snakeState->gameState = GAME_MENU;
}

static void updatePlayer(SnakeState* state) {
	// set current head texture based on direction
	setBodyTexture(state->player.head, state->player.direction, false, false);

	// new head position
	Vec2f newPos = state->player.head->position;
	switch (state->player.direction) {
	case UP: newPos.y++; break;
	case DOWN: newPos.y--; break;
	case LEFT: newPos.x--; break;
	case RIGHT: newPos.x++; break;
	}

	if (collideObstacles(newPos.x, newPos.y, state) || collideWalls(newPos.x, newPos.y) || collidePlayer(newPos.x, newPos.y, &state->player)) {
		// check for collision with obstacles, walls or player
		state->gameState = GAME_OVER;
		return;
	} else if (collideFood(newPos.x, newPos.y, &state->food)) {
		// check for collision with food
		state->player.length++;
		state->score++;

		// add new head to linked list
		insertHead(&state->player, newPos.x, newPos.y);

		if (AVAILABLE_SPACES == 0) {
			// game over
			state->gameState = GAME_OVER;
			return;
		}
	} else {
		// normal movement
		insertHead(&state->player, newPos.x, newPos.y);
		popTail(&state->player);
	}

	setBodyTexture(state->player.head, state->player.direction, true, false);
	setBodyTexture(state->player.tail, state->player.tail->prev->direction, false, true);
}

static void updateFood(SnakeState* state) {
	SnakeFood* food = &state->food;

	if (collidePlayer(food->position.x, food->position.y, &state->player)) {
		// move food to new location
		food->position = emptyPosition(state, true);
		food->sprite.position = scale(food->position);
	}
}

// game function
void updateSnake(Game* game, void* state) {
	SnakeState* snakeState = state;

	switch (snakeState->gameState) {
	case GAME_MENU:
		// check for start
		if (game->keysDown[GLFW_KEY_SPACE] && game->keyTime[GLFW_KEY_SPACE] == 0.0) {
			snakeState->gameState = GAME_PLAY;
		}
		break;
	case GAME_OVER:
		// check for restart or quit
		if (game->keysDown[GLFW_KEY_SPACE] && game->keyTime[GLFW_KEY_SPACE] == 0.0) {
			restartGame(snakeState);
		}
		break;
	case GAME_PLAY:
		// setBodyTexture(snakeState->player.head, snakeState->player.direction, true, false);

		if (snakeState->timeSinceLastMove >= 0) {
			// prevent keystrokes from being registered before game starts
			if (game->keysDown[GLFW_KEY_W] || game->keysDown[GLFW_KEY_UP]) {
				if (snakeState->player.head->direction != DOWN) {
					snakeState->player.direction = UP;
				}
			} else if (game->keysDown[GLFW_KEY_A] || game->keysDown[GLFW_KEY_LEFT]) {
				if (snakeState->player.head->direction != RIGHT) {
					snakeState->player.direction = LEFT;
				}
			} else if (game->keysDown[GLFW_KEY_S] || game->keysDown[GLFW_KEY_DOWN]) {
				if (snakeState->player.head->direction != UP) {
					snakeState->player.direction = DOWN;
				}
			} else if (game->keysDown[GLFW_KEY_D] || game->keysDown[GLFW_KEY_RIGHT]) {
				if (snakeState->player.head->direction != LEFT) {
					snakeState->player.direction = RIGHT;
				}
			}
		}

		// check if it is time to move
		if (snakeState->timeSinceLastMove > snakeState->moveInterval) {
			snakeState->timeSinceLastMove = 0;

			updatePlayer(snakeState);
			updateFood(snakeState);
		}
		snakeState->timeSinceLastMove += game->deltaTime;
		break;
	}
}

static void renderTiles(Renderer* renderer, const SnakeState* state) {
	// render alternating tiles
	for (int i = 0; i < (SNAKE_BOARD_WIDTH - 2) * (SNAKE_BOARD_HEIGHT - 2); i++) {
		renderSprite(renderer, state->tiles[i]);
	}
}

static void renderPlayer(Game* game, const SnakePlayer* player) {
	// update player position and transform and render
	for (SnakePlayerBody current = player->head; current != NULL; current = current->next) {
		renderSprite(&game->renderer, &current->sprite);
	}
}

static void renderFood(Game* game, const SnakeFood* food) {
	// update food position and transform and render
	renderSprite(&game->renderer, &food->sprite);
}

static void renderObstacles(Game* game, const SnakeState* state) {
	// update obstacle position and transform and render
	for (int i = 0; i < state->numObstacles; i++) {
		renderSprite(&game->renderer, &state->obstacles[i]->sprite);
	}
}

static void renderWalls(Game* game, const SnakeState* state) {
	// update wall position and transform and render
	for (int i = 0; i < (SNAKE_BOARD_WIDTH + SNAKE_BOARD_HEIGHT - 2) * 2; i++) {
		renderSprite(&game->renderer, state->walls[i]);
	}
}

static void renderScore(Game* game, int score) {
	// render score
	char scoreStr[16];
	sprintf(scoreStr, "Score:%d", score);
	renderStringLeft(&game->renderer, scoreStr, 14, 10, 32);
}

// game function
void renderSnake(Game* game, void* state) {
	SnakeState* snakeState = state;

	switch (snakeState->gameState) {
	case GAME_MENU:
		// show start message eg press space to start
		RENDER_MENU("Snake", "Press SPACE to start or M to return to menu");
		break;
	case GAME_PLAY:
		// render game
		renderTiles(&game->renderer, snakeState);
		renderPlayer(game, &snakeState->player);
		renderFood(game, &snakeState->food);
		renderObstacles(game, snakeState);
		renderWalls(game, snakeState);
		renderScore(game, snakeState->score);
		break;
	case GAME_OVER:
	{
		// show game over message and score
		char scoreStr[16];
		sprintf(scoreStr, "Score: %d", snakeState->score);
		RENDER_MENU(scoreStr, "Press SPACE to restart or M to return to menu");
		break;
	}
	}
}

// game function
void exitSnake(Game* game, void* state) {
	SnakeState* snakeState = state;

	SnakePlayerBody current = snakeState->player.head;
	for (SnakePlayerBody next = current->next; current != NULL; current = next) {
		next = current->next;
		free(current);
	}

	for (int i = 0; i < snakeState->numObstacles; i++) {
		free(snakeState->obstacles[i]);
	}
	free(snakeState->obstacles);

	for (int i = 0; i < (SNAKE_BOARD_WIDTH - 2) * (SNAKE_BOARD_HEIGHT - 2); i++) {
		free(snakeState->tiles[i]);
	}
	free(snakeState->tiles);

	for (int i = 0; i < (SNAKE_BOARD_WIDTH + SNAKE_BOARD_HEIGHT - 2) * 2; i++) {
		free(snakeState->walls[i]);
	}

	free(snakeState->walls);

	free(snakeState->player.texture.pixels);
	free(snakeState->emptyTexture1.pixels);
	free(snakeState->emptyTexture2.pixels);
	free(snakeState->foodTexture.pixels);
	free(snakeState->obstacleTexture.pixels);
	free(snakeState->wallTexture.pixels);

	free(snakeState);
}
