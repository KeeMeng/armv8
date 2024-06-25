#include "snakesAndLadders.h"
#include "../utilities.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#define SNL_TILE_SIZE (600 / SNL_BOARD_DIM)
#define SNL_SPRITE_SIZE 32
#define BUFFER_MAX 1024

#define COLUMN(t) (t % SNL_BOARD_DIM)
#define ROW(t) ((t / SNL_BOARD_DIM) % SNL_BOARD_DIM)

static void initialiseSNLBoard(SNLState* state) {
	for (int i = 0; i < SNL_BOARD_AREA; i++) {
		BoardCell boardcell = { .number = i, .object.type = NONE };
		state->board[i] = boardcell;
	}
}

// load media
static void loadMediaSnL(void* state) {
	SNLState* snlState = state;

	// load wall texture
	loadTexture(&snlState->tile1Texture, "./textures/snl_tile3.png");
	loadTexture(&snlState->tile2Texture, "./textures/snl_tile4.png");

	// load player texture
	loadTexture(&snlState->playerTexture, "./textures/snl_player.png");

	// load dice texture
	loadTexture(&snlState->diceTexture, "./textures/dice.png");

	// load snake texture
	loadTexture(&snlState->snakeTexture, "./textures/snake600.png");

	// load ladder texture
	loadTexture(&snlState->ladderTexture, "./textures/ladder600.png");

}

static void initialisePlayer(SNLState* state) {
	for (int i = 0; i < 2; i++) {
		SnLPlayer p;
		p.squareNo = 0;
		p.delayTime = 0.5;

		Sprite spr_tile;
		spr_tile.angle = 0;
		spr_tile.size.x = SNL_SPRITE_SIZE;
		spr_tile.size.y = SNL_SPRITE_SIZE;
		spr_tile.origin.y = 0;
		spr_tile.scale.x = (SNL_TILE_SIZE * 0.7) / SNL_SPRITE_SIZE;
		spr_tile.scale.y = (SNL_TILE_SIZE * 0.7) / SNL_SPRITE_SIZE;
		spr_tile.colour.x = 1.0;
		spr_tile.colour.y = 1.0;
		spr_tile.colour.z = 1.0;
		spr_tile.texture = &state->playerTexture;

		switch (i) {
		case 0:
			spr_tile.origin.x = spr_tile.size.x * 0.35;
			spr_tile.texCoords.left = 0;
			spr_tile.texCoords.bottom = 0;
			spr_tile.texCoords.right = SNL_SPRITE_SIZE;
			spr_tile.texCoords.top = SNL_SPRITE_SIZE;
			p.sprite = spr_tile;
			state->player1 = p;
			break;
		default:
			spr_tile.origin.x = spr_tile.size.x * 0.65;
			spr_tile.texCoords.left = SNL_SPRITE_SIZE;
			spr_tile.texCoords.bottom = 0;
			spr_tile.texCoords.right = 2 * SNL_SPRITE_SIZE;
			spr_tile.texCoords.top = SNL_SPRITE_SIZE;
			p.sprite = spr_tile;
			state->player2 = p;
			break;
		}

		spr_tile.position.x = 0;
		spr_tile.position.y = 0;
	}
}

static void initialiseSnLObjects(SNLState* state) {
	// generate snakes
	for (int i = 0; i < SNL_SL_NUM; i++) {
		SnakeLadder snake;
		int beginning = (rand() % 97) + 2;
		int end = (rand() % (beginning - 1)) + 1;

		if (state->board[beginning].object.type == NONE && state->board[end].object.type == NONE) {
			snake.beginning = beginning;
			snake.end = end;
			state->snakes[i] = snake;
			state->board[snake.beginning].object.type = SNAKE;
			state->board[snake.end].object.type = SNAKE;

			state->board[snake.beginning].object.skip = snake;
			state->board[snake.end].object.skip = snake;
		} else {
			i--;
		}
	}

	// generate ladders
	for (int i = 0; i < SNL_SL_NUM; i++) {
		SnakeLadder ladder;
		int end = (rand() % 97) + 2;
		int beginning = (rand() % (end - 1)) + 1;

		if (state->board[beginning].object.type == NONE && state->board[end].object.type == NONE) {
			ladder.beginning = beginning;
			ladder.end = end;
			state->ladders[i] = ladder;
			state->board[ladder.beginning].object.type = LADDER;
			state->board[ladder.end].object.type = LADDER;

			state->board[ladder.beginning].object.skip = ladder;
			state->board[ladder.end].object.skip = ladder;
		} else {
			i--;
		}
	}
}

