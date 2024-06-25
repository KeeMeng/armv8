#include "minotaurs.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#define MINOTAURS_SPRITE_SIZE 20

static void respawnPlayer(MinotaurState* state);
static void gameOver(MinotaurState* m);

// define maze
char maze[MINOTAURS_MAZEHEIGHT][MINOTAURS_MAZEWIDTH + 1] = {
	"########################################",
	"#                                      #",
	"# #################  ################# #",
	"# #                                  # #",
	"# # ###############  ############### # #",
	"# # #                              # # #",
	"#   # #############  ############# #   #",
	"# # # #   #   #   #  #   #   #   # # # #",
	"# # #   #   #   # #  # #   #   #   # # #",
	"# # # ########### #  # ########### # # #",
	"# # #             #  #             # # #",
	"# # ############# #  # ############# # #",
	"#    #            #  #            #    #",
	"# ## # # ###### # #  # # ###### # # ## #",
	"# #  # #        # #  # #        # #  # #",
	"# # ## ######## # #  # # ######## ## # #",
	"# #  # #        # #  # #        # #  # #",
	"# ## # # ######## #  # ######## # # ## #",
	"#    # #        # #  # #        # #    #",
	"# # ## ######## # #  # # ######## ## # #",
	"# #  # #        # #  # #        # #  # #",
	"# ## # # ######## #  # ######## # # ## #",
	"# #  # #        # #  # #        # #  # #",
	"#   ## # ###### # #  # # ###### # ##   #",
	"# #  #                            #  # #",
	"# ## ##############  ############## ## #",
	"#                                      #",
	"# #################  ################# #",
	"#                                      #",
	"########################################"
};

static int adjacencyMatrix[MINOTAURS_NUM_TILES][MINOTAURS_NUM_TILES];

// load media
static void loadMediaMinotaurs(void* state) {
	MinotaurState* mState = state;

	// load wall texture
	loadTexture(&mState->wallTexture, "./textures/minotaur_wall.png");

	// load player texture
	loadTexture(&mState->playerTexture, "./textures/minotaur_player.png");

	// load coin texture
	loadTexture(&mState->coinTexture, "./textures/coin.png");
}

// build adjacency matrix
static void buildAdjacencyMatrix(void) {
	// reset adjacency matrix to 0
	memset(adjacencyMatrix, 0, sizeof(int) * MINOTAURS_NUM_TILES * MINOTAURS_NUM_TILES);

	// iterate through all tiles
	int row;
	int col;
	for (int i = 0; i < MINOTAURS_NUM_TILES; i++) {
		// check if tile is occupied
		row = i / MINOTAURS_MAZEWIDTH;
		col = i % MINOTAURS_MAZEWIDTH;

		if (maze[row][col] == '#') {
			// wall - not connected to any tiles
			for (int j = 0; j < MINOTAURS_MAZEWIDTH * MINOTAURS_MAZEHEIGHT; j++) {
				adjacencyMatrix[i][j] = 0;
				adjacencyMatrix[j][i] = 0;
			}
		} else {
			// connect to self
			adjacencyMatrix[i][i] = 1;

			// connect to non-wall adjacent neighbours left column
			if (col > 0) {
				if (maze[row][col - 1] != '#') {
					adjacencyMatrix[i][row * MINOTAURS_MAZEWIDTH + col - 1] = 1;
				}
			}

			// centre column
			if (row > 0) {
				if (maze[row - 1][col] != '#') {
					adjacencyMatrix[i][(row - 1) * MINOTAURS_MAZEWIDTH + col] = 1;
				}
			}

			if (row < MINOTAURS_MAZEHEIGHT - 1) {
				if (maze[row + 1][col] != '#') {
					adjacencyMatrix[i][(row + 1) * MINOTAURS_MAZEWIDTH + col] = 1;
				}
			}

			// right column
			if (col < MINOTAURS_MAZEWIDTH - 1) {
				if (maze[row][col + 1] != '#') {
					adjacencyMatrix[i][row * MINOTAURS_MAZEWIDTH + col + 1] = 1;
				}
			}
		}
	}
}

