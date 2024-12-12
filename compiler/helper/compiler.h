#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include "editor.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void resolve(ASTNode *node, Error *errors);
char* generate_code(ASTNode *node);
ASTNode *find_world(ASTNode *world, char* name);

#endif  // COMPILER_H