static void initDice(SNLState* state) {
	Dice d;
	d.number = 5;
	d.cooldown = 0;

	Sprite spr_tile;
	spr_tile.angle = 0;
	spr_tile.origin.x = 0;
	spr_tile.origin.y = 0;
	spr_tile.size.x = SNL_SPRITE_SIZE;
	spr_tile.size.y = SNL_SPRITE_SIZE;
	spr_tile.scale.x = 80 / SNL_SPRITE_SIZE;
	spr_tile.scale.y = 80 / SNL_SPRITE_SIZE;
	spr_tile.texCoords.left = (d.number - 1) * SNL_SPRITE_SIZE;
	spr_tile.texCoords.bottom = 0;
	spr_tile.texCoords.right = d.number * SNL_SPRITE_SIZE;
	spr_tile.texCoords.top = SNL_SPRITE_SIZE;
	spr_tile.colour.x = 1.0;
	spr_tile.colour.y = 1.0;
	spr_tile.colour.z = 1.0;
	spr_tile.texture = &state->diceTexture;
	spr_tile.position.x = 10;
	spr_tile.position.y = 440;

	d.spite = spr_tile;
	state->dice = d;

}

static void startGame(SNLState* state) {
	initDice(state);
	initialisePlayer(state);
	state->gameState = GAME_PLAY;
}

void initSNL_RPG(Game* game, void** state) {
	// allocate state
	SAFE_MALLOC(*state, sizeof(SNLState));
	SNLState* s = *state;
	s->dicePressed = false;
	s->turnPassed = true;
	s->turn = true;
	s->gameState = GAME_MENU;
	srand(time(NULL));

	// create board
	initialiseSNLBoard(s);

	// create snakes and ladders
	initialiseSnLObjects(s);

	// load media into state
	loadMediaSnL(s);
}

static void updatePlayer(Game* game, SNLState* state, SnLPlayer* player) {
	if (player->squareNo >= SNL_BOARD_AREA - 1) {
		player->squareNo = SNL_BOARD_AREA - 1;
	}
	BoardObject currObj = state->board[player->squareNo].object;
	if (player->delayTime <= 0) {
		if (currObj.type == SNAKE) {
			if (currObj.skip.beginning == player->squareNo) {
				player->squareNo = currObj.skip.end;
			}
		}
		if (currObj.type == LADDER) {
			if (currObj.skip.beginning == player->squareNo) {
				player->squareNo = currObj.skip.end;
			}
		}
	} else {
		player->delayTime -= game->deltaTime;
	}
}

static void rollDice(Game* game, SNLState* state) {
	SnLPlayer* player;
	if (state->turn) {
		player = &state->player1;
	} else {
		player = &state->player2;
	}

	if (state->dice.cooldown <= 0) {
		if (game->keysDown[GLFW_KEY_P]) {
			state->dice.number = (rand() % 6) + 1;
			state->dicePressed = true;
		} else if (state->dicePressed) {
			state->dice.cooldown = 1;
			player->delayTime = 0.5;
			player->squareNo += state->dice.number;
			state->turnPassed = false;
		}

		if (state->dicePressed && !state->turnPassed) {
			state->turn = !state->turn;
			state->dicePressed = false;
			state->turnPassed = true;
		}
	} else {
		state->dice.cooldown -= game->deltaTime;
	}
}

