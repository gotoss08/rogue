#ifndef BSP_IMPL
#define BSP_IMPL

#include <string.h>

#include "game.h"

#define MIN_LEAF_SIZE 25
#define MIN_LEAF_SIZE_PERCENT 0.4
#define MAX_LEAF_SIZE_PERCENT 0.7

typedef struct BSP_Leaf {
    int width;
    int height;

    struct BSP_Leaf* left;
    struct BSP_Leaf* right;
} BSP_Leaf;

BSP_Leaf* create_leaf(int width, int height) {

    BSP_Leaf* leaf = malloc(sizeof(BSP_Leaf));

    leaf->width = width;
    leaf->height = height;

    int direction = random_int(1, 2); // 1 = horizontal, 2 = vertical
    int position = -1;

    if (direction == 1) { // horizontal
        position = random_int(MIN_LEAF_SIZE_PERCENT * width, MAX_LEAF_SIZE_PERCENT * width);
    } else {
        position = random_int(MIN_LEAF_SIZE_PERCENT * height, MAX_LEAF_SIZE_PERCENT * height);
    }

    if (position < MIN_LEAF_SIZE) return NULL;

    if (direction == 1) {
        leaf->left = create_leaf(position, height);
        leaf->right = create_leaf(width - position, height);
    } else {
        leaf->left = create_leaf(width, position);
        leaf->right = create_leaf(width, height - position);
    }

    return leaf;

}

void print_tree(BSP_Leaf* tree, int level) {

    if (tree == NULL) return;

    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    printf("[%d] %dx%d\n", level, tree->width, tree->height);

    print_tree(tree->left, level + 1);
    print_tree(tree->right, level + 1);

}

Color colors[] = {
    RED,
    GREEN,
    BLUE,
    GOLD,
    PINK,
    PURPLE,
    VIOLET,
    BROWN,
    MAGENTA,
    MAROON,
    LIME,
};

void render_tree(BSP_Leaf* tree, int cursor_x, int cursor_y, int level) {

    if (tree == NULL) return;

    DrawRectangle(cursor_x, cursor_y, tree->width, tree->height, colors[level]);

    if (tree->left != NULL) {

        render_tree(tree->left, cursor_x, cursor_y, level + 1);

    }

    if (tree->right != NULL) {

        render_tree(tree->right, cursor_x + (tree->width - tree->right->width), cursor_y + (tree->height - tree->right->height), level + 1);

    }

}

BSP_Leaf* build_bsp_dungeon(Tile* tiles[MAP_SIZE][MAP_SIZE], int width, int height) {

    BSP_Leaf* dungeon = create_leaf(width, height);
    return dungeon;

}

#endif // BSP_IMPL