// return the next tile along the path
static int shortestPath(int start, int end) {
	// use dijkstra's algorithm

	// store current distance from start
	int distances[MINOTAURS_NUM_TILES];
	memset(distances, 0, sizeof(distances));

	// store previous nodes
	int prevNodes[MINOTAURS_NUM_TILES];
	memset(prevNodes, 0, sizeof(prevNodes));

	bool visited[MINOTAURS_NUM_TILES];

	for (int i = 0; i < MINOTAURS_NUM_TILES; i++) {
		distances[i] = -1;
		prevNodes[i] = -1;
		visited[i] = false;
	}

	// implement node queue
	bool found = false;
	int nextNode = start;
	int result;

	distances[start] = 0;
	prevNodes[start] = start;

	while (!found) {
		// dequeue
		int node = nextNode;
		visited[node] = true;

		// check all nodes neighbouring the current node and compute the smallest distance of them all
		for (int i = 0; i < MINOTAURS_NUM_TILES; i++) {
			if (!visited[i]) {
				if (adjacencyMatrix[node][i] > 0) {
					// edge exists connecting nodes
					if (distances[node] + adjacencyMatrix[node][i] < distances[i] || distances[i] == -1) {
						distances[i] = distances[node] + adjacencyMatrix[node][i];
						prevNodes[i] = node;
					}
				}
			}
		}

		// determine next node
		if (node == end) {
			// trace back path
			int current = end;
			int prev = end;

			while (current != start) {
				prev = current;
				current = prevNodes[current];
			}

			result = prev;
			found = true;
		} else {
			// determine next node
			int minDistance = -1;
			for (int i = 0; i < MINOTAURS_NUM_TILES; i++) {
				if (!visited[i]) {
					// found candidate
					if ((minDistance < 0 || distances[i] < minDistance) && distances[i] >= 0) {
						minDistance = distances[i];
						nextNode = i;
					}
				}
			}

			if (minDistance == -1) {
				result = -1;
				found = true;
			}
		}
	}

	return result;
}

// initialise player
static void initialisePlayer(MinotaurState* state, MinotaurPlayer* player) {
	// set player position
	player->position.x = 400;
	player->position.y = 200;
	player->size.x = 12;
	player->size.y = 12;
	player->velocity.x = 0;
	player->velocity.y = 0;
	player->sprite.size = player->size;
	player->sprite.scale.x = 1.0;
	player->sprite.scale.y = 1.0;
	player->sprite.texture = &state->playerTexture;
	player->sprite.texCoords.left = 0;
	player->sprite.texCoords.right = MINOTAURS_SPRITE_SIZE;
	player->sprite.texCoords.bottom = 0;
	player->sprite.texCoords.top = MINOTAURS_SPRITE_SIZE;
	player->sprite.angle = 0;
	player->sprite.origin.x = 6;
	player->sprite.origin.y = 6;
	player->sprite.colour.x = 1.0;
	player->sprite.colour.y = 1.0;
	player->sprite.colour.z = 1.0;
	player->speed = MINOTAURS_PLAYER_SPEED;
	player->dir = 1;
}