void updateSNL_RPG(Game* game, void* state){
	SNLState* s = state;

	if (s->gameState == GAME_MENU || s->gameState == GAME_OVER) {
		if (game->keysDown[GLFW_KEY_SPACE] && game->keyTime[GLFW_KEY_SPACE] == 0.0) {
			// reset game
			startGame(s);
		}
	} else if (s->gameState == GAME_PLAY) {
		rollDice(game, s);
		updatePlayer(game, s, &s->player1);
		updatePlayer(game, s, &s->player2);
		if (s->player1.squareNo >= SNL_BOARD_AREA - 1) {
			s->player1.squareNo = SNL_BOARD_AREA - 1;
			s->gameState = GAME_OVER;
		} else if (s->player2.squareNo >= SNL_BOARD_AREA - 1) {
			s->player2.squareNo = SNL_BOARD_AREA - 1;
			s->gameState = GAME_OVER;
		}
	}
}

static void renderBoard(Game* game, SNLState* state) {
	Sprite spr_tile;
	spr_tile.angle = 0;
	spr_tile.origin.x = 0;
	spr_tile.origin.y = 0;
	spr_tile.size.x = SNL_TILE_SIZE;
	spr_tile.size.y = SNL_TILE_SIZE;
	spr_tile.scale.x = SNL_TILE_SIZE / SNL_SPRITE_SIZE;
	spr_tile.scale.y = SNL_TILE_SIZE / SNL_SPRITE_SIZE;
	spr_tile.texCoords.left = 1;
	spr_tile.texCoords.bottom = 1;
	spr_tile.texCoords.right = SNL_SPRITE_SIZE;
	spr_tile.texCoords.top = SNL_SPRITE_SIZE;
	spr_tile.colour.x = 1.0;
	spr_tile.colour.y = 1.0;
	spr_tile.colour.z = 1.0;

	for (int i = 0; i < SNL_BOARD_AREA; i++) {
		if (i % 2 == 0) {
			spr_tile.texture = &state->tile1Texture;
		} else {
			spr_tile.texture = &state->tile2Texture;
		}
		if ((i / SNL_BOARD_DIM) % 2 == 0) {
			spr_tile.position.x = 100 + (COLUMN(i) * SNL_TILE_SIZE);
		} else {
			spr_tile.position.x = 700 - ((COLUMN(i) + 1) * SNL_TILE_SIZE);
		}
		spr_tile.position.y = ROW(i) * SNL_TILE_SIZE;
		renderSprite(&game->renderer, &spr_tile);

		char tileNum[4];
		sprintf(tileNum, "%d", i);

		renderString(&game->renderer, tileNum, spr_tile.position.x + 3, spr_tile.position.y + 1, 15, 15);
	}
}

