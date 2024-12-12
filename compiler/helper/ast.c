#include "ast.h"

ASTNode *create_program_node(ASTNode *screen, ASTNode *player, ASTNode *worlds) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_PROGRAM;
    node->data.program.screen = screen;
    node->data.program.player = player;
    node->data.program.worlds = worlds;
    node->next = NULL;
    return node;
}

ASTNode *create_screen_node(Location pos, char* title, int width, int height, float zoom){
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_SCREEN;
    node->data.screen.title = title;
    node->data.screen.width = width;
    node->data.screen.height = height;
    node->data.screen.zoom = zoom/100;
    node->pos = copy_pos(pos);
    node->next = NULL;
    return node;
}

ASTNode *create_player_node(Location pos, char *startName, int width, int height) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_PLAYER;
    node->data.player.startWorld = create_world_node((Location) {-1, -1}, startName, 0, 0, NULL);
    node->data.player.width = width;
    node->data.player.height = height;
    node->pos = copy_pos(pos);
    node->next = NULL;
    return node;
}

ASTNode *create_world_node(Location pos, char *name, int startX, int startY, ASTNode *contents) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_WORLD;
    node->data.world.name = strdup(name);
    node->data.world.startX = startX;
    node->data.world.startY = -startY;
    node->data.world.contents = contents;
    node->pos = copy_pos(pos);
    node->next = NULL;
    return node;
}

ASTNode *create_gameobject_node(Location pos, NodeType type, ASTNode *rect, char *flag) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->data.gameobject.rect = rect;
    node->data.gameobject.flag = flag;
    node->pos = copy_pos(pos);
    node->next = NULL;
    node->data.gameobject.worldLink = NULL;
    return node;
}

ASTNode *create_rect_node(Location pos, int x, int y, int width, int height) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_RECT;
    node->data.rect.x = x;
    node->data.rect.y = -y;
    node->data.rect.width = width;
    node->data.rect.height = height;
    node->pos = copy_pos(pos);
    node->next = NULL;
    return node;
}

void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("-  ");  
    }
}

void print_ast(ASTNode *node, int indent) {
    while (node) {
        print_indent(indent);
        printf(NodeTypeString[node->type]);

        switch (node->type) {
            case NODE_PROGRAM:
                printf(":\n");
                print_ast(node->data.program.screen, indent + 1);
                print_ast(node->data.program.player, indent + 1);
                print_indent(indent + 1);
                printf("Worlds:\n");
                print_ast(node->data.program.worlds, indent + 2);
                break;
            case NODE_SCREEN:
                printf("(title=%s, width=%d, height=%d, zoom=%.3ff)\n",
                    node->data.screen.title, node->data.screen.width, node->data.screen.height, node->data.screen.zoom);
                break;
            case NODE_PLAYER:
                printf("(name=%s, width=%d, height=%d)\n",
                    node->data.player.startWorld->data.world.name, 
                    node->data.player.width, node->data.player.height);
                break;

            case NODE_WORLD:
                printf("(name=%s, startx=%d, starty=%d):\n", 
                    node->data.world.name, 
                    node->data.world.startX, node->data.world.startY);
                print_ast(node->data.world.contents, indent + 1);
                break;
            case NODE_CHECKPOINT:
            case NODE_BLOCK:
            case NODE_PLATFORM:
            case NODE_SPIKE:
                print_ast(node->data.gameobject.rect, 0);
                printf("\n");
                break;
            case NODE_FINISH:
                print_ast(node->data.gameobject.rect, 0);
                printf(" with next world: %s\n", node->data.gameobject.worldLink->data.world.name);
                break;
            case NODE_RECT:
                printf("(x=%d, y=%d, width=%d, height=%d)", 
                    node->data.rect.x, node->data.rect.y, 
                    node->data.rect.width, node->data.rect.height);
                break;
            default:
                printf("Unknown node type\n");
        }

        node = node->next;
    }
}

void free_ast(ASTNode *node) {
    while (node) {
        ASTNode *next = node->next;
        if (node->type == NODE_PROGRAM){
            free_ast(node->data.program.player);
            free_ast(node->data.program.worlds);
        }
        else if (node->type == NODE_WORLD){ 
            free(node->data.world.name); 
            free_ast(node->data.world.contents);
        }
        free(node);
        node = next;
    }
}

ASTNode *find_world(ASTNode *world, char* name){
    if(world->type != NODE_WORLD) return NULL;

    while(world){
        if(strcmp(world->data.world.name, name) == 0) return world;
        world = world->next;
    }

    return NULL;
}