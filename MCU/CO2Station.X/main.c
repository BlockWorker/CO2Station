// PIC24FJ512GU405 Configuration Bit Settings

// 'C' source line config statements

// FSEC
#pragma config BWRP = OFF               // Boot Segment Write-Protect bit (Boot Segment may be written)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = OFF            // Alternate Interrupt Vector Table bit (Disabled AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Flash Page Address Limit bits (Enter Hexadecimal value)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Source Selection (Fast RC Oscillator with divide-by-N with PLL module (FRCPLL) )
#pragma config PLLMODE = PLL96DIV2      // PLL Mode Selection (96 MHz PLL. Oscillator input is divided by 2 (8 MHz input))
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFCN = ON            // OSC2 Pin Function bit (RC15 is general purpose digital I/O pin)
#pragma config SOSCEN = ON              // SOSC Power Selection Configuration bits (SOSC crystal driver is enabled (SOSCI/SOSCO mode))
#pragma config PLLSS = FRC              // PLL Secondary Selection Configuration bit (PLL is fed by the on-chip Fast RC (FRC) oscillator)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration bit (Allow only one reconfiguration)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler bits (1:32,768)
#pragma config FWPSA = PR128            // Watchdog Timer Prescaler bit (1:128)
#pragma config FWDTEN = ON_DIS_SLEEP    // Watchdog Timer Enable bits (WDT is enabled only while device is active (disabled in sleep, SWDTEN disabled))
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (WDT in non-windowed mode)
#pragma config WDTWIN = WIN25           // Watchdog Timer Window Select bits (WDT window is 25% of WDT period)
#pragma config WDTCMX = WDTCLK          // WDT MUX Source Select bits (WDT clock source is determined by the WDTCLK Configuration bits)
#pragma config WDTCLK = SOSC            // WDT Clock Source Select bits (WDT uses SOSC input)

// FPOR
#pragma config BOREN = ON               // Brown Out Enable bit (Brown Out is enabled)
#pragma config LPREGEN = ON             // Low power regulator control (Low power regulator controlled by RETEN is enabled)
#pragma config LPBOREN = ON             // Downside Voltage Protection Enable bit (Low power BOR is enabled when BOR is inactive)

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)
#pragma config BTSWP = OFF              // BOOTSWP Instruction Enable bit (BOOTSWP instruction is disabled)

// FDMTIVTL
#pragma config DMTIVTL = 0xFFFF         // DMT Window Interval Lower 16 bits (Enter Hexadecimal value)

// FDMTIVTH
#pragma config DMTIVTH = 0xFFFF         // DMT Window Interval Upper 16 bits (Enter Hexadecimal value)

// FDMTCNTL
#pragma config DMTCNTL = 0xFFFF         // DMT Instruction Count Time-out Value Lower 16 bits (Enter Hexadecimal value)

// FDMTCNTH
#pragma config DMTCNTH = 0xFFFF         // DMT Instruction Count Time-out Value Higher 16 bits (Enter Hexadecimal value)

// FDMT
#pragma config DMTDIS = OFF             // DMT Disable bit (Dead Man Timer is disabled and can be enabled by software)

// FDEVOPT1
#pragma config ALTCMPI = OFF            // Alternate Comparator Input Enable bit (C2INC and C3INC are on RG9 and RD11)
#pragma config TMPRPIN = OFF            // Tamper Pin Enable bit (TMPRN pin function is disabled)
#pragma config SOSCHP = OFF             // SOSC High Power Enable bit (valid only when SOSCSEL = 1 (SOSC low power mode)
#pragma config ALTI2C1 = ON             // Alternate I2C pin Location (ASDA1 and ASCL1 on RB5 and RB4)
#pragma config SMB3EN = ON              // SMBus 3 Enable bit (SMBus 3.0 input levels)

#include <xc.h>

void set_pmd_bits() {
    PMD1 = 0xf870; //disable all timers, UART1+2, SPI2; enable I2C1, SPI1, ADC
    PMD3 = 0x044e; //disable comparators, DAC, UART3, I2C2+3; enable RTCC, CRC
    PMD4 = 0x002a; //disable UART4, REFO, HVLD; enable USB
    PMD5 = 0x00ff; //disable all MCCP
    PMD6 = 0x0043; //disable LCD and SPI3+4
    PMD7 = 0x0000; //enable all DMA channels
    PMD8 = 0x01fc; //disable DMT, UART5+6, all CLC
}

int main() {
    set_pmd_bits();
    
    return 0;
}
