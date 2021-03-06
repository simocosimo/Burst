//
// Burst
// Source/Frontend/Parser/BurstParser.c
//
#include "Frontend/Parser/BurstParser.h"

int parser_create
(
    BurstTokenArray *pTokens,
    BurstParser **ppParser
)
{
    if (NULL != (*ppParser))
        return BURST_FAIL;
    
    if (NULL == ((*ppParser) = (BurstParser *) malloc(sizeof(BurstParser))))
        return BURST_FAIL;
    
    (*ppParser)->currentTokenIndex = 0x0;
    (*ppParser)->pTokens = pTokens;
    (*ppParser)->pAST = NULL;
    
    ast_create(&(*ppParser)->pAST);
    
    return BURST_SUCCESS;
}

int parser_run
(
    BurstParser *pParser
)
{
    if (NULL == pParser)
        return BURST_FAIL;
    
    if (NULL == pParser->pTokens)
        return BURST_FAIL;
    
    while (NULL != parser_getToken(pParser))
    {
        bool bParseSuccess = false;
        int tokenIndexSave = pParser->currentTokenIndex;
        
        BurstASTNode *pCurrentASTNode = NULL;
        
        if (!bParseSuccess && parser_parseVariableDeclaration(pParser,
            &pCurrentASTNode))
            bParseSuccess = true;
        else if (!bParseSuccess)
                pParser->currentTokenIndex = tokenIndexSave;
        
        if (!bParseSuccess && parser_parseFunctionDeclaration(pParser,
            &pCurrentASTNode))
            bParseSuccess = true;
        else if (!bParseSuccess)
                pParser->currentTokenIndex = tokenIndexSave;
        
        // if (!parser_parseVariableDeclaration(pParser, &pCurrentASTNode) ||
        //     !parser_parseFunctionDeclaration(pParser, &pCurrentASTNode))
        //     pParser->currentTokenIndex = tokenIndexSave;
        
        if (bParseSuccess && NULL != pCurrentASTNode)
            assert(BURST_SUCCESS == ast_add(pCurrentASTNode, pParser->pAST));
        else
            break;
    }
    
    return BURST_SUCCESS;
}

BurstToken *parser_getToken
(
    BurstParser *pParser
)
{
    if (NULL == pParser)
        return NULL;
    
    if (NULL == pParser->pTokens)
        return NULL;
    
    return token_array_at(pParser->currentTokenIndex, pParser->pTokens);
}

bool parser_seesToken
(
    int tokenType,
    BurstParser *pParser
)
{
    BurstToken *pToken = NULL;
    
    if (NULL == pParser)
        return false;
    
    if (NULL == pParser->pTokens)
        return false;
    
    if (NULL == (pToken = parser_getToken(pParser)))
        return false;
    
    return tokenType == pToken->type;
}

void parser_advanceToken
(
    BurstParser *pParser
)
{
    if (NULL == pParser)
        return;
    
    pParser->currentTokenIndex++;
}

int parser_destroy
(
    BurstParser *pParser
)
{
    if (NULL == pParser)
        return BURST_FAIL;
    
    if (NULL != pParser->pAST)
        ast_destroy(pParser->pAST);
    
    free(pParser);
    
    return BURST_SUCCESS;
}

bool parser_parseBlock
(
    BurstParser *pParser,    // IN
    BurstASTNode **ppASTNode // OUT
)
{
    if (NULL == pParser)
        return false;
    
    if (NULL != (*ppASTNode))
        return false;
    
    if (!parser_seesToken(BURST_LBRACE_TOKEN, pParser))
        return false;
    
    // Advance past '{'
    parser_advanceToken(pParser);
    
    // TODO...
    
    if (!parser_seesToken(BURST_RBRACE_TOKEN, pParser))
        return false;
    
    // Advance past '}'
    parser_advanceToken(pParser);
    
    return false;
}

