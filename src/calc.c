#include "calc.h"
#include <string.h>   // for strlen, strcpy, etc.
#include <stdlib.h>   // for atof
#include <stdbool.h>
#include <math.h>     // sin, cos, tan, pow
#include <ctype.h>    // isdigit
#include <stdio.h>    // sprintf, etc.


/**
 * @brief Holds the user-typed expression"
 */
static char expressionBuffer[MAX_EXPR_LEN];

static int  exprIndex = 0;
static bool errorFlag = false;

/**
 * @brief Stores last result to allow user to start a new expression with an operator (e.g. +5 => lastResult+5).
 */
static double lastResult   = 0.0;
static bool   hasLastResult= false;

/**
 * @brief Convert degrees to radians for trig
 */
static double degToRad(double deg)
{
    return deg*(M_PI/180.0);
}

/**
 * @brief Initialises calculator (clear buffer, reset error). 
 *        lastResult remains for continuing calculations.
 */
void Calc_Init(void)
{
    memset(expressionBuffer, 0, sizeof(expressionBuffer));
    exprIndex=0;
    errorFlag=false;
}

/**
 * @brief Function to insert into the expression buffer
 *        Expand 's','c','t' => "sin","cos","tan". If '?' => ignore. Otherwise store char. 
 *        Return -1 if near full. Return 0 if success.
 */
int Calc_AddChar(char inputChar)
{
    if(inputChar == '?') {
        // Ignore placeholders
        return 0;
    }

    // Check if adding three characters would exceed buffer
    if(exprIndex >= (MAX_EXPR_LEN - 4)) {
        return -1; // No space
    }

    // Expand 's' to "sin", 'c' to "cos", 't' to "tan"
    const char *expansion = NULL;
    
    if(inputChar == 's') {
        expansion = "sin";
    } 
    else if(inputChar == 'c') {
        expansion = "cos";
    } 
    else if(inputChar == 't') {
        expansion = "tan";
    }

    if(expansion) {
        // Append each character of the expansion
        for(int i = 0; expansion[i] != '\0'; i++) {
            expressionBuffer[exprIndex++] = expansion[i];
        }
        expressionBuffer[exprIndex] = '\0'; // Null-terminate the string
        return 0;
    }

    // For normal characters, add to buffer directly
    expressionBuffer[exprIndex++] = inputChar;
    expressionBuffer[exprIndex] = '\0';

    return 0;
}

/**
 * @brief Clears buffer
 */
void Calc_ClearExpression(void)
{
    memset(expressionBuffer,0,sizeof(expressionBuffer));
    exprIndex=0;
    errorFlag=false;
}

static double parseAndEvaluate(void);

/**
 * @brief Evaluate the expression. 
 *        If empty => return last result (or 0)
 *        If first char is operator & we have lastResult => prepend lastResult
 */

double Calc_Evaluate(void)
{
    errorFlag=false;

    if(exprIndex==0){
        // no typed expression
        return hasLastResult ? lastResult : 0.0;
    }

    // If the first character is an operator, then use the previous answer at the start of the expression
    {
    char c = expressionBuffer[0];  // Get the first character of the expression

    // Check if the first character is an operator and if we have a previous result
    if ((c == '+' || c == '-' || c == '*' || c == '/' || c == '^') && hasLastResult) {
        
        char temp[128];  // Temporary buffer to construct the new expression
        
        // Create a new expression by appending the last result, the operator, and the rest of the expression
        sprintf(temp, "%.6f", lastResult);  // Convert lastResult to a string with 6 decimal places
        temp[strlen(temp)] = c;             // Append the operator
        temp[strlen(temp) + 1] = '\0';      // Null-terminate after the operator

        // Append the rest of the expression, skipping the first character (the operator)
        strcat(temp, &expressionBuffer[1]); 

        // Copy the new expression back to the main buffer
        strncpy(expressionBuffer, temp, sizeof(expressionBuffer) - 1);
        expressionBuffer[sizeof(expressionBuffer) - 1] = '\0';  // Ensure null termination

        // Update the expression index to reflect the new length
        exprIndex = (int)strlen(expressionBuffer);
    }
}

    double val= parseAndEvaluate();
    if(!errorFlag){
        lastResult   = val;
        hasLastResult= true;
        return val;
    }
    return 0.0;
}

int Calc_HadError(void)
{
    return (errorFlag? 1:0);
}

const char* Calc_GetExpression(void)
{
    return expressionBuffer;
}

//////////////////// Implementation Part ////////////////////

/// The expression buffer is parsed as tokens, multiple passes are completed to handle BODMAS for ^, * /, + -.

typedef enum {
    TOKEN_NUMBER,
    TOKEN_OPERATOR,  // + - * / ^
    TOKEN_TRIG
} TokenType;

typedef struct {
    TokenType type;
    double    numberVal;
    char      opChar;  
    char      trigString[16];
} CalcToken;

static CalcToken tokens[32];
static int       tokenCount=0;

static int tokeniseExpression(void);
static double processTokens(void);

/**
 * @brief parseAndEvaluate => tokenise, process => final
 */
