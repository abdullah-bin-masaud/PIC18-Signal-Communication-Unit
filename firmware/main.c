/**
 * Project: PIC18 Microcontroller Signal Communication Unit
 * Target MCU: PIC18F4550 @ 20.0 MHz Crystal Oscillator
 * Compiler: Microchip MPLAB XC8 v2.40+
 * Description: Interrupt-driven signal capture on RB0/INT0 with debouncing,
 *              non-volatile EEPROM event logging, and UART telemetry @ 9600 baud.
 */

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "eeprom.h"

// =============================================================================
// PIC18F4550 Configuration Bit Settings
// =============================================================================
#pragma config PLLDIV = 5         // Divide by 5 (20 MHz oscillator input)
#pragma config CPUDIV = OSC1_PLL2 // [OSC1/OSC2 Src: /1][96 MHz PLL Src: /2]
#pragma config USBDIV = 2         // USB clock source comes from the 96 MHz PLL divided by 2
#pragma config FOSC = HS          // High Speed Oscillator (HS)
#pragma config FCMEN = OFF        // Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF         // Oscillator Switchover mode disabled
#pragma config PWRT = ON          // Power-up Timer enabled
#pragma config BOR = ON           // Brown-out Reset enabled
#pragma config BORV = 3           // Minimum setting
#pragma config VREGEN = OFF       // USB voltage regulator disabled
#pragma config WDT = OFF          // Watchdog Timer disabled
#pragma config MCLRE = ON         // MCLR pin enabled; RE3 input pin disabled
#pragma config LVP = OFF          // Single-Supply ICSP disabled
#pragma config XINST = OFF        // Extended Instruction Set disabled

#define _XTAL_FREQ 20000000UL     // 20 MHz Oscillator Frequency

// Global event counter
volatile uint16_t g_event_counter = 0;
volatile uint8_t  g_flag_new_event = 0;
uint8_t           g_eeprom_ptr = 0x00;

// =============================================================================
// Interrupt Service Routine (High Priority)
// =============================================================================
void __interrupt() ISR(void) {
    if (INTCONbits.INT0IF) {
        // RB0/INT0 external trigger detected
        // Simple hardware debouncing check
        __delay_ms(15);
        if (PORTBbits.RB0 == 0) { // Active-low button/sensor trigger
            g_event_counter++;
            g_flag_new_event = 1;
            LATCbits.LATC0 ^= 1;  // Toggle indicator LED on RC0
        }
        INTCONbits.INT0IF = 0;    // Clear interrupt flag
    }
}

// =============================================================================
// EEPROM Functions
// =============================================================================
void EEPROM_Write(uint8_t addr, uint8_t data) {
    EEADR = addr;
    EEDATA = data;
    EECON1bits.EEPGD = 0;  // Access data EEPROM memory
    EECON1bits.CFGS = 0;   // Access Flash/EEPROM, not config registers
    EECON1bits.WREN = 1;   // Enable write operations

    INTCONbits.GIE = 0;    // Disable interrupts for required unlock sequence
    EECON2 = 0x55;         // Unlock sequence byte 1
    EECON2 = 0xAA;         // Unlock sequence byte 2
    EECON1bits.WR = 1;     // Initiate write cycle
    INTCONbits.GIE = 1;    // Restore interrupts

    while (EECON1bits.WR); // Wait for completion
    EECON1bits.WREN = 0;   // Inhibit writes
}

uint8_t EEPROM_Read(uint8_t addr) {
    EEADR = addr;
    EECON1bits.EEPGD = 0;  // Point to Data EEPROM
    EECON1bits.CFGS = 0;   // Access Flash/EEPROM
    EECON1bits.RD = 1;     // Start read operation
    return EEDATA;
}

// =============================================================================
// UART Functions (9600 Baud @ 20 MHz Fosc)
// =============================================================================
void UART_Init(void) {
    TRISCbits.TRISC6 = 0;  // RC6 (TX) as output
    TRISCbits.TRISC7 = 1;  // RC7 (RX) as input

    // Baud rate calculation for 9600 @ 20MHz:
    // SPBRG = (Fosc / (64 * Baud)) - 1 = (20000000 / (64 * 9600)) - 1 = 31.55 ~ 31
    SPBRG = 31;
    TXSTAbits.BRGH = 0;    // Low-speed baud rate
    TXSTAbits.SYNC = 0;    // Asynchronous mode
    RCSTAbits.SPEN = 1;    // Enable serial port pins
    TXSTAbits.TXEN = 1;    // Enable transmitter
    RCSTAbits.CREN = 1;    // Enable continuous receiver
}

void UART_Write(char data) {
    while (!TXSTAbits.TRMT); // Wait until transmit buffer is empty
    TXREG = data;
}

void UART_Write_Text(const char* text) {
    while (*text) {
        UART_Write(*text++);
    }
}

// =============================================================================
// Main Program
// =============================================================================
void main(void) {
    // 1. Port Configurations
    ADCON1 = 0x0F;         // Configure all analog pins as digital I/O
    TRISBbits.TRISB0 = 1;  // RB0/INT0 as input (external sensor/button)
    TRISCbits.TRISC0 = 0;  // RC0 as output (heartbeat/event LED)
    LATCbits.LATC0 = 0;

    // 2. Initialize Peripherals
    UART_Init();
    __delay_ms(100);

    UART_Write_Text("\r\n========================================\r\n");
    UART_Write_Text("[SYS] PIC18F4550 Signal Unit Initialized\r\n");
    UART_Write_Text("[SYS] Telemetry Baud: 9600 8-N-1\r\n");
    UART_Write_Text("========================================\r\n");

    // 3. External Interrupt Configuration
    INTCON2bits.INTEDG0 = 0; // Trigger on falling edge of RB0
    INTCONbits.INT0IF = 0;   // Clear flag
    INTCONbits.INT0IE = 1;   // Enable INT0 external interrupt
    INTCONbits.GIE = 1;      // Enable global interrupts
    INTCONbits.PEIE = 1;     // Enable peripheral interrupts

    char tx_buffer[64];

    while (1) {
        if (g_flag_new_event) {
            g_flag_new_event = 0;

            // Log event sequence into non-volatile EEPROM
            // Memory layout: [Addr: Event_LSB, Addr+1: Event_MSB]
            EEPROM_Write(g_eeprom_ptr, (uint8_t)(g_event_counter & 0xFF));
            EEPROM_Write(g_eeprom_ptr + 1, (uint8_t)((g_event_counter >> 8) & 0xFF));
            g_eeprom_ptr = (g_eeprom_ptr + 2) % 250; // Circular buffer across EEPROM

            // Transmit formatted telemetry record over UART
            sprintf(tx_buffer, "[EVENT] #%u | Pulse Captured | EEPROM Logged at 0x%02X\r\n",
                    g_event_counter, g_eeprom_ptr);
            UART_Write_Text(tx_buffer);
        }
        __delay_ms(10);
    }
}
