%{
#include "platform.tab.h"
#include "../helper/error.h"

Location startPos = {1,1};
Location *pos = &startPos;
Location *objpos = NULL;
%}

DIGIT [0-9]
LETTER [a-zA-Z]

%%
"//"[^\n]*         { /* Ignore single-line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/" {
    for (int i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') pos->line++;
    }
}

"screen"                        { free(objpos); objpos = copy_pos(*pos); pos->column += yyleng; return SCREEN; }
"player"                        { free(objpos); objpos = copy_pos(*pos); pos->column += yyleng; return PLAYER; }
"world"                         { free(objpos); objpos = copy_pos(*pos); pos->column += yyleng; return WORLD; }
"checkpoint"                    { free(objpos); objpos = copy_pos(*pos); pos->column += yyleng; return CHECKPOINT; }
"block"                         { free(objpos); objpos = copy_pos(*pos); pos->column += yyleng; return BLOCK; }
"platform"                      { free(objpos); objpos = copy_pos(*pos); pos->column += yyleng; return PLATFORM; }
"finish"                        { free(objpos); objpos = copy_pos(*pos); pos->column += yyleng; return FINISH; }
"spike"                         { free(objpos); objpos = copy_pos(*pos); pos->column += yyleng; return SPIKE; }
"("                             { pos->column += yyleng; return LPAREN; }
")"                             { pos->column += yyleng; return RPAREN; }
"{"                             { pos->column += yyleng; return LBRACE; }
"}"                             { pos->column += yyleng; return RBRACE; }
","                             { pos->column += yyleng; return COMMA; }
";"                             { pos->column += yyleng; return SEMI; }
{LETTER}({LETTER}|{DIGIT}|"_")* { pos->column += yyleng; yylval.str = strdup(yytext); return IDENT; }
\"[^\"]+\"                      { pos->column += yyleng; yylval.str = strdup(yytext); return STRING; }
{DIGIT}+                        { pos->column += yyleng; yylval.num = atoi(yytext); return NUMBER; }
[ \t]+                          { pos->column += yyleng; }
\n                              { pos->line++; pos->column = 0; }
.                               { printf("Unexpected character %d:%d: '%s'\n", pos->line, pos->column, yytext); }

%%
int yywrap() { return 1; }