static void renderSnakes(Game* game, SNLState* state) {
	// for every snake:
	for (int i = 0; i < SNL_SL_NUM; i++) {
		SnakeLadder currSnake = state->snakes[i];
		int xDiff;
		// yDiff is the distance between the rows:
		int yDiff = ROW(currSnake.beginning) - ROW(currSnake.end);

		// xDiff depends on the row each snake endpoint is on:
		if (ROW(currSnake.beginning) % 2 == ROW(currSnake.end) % 2 && ROW(currSnake.beginning) % 2 == 0) {
			// even to even
			xDiff = (COLUMN(currSnake.end) - COLUMN(currSnake.beginning)) % SNL_BOARD_DIM;
		} else if (ROW(currSnake.beginning) % 2 == ROW(currSnake.end) % 2 && ROW(currSnake.beginning) % 2 == 1) {
			// odd to odd
			xDiff = (COLUMN(currSnake.beginning) - COLUMN(currSnake.end)) % SNL_BOARD_DIM;
		} else if (ROW(currSnake.beginning) % 2 != ROW(currSnake.end) % 2 && ROW(currSnake.beginning) % 2 == 1){
			// odd to even
			xDiff = ((COLUMN(currSnake.end) + COLUMN(currSnake.beginning)) - 9) % SNL_BOARD_DIM;
		} else {
			// even to odd
			xDiff = (-(COLUMN(currSnake.beginning) + COLUMN(currSnake.end)) + 9) % SNL_BOARD_DIM;
		}

		Sprite spr_tile;

		spr_tile.texture = &state->snakeTexture;

		spr_tile.size.x = SNL_TILE_SIZE;
		spr_tile.size.y = sqrt(xDiff * xDiff + yDiff * yDiff) * SNL_TILE_SIZE;

		spr_tile.origin.x = spr_tile.size.x / 2;
		spr_tile.origin.y = 0;

		if (yDiff == 0) {
			if (ROW(currSnake.beginning) % 2 == 1) {
				spr_tile.angle = M_PI / 2;
			} else {
				spr_tile.angle = -M_PI / 2;
			}
		} else {
			spr_tile.angle = (float) atan((double) (xDiff) / yDiff);
		}

		spr_tile.angle = (float) atan((double) (xDiff) / yDiff);

		spr_tile.scale.x = 1;
		spr_tile.scale.y = 1;

		spr_tile.texCoords.left = 0;
		spr_tile.texCoords.bottom = 0;
		spr_tile.texCoords.right = 600;
		spr_tile.texCoords.top = 600;

		float hue = (float) (i) / SNL_SL_NUM;
		hsv_to_rgb(hue, 1, 1, &spr_tile.colour.x, &spr_tile.colour.y, &spr_tile.colour.z);

		if (ROW(currSnake.end) % 2 == 0) {
			spr_tile.position.x = 130 + COLUMN(currSnake.end) * SNL_TILE_SIZE;
		} else {
			spr_tile.position.x = 670 - COLUMN(currSnake.end) * SNL_TILE_SIZE;
		}
		spr_tile.position.y = (ROW(currSnake.end) + 0.65) * SNL_TILE_SIZE;
		renderSprite(&game->renderer, &spr_tile);
	}
}

static void renderLadders(Game* game, SNLState* state) {
	// for every ladder:
	for (int i = 0; i < SNL_SL_NUM; i++) {
		SnakeLadder currLadder = state->ladders[i];
		int xDiff;
		// yDiff is the distance between the rows:
		int yDiff = ROW(currLadder.beginning) - ROW(currLadder.end);

		// xDiff depends on the row each ladder endpoint is on:
		// even to even
		if (ROW(currLadder.beginning) % 2 == ROW(currLadder.end) % 2 && ROW(currLadder.beginning) % 2 == 0) {
			xDiff = (COLUMN(currLadder.end) - COLUMN(currLadder.beginning)) % SNL_BOARD_DIM;

		// odd to odd
		} else if (ROW(currLadder.beginning) % 2 == ROW(currLadder.end) % 2 && ROW(currLadder.beginning) % 2 == 1) {
			xDiff = (COLUMN(currLadder.beginning) - COLUMN(currLadder.end)) % SNL_BOARD_DIM;

		// odd to even
		} else if (ROW(currLadder.beginning) % 2 != ROW(currLadder.end) % 2 && ROW(currLadder.beginning) % 2 == 1){
			xDiff = ((COLUMN(currLadder.end) + COLUMN(currLadder.beginning)) - 9) % SNL_BOARD_DIM;

		// even to odd
		} else {
			xDiff = (-(COLUMN(currLadder.beginning) + COLUMN(currLadder.end)) + 9) % SNL_BOARD_DIM;
		}

		Sprite spr_tile;

		spr_tile.texture = &state->ladderTexture;

		spr_tile.size.x = SNL_TILE_SIZE;
		spr_tile.size.y = sqrt(xDiff * xDiff + yDiff * yDiff) * SNL_TILE_SIZE;

		spr_tile.origin.x = spr_tile.size.x / 2;
		spr_tile.origin.y = 0;

		if (yDiff == 0) {
			if (ROW(currLadder.beginning) % 2 == 1) {
				spr_tile.angle = M_PI / 2;
			} else {
				spr_tile.angle = -M_PI / 2;
			}
		} else {
			spr_tile.angle = (float) atan((double) (xDiff) / yDiff);
		}

		spr_tile.scale.x = 1;
		spr_tile.scale.y = 1;

		spr_tile.texCoords.left = 0;
		spr_tile.texCoords.bottom = 0;
		spr_tile.texCoords.right = 600;
		spr_tile.texCoords.top = 600;

		float hue = (float) (i) / SNL_SL_NUM;
		hsv_to_rgb(hue, 0.5, 1, &spr_tile.colour.x, &spr_tile.colour.y, &spr_tile.colour.z);

		if (ROW(currLadder.beginning) % 2 == 0) {
			spr_tile.position.x = 130 + COLUMN(currLadder.beginning) * SNL_TILE_SIZE;
		} else {
			spr_tile.position.x = 670 - COLUMN(currLadder.beginning) * SNL_TILE_SIZE;
		}
		spr_tile.position.y = (ROW(currLadder.beginning) + 0.35) * SNL_TILE_SIZE;
		renderSprite(&game->renderer, &spr_tile);
	}
}