// update player
static void updatePlayer(Game* game, MinotaurPlayer* player) {
	// check keys
	if (game->keysDown[GLFW_KEY_A] || game->keysDown[GLFW_KEY_LEFT]) {
		// set velocity to left
		player->velocity.x = -player->speed;
		player->sprite.scale.x = -1;
		player->dir = 0;
	} else if (game->keysDown[GLFW_KEY_D] || game->keysDown[GLFW_KEY_RIGHT]) {
		// set velocity to right
		player->velocity.x = player->speed;
		player->sprite.scale.x = 1;
		player->dir = 1;
	} else {
		player->velocity.x = 0;
	}

	if (game->keysDown[GLFW_KEY_W] || game->keysDown[GLFW_KEY_UP]) {
		// set velocity to up
		player->velocity.y = player->speed;
		player->dir = 2;
	} else if (game->keysDown[GLFW_KEY_S] || game->keysDown[GLFW_KEY_DOWN]) {
		// set velocity to down
		player->velocity.y = -player->speed;
		player->dir = 3;
	} else {
		player->velocity.y = 0;
	}

	// check for collision with tiles
	double tileX;
	double tileY;
	double playerNextX = player->position.x + player->velocity.x * game->deltaTime;
	double playerNextY = player->position.y + player->velocity.y * game->deltaTime;

	for (int i = 0; i < MINOTAURS_MAZEHEIGHT; i++) {
		for (int j = 0; j < MINOTAURS_MAZEWIDTH; j++) {
			// check that tile is wall
			if (maze[i][j] == '#') {
				tileX = j * MINOTAURS_TILESIZE + MINOTAURS_TILESIZE / 2;
				tileY = (MINOTAURS_MAZEHEIGHT - i) * MINOTAURS_TILESIZE - MINOTAURS_TILESIZE / 2;

				// check for horizontal collision
				if (fabs(playerNextX - tileX) < (player->size.x + MINOTAURS_TILESIZE) / 2 && fabs(player->position.y - tileY) < (player->size.y + MINOTAURS_TILESIZE) / 2) {
					// collision has occured
					if (player->position.x < tileX) {
						player->position.x = tileX - (player->size.x + MINOTAURS_TILESIZE) / 2;
					} else {
						player->position.x = tileX + (player->size.x + MINOTAURS_TILESIZE) / 2;
					}
					player->velocity.x = 0;
				}

				// check for vertical collision
				if (fabs(player->position.x - tileX) < (player->size.x + MINOTAURS_TILESIZE) / 2 && fabs(playerNextY - tileY) < (player->size.y + MINOTAURS_TILESIZE) / 2) {
					// collision has occured
					if (player->position.y < tileY) {
						player->position.y = tileY - (player->size.y + MINOTAURS_TILESIZE) / 2;
					} else {
						player->position.y = tileY + (player->size.y + MINOTAURS_TILESIZE) / 2;
					}

					player->velocity.y = 0;
				}
			}
		}
	}

	player->position.x += player->velocity.x * game->deltaTime;
	player->position.y += player->velocity.y * game->deltaTime;

	// frame time
	player->frameTime += game->deltaTime;

	if (fabs(player->velocity.x) > 0 || fabs(player->velocity.y) > 0) {
		if (player->frameTime > 1) {
			player->frameTime = 0;
		}

		// moving
		player->sprite.texCoords.left = (int) (4 + ((player->frameTime) / 1.0) * 7.0) * MINOTAURS_SPRITE_SIZE;
	} else {
		if (player->frameTime > 0.8) {
			player->frameTime = 0;
		}

		// moving
		if (player->frameTime < 0.2) {
			player->sprite.texCoords.left = 0;
		} else if (player->frameTime < 0.4) {
			player->sprite.texCoords.left = MINOTAURS_SPRITE_SIZE;
		} else if (player->frameTime < 0.6) {
			player->sprite.texCoords.left = 2 * MINOTAURS_SPRITE_SIZE;
		} else if (player->frameTime < 0.8) {
			player->sprite.texCoords.left = MINOTAURS_SPRITE_SIZE;
		}
	}

	player->sprite.texCoords.right = player->sprite.texCoords.left + MINOTAURS_SPRITE_SIZE;
}

// render player
static void renderPlayer(Game* game, MinotaurPlayer* player) {
	// draw player
	player->sprite.position = player->position;
	renderSprite(&game->renderer, &player->sprite);
}

