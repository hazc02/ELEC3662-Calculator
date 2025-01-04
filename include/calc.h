#ifndef CALC_H
#define CALC_H

/**
 * @file calc.h
 * @brief This header file contains function prototypes and data structures
 *        for the calculator logic module.
 *
 * The calculator module stores individual keystrokes (digits/operators)
 * in a buffer, tokenises them into a series of integers and operators,
 * and evaluates the resulting expression according to BODMAS rules.
 */

/**
 * @brief Initialises the calculator module.
 *        Clears internal buffers and resets state.
 */
void Calc_Init(void);

/**
 * @brief Adds a single character (digit or operator) to the expression buffer.
 * 
 * @param c The character to be added ('0'-'9', '+', '-', '*', '/', '=').
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
 * @brief Performs a BODMAS-based evaluation of the stored expression.
 *        For example, the array ['1','1','+','4','5'] becomes [11, '+', 45],
 *        which is then evaluated.
 * 
 * @return long The final result of evaluating the expression.
 *         If an error occurs (e.g. division by zero),
 *         a special error flag is set (accessible via Calc_HadError()).
 */
long Calc_Evaluate(void);

/**
 * @brief Indicates whether the last evaluation encountered an error
 *        (e.g. division by zero).
 * 
 * @return int 
 *         1 -> An error occurred (e.g., divide by zero).
 *         0 -> No error in the last evaluation.
 */
int Calc_HadError(void);

#endif // CALC_H
