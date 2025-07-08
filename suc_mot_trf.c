Para controlar un motor trifásico con el PIC16F877, necesitamos generar tres señales PWM desfasadas 120 grados entre sí. El PIC16F877 tiene módulos CCP (Capture/Compare/PWM) que pueden usarse para dos señales, pero la tercera debe generarse por software. Aquí tienes un programa que logra esto usando SPWM (Sinusoidal PWM):

```c
#include <xc.h>
#include <stdint.h>

#pragma config FOSC = HS    // Oscilador externo HS
#pragma config WDTE = OFF   // Watchdog desactivado
#pragma config PWRTE = ON   // Power-up Timer activado
#pragma config BOREN = ON   // Brown-out Reset activado
#pragma config LVP = OFF    // Low Voltage Programming desactivado
#pragma config CPD = OFF    // Data Code Protection desactivado
#pragma config WRT = OFF    // Flash Write Protection desactivado
#pragma config CP = OFF     // Code Protection desactivado

#define _XTAL_FREQ 20000000 // Frecuencia del cristal: 20 MHz

// Definición de pines para las fases
#define PHASE_A RC0
#define PHASE_B RC1
#define PHASE_C RC2

// Variables globales
volatile uint8_t pwm_step = 0;
volatile uint8_t duty_A = 0, duty_B = 0, duty_C = 0;
uint8_t sin_index = 0;

// Tabla senoidal (60 valores, 0-100)
const uint8_t sin_table[60] = {
    50, 53, 56, 59, 62, 65, 68, 71, 74, 77, 80, 82, 85, 87, 89, 91,
    93, 95, 96, 97, 98, 99, 99, 100, 100, 100, 99, 99, 98, 97, 96, 95,
    93, 91, 89, 87, 85, 82, 80, 77, 74, 71, 68, 65, 62, 59, 56, 53,
    50, 47, 44, 41, 38, 35, 32, 29, 26, 23, 20, 18, 15, 13, 11, 9,
    7, 5, 4, 3, 2, 1, 1, 0, 0, 0, 1, 1, 2, 3, 4, 5,
    7, 9, 11, 13, 15, 18, 20, 23, 26, 29, 32, 35, 38, 41, 44, 47
};

void __interrupt() ISR(void) {
    // Interrupción Timer0 (PWM)
    if (T0IF) {
        T0IF = 0;           // Limpiar bandera
        TMR0 = 251;         // Recarga para 1 µs (20MHz)
        
        pwm_step++;
        if (pwm_step >= 100) pwm_step = 0;
        
        // Actualizar salidas PWM
        PHASE_A = (pwm_step < duty_A) ? 1 : 0;
        PHASE_B = (pwm_step < duty_B) ? 1 : 0;
        PHASE_C = (pwm_step < duty_C) ? 1 : 0;
    }
    
    // Interrupción Timer1 (Actualización SPWM)
    if (TMR1IF) {
        TMR1IF = 0;         // Limpiar bandera
        TMR1H = 0xFC;       // Recarga para 50Hz base (333.33 µs)
        TMR1L = 0x18;
        
        // Actualizar duty cycles desde tabla senoidal
        duty_A = sin_table[sin_index];
        duty_B = sin_table[(sin_index + 20) % 60]; // 120° de desfase
        duty_C = sin_table[(sin_index + 40) % 60]; // 240° de desfase
        
        sin_index++;
        if (sin_index >= 60) sin_index = 0;
    }
}

void main(void) {
    // Configurar puertos
    TRISC = 0x00;   // PORTC como salida
    PORTC = 0x00;   // Inicializar en bajo
    
    // Configurar Timer0 (PWM portadora @ 10kHz)
    T0CS = 0;       // Reloj interno
    PSA = 0;        // Preescaler asignado
    PS2 = 0; PS1 = 0; PS0 = 0; // Preescaler 1:2
    TMR0 = 251;     // 1 µs por paso (100 pasos = 100µs → 10kHz)
    T0IF = 0;
    T0IE = 1;
    
    // Configurar Timer1 (Modulación @ 50Hz)
    TMR1CS = 0;     // Reloj interno
    T1CKPS1 = 0; T1CKPS0 = 0; // Preescaler 1:1
    TMR1H = 0xFC;   // Recarga para 333.33 µs
    TMR1L = 0x18;
    TMR1IF = 0;
    TMR1IE = 1;
    
    // Configurar interrupciones
    GIE = 1;        // Habilitar interrupciones globales
    PEIE = 1;       // Habilitar interrupciones periféricas
    
    while(1) {
        // Bucle principal (todo se maneja por interrupciones)
        SLEEP();
    }
}
```

**Explicación del funcionamiento:**

1. **Configuración de hardware:**
   - Oscilador externo de 20 MHz
   - PORTC como salida para las 3 fases (pines RC0, RC1, RC2)
   - Timer0 para la portadora PWM (10 kHz)
   - Timer1 para la modulación senoidal (50 Hz)

2. **Generación PWM:**
   - **Timer0 (10 kHz):** Genera la base de tiempo para el PWM (100 pasos = 100 µs)
   - En cada interrupción actualiza las salidas comparando `pwm_step` con los valores de `duty_A`, `duty_B`, `duty_C`

3. **Modulación senoidal (SPWM):**
   - **Timer1 (3 kHz):** Actualiza los ciclos de trabajo 60 veces por ciclo de 50Hz
   - Usa una tabla precalculada con valores senoidales (0-100)
   - Desfases de 120° y 240° se logran con desplazamientos de 20 y 40 posiciones en la tabla

4. **Tabla senoidal:**
   - 60 valores que representan un ciclo completo
   - Valores entre 0 y 100 corresponden al ciclo de trabajo PWM
   - Calculada con la fórmula: `50 + 50 * sin(2π * i / 60)`

**Conexión física:**
1. Conecta los pines RC0, RC1, RC2 a las entradas de tu driver de motor (ej. inversor trifásico con IGBTs o MOSFETs)
2. Usa circuitos de aislamiento (optocopladores) entre el PIC y el driver
3. Alimenta el motor con una fuente adecuada a su voltaje nominal

**Consideraciones importantes:**
- **Frecuencias:** Ajusta los valores de recarga de los timers si cambias la frecuencia del cristal
- **Protecciones:** Implementa hardware de protección contra cortocircuitos
- **Dead-time:** Necesitarás circuitos externos o lógica adicional para generar dead-time
- **Arranque:** Puedes añadir una rampa de aceleración incrementando gradualmente la frecuencia

Este código genera señales SPWM trifásicas balanceadas que permiten controlar la velocidad y dirección de un motor de inducción trifásico. Para invertir el sentido de giro, intercambia dos de las fases (ej. fase B con fase C).
