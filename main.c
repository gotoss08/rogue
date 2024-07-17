#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define CELL_SIZE 12

#define RATIO_OFFSET 30
#define MIN_RATIO 50 - RATIO_OFFSET
#define MAX_RATIO 50 + RATIO_OFFSET
#define MIN_ROOM_SIZE 7
#define TREE_GENERATION_ITERATIONS 10

typedef struct Leaf Leaf;

struct Leaf {
	
	Leaf* left;
	Leaf* right;

	int x, y, w, h;

};

typedef enum {
	FLOOR = 0,
	WALL
} CellType;

typedef struct {
	CellType type;
} Cell;

typedef struct Room Room;

struct Room {

	Room* left;
	Room* right;
	Room* up;
	Room* down;

	int x, y, w, h;

};

#define MAX_ROOMS_SIZE 256

typedef struct {
	Cell cells[MAP_WIDTH][MAP_HEIGHT];
	Room rooms[MAX_ROOMS_SIZE];
	int roomsCount;
} Map;

void generateLeafs(Leaf* parent, int depth) {

	if (depth == 0) return;

	bool canFitW = parent->w - (parent->w / 2) > MIN_ROOM_SIZE;
	bool canFitH = parent->h - (parent->h / 2) > MIN_ROOM_SIZE;

	if (!canFitW && !canFitH) return;

	bool splitV;

	if (canFitW && canFitH) splitV = GetRandomValue(0, 1);
	else if (canFitW && !canFitH) splitV = true;
	else splitV = false;

	double ratio;

	if (splitV) {

		if (!canFitW) return;

		int newW;

		do {
			ratio = (double)GetRandomValue(MIN_RATIO, MAX_RATIO) / 100;
			newW = floor((double)parent->w * ratio);
		} while (newW < MIN_ROOM_SIZE || parent->w - newW < MIN_ROOM_SIZE);
		
		parent->left = calloc(1, sizeof(Leaf));

		assert(parent->left != NULL);

		parent->left->x = parent->x;
		parent->left->y = parent->y;
		parent->left->w = newW;
		parent->left->h = parent->h;

		parent->right = calloc(1, sizeof(Leaf));

		assert(parent->right != NULL);

		parent->right->x = parent->x + parent->left->w;
		parent->right->y = parent->y;
		parent->right->w = parent->w - parent->left->w;
		parent->right->h = parent->h;
	}
	else {

		if (!canFitH) return;

		int newH;
		do {
			ratio = (double)GetRandomValue(MIN_RATIO, MAX_RATIO) / 100;
			newH = floor((double)parent->h * ratio);
		} while (newH < MIN_ROOM_SIZE || parent->h - newH < MIN_ROOM_SIZE);

		parent->left = calloc(1, sizeof(Leaf));

		assert(parent->left != NULL);

		parent->left->x = parent->x;
		parent->left->y = parent->y;
		parent->left->w = parent->w;
		parent->left->h = newH;

		parent->right = calloc(1, sizeof(Leaf));

		assert(parent->right != NULL);

		parent->right->x = parent->x;
		parent->right->y = parent->y + parent->left->h;
		parent->right->w = parent->w;
		parent->right->h = parent->h - parent->left->h;

	}

	generateLeafs(parent->left, depth - 1);
	generateLeafs(parent->right, depth - 1);

}

Leaf* generateTree() {

	Leaf* tree = calloc(1, sizeof(Leaf));

	assert(tree != NULL);

	tree->x = 0;
	tree->y = 0;
	tree->w = MAP_WIDTH;
	tree->h = MAP_HEIGHT;

	return tree;

}

void freeTree(Leaf* tree) {
	
	if (tree->left) freeTree(tree->left);
	if (tree->right) freeTree(tree->right);

	free(tree);

}

