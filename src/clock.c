// PPL Defines //
#define SYSCTL_RIS_R 		  (*((volatile unsigned long *)0x400FE050)) 
#define SYSCTL_RCC_R 		  (*((volatile unsigned long *)0x400FE060))
#define SYSCTL_RCC2_R         (*((volatile unsigned long *)0x400FE070))	

// SysTick Defines	//
#define NVIC_ST_CTRL_R        (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R      (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R     (*((volatile unsigned long *)0xE000E018))

void SysTick_init(void){
	// Disable SysTick during Setup, ensuring the timer is off whilst configuring
	NVIC_ST_CTRL_R = 0; 
	
	// Maximum Reload Value, ensures the largest possible delay with the max 24bit value
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	
	// Clear the current value counter
	NVIC_ST_CURRENT_R = 0;  
	
	// Enable SysTick with Core Clock
	NVIC_ST_CTRL_R = 0x00000005;   
}

void PLL_init(void){
	// Using RCC2 for extended clock configuration
	SYSCTL_RCC2_R |=  0x80000000;
	
	// Bypass PPL during Initialisation, allowing the system to use the raw oscillator whilst initalising
	SYSCTL_RCC2_R |=  0x00000800;
	
	// Selecting External Crystal Freqeucny
	// Configure External Crystal to 16MHz (0x15)
	// Clears the External Oscillator field and set it appropriately to the connected crystal.
	 SYSCTL_RCC_R = (SYSCTL_RCC_R &~0x000007C0) + 0x00000540; 

	 // Using bits 6-4 to Select External Oscillator as The Clock
	 SYSCTL_RCC2_R &= ~0x00000070;

	// Activate PLL, clearing power down bit
	SYSCTL_RCC2_R &= ~0x00002000;

	// Configure System Divider to 80MHz
	SYSCTL_RCC2_R |= 0x40000000; // Bit 30 (DIV400): Enables a 400MHz PLL
	SYSCTL_RCC2_R = (SYSCTL_RCC2_R & 0x1FC00000) + (4 << 22); // Bits 28-22: Sets the system clock divider to 5. (400MHz / 5 = 80MHz)

	// Wait whilst we allow PLL to Lock 
	while ((SYSCTL_RIS_R & 0x00000040) == 0) {}; //  Bit 6 will indicate when the PLL has locked onto our target frequency.

	// Clear the bypass to utilse the PLL output
	SYSCTL_RCC2_R &= ~0x00000800;
}

// An input of 1, would result in a 12.5ns delay
void SysTick_wait(unsigned long delay){
	// Set counts to wait
	NVIC_ST_RELOAD_R = delay - 1; 

	// Clear Current
	NVIC_ST_CURRENT_R = 0; 

	// Begin Count
	while((NVIC_ST_CTRL_R&0x00010000)==0) {}
}

// Adjusting for input of milliseconds
void delay_ms(unsigned long delay){
	SysTick_wait(delay * 800000);
}

void delay_us(unsigned long delay){
	SysTick_wait(delay * 80);
}

