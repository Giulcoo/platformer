#include "compiler.h"

void resolve(ASTNode *node, Error *errors){
    while (node) {
        switch (node->type) {
            case NODE_PROGRAM:
                // Create default screen, if null
                if(!node->data.program.screen){
                    node->data.program.screen = create_screen_node(*node->pos, "Platformer Game", -1, -1, 100);
                }

                ASTNode *worlds = node->data.program.worlds;
                resolve(node->data.program.screen, errors);
                resolve(node->data.program.player, errors);
                resolve(worlds, errors);

                // Check if player start world exists and if it does create link
                ASTNode *player = node->data.program.player;
                ASTNode* world = find_world(worlds, player->data.player.startWorld->data.world.name);

                if(world){
                    free_ast(player->data.player.startWorld);
                    player->data.player.startWorld = world;
                }
                else{
                    add_error(errors, player->pos, "Start world for player does not exist.");
                }

                // Check if finish worlds exist
                world = worlds;
                while(world){
                    ASTNode* gObject = world->data.world.contents;
                    while (gObject)
                    {
                        if(gObject->type == NODE_FINISH){
                            ASTNode* finishWorld = find_world(worlds, gObject->data.gameobject.flag);
                            if(finishWorld){
                                gObject->data.gameobject.worldLink = finishWorld;
                            }
                            else{
                                add_error(errors, gObject->pos, concat(concat("The world ", gObject->data.gameobject.flag), " does not exist"));
                            }
                        } 

                        gObject = gObject->next;
                    }
                    world = world->next;
                }
                break;
            case NODE_SCREEN:
                // Set screen string for code generation
                int screenStringLength = snprintf(NULL, 0, "%s, %d, %d, %.3ff", 
                    node->data.screen.title, node->data.screen.width, node->data.screen.height, node->data.screen.zoom);

                node->str = (char*)malloc(screenStringLength + 1); 

                if (node->str != NULL) {
                    snprintf(node->str, screenStringLength + 1, "%s, %d, %d, %.3ff", 
                        node->data.screen.title, node->data.screen.width, node->data.screen.height, node->data.screen.zoom);
                }
                break;
            case NODE_PLAYER:
                // Set player string for code generation
                int playerStringLength = snprintf(NULL, 0, "&%s, {%d, %d}", 
                    node->data.player.startWorld->data.world.name, node->data.player.width, node->data.player.height);

                node->str = (char*)malloc(playerStringLength + 1); 

                if (node->str != NULL) {
                    snprintf(node->str, playerStringLength + 1, "&%s, {%d, %d}", 
                        node->data.player.startWorld->data.world.name, node->data.player.width, node->data.player.height);
                }
                break;
            case NODE_WORLD: 
                // Set world string to "struct World world = { ... };" for code generation
                int amountOfGameobjects = 0;
                ASTNode *gObject = node->data.world.contents;
                while (gObject)
                {
                    amountOfGameobjects++;
                    gObject = gObject->next;
                }

                char *worldStringName = replace_char(node->data.world.name, '_', ' ');

                int worldStringLength = snprintf(NULL, 0, "\"%s\", {%d,%d}, NULL, %d", 
                    worldStringName, node->data.world.startX, node->data.world.startY, amountOfGameobjects);
            
                char *worldContent = (char*)malloc(worldStringLength + 1); 

                if (worldContent != NULL) {
                    snprintf(worldContent, worldStringLength + 1, "\"%s\", {%d,%d}, NULL, %d", 
                        worldStringName, node->data.world.startX, node->data.world.startY, amountOfGameobjects);
                }

                node->str = read_file("code/world.txt");
                node->str = replace_placeholder(node->str, "#name#", node->data.world.name);
                node->str = replace_placeholder(node->str, "#content#", worldContent);

                resolve(node->data.world.contents, errors);
                break;
            case NODE_BLOCK:
            case NODE_PLATFORM:
            case NODE_CHECKPOINT:
            case NODE_FINISH:
            case NODE_SPIKE:
                // Set gameobject string for code generation
                node->str = read_file("code/gameobject.txt");

                char *objectFlag;
                if(node->type == NODE_FINISH){
                    objectFlag = concat("&", node->data.gameobject.flag);
                }
                else{
                    objectFlag = "NULL";
                }

                resolve(node->data.gameobject.rect, errors);
                node->str = replace_placeholder(node->str, "#type#", NodeTypeString[node->type]);
                node->str = replace_placeholder(node->str, "#rect#", node->data.gameobject.rect->str);
                node->str = replace_placeholder(node->str, "#color#", GameObjectColor[node->type - NODE_CHECKPOINT]);
                node->str = replace_placeholder(node->str, "#flag#", objectFlag);
                break;
            case NODE_RECT:
                int rectLength = snprintf(NULL, 0, "{%d,%d,%d,%d}", 
                    node->data.rect.x, node->data.rect.y, node->data.rect.width, node->data.rect.height);
            
                node->str = (char*)malloc(rectLength + 1); 

                if (node->str != NULL) {
                    snprintf(node->str, rectLength + 1, "{%d,%d,%d,%d}", 
                        node->data.rect.x, node->data.rect.y, node->data.rect.width, node->data.rect.height);
                }
                break;
            default:
                break;
        }

        node = node->next;
    }
}

char* generate_code(ASTNode *node){
    char* result = "";

    while (node) {
        switch (node->type) {
            case NODE_PROGRAM:
                // Connect game elements to one file
                result = read_file("code/main.txt");

                char* worldDefinitionString = "";
                ASTNode* world = node->data.program.worlds;
                while (world)
                {
                    worldDefinitionString = concat(worldDefinitionString, world->str);
                    world = world->next;
                }

                result = replace_placeholder(result, "#worlds#", worldDefinitionString);
                result = replace_placeholder(result, "#gameobjects#", generate_code(node->data.program.worlds));
                result = replace_placeholder(result, "#screen#", node->data.program.screen->str); 
                result = replace_placeholder(result, "#player#", node->data.program.player->str);
                break;
            case NODE_WORLD:
                // Create string: world.objects = (GameObject[]){ ... }
                char* worldResult = read_file("code/gameobjects.txt");

                worldResult = replace_placeholder(worldResult, "#world#", node->data.world.name);
                worldResult = replace_placeholder(worldResult, "#gameobjects#", generate_code(node->data.world.contents));

                if(node->next) worldResult = concat(worldResult, "\n");
                result = concat(result, worldResult);
                break;
            case NODE_BLOCK:
            case NODE_PLATFORM:
            case NODE_CHECKPOINT:
            case NODE_FINISH:
            case NODE_SPIKE:
                // Concat all sibling gameobjects strings to one string
                if(node->next) node->str = concat(node->str , "\n");
                result = concat(result, node->str);
                break;
            default:
                printf("Unknown node type\n");
                return "";
        }

        node = node->next;
    }

    return result;
}