bool parser_parseVariableDeclaration
(
    BurstParser *pParser,
    BurstASTNode **ppASTNode
)
{
    BurstVariableDeclarationNode *pVariableDeclaration = NULL;
    
    BurstToken *pVariableTypeToken = NULL;
    BurstToken *pVariableNameToken = NULL;
    
    if (NULL == pParser)
        return false;
    
    if (NULL != (*ppASTNode))
        return false;
    
    if (!parser_seesToken(BURST_TYPE_TOKEN, pParser))
        return false;
    
    pVariableTypeToken = parser_getToken(pParser);
    parser_advanceToken(pParser);
    
    if (!parser_seesToken(BURST_IDENTIFIER_TOKEN, pParser))
        return false;
    
    pVariableNameToken = parser_getToken(pParser);
    parser_advanceToken(pParser);
    
    assert(BURST_SUCCESS == variable_declaration_node_create(
        pVariableTypeToken->pStringValue,
        pVariableNameToken->pStringValue,
        &pVariableDeclaration
    ));
    
    if (parser_seesToken(BURST_EQUALS_TOKEN, pParser))
    {
        parser_advanceToken(pParser);
        
        if (!parser_parseValueExpression(pParser,
            &pVariableDeclaration->pValueExpressionNode))
            PARSER_ERROR("Expected value after '='!");
    }
    
    assert(BURST_SUCCESS == ast_node_create(
        BURST_VARIABLE_DECLARATION_NODE, pVariableDeclaration, ppASTNode
    ));
    
    // Skip line-ending semicolon, if it exists.
    if (parser_seesToken(BURST_SEMICOLON_TOKEN, pParser))
        parser_advanceToken(pParser);
    
    return true;
}

bool parser_parseFunctionDeclaration
(
    BurstParser *pParser,
    BurstASTNode **ppASTNode
)
{
    BurstToken *pFunctionReturnTypeToken = NULL;
    BurstToken *pFunctionIdentifierToken = NULL;
    
    // BurstFunctionDeclarationNode *pFunctionDeclaration = NULL;
    
    if (NULL == pParser)
        return false;
    
    if (NULL != (*ppASTNode))
        return false;
    
    // TODO: BurstFunctionDeclarationNode
    
    if (!parser_seesToken(BURST_KEYWORD_TOKEN, pParser))
        return false;
    
    // pFunctionReturnTypeToken = parser_getToken(pParser);
    parser_advanceToken(pParser);
    
    if (!parser_seesToken(BURST_IDENTIFIER_TOKEN, pParser))
        return false;
    
    pFunctionIdentifierToken = parser_getToken(pParser);
    parser_advanceToken(pParser);
    
    if (!parser_seesToken(BURST_LPAREN_TOKEN, pParser))
        return false;
    
    // Advance past '('
    parser_advanceToken(pParser);
    
    while (!parser_seesToken(BURST_RPAREN_TOKEN, pParser))
    {
        // TODO: Capture function's argument list
        
        parser_advanceToken(pParser);
    }
    
    // Advance past ')'
    parser_advanceToken(pParser);
    
    if (!parser_seesToken(BURST_COLON_TOKEN, pParser))
        return false;
    
    // Advance past ':'
    parser_advanceToken(pParser);
    
    if (!parser_seesToken(BURST_TYPE_TOKEN, pParser))
        return false;
    
    pFunctionReturnTypeToken = parser_getToken(pParser);
    parser_advanceToken(pParser);
    
    // TODO: Parse function block
    
    printf("Function - Return Type: %s, Name: %s\n",
        pFunctionReturnTypeToken->pStringValue,
        pFunctionIdentifierToken->pStringValue);
    
    return false;
}

bool parser_parseValueExpression
(
    BurstParser *pParser,
    BurstASTNode **ppASTNode
)
{
    BurstASTNode *pValueNode = NULL;
    BurstValueExpressionNode *pValueExpressionNode = NULL;
    
    if (NULL == pParser)
        return false;
    
    if (NULL != (*ppASTNode))
        return false;
    
    if (parser_parseLiteralExpression(pParser, &pValueNode))
    {
        assert(BURST_SUCCESS == value_expression_node_create(
            BURST_LITERAL_EXPRESSION_NODE, pValueNode, &pValueExpressionNode
        ));
        assert(BURST_SUCCESS == ast_node_create(
            BURST_VALUE_EXPRESSION_NODE, pValueExpressionNode, ppASTNode
        ));
        
        return true;
    }
    else if (parser_parseArithmeticExpression(pParser, &pValueNode))
    {
        assert(BURST_SUCCESS == value_expression_node_create(
            BURST_ARITHMETIC_EXPRESSION_NODE, pValueNode, &pValueExpressionNode
        ));
        assert(BURST_SUCCESS == ast_node_create(
            BURST_VALUE_EXPRESSION_NODE, pValueExpressionNode, ppASTNode
        ));
        
        return true;
    }
    else if (parser_parseReferenceExpression(pParser, &pValueNode))
    {
        assert(BURST_SUCCESS == value_expression_node_create(
            BURST_REFERENCE_EXPRESSION_NODE, pValueNode, &pValueExpressionNode
        ));
        assert(BURST_SUCCESS == ast_node_create(
            BURST_VALUE_EXPRESSION_NODE, pValueExpressionNode, ppASTNode
        ));
        
        return true;
    }
    
    return false;
}