// initialise enemy
static void initialiseEnemy(MinotaurState* state, MinotaurEnemy* enemy, int type) {
	// set player position
	enemy->position.x = MINOTAURS_TILESIZE * 1.5;
	enemy->position.y = MINOTAURS_TILESIZE * 1.5;

	switch (type) {
	case 0:
		enemy->position.x = MINOTAURS_TILESIZE * 1.5;
		enemy->position.y = MINOTAURS_TILESIZE * 1.5;
		enemy->sprite.colour.x = 1.0;
		enemy->sprite.colour.y = 0.5;
		enemy->sprite.colour.z = 0.5;
		break;

	case 1:
		enemy->position.x = MINOTAURS_MAZEWIDTH * MINOTAURS_TILESIZE - MINOTAURS_TILESIZE * 1.5;
		enemy->position.y = MINOTAURS_TILESIZE * 1.5;
		enemy->sprite.colour.x = 0.5;
		enemy->sprite.colour.y = 1.0;
		enemy->sprite.colour.z = 0.5;
		break;

	case 2:
		enemy->position.x = MINOTAURS_MAZEWIDTH * MINOTAURS_TILESIZE - MINOTAURS_TILESIZE * 1.5;
		enemy->position.y = MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE - MINOTAURS_TILESIZE * 1.5;
		enemy->sprite.colour.x = 0.5;
		enemy->sprite.colour.y = 0.5;
		enemy->sprite.colour.z = 1.0;
		break;

	default:
		enemy->position.x = MINOTAURS_TILESIZE * 1.5;
		enemy->position.y = MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE - MINOTAURS_TILESIZE * 1.5;
		enemy->sprite.colour.x = 1.0;
		enemy->sprite.colour.y = 1.0;
		enemy->sprite.colour.z = 0.5;
		break;
	}

	enemy->type = type;
	enemy->size.x = 16;
	enemy->size.y = 16;
	enemy->velocity.x = 0;
	enemy->velocity.y = 0;
	enemy->sprite.size = enemy->size;
	enemy->sprite.scale.x = 1;
	enemy->sprite.scale.y = -1;
	enemy->sprite.texture = &state->playerTexture;
	enemy->sprite.texCoords.left = 0;
	enemy->sprite.texCoords.right = MINOTAURS_SPRITE_SIZE;
	enemy->sprite.texCoords.bottom = 0;
	enemy->sprite.texCoords.top = MINOTAURS_SPRITE_SIZE;
	enemy->sprite.origin.x = 8;
	enemy->sprite.origin.y = 8;
	enemy->sprite.angle = 0;
	enemy->speed = MINOTAURS_ENEMY_SPEED + ((state->player.speed - MINOTAURS_ENEMY_SPEED) * state->gameRound / 50.0);
	enemy->target = -1;

	if (state->gameRound > 50) {
		enemy->speed = state->player.speed - 1;
	}

	enemy->alive = true;
}

static int coordsToTile(float x, float y) {
	int col = x / MINOTAURS_TILESIZE;
	int row = (MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE - y) / MINOTAURS_TILESIZE;
	return row * MINOTAURS_MAZEWIDTH + col;
}

static void tileToCoords(int tile, float* x, float* y) {
	// get row and column
	int col = tile % MINOTAURS_MAZEWIDTH;
	int row = tile / MINOTAURS_MAZEWIDTH;

	*x = col * MINOTAURS_TILESIZE + MINOTAURS_TILESIZE / 2;
	*y = MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE - row * MINOTAURS_TILESIZE - MINOTAURS_TILESIZE / 2;
}

