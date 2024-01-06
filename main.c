#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "bsp.h"

Game game;

Player init_player() {

    Actor actor;

    actor.coord.x = 0;
    actor.coord.y = 0;

    actor.glyph.ch = "@";
    actor.glyph.color = GREEN;

    Player player;
    player.actor = actor;

    return player;

}

MapCamera init_camera() {

    MapCamera camera = {0};
    return camera;

}

void update_camera_target(MapCamera* camera, Coord new_target) {
    camera->target_pos = new_target;
}

void update_camera(MapCamera* camera) {

    const int screen_width = GetScreenWidth();
    const int screen_height = GetScreenHeight();

    camera->pos.x = lerp(camera->pos.x, camera->target_pos.x * MAP_TILE_SIZE - screen_width / 2, DEFAULT_LERP_T);
    camera->pos.y = lerp(camera->pos.y, camera->target_pos.y * MAP_TILE_SIZE - screen_height / 2, DEFAULT_LERP_T);

}

Map generate_map() {

    TraceLog(LOG_INFO, "Generating map...");

    Map map;

    for (size_t y = 0; y < MAP_SIZE; ++y) {
        for (size_t x = 0; x < MAP_SIZE; ++x) {

            Tile t;
            t.glyph = (Glyph) { .color = WHITE };
            t.bg_color = BLACK;

            switch (RND(2)) {
                case 0: t.glyph.ch = "#"; break;
                case 1: t.glyph.ch = "."; break;
                default: t.glyph.ch = "~"; break;
            }

            map.tiles[y][x] = t;

        }
    }

    return map;

}

void render_tile(const Tile* tile, const int x, const int y) {

    const Vector2 pos = { .x = (x) * MAP_TILE_SIZE - game.camera.pos.x, .y = (y) * MAP_TILE_SIZE - game.camera.pos.y};
    DrawRectangle(pos.x, pos.y, MAP_TILE_SIZE, MAP_TILE_SIZE, tile->bg_color);
    DrawTextEx(game.font, tile->glyph.ch, pos, MAP_FONT_SIZE, 0, tile->glyph.color);

}

void render_actor(const Actor* actor) {

    const Vector2 pos = { .x = (actor->coord.x) * MAP_TILE_SIZE - game.camera.pos.x, .y = (actor->coord.y) * MAP_TILE_SIZE - game.camera.pos.y};
    DrawRectangle(pos.x, pos.y, MAP_TILE_SIZE, MAP_TILE_SIZE, BLACK);
    DrawTextEx(game.font, actor->glyph.ch, pos, MAP_FONT_SIZE, 0, actor->glyph.color);

}

void render_map(const Map* map) {

    for (size_t y = 0; y < MAP_SIZE; ++y) {
        for (size_t x = 0; x < MAP_SIZE; ++x) {
            render_tile(&map->tiles[y][x], x, y);
        }
    }

}

void handle_keys() {

    bool has_event = false;
    Coord* coord = &game.player.actor.coord;

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        has_event = true;
        coord->x -= 1;
    }

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        has_event = true;
        coord->x += 1;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
        has_event = true;
        coord->y -= 1;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
        has_event = true;
        coord->y += 1;
    }

    if (has_event) {
        // printf("player position changed: %dx%d\n", coord->x, coord->y);
    }

}

int main() {

    srand(time(0));

    BSP_Leaf* tree = build_bsp_dungeon(NULL, 128, 128);
    print_tree(tree, 0);

    InitWindow(800, 600, "Raylib Rogue");

    game.font = LoadFontEx(MAP_FONT_NAME, MAP_FONT_SIZE, NULL, 0);
    game.player = init_player();
    game.map = generate_map();
    game.camera = init_camera();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        //handle_keys();
        //update_camera_target(&game.camera, game.player.actor.coord);
        //update_camera(&game.camera);

        BeginDrawing();

        ClearBackground(BLACK);

        render_tree(tree, 0, 0, 0);

        //render_map(&game.map);
        //render_actor(&game.player.actor);

        EndDrawing();
    }

    CloseWindow();

    printf("Hello, World!\n");
    return 0;
}
