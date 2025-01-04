/**
 * @file calc.c
 * @brief Implements the core Calculator Logic Module, 
 *        which tokenises the expression buffer and evaluates using BODMAS.
 *
 * This version includes a helper function Calc_GetExpression()
 * to let the main UI code display the entire buffer.
 */

#include "calc.h"
#include <string.h>   // for memset, strcpy
#include <stdlib.h>   // for atoi, strtol
#include <stdio.h>    // for sprintf

/** 
 * We maintain an internal buffer (expressionBuffer) to store keystrokes:
 * digits ('0'..'9') and operators ('+','-','*','/').
 * Example: If the user presses 1,1,+,4,5, the buffer ends with "11+45".
 * 
 * Tokenisation then groups consecutive digits into integer tokens 
 * (e.g. "11", "45") separated by operators.
 */

/** Maximum length of the input expression we can store. */
#define MAX_EXPR_LEN 64

/** Internal buffer to store the raw characters (digits/operators). */
static char expressionBuffer[MAX_EXPR_LEN];

/** Keeps track of how many characters are currently in expressionBuffer. */
static int exprIndex = 0;

/** Flag indicating whether an error (e.g. divide by zero) occurred. */
static int errorFlag = 0;

/* --------------------------------------------------------------------------
 *                 PROTOTYPES FOR HELPER (PRIVATE) FUNCTIONS
 * -------------------------------------------------------------------------- */

/**
 * @brief Tokenises expressionBuffer into numeric tokens and operator tokens.
 *        e.g. "11+45" -> numeric tokens [11, 45], operator ['+'].
 * 
 * @param tokenValues Array to store numeric tokens.
 * @param tokenOps    Array to store operator tokens.
 * @param tokenCount  Pointer to an integer that will hold the number of numeric tokens.
 * @return 0 if successful, -1 if an unknown character or parse error occurs.
 */
static int tokeniseExpression(long *tokenValues, char *tokenOps, int *tokenCount);

/**
 * @brief Evaluates the token arrays using a simple two-pass BODMAS approach:
 *        1) * and / (from left to right)
 *        2) + and - (from left to right)
 * 
 * @param tokenValues Array of numeric tokens.
 * @param tokenOps    Array of operators (+, -, *, /).
 * @param tokenCount  Pointer to the count of numeric tokens.
 * @return 0 if successful, -1 if an error occurs (e.g. divide by zero).
 */
static int evaluateTokens(long *tokenValues, char *tokenOps, int *tokenCount);

/* --------------------------------------------------------------------------
 *                      PUBLIC (EXTERN) FUNCTIONS
 * -------------------------------------------------------------------------- */

void Calc_Init(void)
{
    // Clear the expression buffer and reset flags
    memset(expressionBuffer, 0, sizeof(expressionBuffer));
    exprIndex = 0;
    errorFlag = 0;
}

int Calc_AddChar(char c)
{
    // Check if buffer is already full
    if (exprIndex >= (MAX_EXPR_LEN - 1))
        return -1;

    // Add the new character, keep the buffer null-terminated
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
    // We'll store tokens in these arrays
    long tokenValues[MAX_EXPR_LEN] = {0};  
    char tokenOps[MAX_EXPR_LEN]    = {0};  
    int tokenCount = 0;

    // Reset errorFlag before evaluating
    errorFlag = 0;

    // Tokenise the raw expression
    if (tokeniseExpression(tokenValues, tokenOps, &tokenCount) < 0) {
        errorFlag = 1;
        return 0;
    }

    // Evaluate the token arrays
    if (evaluateTokens(tokenValues, tokenOps, &tokenCount) < 0) {
        errorFlag = 1;
        return 0;
    }

    // Final result is in tokenValues[0]
    return tokenValues[0];
}

int Calc_HadError(void)
{
    return errorFlag;
}

const char* Calc_GetExpression(void)
{
    // Return a pointer to the internal buffer for display purposes
    return expressionBuffer;
}

/* --------------------------------------------------------------------------
 *              PRIVATE HELPER FUNCTION IMPLEMENTATIONS
 * -------------------------------------------------------------------------- */

static int tokeniseExpression(long *tokenValues, char *tokenOps, int *tokenCount)
{
    int length = exprIndex; 
    int count = 0;         
    int opCount = 0;       
    long currentNumber = 0;
    int buildingNumber = 0; // indicates if we're in the middle of reading digits
    int i;

    for (i = 0; i < length; i++) {
        char c = expressionBuffer[i];

        // If it's a digit, build up the number
        if (c >= '0' && c <= '9') {
            buildingNumber = 1;
            currentNumber = (currentNumber * 10) + (c - '0');
        } else {
            // If we were building a number, store that number before the operator
            if (buildingNumber) {
                tokenValues[count++] = currentNumber;
                currentNumber = 0;
                buildingNumber = 0;
            }

            // Check if it's a valid operator
            if (c == '+' || c == '-' || c == '*' || c == '/') {
                tokenOps[opCount++] = c;
            } 
            // '=' or 'C' are control characters, ignore them in the token list
            else if (c == '=' || c == 'C') {
                // do nothing
            }
            else {
                // unknown character -> error
                return -1;
            }
        }
    }

    // If the expression ended with digits, store the last number
    if (buildingNumber) {
        tokenValues[count++] = currentNumber;
    }

    *tokenCount = count;
    return 0;
}

static int evaluateTokens(long *tokenValues, char *tokenOps, int *tokenCount)
{
    int count = *tokenCount;
    int i;

    // First pass: * and /
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
                if (rhs == 0) {
                    return -1; // division by zero
                }
                result = lhs / rhs; // integer division
            }

            tokenValues[i] = result; // store back
            // shift everything left to close gap
            for (int j = i + 1; j < (count - 1); j++) {
                tokenValues[j] = tokenValues[j + 1];
                tokenOps[j - 1] = tokenOps[j];
            }
            count--;
            // do NOT increment i, as we must re-check position i
        } else {
            i++;
        }
    }

    // Second pass: + and -
    i = 0;
    while (i < (count - 1)) {
        char op = tokenOps[i];
        if (op == '+' || op == '-') {
            long lhs = tokenValues[i];
            long rhs = tokenValues[i + 1];
            long result = (op == '+') ? (lhs + rhs) : (lhs - rhs);

            tokenValues[i] = result;
            // shift arrays
            for (int j = i + 1; j < (count - 1); j++) {
                tokenValues[j] = tokenValues[j + 1];
                tokenOps[j - 1] = tokenOps[j];
            }
            count--;
        } else {
            i++;
        }
    }

    *tokenCount = count;
    return 0;
}
