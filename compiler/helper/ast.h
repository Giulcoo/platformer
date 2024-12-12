#ifndef AST_H
#define AST_H

#include "editor.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum NodeType { 
    NODE_PROGRAM, 
    NODE_SCREEN,
    NODE_PLAYER, 
    NODE_WORLD, 
    NODE_CHECKPOINT, 
    NODE_BLOCK, 
    NODE_PLATFORM, 
    NODE_FINISH, 
    NODE_SPIKE, 
    NODE_RECT 
} NodeType;

static const char *NodeTypeString[] = {
    "PROGRAM", "SCREEN", "PLAYER", "WORLD", "CHECKPOINT", "BLOCK", "PLATFORM", "FINISH", "SPIKE", ""
};

static const char *GameObjectColor[] = {
    "GOLD", "BROWN", "GRAY", "BLUE", "RED"
};

typedef struct ASTNode {
    NodeType type;
    struct ASTNode *next; 
    struct Location *pos;
    char* str;

    union {
        struct { struct ASTNode *screen, *player, *worlds; } program;
        struct { char *title; int width, height; float zoom; } screen;
        struct { struct ASTNode* startWorld; int width, height; } player;
        struct { struct ASTNode *contents; char *name; int startX, startY; } world;
        struct { struct ASTNode *rect, *worldLink; char *flag; } gameobject;
        struct { int x, y, width, height; } rect;
    } data;
} ASTNode;

ASTNode *create_program_node(ASTNode *screen, ASTNode *player, ASTNode *worlds);
ASTNode *create_screen_node(Location pos, char *title, int width, int height, float zoom);
ASTNode *create_player_node(Location pos, char *startName, int width, int height);
ASTNode *create_world_node(Location pos, char *name, int startX, int startY, ASTNode *contents);
ASTNode *create_gameobject_node(Location pos, NodeType type, ASTNode *rect, char *flag);
ASTNode *create_rect_node(Location pos, int x, int y, int width, int height);
void print_ast(ASTNode *node, int indent);
void free_ast(ASTNode *node);
ASTNode *find_world(ASTNode *world, char* name);

#endif  // AST_H
