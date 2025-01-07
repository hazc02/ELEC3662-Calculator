#ifndef CALC_H
#define CALC_H

/**
 * @file calc.h
 * @brief Pure C Calculator logic module:
 *        - Expression buffer
 *        - Trig in degrees typed like sin30 (no parentheses for angles)
 *        - Exponent '^', plus + - * /
 *        - If first token is an operator, we use the last result
 *        - Strips trailing zeros up to 3 decimal places
 *        - No bracket logic, bracket => error
 */

/// Initialises the calculator state (clears expression buffer, error flags)
void   Calc_Init(void);

/// Adds one character. If 's','c','t' => expands to "sin","cos","tan". If '?' => ignore. If buffer is near full => return -1
int    Calc_AddChar(char inputChar);

/// Clears the current expression
void   Calc_ClearExpression(void);

/// Evaluates with multi-pass approach. If first token is operator & we have last result => prepend that. Returns final value or 0 if error
double Calc_Evaluate(void);

/// Returns 1 if error, 0 if no error
int    Calc_HadError(void);

/// Returns pointer to the expression buffer (for LCD)
const char* Calc_GetExpression(void);

#define M_PI 3.14159265358979323846
#define MAX_EXPR_LEN 64

#endif // CALC_H
