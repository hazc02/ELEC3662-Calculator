#include "calc.h"
#include <string.h>   // for strlen, strcpy, etc.
#include <stdlib.h>   // for atof
#include <stdbool.h>
#include <math.h>     // sin, cos, tan, pow
#include <ctype.h>    // isdigit
#include <stdio.h>    // sprintf, etc.

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_EXPR_LEN 64

/**
 * @brief Holds the user-typed expression, e.g. "sin30+2^3"
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
 * @brief Expand 's','c','t' => "sin","cos","tan". If '?' => ignore. Otherwise store char. 
 *        Return -1 if near full. Return 0 if success.
 */
int Calc_AddChar(char inputChar)
{
    if(inputChar == '?') {
        // Ignore placeholders
        return 0;
    }

    // Check if adding three characters (sin, cos, tan) would exceed buffer
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
 * @brief Clears expression
 */
void Calc_ClearExpression(void)
{
    memset(expressionBuffer,0,sizeof(expressionBuffer));
    exprIndex=0;
    errorFlag=false;
}

/**
 * @brief Evaluate the expression. 
 *        If empty => return last result (or 0)
 *        If first char is operator & we have lastResult => prepend lastResult
 */
static double parseAndEvaluate(void);
double Calc_Evaluate(void)
{
    errorFlag=false;

    if(exprIndex==0){
        // no typed expression
        return hasLastResult? lastResult:0.0;
    }

    // if first char is operator => prepend lastResult if we have one
    {
        char c= expressionBuffer[0];
        if((c=='+'|| c=='-'|| c=='*'|| c=='/'|| c=='^') && hasLastResult){
            char temp[128];
            sprintf(temp,"%.6f%c%s", lastResult, c, &expressionBuffer[1]);
            strncpy(expressionBuffer, temp, sizeof(expressionBuffer));
            exprIndex= (int)strlen(expressionBuffer);
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

/// We'll parse the expression as tokens, then do multi-pass for ^, * /, + -.

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

static int tokenizeExpression(void);
static double processTokens(void);

/**
 * @brief parseAndEvaluate => tokenize, process => final
 */
static double parseAndEvaluate(void)
{
    tokenCount=0;
    if(tokenizeExpression()<0){
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
    // e.g. "sin30.5"
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
 * @brief Tokenise the expressionBuffer => fill tokens
 */
static int tokenizeExpression(void)
{
    const char* p= expressionBuffer;
    tokenCount=0;

    while(*p!='\0'){
        // skip spaces
        while(isspace((unsigned char)*p)) p++;
        if(*p=='\0') break;

        // bracket => error
        if(*p=='(' || *p==')'){
            errorFlag=true;
            return -1;
        }

        // operator?
        if(strchr("+-*/^", *p)!=NULL){
            tokens[tokenCount].type= TOKEN_OPERATOR;
            tokens[tokenCount].opChar= *p;
            tokenCount++;
            p++;
            continue;
        }

        // trig? "sin","cos","tan"
        if(strncmp(p,"sin",3)==0 || strncmp(p,"cos",3)==0 || strncmp(p,"tan",3)==0){
            // read up to next operator or space
            char trigBuf[16];
            int tIndex=0;
            while(*p && !isspace((unsigned char)*p) && !strchr("+-*/^", *p)){
                if(tIndex<15) trigBuf[tIndex++]=*p;
                p++;
            }
            trigBuf[tIndex]='\0';

            tokens[tokenCount].type= TOKEN_TRIG;
            strncpy(tokens[tokenCount].trigString, trigBuf, 15);
            tokens[tokenCount].trigString[15]='\0';
            tokenCount++;
            continue;
        }

        // number => digits + decimal + optional leading '-'
        if(isdigit((unsigned char)*p) || *p=='.' || (*p=='-' && isdigit((unsigned char)*(p+1)))){
            char numBuf[32];
            int n=0;
            // if leading '-'
            if(*p=='-'){
                numBuf[n++]= *p;
                p++;
            }
            while(*p && !isspace((unsigned char)*p) && !strchr("+-*/^", *p)){
                if(n<31) numBuf[n++]=*p;
                p++;
            }
            numBuf[n]='\0';

            tokens[tokenCount].type= TOKEN_NUMBER;
            tokens[tokenCount].numberVal= atof(numBuf);
            tokenCount++;
            continue;
        }

        // unknown => error
        errorFlag=true;
        return -1;
    }

    return 0; // success
}

/**
 * @brief multi-pass: first interpret trig => number, then do exponent pass, then * /, then + -.
 */
static double processTokens(void)
{
    // interpret trig
    for(int i=0;i<tokenCount;i++){
        if(tokens[i].type== TOKEN_TRIG){
            double val= evaluateTrig(tokens[i].trigString);
            tokens[i].type= TOKEN_NUMBER;
            tokens[i].numberVal= val;
        }
    }

    double numbers[32];
    char   operators[32];
    int numCount=0, opCount=0;

    for(int i=0;i<tokenCount;i++){
        if(tokens[i].type==TOKEN_NUMBER){
            numbers[numCount++]= tokens[i].numberVal;
        }
        else if(tokens[i].type==TOKEN_OPERATOR){
            operators[opCount++]= tokens[i].opChar;
        }
        else {
            errorFlag=true;
            return 0.0;
        }
    }

    // pass1 => '^'
    for(int i=0;i<opCount; ){
        if(operators[i]=='^'){
            if(i>= numCount-1){ errorFlag=true; break;}
            double lhs= numbers[i];
            double rhs= numbers[i+1];
            double r= pow(lhs,rhs);
            numbers[i]= r;
            // shift left
            for(int j=i+1;j<numCount-1;j++){
                numbers[j]=numbers[j+1];
            }
            for(int j=i;j<opCount-1;j++){
                operators[j]=operators[j+1];
            }
            numCount--;
            opCount--;
        }
        else i++;
    }

    // pass2 => * /
    for(int i=0;i<opCount; ){
        if(operators[i]=='*' || operators[i]=='/'){
            if(i>= numCount-1){ errorFlag=true; break;}
            double lhs= numbers[i];
            double rhs= numbers[i+1];
            double r=0.0;
            if(operators[i]=='*') r= lhs*rhs;
            else {
                if(rhs==0.0){errorFlag=true; break;}
                r= lhs/rhs;
            }
            numbers[i]= r;
            for(int j=i+1;j<numCount-1;j++){
                numbers[j]=numbers[j+1];
            }
            for(int j=i;j<opCount-1;j++){
                operators[j]=operators[j+1];
            }
            numCount--;
            opCount--;
        }
        else i++;
    }

    // pass3 => + -
    for(int i=0;i<opCount; ){
        if(operators[i]=='+'|| operators[i]=='-'){
            if(i>= numCount-1){errorFlag=true; break;}
            double lhs= numbers[i];
            double rhs= numbers[i+1];
            double r= (operators[i]=='+')?(lhs+rhs):(lhs-rhs);
            numbers[i]= r;
            for(int j=i+1;j<numCount-1;j++){
                numbers[j]= numbers[j+1];
            }
            for(int j=i;j<opCount-1;j++){
                operators[j]=operators[j+1];
            }
            numCount--;
            opCount--;
        }
        else i++;
    }

    if(errorFlag || numCount!=1){
        errorFlag=true;
        return 0.0;
    }
    return numbers[0];
}
