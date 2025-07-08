#include <xc.h>
#include <stdint.h>

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 20000000

// Configurar pines para las fases
#define PHASE_A RC0
#define PHASE_B RC1
#define PHASE_C RC2

volatile uint8_t pwm_step = 0;
volatile uint8_t duty_A = 0;
volatile uint8_t duty_B = 0;
volatile uint8_t duty_C = 0;

// Tabla senoidal: 60 valores, de 0 a 100 (cambiar a entero sin signo de 0 a 100)
const uint8_t sin_table[60] = {
    50, 53, 56, 59, 62, 65, 68, 71, 74, 77, 80, 82, 85, 87, 89, 91, 
    93, 95, 96, 97, 98, 99, 99, 100, 100, 100, 99, 99, 98, 97, 96, 95, 
    93, 91, 89, 87, 85, 82, 80, 77, 74, 71, 68, 65, 62, 59, 56, 53, 
    50, 47, 44, 41, 38, 35, 32, 29, 26, 23, 20, 18, 15, 13, 11, 9,
    7, 5, 4, 3, 2, 1, 1, 0, 0, 0, 1, 1, 2, 3, 4, 5,
    7, 9, 11, 13, 15, 18, 20, 23, 26, 29, 32, 35, 38, 41, 44, 47
};

uint8_t sin_index = 0;

void interrupt ISR(void) {
    // Interrupción del Timer0
    if (T0IF) {
        T0IF = 0; // Limpiar bandera de interrupción del Timer0
        TMR0 = 251; // Recargar para 1 us (20 MHz, 0.2 us por ciclo, 5 ciclos -> 256-5=251)

        // Actualizar el paso PWM
        pwm_step++;
        if (pwm_step >= 100) {
            pwm_step = 0;
        }

        // Actualizar los pines
        PHASE_A = (pwm_step < duty_A) ? 1 : 0;
        PHASE_B = (pwm_step < duty_B) ? 1 : 0;
        PHASE_C = (pwm_step < duty_C) ? 1 : 0;
    }

    // Interrupción del Timer1
    if (TMR1IF) {
        TMR1IF = 0; // Limpiar bandera
        // Recargar Timer1 para 333.33 us
        TMR1H = 0xF9;
        TMR1L = 0x7D; // 0xF97D = 63869, 65536-63869=1667 ticks (1667*0.2 us = 333.4 us)

        // Actualizar los ciclos de trabajo desde la tabla senoidal
        duty_A = sin_table[sin_index];
        // Fase B: desplazada 120 grados (20 posiciones en una tabla de 60)
        duty_B = sin_table[(sin_index + 20) % 60];
        // Fase C: desplazada 240 grados (40 posiciones)
        duty_C = sin_table[(sin_index + 40) % 60];

        sin_index++;
        if (sin_index >= 60) {
            sin_index = 0;
        }
    }
}

void main(void) {
    // Configurar pines de salida
    TRISC0 = 0; // PHASE_A como salida
    TRISC1 = 0; // PHASE_B
    TRISC2 = 0; // PHASE_C

    // Inicializar pines
    PHASE_A = 0;
    PHASE_B = 0;
    PHASE_C = 0;

    // Configurar Timer0 para interrupción cada 1 us
    T0CS = 0;   // Reloj interno (Fosc/4)
    PSA = 0;    // Asignar preescaler al Timer0
    PS2 = 0; PS1 = 0; PS0 = 0; // Preescaler 1:1 (bits PS<2:0> = 000)
    TMR0 = 251; // Valor inicial para contar 5 ticks (1 us)
    T0IF = 0;   // Limpiar bandera
    T0IE = 1;   // Habilitar interrupción Timer0

    // Configurar Timer1
    TMR1CS = 0; // Reloj interno (Fosc/4)
    T1CKPS1 = 0; T1CKPS0 = 0; // Preescaler 1:1
    TMR1H = 0xF9;
    TMR1L = 0x7D;
    TMR1IF = 0; // Limpiar bandera
    TMR1IE = 1; // Habilitar interrupción Timer1

    // Configurar interrupciones
    GIE = 1;    // Habilitar interrupciones globales
    PEIE = 1;   // Habilitar interrupciones periféricas

    while (1) {
        // Bucle principal, no hace nada, todo en interrupciones
        __nop();
    }
}
