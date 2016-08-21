//
// Burst
// Include/Parser/BurstParser.h
//
#ifndef __BURST_PARSER_H__
#define __BURST_PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>

#include "BurstErrorCodes.h"
#include "BurstErrorMessages.h"

#include "Lexer/Token/BurstToken.h"

#define BURST_PARSER_GOOD 0x00
#define BURST_PARSER_ERR  0x01
#define BURST_PARSER_EOF  0x02

struct burstParser;
struct burstParser
{
    int currentTokenIndex;
    
    BurstTokenArray *pTokens;
};
typedef struct burstParser BurstParser;

int parser_create
(
    BurstTokenArray *pTokens, // IN
    BurstParser **ppParser    // OUT
);

int parser_run
(
    BurstParser *pParser // IN
);

BurstToken *parser_getToken
(
    BurstParser *pParser // IN
);

bool parser_seesToken
(
    int tokenType,       // IN
    BurstParser *pParser // IN
);

void parser_advanceToken
(
    BurstParser *pParser // IN
);

int parser_destroy
(
    BurstParser *pParser // IN
);

#endif