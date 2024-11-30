#include "clock.h"

void SysTick_init(void) {
    // Disable SysTick during setup
    NVIC_ST_CTRL_R = 0;
    
    // Set reload value to maximum (24-bit value)
    NVIC_ST_RELOAD_R = 0x00FFFFFF;
    
    // Clear current value
    NVIC_ST_CURRENT_R = 0;
    
    // Enable SysTick with core clock
    NVIC_ST_CTRL_R = 0x00000005;
}

void PLL_init(void) {
    // 1. Use RCC2 for advanced features
    SYSCTL_RCC2_R |= 0x80000000; // USERCC2
    
    // 2. Bypass PLL while initializing
    SYSCTL_RCC2_R |= 0x00000800; // BYPASS2
    
    // 3. Set crystal value and oscillator source
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000007C0) + 0x00000540; // Clear and set XTAL to 16 MHz
    SYSCTL_RCC2_R &= ~0x00000070; // Clear oscillator source bits
    SYSCTL_RCC2_R += 0x00000000;  // Set oscillator source to main oscillator
    
    // 4. Activate PLL by clearing PWRDN2
    SYSCTL_RCC2_R &= ~0x00002000;
    
    // 5. Set the desired system divider and the USESYSDIV bit
    SYSCTL_RCC2_R |= 0x40000000;  // USESYSDIV
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + (4 << 22); // Configure for 80 MHz clock
    
    // 6. Wait for the PLL to lock by polling PLLLRIS
    while ((SYSCTL_RIS_R & 0x00000040) == 0) {}; // Wait for PLLLRIS bit
    
    // 7. Enable use of PLL by clearing BYPASS
    SYSCTL_RCC2_R &= ~0x00000800;
}

void SysTick_wait(unsigned long delay) {
    // Set reload value
    NVIC_ST_RELOAD_R = delay - 1;
    
    // Clear current value
    NVIC_ST_CURRENT_R = 0;
    
    // Wait for count flag
    while ((NVIC_ST_CTRL_R & 0x00010000) == 0) {}
}

// ms Delay Function
void delay_ms(unsigned long delay) {
    unsigned long i;
    for (i = 0; i < delay; i++) {
        SysTick_wait(80000); // 1 ms delay at 80 MHz
    }
}

// us Delay Function
void delay_us(unsigned long delay) {
    unsigned long i;
    for (i = 0; i < delay; i++) {
        SysTick_wait(80); // 1 us delay at 80 MHz
    }
}