// update enemy
static void updateEnemy(Game* game, MinotaurState* state, MinotaurEnemy* enemy) {
	// get current tile
	int enemyTile = coordsToTile(enemy->position.x, enemy->position.y);
	int playerTile = coordsToTile(state->player.position.x, state->player.position.y);

	// get next tile
	int nextTile = 0;

	// determine next tile based on type
	if (enemy->type == 0) {
		float targetX = state->player.position.x;
		float targetY = state->player.position.y;

		// target tile ahead of player if present
		switch (state->player.dir) {
		case 0:
			// left
			targetX -= MINOTAURS_TILESIZE;
			break;
		case 1:
			// right
			targetX += MINOTAURS_TILESIZE;
			break;
		case 2:
			// up
			targetY += MINOTAURS_TILESIZE;
			break;
		default:
			// down
			targetY -= MINOTAURS_TILESIZE;
			break;
		}

		// check if target coordinates are on map
		int targetTile;
		if (targetX > 0 && targetX < MINOTAURS_MAZEWIDTH * MINOTAURS_TILESIZE && targetY > 0 && targetY < MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE) {
			// on map - check that not a wall
			targetTile = coordsToTile(targetX, targetY);

			if (maze[targetTile / MINOTAURS_MAZEWIDTH][targetTile % MINOTAURS_MAZEWIDTH] == '#') {
				// wall found - target player instead
				targetTile = coordsToTile(state->player.position.x, state->player.position.y);
			}
		} else {
			targetTile = coordsToTile(state->player.position.x, state->player.position.y);
		}

		nextTile = shortestPath(enemyTile, targetTile);
	} else if (enemy->type == 1) {
		float targetX = state->player.position.x;
		float targetY = state->player.position.y;

		// target tile ahead of player if present
		switch (state->player.dir) {
		case 0:
			// left
			targetX -= MINOTAURS_TILESIZE;
			break;

		case 1:
			// right
			targetX += MINOTAURS_TILESIZE;
			break;

		case 2:
			// up
			targetY += MINOTAURS_TILESIZE;
			break;

		default:
			// down
			targetY -= MINOTAURS_TILESIZE;
			break;
		}

		// check if target coordinates are on map
		int targetTile;
		if (targetX > 0 && targetX < MINOTAURS_MAZEWIDTH * MINOTAURS_TILESIZE && targetY > 0 && targetY < MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE) {
			// on map - check that not a wall
			targetTile = coordsToTile(targetX, targetY);

			if (maze[targetTile / MINOTAURS_MAZEWIDTH][targetTile % MINOTAURS_MAZEWIDTH] == '#') {
				// wall found - target player instead
				targetTile = coordsToTile(state->player.position.x, state->player.position.y);
			}
		} else {
			targetTile = coordsToTile(state->player.position.x, state->player.position.y);
		}

		nextTile = shortestPath(enemyTile, targetTile);
	} else if (enemy->type == 2 || enemy->type == 3) {
		// type 2: if player is not in the same horizontal half of the map, chase a random tile until the player is
		// type 3: if player is not in the same vertical half of the map, chase a random tile until the player is

		bool isInSection = true;

		if (enemy->type == 2) {
			if (enemy->position.x < MINOTAURS_MAZEWIDTH * MINOTAURS_TILESIZE / 2) {
				if (state->player.position.x < MINOTAURS_MAZEWIDTH * MINOTAURS_TILESIZE / 2) {
					isInSection = true;
				} else {
					isInSection = false;
				}
			} else {
				if (state->player.position.x < MINOTAURS_MAZEWIDTH * MINOTAURS_TILESIZE / 2) {
					isInSection = false;
				} else {
					isInSection = true;
				}
			}
		} else {
			if (enemy->position.y < MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE / 2) {
				if (state->player.position.y < MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE / 2) {
					isInSection = true;
				} else {
					isInSection = false;
				}
			} else {
				if (state->player.position.y < MINOTAURS_MAZEHEIGHT * MINOTAURS_TILESIZE / 2) {
					isInSection = false;
				} else {
					isInSection = true;
				}
			}
		}

		if (isInSection) {
			// chase player
			nextTile = shortestPath(enemyTile, playerTile);

			// reset target
			enemy->target = -1;
		} else {
			if (enemy->target == -1 || enemyTile == enemy->target) {
				// choose a random time
				do {
					enemy->target = rand() % MINOTAURS_NUM_TILES;
				} while (maze[enemy->target / MINOTAURS_MAZEWIDTH][enemy->target % MINOTAURS_MAZEWIDTH] == '#');
			}
			nextTile = shortestPath(enemyTile, enemy->target);
		}
	}

	if (nextTile != -1) {
		// get coordinates
		float x;
		float y;

		tileToCoords(nextTile, &x, &y);

		// move towards x and y
		int hDir = 0;
		int vDir = 0;

		if (enemy->position.x < x) {
			enemy->velocity.x = enemy->speed;
			hDir = -1;
		} else if (enemy->position.x > x) {
			enemy->velocity.x = -enemy->speed;
			hDir = 1;
		} else {
			enemy->velocity.x = 0;
			hDir = 0;
		}

		if (enemy->position.y < y) {
			enemy->velocity.y = enemy->speed;
			vDir = -1;
		} else if (enemy->position.y > y) {
			enemy->velocity.y = -enemy->speed;
			vDir = 1;
		} else {
			enemy->velocity.y = 0;
			vDir = 0;
		}

		// predict new coordinate
		double newX = enemy->position.x + game->deltaTime * enemy->velocity.x;
		double newY = enemy->position.y + game->deltaTime * enemy->velocity.y;

		int newHDir;
		int newVDir;

		if (newX < x) {
			newHDir = -1;
		} else if (newX > x) {
			newHDir = 1;
		} else {
			newHDir = 0;
		}

		if (newY < y) {
			newVDir = -1;
		} else if (newY > y) {
			newVDir = 1;
		} else {
			newVDir = 0;
		}

		if (hDir != newHDir) {
			enemy->velocity.x = (x - enemy->position.x) / game->deltaTime;
		}

		if (vDir != newVDir) {
			enemy->velocity.y = (y - enemy->position.y) / game->deltaTime;
		}
	} else {
		enemy->velocity.x = 0;
		enemy->velocity.y = 0;
	}

	// check for collision with player
	if (fabs(enemy->position.x - state->player.position.x) < (enemy->size.x + state->player.size.x) / 2 && fabs(enemy->position.y - state->player.position.y) < (enemy->size.y + state->player.size.y) / 2) {
		// collision
		respawnPlayer(state);
	}

	enemy->position.x += game->deltaTime * enemy->velocity.x;
	enemy->position.y += game->deltaTime * enemy->velocity.y;
}