bool parser_parseLiteralExpression
(
    BurstParser *pParser,
    BurstASTNode **ppASTNode
)
{
    BurstToken *pLiteralExpressionToken = NULL;
    BurstLiteralExpressionNode *pLiteralExpressionNode = NULL;
    
    if (NULL == pParser)
        return false;
    
    if (NULL != (*ppASTNode))
        return false;
    
    // TODO: Support more than just number literals
    if (parser_seesToken(BURST_NUMBER_TOKEN, pParser))
    {
        pLiteralExpressionToken = parser_getToken(pParser);
        parser_advanceToken(pParser);
        
        assert(BURST_SUCCESS == literal_expression_node_create(
            pLiteralExpressionToken->pStringValue, &pLiteralExpressionNode
        ));
        assert(BURST_SUCCESS == ast_node_create(
            BURST_LITERAL_EXPRESSION_NODE, pLiteralExpressionNode, ppASTNode
        ));
        
        return true;
    }
    
    // Skip line-ending semicolon, if it exists.
    if (parser_seesToken(BURST_SEMICOLON_TOKEN, pParser))
        parser_advanceToken(pParser);
    
    return false;
}

bool parser_parseArithmeticExpression
(
    BurstParser *pParser,
    BurstASTNode **ppASTNode
)
{
    BurstArithmeticExpressionNode *pArithmeticExpression = NULL;
    
    BurstASTNode *pLeftSideNode  = NULL;
    BurstASTNode *pRightSideNode = NULL;
    
    char *pOperator = NULL;
    
    if (NULL == pParser)
        return false;
    
    if (NULL != (*ppASTNode))
        return false;
    
    if (!parser_seesToken(BURST_LBRACKET_TOKEN, pParser))
        return false;
    
    // Advance past '['
    parser_advanceToken(pParser);
    
    if (!parser_parseValueExpression(pParser, &pLeftSideNode))
        return false;
    
    if (!parser_seesToken(BURST_ADD_TOKEN, pParser) &&
        !parser_seesToken(BURST_SUBTRACT_TOKEN, pParser) &&
        !parser_seesToken(BURST_MULTIPLY_TOKEN, pParser) &&
        !parser_seesToken(BURST_DIVIDE_TOKEN, pParser))
        return false;
    
    pOperator = (parser_getToken(pParser)->pStringValue);
    parser_advanceToken(pParser);
    
    if (!parser_parseValueExpression(pParser, &pRightSideNode))
        return false;
    
    if (!parser_seesToken(BURST_RBRACKET_TOKEN, pParser))
        PARSER_ERROR("Expected closing bracket after arithmetic expression!");
    
    // Advance past ']'
    parser_advanceToken(pParser);
    
    assert(BURST_SUCCESS == arithmetic_expression_node_create(
        pLeftSideNode,
        pRightSideNode,
        pOperator,
        &pArithmeticExpression
    ));
    assert(BURST_SUCCESS == ast_node_create(
        BURST_ARITHMETIC_EXPRESSION_NODE, pArithmeticExpression, ppASTNode
    ));
    
    // Skip line-ending semicolon, if it exists.
    if (parser_seesToken(BURST_SEMICOLON_TOKEN, pParser))
        parser_advanceToken(pParser);
    
    return true;
}

bool parser_parseReferenceExpression
(
    BurstParser *pParser,
    BurstASTNode **ppASTNode
)
{
    BurstToken *pIdentifierToken = NULL;
    BurstReferenceExpressionNode *pReferenceExpression = NULL;
    
    if (NULL == pParser)
        return false;
    
    if (NULL != (*ppASTNode))
        return false;
    
    if (!parser_seesToken(BURST_IDENTIFIER_TOKEN, pParser))
        return false;
    
    pIdentifierToken = parser_getToken(pParser);
    parser_advanceToken(pParser);
    
    assert(BURST_SUCCESS == reference_expression_node_create(
        pIdentifierToken->pStringValue, &pReferenceExpression
    ));
    assert(BURST_SUCCESS == ast_node_create(
        BURST_REFERENCE_EXPRESSION_NODE, pReferenceExpression, ppASTNode
    ));
    
    // Skip line-ending semicolon, if it exists.
    if (parser_seesToken(BURST_SEMICOLON_TOKEN, pParser))
        parser_advanceToken(pParser);
    
    return true;
}