static void renderDice(Game* game, SNLState* s) {
	s->dice.spite.texCoords.left = (s->dice.number - 1) * 32;
	s->dice.spite.texCoords.right = s->dice.number * 32;

	if (s->turn) {
		s->dice.spite.position.x = 10;
		s->dice.spite.position.y = 440;
		renderStringLeft(&game->renderer, "Player", 5, 550, 20);
		renderStringLeft(&game->renderer, "1:", 5, 520, 20);
	} else {
		s->dice.spite.position.x = 710;
		s->dice.spite.position.y = 440;
		renderStringRight(&game->renderer, "Player", 5, 550, 20);
		renderStringRight(&game->renderer, "2:", 5, 520, 20);
	}

	renderSprite(&game->renderer, &s->dice.spite);
}

static void renderPlayer(Game* game, SnLPlayer player) {
	if (ROW(player.squareNo) % 2 == 0) {
		player.sprite.position.x = 100 + (COLUMN(player.squareNo) + 0.5) * SNL_TILE_SIZE;
	} else {
		player.sprite.position.x = 700 - (COLUMN(player.squareNo) + 0.5) * SNL_TILE_SIZE;
	}
	player.sprite.position.y = 10 + (ROW(player.squareNo) * SNL_TILE_SIZE);
	renderSprite(&game->renderer, &player.sprite);
}

void renderSNL_RPG(Game* game, void* state) {
	SNLState* s = state;
	switch (s->gameState) {
	case GAME_PLAY:
		renderBoard(game, state);
		renderSnakes(game, state);
		renderLadders(game, state);
		renderDice(game, s);
		renderPlayer(game, s->player1);
		renderPlayer(game, s->player2);
		break;
	case GAME_MENU:
		RENDER_MENU("Snakes and Ladders", "Press SPACE to start or M to return to menu");
		break;
	case GAME_OVER:
		RENDER_MENU((s->turn) ? "Player 1 Wins!" : "Player 2 Wins!", "Press SPACE to restart or M to return to menu");
		break;
	}
}

void exitSNL_RPG(Game* game, void* state) {
	SNLState* s = state;
	free(s->ladderTexture.pixels);
	free(s->diceTexture.pixels);
	free(s->playerTexture.pixels);
	free(s->tile1Texture.pixels);
	free(s->tile2Texture.pixels);
	free(s->snakeTexture.pixels);
	free(state);
}