static double parseAndEvaluate(void)
{
    tokenCount=0;
    if(tokeniseExpression()<0){
        errorFlag=true;
        return 0.0;
    }
    double finalVal= processTokens();
    return finalVal;
}

/**
 * @brief Evaluate e.g. "sin30" => sin(30 deg).
 */
static double evaluateTrig(const char* trigStr)
{
    
    char func[4];
    strncpy(func, trigStr,3);
    func[3]='\0';

    double angleDeg= atof(&trigStr[3]);
    double rad= degToRad(angleDeg);

    if(strcmp(func,"sin")==0) return sin(rad);
    if(strcmp(func,"cos")==0) return cos(rad);
    if(strcmp(func,"tan")==0) return tan(rad);
    errorFlag=true;
    return 0.0;
}

/**
 * @brief Tokenise the expressionBuffer and populate the tokens array
 */
static int tokeniseExpression(void)
{
    const char *currentChar = expressionBuffer;
    tokenCount = 0;

    while (*currentChar != '\0') {

        // Handle operator (+, -, *, /, ^)
        if (strchr("+-*/^", *currentChar)) {
            tokens[tokenCount].type = TOKEN_OPERATOR;
            tokens[tokenCount].opChar = *currentChar;
            tokenCount++;
            currentChar++;
        }
        // Handle trigonometric functions (sin, cos, tan)
        else if (strncmp(currentChar, "sin", 3) == 0 || 
                 strncmp(currentChar, "cos", 3) == 0 || 
                 strncmp(currentChar, "tan", 3) == 0) {

            char trigBuffer[16] = {0};  // Buffer for trigonometric function
            int i = 0;

            while (*currentChar && !isspace((unsigned char)*currentChar) && 
                   !strchr("+-*/^", *currentChar) && i < sizeof(trigBuffer) - 1) {
                trigBuffer[i++] = *currentChar++;
            }
            trigBuffer[i] = '\0';

            tokens[tokenCount].type = TOKEN_TRIG;
            strncpy(tokens[tokenCount].trigString, trigBuffer, sizeof(tokens[tokenCount].trigString) - 1);
            tokenCount++;
        }
        // Handle numbers (including decimals and negatives)
        else if (isdigit((unsigned char)*currentChar) || *currentChar == '.' || 
                (*currentChar == '-' && isdigit((unsigned char)*(currentChar + 1)))) {

            char numBuffer[32] = {0};  // Buffer for numbers
            int j = 0;

            if (*currentChar == '-') {
                numBuffer[j++] = *currentChar++;
            }

            while (*currentChar && !isspace((unsigned char)*currentChar) && 
                   !strchr("+-*/^", *currentChar) && j < sizeof(numBuffer) - 1) {
                numBuffer[j++] = *currentChar++;
            }
            numBuffer[j] = '\0';

            tokens[tokenCount].type = TOKEN_NUMBER;
            tokens[tokenCount].numberVal = atof(numBuffer);
            tokenCount++;
        }
        // Handle unrecognised characters
        else {
            errorFlag = true;
            return -1;
        }
    }

    return 0;  // Successful tokenisation
}

/**
 * @brief Multi-pass token processing
 */
static double processTokens(void)
{
    // Evaluate trigonometric functions and replace them with numerical values
    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].type == TOKEN_TRIG) {
            tokens[i].numberVal = evaluateTrig(tokens[i].trigString);
            tokens[i].type = TOKEN_NUMBER;
        }
    }

    double numbers[32];
    char operators[32];
    int numCount = 0, opCount = 0;

    // Separate tokens into numbers and operators
    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].type == TOKEN_NUMBER) {
            numbers[numCount++] = tokens[i].numberVal;
        } else if (tokens[i].type == TOKEN_OPERATOR) {
            operators[opCount++] = tokens[i].opChar;
        } else {
            errorFlag = true;
            return 0.0;
        }
    }

    // Process operations based on precedence
    char precedence[] = {'^', '*', '/', '+', '-'};
    for (int p = 0; p < sizeof(precedence); p++) {
        for (int i = 0; i < opCount; ) {
            if (operators[i] == precedence[p]) {
                if (i >= numCount - 1) {
                    errorFlag = true;
                    return 0.0;
                }
                double result = (operators[i] == '^') ? pow(numbers[i], numbers[i + 1]) :
                                (operators[i] == '*') ? numbers[i] * numbers[i + 1] :
                                (operators[i] == '/') ? (numbers[i + 1] != 0 ? numbers[i] / numbers[i + 1] : (errorFlag = true, 0.0)) :
                                (operators[i] == '+') ? numbers[i] + numbers[i + 1] :
                                                        numbers[i] - numbers[i + 1];

                if (errorFlag) return 0.0;
                
                numbers[i] = result;

                // Shift arrays to remove processed operator and number
                for (int j = i + 1; j < numCount - 1; j++) {
                    numbers[j] = numbers[j + 1];
                }
                for (int j = i; j < opCount - 1; j++) {
                    operators[j] = operators[j + 1];
                }
                numCount--;
                opCount--;
            } else {
                i++;
            }
        }
    }

    if (numCount != 1) {
        errorFlag = true;
        return 0.0;
    }
    return numbers[0];
}

