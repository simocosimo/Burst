//
// Burst
// Include/Frontend/Parser/AST/Nodes/BurstLiteralExpressionNode.h
//
#ifndef __BURST_AST_LITERAL_EXPRESSION_NODE_H__
#define __BURST_AST_LITERAL_EXPRESSION_NODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "BurstCommons.h"
#include "BurstErrorCodes.h"
#include "BurstErrorMessages.h"

#include "BurstASTNode.h"

struct burstLiteralExpressionNode;
typedef struct burstLiteralExpressionNode
{
    // TODO (Giga): Figure out how to accurately represent different datatypes
    //  within literal expressions.
    char *pValueString;
} BurstLiteralExpressionNode;

int literal_expression_node_create
(
    char *pExpressionValue,             // IN
    BurstLiteralExpressionNode **ppNode // OUT
);

int literal_expression_node_destroy
(
    BurstLiteralExpressionNode *pNode // IN
);

#endif