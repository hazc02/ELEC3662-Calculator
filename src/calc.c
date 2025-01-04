/**
 * @file calc.c
 * @brief This file contains all the logic for processing a series of characters 
 *        into an expression array and then evaluating that expression 
 *        using BODMAS rules.
 *
 * We collect keystrokes (digits/operators), store them in an internal buffer,
 * then convert (tokenise) them into integers and operators. Finally,
 * we apply a two-pass approach to respect BODMAS:
 *  1) * and / (left to right)
 *  2) + and - (left to right)
 */

#include "calc.h"
#include <string.h>   // for memset, strcpy
#include <stdlib.h>   // for atoi, strtol
#include <stdio.h>    // for sprintf

/** 
 * ---------------------- INTERNAL EXPLANATION & DATA ----------------------
 * We maintain an internal buffer (expressionBuffer) to store keystrokes 
 * such as digits (0-9) and operators (+, -, *, /).
 * 
 * EXAMPLE: 
 * If the user presses: 1, 1, +, 4, 5,
 * expressionBuffer will end up as {'1','1','+','4','5','\0'}.
 * 
 * When the user calls Calc_Evaluate(), we tokenise these characters 
 * so consecutive digits become a single integer: [11, '+', 45].
 * Then we perform a two-stage evaluation: 
 *   - first pass for '*' and '/', 
 *   - second pass for '+' and '-'.
 * 
 * The final result is returned to the caller.
 * -----------------------------------------------------------------------
 */

/** Maximum length of the input expression we can store. Adjust as needed. */
#define MAX_EXPR_LEN 64

/** Internal buffer to store raw characters (digits/operators) */
static char expressionBuffer[MAX_EXPR_LEN];

/** Keeps track of how many characters are currently in expressionBuffer */
static int exprIndex = 0;

/** Flag that indicates whether an error (e.g. divide by zero) occurred */
static int errorFlag = 0;

/* --------------------------------------------------------------------------
 *                      HELPER FUNCTION PROTOTYPES
 * -------------------------------------------------------------------------- */

/**
 * @brief Converts the expressionBuffer into a list of tokens (integers or operators).
 *        e.g. '1','1','+','4','5' becomes numeric tokens [11,45] and operator ['+'].
 * 
 * @param tokenValues  Array of integer tokens (for numbers).
 * @param tokenOps     Array of operators stored as chars (for +, -, *, /).
 * @param tokenCount   Pointer to an integer that will store the count of numeric tokens.
 * @return 0 if successful, -1 if an error occurs (e.g. unknown character).
 */
static int tokeniseExpression(long *tokenValues, char *tokenOps, int *tokenCount);

/**
 * @brief Applies a two-pass BODMAS evaluation to the tokens:
 *        1) * and / (left to right)
 *        2) + and - (left to right)
 * 
 * @param tokenValues  Array of integer tokens (numbers).
 * @param tokenOps     Array of operators (chars like '+','-','*','/').
 * @param tokenCount   Pointer to the number of numeric tokens.
 * @return 0 if successful, -1 if an error occurs (e.g. division by zero).
 */
static int evaluateTokens(long *tokenValues, char *tokenOps, int *tokenCount);

/* --------------------------------------------------------------------------
 *                            PUBLIC FUNCTIONS
 * -------------------------------------------------------------------------- */

void Calc_Init(void)
{
    // Clear the expression buffer and reset the error flag
    memset(expressionBuffer, 0, sizeof(expressionBuffer));
    exprIndex = 0;
    errorFlag = 0;
}

int Calc_AddChar(char c)
{
    // If we've reached the maximum length, return an error
    if (exprIndex >= (MAX_EXPR_LEN - 1)) {
        return -1; // buffer is full
    }

    // Add the character to the expression buffer and keep it null-terminated
    expressionBuffer[exprIndex++] = c;
    expressionBuffer[exprIndex] = '\0';

    return 0; // success
}

void Calc_ClearExpression(void)
{
    memset(expressionBuffer, 0, sizeof(expressionBuffer));
    exprIndex = 0;
    errorFlag = 0;
}

long Calc_Evaluate(void)
{
    // We'll store up to as many tokens as needed. 
    // Typically, tokens are fewer than raw characters because digits get combined.
    long tokenValues[MAX_EXPR_LEN] = {0};  // numeric tokens
    char tokenOps[MAX_EXPR_LEN]    = {0};  // operators
    int tokenCount = 0;                   // number of numeric tokens found

    errorFlag = 0; // clear error before evaluation

    // 1) Convert expressionBuffer -> arrays of tokens
    if (tokeniseExpression(tokenValues, tokenOps, &tokenCount) < 0) {
        // If any parse/tokenisation error occurs, set the error flag
        errorFlag = 1;
        return 0; 
    }

    // 2) Evaluate the tokens using the BODMAS approach
    if (evaluateTokens(tokenValues, tokenOps, &tokenCount) < 0) {
        // e.g. division by zero encountered
        errorFlag = 1;
        return 0;
    }

    // The result should be in tokenValues[0] if everything worked correctly
    return tokenValues[0];
}

