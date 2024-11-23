#ifndef CLOCK_H  
#define CLOCK_H

// PLL (Phase-Locked Loop) Register Definitions
#define SYSCTL_RIS_R           (*((volatile unsigned long *)0x400FE050)) 
#define SYSCTL_RCC_R           (*((volatile unsigned long *)0x400FE060))
#define SYSCTL_RCC2_R          (*((volatile unsigned long *)0x400FE070))

// SysTick Timer Register Definitions
#define NVIC_ST_CTRL_R         (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R       (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R      (*((volatile unsigned long *)0xE000E018))

// Function Prototypes
/**
 * @brief Initialises the SysTick timer.
 * Configures the SysTick timer to use the core clock as its source, sets the reload
 * value to its maximum to allow the largest delay, and clears the current value counter.
 */
void SysTick_init(void);

/**
 * @brief Initialises the PLL to set the system clock to 80 MHz.
 * Sets up the Phase-Locked Loop (PLL) to generate a system clock frequency of 80 MHz,
 */
void PLL_init(void);

/**
 * @brief Implements a delay in clock cycles using the SysTick timer.
 * 
 * @param delay The number of clock cycles to wait.
 * 
 * @note At an 80 MHz clock, an input of 1 corresponds to a delay of 12.5 nanoseconds.
 */
void SysTick_wait(unsigned long delay);

/**
 * @brief Implements a delay in milliseconds.
 * 
 * @param delay The number of milliseconds to wait.
 * 
 * @note This function calculates the appropriate delay based on an 80 MHz system clock.
 */
void delay_ms(unsigned long delay);

/**
 * @brief Implements a delay in microseconds.
 * 
 * @param delay The number of microseconds to wait.
 * 
 * @note This function calculates the appropriate delay based on an 80 MHz system clock.
 */
void delay_us(unsigned long delay);

#endif  // CLOCK_H

