#define F_CPU 16000000UL

#define DISPLAY_DIR DDRD
#define DISPLAY_PORT PORTD
#define DEC_POINT_PORT PORTD7

#define DISP_SELECTION_DIR DDRB
#define DISP_SELECTION_PORT PORTB


#include <avr/io.h>
#include<util/delay.h>
#include <stdio.h>

int decPoint;

void float_to_int_array(float voltage_reading, int *voltage, int *size) {
    char str[10];
    int i, j = 0;

    // Convert float to string
    sprintf(str, "%.2f", voltage_reading);

    // Iterate over each character in the string
    for(i = 0; str[i] != '\0'; i++) {
        // If the character is a digit, convert it to an integer and add it to the array
        if(str[i] >= '0' && str[i] <= '9') {
            voltage[j] = str[i] - '0';
            j++;
        }

        else{
            decPoint = i;
        }
    }
    // Update the size of the array
    *size = j;
}

void int_array_to_hex(int *voltage, int size){
    
    if (decPoint == 1){             //  If there is only 1 digit before decimal, 
        voltage[3] = voltage[2];    //  point, shift all digits to the right and add 0 at the start
        voltage[2] = voltage[1];    //  example 9.80 -> 09.80
        voltage[1] = voltage[0];
        voltage[0] = 0;
    }

    for(int i = 0; i < size; ++i){  //convets digits to the equivalent hexadecimal values
                                    //(specified in display documentation)
        if (voltage[i] == 0){
            voltage[i] = 0x7E;          
        }
        
        else if (voltage[i] == 1){
            voltage[i] = 0x30;
        }

        else if (voltage[i] == 2){
            voltage[i] = 0x6D;
        }

        else if (voltage[i] == 3){
            voltage[i] = 0x79;
        }

        else if (voltage[i] == 4){
            voltage[i] = 0x33;
        }

        else if (voltage[i] == 5){
            voltage[i] = 0x5B;
        }

        else if (voltage[i] == 6){
            voltage[i] = 0x5F;
        }

        else if (voltage[i] == 7){
            voltage[i] = 0x70;
        }

        else if (voltage[i] == 8){
            voltage[i] = 0x7F;
        }

        else if (voltage[i] == 9){
            voltage[i] = 0x7B;
        }
    }
}

void write(int *voltage){
    
    DISP_SELECTION_DIR = 0b00001111;
    DISPLAY_DIR = 0xff;
    
    for(int i = 0; i < 20; i++){

        DISP_SELECTION_PORT = ~0b00000001;
        DISPLAY_PORT = ~voltage[0];
        DISPLAY_PORT |= (1<<DEC_POINT_PORT);
        _delay_ms(5);

        DISP_SELECTION_PORT = ~0b00000010;
        DISPLAY_PORT = ~voltage[1];
        DISPLAY_PORT &= ~(1<<DEC_POINT_PORT);
        _delay_ms(5);
        
        DISP_SELECTION_PORT = ~0b00000100;
        DISPLAY_PORT = ~voltage[2];
        DISPLAY_PORT |= (1<<DEC_POINT_PORT);
        _delay_ms(5);

        DISP_SELECTION_PORT = ~0b00001000;
        DISPLAY_PORT = ~voltage[3];
        DISPLAY_PORT |= (1<<DEC_POINT_PORT);
        _delay_ms(5);
    }
    
}

void display(float voltage_reading){
    
    int voltage[4];
    int size;
    float_to_int_array(voltage_reading, voltage, &size);
    int_array_to_hex(voltage, 4);
    write(voltage);
}

void ADC_init(void)
{
   ADMUX |= (1 << REFS0) | (1 << MUX0);   //AVCC equal to VCC and ADC1 input
   ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

int main(void){
    float voltage_reading;
    
    ADC_init();
    _delay_ms(10);
    
    while(1){
        voltage_reading = (((float)ADC)*5.0/1024.0)*12.2/2.2;
        display(voltage_reading);
    }
    
}