int Calc_HadError(void)
{
    return errorFlag;
}

/* --------------------------------------------------------------------------
 *                       HELPER FUNCTION IMPLEMENTATIONS
 * -------------------------------------------------------------------------- */

/**
 * @brief Scans through expressionBuffer, building consecutive digits into integers.
 * 
 * EXAMPLE:
 *   expressionBuffer = {'1','1','+','4','5','\0'}
 *   => tokenValues = [11, 45], tokenOps = ['+'], tokenCount = 2 (numbers)
 *
 * @return 0 if success, -1 if an unknown character is found or any parse error
 */
static int tokeniseExpression(long *tokenValues, char *tokenOps, int *tokenCount)
{
    int length = exprIndex; // number of actual characters in the expression buffer
    int count = 0;          // how many numeric tokens found
    int opCount = 0;        // how many operators found
    int i = 0;

    long currentNumber = 0;
    int buildingNumber = 0; // indicates if we're currently collecting digits for a number

    for (i = 0; i < length; i++) {
        char c = expressionBuffer[i];

        // Check if it's a digit
        if (c >= '0' && c <= '9') {
            buildingNumber = 1;
            currentNumber = (currentNumber * 10) + (c - '0');
        } else {
            // It's not a digit, so possibly an operator or special character
            if (buildingNumber) {
                // We just finished building a number
                tokenValues[count++] = currentNumber;
                currentNumber = 0;
                buildingNumber = 0;
            }

            // If it's a standard operator, store it
            if (c == '+' || c == '-' || c == '*' || c == '/') {
                tokenOps[opCount++] = c;
            } 
            // '=' or 'C' are control characters, so we ignore them in tokenOps
            else if (c == '=' || c == 'C') {
                // do nothing here
            } 
            else {
                // unknown character -> error
                return -1;
            }
        }
    }

    // If the expression ended with digits, we need to add that final number
    if (buildingNumber) {
        tokenValues[count++] = currentNumber;
    }

    // Store the count of numeric tokens
    *tokenCount = count;

    return 0; // success
}

/**
 * @brief Two-pass BODMAS evaluation:
 *        - First pass for '*' and '/' from left to right.
 *        - Second pass for '+' and '-' from left to right.
 * 
 * We modify the arrays in-place, reducing the number of tokens each time an operation is performed.
 */
static int evaluateTokens(long *tokenValues, char *tokenOps, int *tokenCount)
{
    int count = *tokenCount; // how many numeric tokens we have
    int i;

    // First pass: handle '*' and '/'
    i = 0;
    while (i < (count - 1)) {
        char op = tokenOps[i];
        if (op == '*' || op == '/') {
            long lhs = tokenValues[i];
            long rhs = tokenValues[i + 1];
            long result = 0;

            if (op == '*') {
                result = lhs * rhs;
            } else {
                // check for division by zero
                if (rhs == 0) {
                    return -1; // error
                }
                result = lhs / rhs; // integer division
            }

            // Replace lhs with the result
            tokenValues[i] = result;

            // Shift everything left by one
            for (int j = i + 1; j < (count - 1); j++) {
                tokenValues[j] = tokenValues[j + 1];
                tokenOps[j - 1] = tokenOps[j];
            }
            count--;
            // Note: do not increment 'i' here as we need to evaluate the new arrangement
        } else {
            i++;
        }
    }

    // Second pass: handle '+' and '-'
    i = 0;
    while (i < (count - 1)) {
        char op = tokenOps[i];
        if (op == '+' || op == '-') {
            long lhs = tokenValues[i];
            long rhs = tokenValues[i + 1];
            long result = 0;

            if (op == '+') {
                result = lhs + rhs;
            } else {
                result = lhs - rhs;
            }

            // Store result in tokenValues[i]
            tokenValues[i] = result;

            // Shift arrays to remove the consumed token
            for (int j = i + 1; j < (count - 1); j++) {
                tokenValues[j] = tokenValues[j + 1];
                tokenOps[j - 1] = tokenOps[j];
            }
            count--;
        } else {
            i++;
        }
    }

    // The final result should now be in tokenValues[0]
    *tokenCount = count;
    return 0; // success
}
