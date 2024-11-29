// clock.h

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
void SysTick_init(void);
void PLL_init(void);
void SysTick_wait(unsigned long delay);
void delay_ms(unsigned long delay);
void delay_us(unsigned long delay);

#endif  // CLOCK_H
