#ifndef CALC_H
#define CALC_H

/**
 * @file calc.h
 * @brief Header file for the Calculator Logic Module,
 *        handling BODMAS expression parsing and evaluation.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialises the calculator module.
 *        Clears internal buffers and resets state.
 */
void Calc_Init(void);

/**
 * @brief Adds a single character (digit or operator) to the expression buffer.
 * 
 * @param c The character to add ('0'..'9', '+', '-', '*', '/', '=').
 * @return int 
 *         0  -> Character successfully added.
 *         -1 -> Expression buffer is full (error).
 */
int Calc_AddChar(char c);

/**
 * @brief Clears the entire expression stored in the calculator module.
 *        Resets the expression buffer and any internal error flags.
 */
void Calc_ClearExpression(void);

/**
 * @brief Performs a BODMAS-based evaluation of the current expression.
 *        E.g. the array ['1','1','+','4','5'] becomes [11, '+', 45],
 *        then evaluated to 56.
 * 
 * @return long The result of evaluating the expression.
 *         If an error occurs (e.g. division by zero),
 *         a special error flag is set (check with Calc_HadError()).
 */
long Calc_Evaluate(void);

/**
 * @brief Indicates whether the last evaluation encountered an error
 *        (e.g. division by zero).
 * 
 * @return int 
 *         1 -> An error occurred
 *         0 -> No error in the last evaluation
 */
int Calc_HadError(void);

/**
 * @brief Returns a read-only pointer to the current expression buffer,
 *        so the UI code can display the entire expression on the LCD.
 * @return const char* pointer to the internal expression buffer.
 */
const char* Calc_GetExpression(void);

#ifdef __cplusplus
}
#endif

#endif // CALC_H
