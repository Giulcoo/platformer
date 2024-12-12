%{
#include <stdio.h>
#include <stdlib.h>
#include "../helper/ast.h"
#include "../helper/compiler.h"
#include "../helper/editor.h"
#include "../helper/error.h"

void yyerror(const char *s);
int yylex();
extern FILE *yyin;
extern Location *pos;
extern Location *objpos;

ASTNode *root = NULL; 
Error rootError = { NULL, NULL, NULL };
Error *errors = &rootError;
%}

%union {
    int num;
    char *str;
    struct ASTNode *node;
    enum NodeType *type;
}

%token SCREEN PLAYER WORLD CHECKPOINT BLOCK PLATFORM FINISH SPIKE
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMI
%token <str> STRING IDENT
%token <num> NUMBER

%type <node> program screen_statement player_statement universe_statement world_statement world_content gameobject rect_statement sqr_statement
%type <type> type_statement

%%
program:
    screen_statement player_statement universe_statement {
        $$ = create_program_node($1, $2, $3);
        root = $$;  
    } 
    | player_statement universe_statement {
        $$ = create_program_node(NULL, $1, $2);
        root = $$;  
    } 
    | error
    { 
        add_error(errors, pos, "A program always needs one player and at least one world."); 
        yyclearin; 
    } 
    ;

screen_statement:
    SCREEN LPAREN STRING COMMA NUMBER COMMA NUMBER COMMA NUMBER RPAREN SEMI
    {
        $$ = create_screen_node(*objpos, $3, $5, $7, $9);
    }
    | SCREEN LPAREN STRING COMMA NUMBER RPAREN SEMI
    {
        $$ = create_screen_node(*objpos, $3, -1, -1, $5);
    }
    | SCREEN LPAREN STRING RPAREN SEMI
    {
        $$ = create_screen_node(*objpos, $3, -1, -1, 100);
    }
    | SCREEN error SEMI
    {
        add_error(errors, objpos, "Wrong screen definition. A screen is always defined like: \n   player(string, int, int, int);"); 
        yyclearin;
    }
    ;

player_statement:
    PLAYER LPAREN IDENT COMMA NUMBER COMMA NUMBER RPAREN SEMI
    {
        $$ = create_player_node(*objpos, $3, $5, $7);
    }
    | PLAYER error SEMI 
    {
        printf("Problem\n");
        add_error(errors, objpos, "Wrong player definition. A player is always defined like: \n   player(identifier, int, int);"); 
        yyclearin;
    }
    ;

universe_statement:
    universe_statement world_statement 
    { 
        $$ = $1; 
        ASTNode *n = $1; 

        if(n){
            while (n->next) n = n->next; 
            n->next = $2; 
        }
    }
    | world_statement { $$ = $1; }
    ;

world_statement:
    WORLD LPAREN IDENT COMMA NUMBER COMMA NUMBER RPAREN LBRACE world_content RBRACE
    {
        $$ = create_world_node(*objpos, $3, $5, $7, $10);
    }
    | WORLD LPAREN IDENT COMMA NUMBER COMMA NUMBER RPAREN LBRACE error RBRACE
    {
        add_error(errors, objpos, "A world can never be empty!");
        yyclearin;
    }
    | WORLD error LBRACE
    { 
        add_error(errors, objpos, "Wrong world definition. A world is always defined like: \n   world(identifier, int, int){ ... }\n");
        yyclearin;
    }
    ;

world_content:
    world_content gameobject 
    { 
        $$ = $1; 
        ASTNode *n = $1; 

        if(n){
            while (n->next) n = n->next; 
            n->next = $2; 
        }
    }
    | gameobject { $$ = $1; }
    ;

gameobject:
    type_statement LPAREN rect_statement RPAREN SEMI
    {
        $$ = create_gameobject_node(*objpos, *$1, $3, "");
    }
    | type_statement LPAREN rect_statement COMMA IDENT RPAREN SEMI
    {
        $$ = create_gameobject_node(*objpos, *$1, $3, $5);
    }
    | SPIKE LPAREN sqr_statement RPAREN SEMI
    {
        $$ = create_gameobject_node(*objpos, NODE_SPIKE, $3, "");
    }
    | type_statement LPAREN error RPAREN SEMI
    { 
        $$ = NULL;
        add_error(errors, objpos, "Rectangle of gameobject is not correct."); 
        yyclearin;
    }
    | error LPAREN rect_statement RPAREN SEMI
    { 
        $$ = NULL;
        add_error(errors, objpos, "Unknown gameobject type."); 
        yyclearin;
    }
    | error LPAREN rect_statement COMMA IDENT RPAREN SEMI
    { 
        $$ = NULL;
        add_error(errors, objpos, "Unknown gameobject type."); 
        yyclearin;
    }
    | SPIKE error SEMI
    { 
        $$ = NULL;
        add_error(errors, objpos, "Wrong spike definition. A spike object is always defined like: \n   spike(int, int, int);"); 
        yyclearin;
    }
    ;
    | error SEMI 
    { 
        $$ = NULL;
        add_error(errors, objpos, "Wrong gameobject definition. A gameobject is always defined like: \n   type(int, int, int, int);"); 
        yyclearin;
    }
    ;

type_statement:
    CHECKPOINT { NodeType type = NODE_CHECKPOINT; $$ = &type; } 
    | BLOCK { NodeType type = NODE_BLOCK; $$ = &type; } 
    | PLATFORM { NodeType type = NODE_PLATFORM; $$ = &type; } 
    | FINISH { NodeType type = NODE_FINISH; $$ = &type; } 
    ;

rect_statement:
    NUMBER COMMA NUMBER COMMA NUMBER COMMA NUMBER
    {
        $$ = create_rect_node(*objpos, $1, $3, $5, $7);
    }

sqr_statement:
    NUMBER COMMA NUMBER COMMA NUMBER
    {
        $$ = create_rect_node(*objpos, $1, $3, $5, $5);
    }

%%
void yyerror(const char *message) {
    printf(message);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <verbose>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening file");
        return 1;
    }

    char* verbose = argv[2];

    yyin = input;  // Set the input stream for Flex

    if (yyparse() == 0) 
    {
        resolve(root, errors);

        if(errors->next){
            print_errors(errors->next);
        }
        else{
            if(strcmp(verbose, "1") == 0) print_ast(root, 0); 
            write_file("../game/main.c", generate_code(root));
            printf("\n");
            free_ast(root); 
        }
    } 
    else 
    {
        printf("Parsing failed.\n");
    }

    fclose(input); 
    return 0;
}