void renderTree(Leaf* tree) {

	if (tree == NULL) return;

	if (tree->left == NULL && tree->right == NULL) {
		DrawRectangleLines(tree->x * CELL_SIZE, tree->y * CELL_SIZE, tree->w * CELL_SIZE, tree->h * CELL_SIZE, GREEN);
	}

	renderTree(tree->left);
	renderTree(tree->right);

}

void clearMap(Map* map) {

	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			map->cells[y][x].type = FLOOR;
		}
	}

	map->roomsCount = 0;

}

Map* createMap() {

	Map* map = calloc(1, sizeof(Map));

	assert(map != NULL);

	clearMap(map);

	return map;

}

void carveRooms(Map* map, Leaf* tree) {

	if (tree == NULL) return;

	if (tree->left == NULL && tree->right == NULL) {
		for (int x = 0; x < tree->w; ++x) {
			for (int y = 0; y < tree->h; ++y) {
				if (x == 0 || x == tree->w - 1 || y == 0 || y == tree->h - 1)
					map->cells[tree->y + y][tree->x + x].type = WALL;
			}
		}
		
		map->rooms[map->roomsCount].x = tree->x;
		map->rooms[map->roomsCount].y = tree->y;
		map->rooms[map->roomsCount].w = tree->w;
		map->rooms[map->roomsCount].h = tree->h;

		map->roomsCount++;

	}

	carveRooms(map, tree->left);
	carveRooms(map, tree->right);

}

void connectRooms(Map* map, Leaf* tree, bool firstIteration) {

	if (tree == NULL || tree->left == NULL || tree->right == NULL) return;

	connectRooms(map, tree->left, false);
	connectRooms(map, tree->right, false);

	//if (firstIteration) return;
	firstIteration = false;

	for (int x = 0; x < tree->w; ++x) {
		for (int y = 0; y < tree->h; ++y) {
			if (x == 0 || x == tree->w - 1 || y == 0 || y == tree->h - 1)
				map->cells[tree->y + y][tree->x + x].type = WALL;
		}
	}

	int cursorX = tree->left->x + floor(tree->left->w / 2), cursorY = tree->left->y + floor(tree->left->h / 2);
	int targetX = tree->right->x + floor(tree->right->w / 2), targetY = tree->right->y + floor(tree->right->h / 2);

	do {

		if (cursorX > targetX) cursorX--;
		else if (cursorX < targetX) cursorX++;

		map->cells[cursorY][cursorX].type = FLOOR;

		if (cursorY > targetY) cursorY--;
		else if (cursorY < targetY) cursorY++;

		map->cells[cursorY][cursorX].type = FLOOR;

	} while (cursorX != targetX || cursorY != targetY);

}

void renderMap(Map* map) {

	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {

			Cell cell = map->cells[y][x];
			Color color;

			switch (cell.type)
			{
			case FLOOR:
				color = RED;
				break;
			case WALL:
				color = DARKGRAY;
				break;
			default:
				break;
			}

			DrawRectangleLines(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);

		}
	}

	//for (int i = 0; i < map->roomsCount; ++i) {
	//	
	//	Room room = map->rooms[i];
	//	DrawRectangleLines(room.x * CELL_SIZE, room.y * CELL_SIZE, room.w * CELL_SIZE, room.h * CELL_SIZE, BLUE);

	//}

}

int main() {

	SetRandomSeed(time(0));

	int generationIterations = TREE_GENERATION_ITERATIONS;

	Leaf* tree = generateTree();
	generateLeafs(tree, generationIterations);

	Map* map = createMap();
	carveRooms(map, tree);
	connectRooms(map, tree, true);

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "rogue 0.0");

	while (!WindowShouldClose()) {

		if (IsKeyPressed(KEY_R)) {
			freeTree(tree);
			tree = generateTree();
			generateLeafs(tree, generationIterations);
			clearMap(map);
			carveRooms(map, tree);
			connectRooms(map, tree, true);
		}

		BeginDrawing();
		ClearBackground(BLACK);
		//renderTree(tree);
		renderMap(map);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}