// render enemy
static void renderEnemy(Game* game, MinotaurEnemy* enemy) {
	// draw enemy
	enemy->sprite.position = enemy->position;
	renderSprite(&game->renderer, &enemy->sprite);
}

// initialise coin
static void initialiseCoin(MinotaurState* state, MinotaurPickup* coin, float x, float y) {
	coin->alive = true;
	coin->position.x = x;
	coin->position.y = y;
	coin->size.x = 4;
	coin->size.y = 4;
	coin->sprite.texture = &state->coinTexture;
	coin->sprite.size.x = coin->size.x;
	coin->sprite.size.y = coin->size.y;
	coin->sprite.scale.x = 1;
	coin->sprite.scale.y = 1;
	coin->sprite.texCoords.left = 0;
	coin->sprite.texCoords.right = 4;
	coin->sprite.texCoords.bottom = 0;
	coin->sprite.texCoords.top = 4;
	coin->sprite.angle = 0;
	coin->sprite.colour.x = 1.0;
	coin->sprite.colour.y = 1.0;
	coin->sprite.colour.z = 1.0;
	coin->sprite.origin.x = 2;
	coin->sprite.origin.y = 2;
}

// update coin
static void updateCoin(MinotaurState* state, MinotaurPickup* coin) {
	// check for collision with player
	if (fabs(coin->position.x - state->player.position.x) < (coin->size.x + state->player.size.x) / 2 && fabs(coin->position.y - state->player.position.y) < (coin->size.y + state->player.size.y) / 2) {
		coin->alive = false;
		state->score += 1;
	}
}

// render coin
static void renderCoin(Game* game, MinotaurPickup* coin) {
	// draw enemy
	coin->sprite.position = coin->position;
	renderSprite(&game->renderer, &coin->sprite);
}

static void resetMinotaurs(MinotaurState* state) {
	// reset all minotaurs
	// set all minotaurs to dead
	for (int i = 0; i < 4; i++) {
		initialiseEnemy(state, &state->enemies[i], i);
	}
}

static void createCoins(MinotaurState* state) {
	// create all coins
	for (int i = 0; i < MINOTAURS_NUM_TILES; i++) {
		state->pickups[i].alive = false;

		int row = i / MINOTAURS_MAZEWIDTH;
		int col = i % MINOTAURS_MAZEWIDTH;
		if (maze[row][col] != '#') {
			// place coin
			initialiseCoin(state, &state->pickups[i], col * MINOTAURS_TILESIZE + MINOTAURS_TILESIZE / 2, (MINOTAURS_MAZEHEIGHT - row) * MINOTAURS_TILESIZE - MINOTAURS_TILESIZE / 2);
		}
	}
}

