#define F_CPU 16000000UL

#define DISPLAY_DIR DDRD
#define DISPLAY_PORT PORTD
#define DEC_POINT_PORT PORTD7

#define DISP_SELECTION_DIR DDRB
#define DISP_SELECTION_PORT PORTB


#include <avr/io.h>
#include<util/delay.h>
#include <stdio.h>
#include <glib.h>

int decPoint;

void float_to_int_array(float voltage, int *voltage_array, int *size) {
    char str[10];
    int i, j = 0;

    // Convert float to string
    sprintf(str, "%.2f", voltage);

    // Iterate over each character in the string
    for(i = 0; str[i] != '\0'; i++) {
        // If the character is a digit, convert it to an integer and add it to the array
        if(str[i] >= '0' && str[i] <= '9') {
            voltage_array[j] = str[i] - '0';
            j++;
        }

        else{
            decPoint = i;
        }
    }
    // Update the size of the array
    *size = j;
}

void int_array_to_hex(int *voltage_array, int size){
    
    if (decPoint == 1){                         //  If there is only 1 digit before decimal point,
        voltage_array[3] = voltage_array[2];    //  shift all digits to the right and add 0 at the start
        voltage_array[2] = voltage_array[1];    //  example 9.80 -> 09.80
        voltage_array[1] = voltage_array[0];
        voltage_array[0] = 0;
    }

    for(int i = 0; i < size; ++i){          
        switch (voltage_array[i]){         //convets digits to the equivalent hexadecimal values                                      
            case 0:                        //(specified in display documentation)
                voltage_array[i] = 0x7E;
            break;

            case 1:
                voltage_array[i] = 0x30;
            break;

            case 2:
                voltage_array[i] = 0x6D;
            break;

            case 3:
                voltage_array[i] = 0x79;
            break;

            case 4:
                voltage_array[i] = 0x33;
            break;

            case 5:
                voltage_array[i] = 0x5B;
            break;

            case 6:
                voltage_array[i] = 0x5F;
            break;

            case 7:
                voltage_array[i] = 0x70;
            break;
            
            case 8:
                voltage_array[i] = 0x7F;
            break;
            
            case 9:
                voltage_array[i] = 0x7B;
            break;  
        }
    }
}

void write(int voltage_array[4]){
    
    DISP_SELECTION_DIR = 0b00001111;
    DISPLAY_DIR = 0xff;
    
    for(int i = 0; i < 20; i++){

        DISP_SELECTION_PORT = ~0b00000001;
        DISPLAY_PORT = ~voltage_array[0];
        DISPLAY_PORT |= (1<<DEC_POINT_PORT);
        _delay_ms(5);

        DISP_SELECTION_PORT = ~0b00000010;
        DISPLAY_PORT = ~voltage_array[1];
        DISPLAY_PORT &= ~(1<<DEC_POINT_PORT);
        _delay_ms(5);
        
        DISP_SELECTION_PORT = ~0b00000100;
        DISPLAY_PORT = ~voltage_array[2];
        DISPLAY_PORT |= (1<<DEC_POINT_PORT);
        _delay_ms(5);

        DISP_SELECTION_PORT = ~0b00001000;
        DISPLAY_PORT = ~voltage_array[3];
        DISPLAY_PORT |= (1<<DEC_POINT_PORT);
        _delay_ms(5);
    }
    
}

void display(float voltage){
    
    int voltage_array[4];
    int size;
    float_to_int_array(voltage, voltage_array, &size);
    int_array_to_hex(voltage_array, 4);
    write(voltage_array);
}

void ADC_init(void){

   ADMUX |= (1 << REFS0) | (1 << MUX0);   //AVCC equal to VCC and ADC1 input
   ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

float read_voltage(float R1, float R2, float Vref){
    // R1 and R2 form a voltage divider.
    // R1 is the smaller one!!!!!!!!!!!!!!!
    // Enter their values in the same order of magnitude
    // Vref is the reference voltage in volts

    float voltage = (((float)ADC) * Vref / 1024.0) * (R1 + R2) / R1;

    return voltage;
}

int main(void){
    
    float voltage;
    
    ADC_init();
    _delay_ms(10);
    
    while(1){
        voltage = read_voltage(2.2, 10.0, 5.0);
        display(voltage);
    }  
}
