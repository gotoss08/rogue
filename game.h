#ifndef GAME_H
#define GAME_H

#define RND(max) rand() % max

int random_int(int min, int max) {
    return min + rand() % (max + 1 - min);
}

float lerp(float v0, float v1, float t) {
    return (1 - t) * v0 + t * v1;
}

#define DEFAULT_LERP_T 0.1

#define MAP_FONT_NAME "DejaVuSansMono.ttf"
#define MAP_FONT_SIZE 32
#define MAP_TILE_SIZE 32

#define MAP_SIZE 64
#define ACTORS_SIZE 1024

typedef struct {
    int x;
    int y;
} Coord;

typedef struct {
    char* ch;
    Color color;
} Glyph;

typedef struct {
    Glyph glyph;
    Color bg_color;
} Tile;

typedef struct {
    Coord coord;
    Glyph glyph;
} Actor;

typedef struct {
    Tile tiles[MAP_SIZE][MAP_SIZE];
    Actor actors[ACTORS_SIZE];
} Map;

typedef struct {
    Coord pos;
    Coord target_pos;
} MapCamera;

typedef struct {
    Actor actor;
} Player;

typedef struct {
    Font font;
    Player player;
    Map map;
    MapCamera camera;
} Game;

#endif // GAME_H