static void resetPlayerCoords(MinotaurState* state) {
	// reset player coordinates
	state->player.position.x = INITIAL_WIDTH / 2;
	state->player.position.y = INITIAL_HEIGHT / 2;
}

// next round
static void nextRound(MinotaurState* state) {
	state->gameRound++;

	// reset minotaurs
	resetMinotaurs(state);

	// create coins
	createCoins(state);

	// reset player coordinate
	resetPlayerCoords(state);
}

static void respawnPlayer(MinotaurState* state) {
	// decrement lives/
	state->lives--;

	if (state->lives == 0) {
		// game over screen
		gameOver(state);
	} else {
		// reset player coordinates
		resetPlayerCoords(state);

		// reset minotaurs
		resetMinotaurs(state);
	}
}

// start game
static void startGame(MinotaurState* m) {
	m->gameState = GAME_PLAY;

	// initialise player
	initialisePlayer(m, &m->player);

	// reset score
	m->score = 0;

	// reset lives
	m->lives = 3;

	resetMinotaurs(m);
	createCoins(m);
}

// game over
static void gameOver(MinotaurState* m) {
	// set state
	m->gameState = GAME_OVER;

	// load leaderboard;
	FILE* scoreFile = fopen("minotaursScores.bin", "rb");

	if (scoreFile != NULL) {
		// read highscores
		size_t numRead = fread(&m->highScores[0], sizeof(unsigned int), MINOTAURS_NUM_SCORES, scoreFile);
		assert(numRead == MINOTAURS_NUM_SCORES);

		// compare new score to high scores
		for (int i = 0; i < MINOTAURS_NUM_SCORES; i++) {
			if ((unsigned int) m->score > m->highScores[i]) {
				// insert score into position and shift all lower scores along
				for (int j = MINOTAURS_NUM_SCORES - 1; j > i; j--) {
					m->highScores[j] = m->highScores[j - 1];
				}

				m->highScores[i] = m->score;
				break;
			}
		}


		// close file and open in write mode
		fclose(scoreFile);
		scoreFile = fopen("minotaursScores.bin", "wb");
	} else {
		// score file does not exist - reopen as write
		scoreFile = fopen("minotaursScores.bin", "wb");

		// create array of scores
		for (int i = 0; i < MINOTAURS_NUM_SCORES; i++) {
			m->highScores[i] = 0;
		}

		m->highScores[0] = m->score;
	}

	// write to file
	fwrite(m->highScores, sizeof(unsigned int), MINOTAURS_NUM_SCORES, scoreFile);

	// close file
	fclose(scoreFile);
}

// initialise function
void initMinotaurs(Game* game, void** state) {
	// allocate state
	SAFE_MALLOC(*state, sizeof(MinotaurState));

	loadMediaMinotaurs(*state);

	// initialise shortest paths
	buildAdjacencyMatrix();

	MinotaurState* m = *state;

	// set state to menu
	m->gameState = GAME_MENU;
	m->gameRound = 0;
}

// update game
void updateMinotaurs(Game* game, void* state) {
	// update player
	MinotaurState* m = state;

	// check game state
	if (m->gameState == GAME_MENU || m->gameState == GAME_OVER) {
		// check for spacebar
		if (game->keysDown[GLFW_KEY_SPACE] == GLFW_PRESS && game->keyTime[GLFW_KEY_SPACE] == 0.0) {
			// start game
			startGame(m);
		}
	} else if (m->gameState == GAME_PLAY) {
		updatePlayer(game, &m->player);

		for (int i = 0; i < 4; i++) {
			if (m->enemies[i].alive) {
				updateEnemy(game, state, &m->enemies[i]);
			}
		}

		for (int i = 0; i < MINOTAURS_NUM_TILES; i++) {
			if (m->pickups[i].alive) {
				updateCoin(state, &m->pickups[i]);
			}
		}

		// check if all coins were collected
		bool roundEnd = true;
		for (int i = 0; i < MINOTAURS_NUM_TILES; i++) {
			if (m->pickups[i].alive) {
				roundEnd = false;
			}
		}

		if (roundEnd) {
			nextRound(state);
		}
	}
}

