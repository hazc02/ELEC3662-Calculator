// PPL Defines //
#define SYSCTL_RIS_R 					(*((volatile unsigned long *)0x400FE050)) 
#define SYSCTL_RCC_R 					(*((volatile unsigned long *)0x400FE060))
#define SYSCTL_RCC2_R         (*((volatile unsigned long *)0x400FE070))	

// SysTick Defines	//
#define NVIC_ST_CTRL_R        (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R      (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R     (*((volatile unsigned long *)0xE000E018))

void SysTick_Init(void){
	// Disable SysTick during Setup, ensuring the timer is off whilst configuring
	NVIC_ST_CTRL_R = 0; 
	
	// Maximum Reload Value, ensures the largest possible delay with the max 24bit value
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	
	// Clear the current value counter
	NVIC_ST_CURRENT_R = 0;  
	
	// Enable SysTick with Core Clock
	NVIC_ST_CTRL_R = 0x00000005;   
}

void PLL_Init(void){
	// Using RCC2 for extended clock configuration
	SYSCTL_RCC2_R |=  0x80000000;
	
	// Bypass PPL during Initialisation, allowing the system to use the raw oscillator whilst initalising
	SYSCTL_RCC2_R |=  0x00000800;
	
	// Selecting External Crystal Value and Oscillator Source
	// Configure External Crystal to 16MHz (0x15)
	// Clears the External Oscillator field and set it appropriately to the connected crystal.
	 SYSCTL_RCC_R = (SYSCTL_RCC_R &~0x000007C0) + 0x00000540; 
}