// render maze
static void renderMaze(Game* game, MinotaurState* state) {
	// create tilemap
	Sprite spr_tile;
	spr_tile.texture = &state->wallTexture;
	spr_tile.angle = 0;
	spr_tile.origin.x = 0;
	spr_tile.origin.y = 0;
	spr_tile.size.x = MINOTAURS_TILESIZE;
	spr_tile.size.y = MINOTAURS_TILESIZE;
	spr_tile.scale.x = 1;
	spr_tile.scale.y = 1;
	spr_tile.texCoords.left = 1.0;
	spr_tile.texCoords.bottom = 1.0;
	spr_tile.texCoords.right = MINOTAURS_TILESIZE;
	spr_tile.texCoords.top = MINOTAURS_TILESIZE;
	spr_tile.colour.x = 1.0;
	spr_tile.colour.y = 1.0;
	spr_tile.colour.z = 1.0;

	// iterate through all tiles
	for (int i = 0; i < MINOTAURS_MAZEHEIGHT; i++) {
		for (int j = 0; j < MINOTAURS_MAZEWIDTH; j++) {
			if (maze[i][j] == '#') {
				spr_tile.position.x = j * MINOTAURS_TILESIZE;
				spr_tile.position.y = (MINOTAURS_MAZEHEIGHT - i - 1) * MINOTAURS_TILESIZE;
				renderSprite(&game->renderer, &spr_tile);
			}
		}
	}
}

// render game
void renderMinotaurs(Game* game, void* state) {
	MinotaurState* m = state;

	if (m->gameState == GAME_PLAY) {
		// render maze
		renderMaze(game, state);

		// render coins
		for (int i = 0; i < MINOTAURS_NUM_TILES; i++) {
			if (m->pickups[i].alive) {
				renderCoin(game, &m->pickups[i]);
			}
		}

		// render player
		renderPlayer(game, &m->player);

		for (int i = 0; i < 4; i++) {
			if (m->enemies[i].alive) {
				renderEnemy(game, &m->enemies[i]);
			}
		}

		char scoreStr[512];
		sprintf(scoreStr, "Score: %d", m->score);
		renderStringLeft(&game->renderer, scoreStr, 20, 582, 16);

		char roundStr[512];
		sprintf(roundStr, "Round: %d", m->gameRound + 1);
		renderStringRight(&game->renderer, roundStr, 20, 582, 16);

		char livesStr[512];
		sprintf(livesStr, "Lives: %d", m->lives);
		renderStringCentered(&game->renderer, livesStr, 582, 16);
	} else if (m->gameState == GAME_MENU) {
		// main menu
		RENDER_MENU("Minotaurs", "Press SPACE to start or M to return to menu");
	} else if (m->gameState == GAME_OVER) {
		// main menu
		renderStringCentered(&game->renderer, "Game Over - High Scores", 500, 32);

		// render score table
		char scoreStr[512];
		for (int i = 0; i < MINOTAURS_NUM_SCORES; i++) {
			sprintf(scoreStr, "%d %d", i + 1, m->highScores[i]);
			renderStringCentered(&game->renderer, scoreStr, 450 - i * 24, 16);
		}

		sprintf(scoreStr, "Score: %d", m->score);
		renderStringCentered(&game->renderer, scoreStr, 150, 32);
		RENDER_SUBTITLE("Press SPACE to restart or M to return to menu");
	}
}

// render minotaurs
void exitMinotaurs(Game* game, void* state) {
	// free textures
	MinotaurState* m = state;
	free(m->wallTexture.pixels);
	free(m->playerTexture.pixels);
	free(m->coinTexture.pixels);
	free(state);
}
