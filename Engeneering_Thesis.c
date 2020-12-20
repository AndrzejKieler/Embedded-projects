/******************************************************/
/**/
/* Oprogramowanie sterownika temperatury*/
/**/
/*Na podstawie danych zczytywanych z czunjika temperatury
 *i zadanej za pomocą prostego interfejsu temperatury
 *program oblicza moc grzałki i reguluje niż załączając
 *przekaźnik na odpowiednie czasy zgodnie z metodą
 *grupowego sterowania mocą
 */
/*Biblioteki*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "LCD/lcd44780.h"
#include <stdlib.h>
#include <util/delay.h>
#include "DS18B20/ds18x20.h"
#include <math.h>

/* Dyrektywy określające przyciski*/
#define KEY1 (1<<PD2)// TAK
#define KEY2 (1<<PD1)// NIE
#define KEY3 (1<<PD0)// Gora
#define KEY4 (1<<PD3)// Dol

/* Dyrektywy operacji na grza³ce*/
#define RLY_OFF PORTB|= (1<<PB5)
#define RLY_ON PORTB&= ~(1<<PB5)

#define TP 2 // czas probkowania


/*Deklaracje funkcji*/
void display_temp(uint8_t x);   //wyświetla temperaturę na LCD
void buttons();                 //umożliwia zadanie temperatury
void measure_temp();            //mierzy lub zapisuje temperature
void burning();                 //zarządza procesem podgrzewania
int power();                    //liczy moc dla kolejnego okresu próbkowania
/**/
/*Zmienne*/
volatile bool temp_flag=0, power_flag,sign_flag=1;
volatile uint8_t timer=0, temp_cnt=0;
uint8_t czujniki_cnt;           //liczba czujników na magistrali
float tk=50,t;
uint8_t subzero, cel, cel_fract_bits;
/**/
// Przyciski
uint8_t key1_lock=0;
uint8_t key2_lock=0;
uint8_t key3_lock=0;
uint8_t key4_lock=0;
/**/
//Sterowanie mocą
double kp=1050, ki=0.185, td=0.4; //nastawy regulatora
float de, ie=0, e_0=0, e_1=0, u;
bool windup_flag=0, stop_flag=0;
volatile uint8_t n=200;
/**/

/* Procedura obsługi przerwania */
ISR(TIMER1_COMPA_vect)
{

			if(timer== 100 || timer==0) //początek sekundy
			{
				temp_flag=1;            // zezwolenie na pomiar/zapis temperatury

										// zapalenie zielonej diody gdy zadana temperatura osiągnięta
				if(round(t)==tk)
				{
					PORTC&=~(1<<PC5);
					PORTC |= (1<<PC5);
				}
				else
				{
					PORTC |= (1<<PC5);
					PORTC ^= (1<<PC4);
            }

            if(timer==0)
            {
                RLY_ON;             // załączanie przekaźnika
                timer=200;          //ustawianie timera na kolejne dwie sekundy
            }
        }

        if(timer==50)
            {
                power_flag=1;       //zezwolenie na obliczenie mocy
            }

        if(timer==n)RLY_OFF;        //wyłączenie przekaźnika

        timer--;                    //dekrementacja timera programowego
    }

/* Funkcja g³ówna */
int main(void)
{
    DDRB|= (1<<PB5);                // Ustawienie pinu wyjścia dla przekaźnika
    RLY_OFF;

    DDRC|=(1<<PC4)|(1<<PC5);        // Ustawienie pinów wyjścia dla diod
    PORTC|=(1<<PC5);

    // Ustawianie wejsc dla przyciskow
    DDRD&=~KEY1;
    DDRD&=~KEY2;
    DDRD&=~KEY3;
    DDRD&=~KEY4;
    PORTD|=KEY1|KEY2|KEY3|KEY4;     // pull-up

    TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10); // Timer1 w trybie CLC i ustawienie prescalera na 64
    OCR1A=1250;                     //max liczba do zliczania
    TIMSK1|=(1<<1);                 // urchomienie timera

    /* sprawdzanie ile czujników DS18xxx widocznych jest na magistrali */
    czujniki_cnt = search_sensors();

    lcd_init();                     //inicjalizacja wyświetlacza

    // Screen powitalny
    lcd_locate(0,3);
    lcd_str("Hello world");
    measure_temp();
    temp_cnt=1;
    _delay_ms(3000);
    lcd_cls();
    measure_temp();
    temp_cnt=0;

    while(1){

        lcd_locate(0,0);
        lcd_str("T: ");
        lcd_locate(1,0);
        lcd_str("Aim T:");
        lcd_locate(1,7);
        lcd_int((int)tk);
        lcd_str("C ");

        buttons();

}


    }


void measure_temp()
{

	if(!temp_cnt)DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );//pomiar temperatury
	else
	{
		//wczytanie i wyœwietlenie temperatury na trzeciej pozycji drugiego wiersza LCD
		if(DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits))
		{
			display_temp(3);
		}
		else {
			lcd_locate(1,0);
			lcd_str(" error "); //błąd odczytu lub brak czujnika
		}
	}
	if(temp_cnt++>0)temp_cnt=0;
	temp_flag=0;

}
void display_temp(uint8_t x) {
	lcd_locate(0,x);
	if(subzero) lcd_str("-");   // dla subzero==1 znak minus (temp. ujemna)
	else lcd_str(" ");          // dla subzero==0 spacja zamiast znaku minus (temp. dodatnia)
	lcd_int(cel);               //liczba stopni
	lcd_str(".");
	lcd_int(cel_fract_bits);    //dziesiętne części stopnia
	lcd_str("C ");
}
void buttons()
{
	if( !key3_lock && !(PIND & KEY3 ) )     //zwiększanie temperatury
		{
		   key3_lock=120;
			   tk++;
		}
		 else if( key3_lock && (PIND & KEY3 ) ) key3_lock--;


	if( !key4_lock && !(PIND & KEY4 ) )     //zmniejszanie temperatury
		{
			   key4_lock=120;
			   tk--;
		}
		else if( key4_lock && (PIND & KEY4 ) ) key4_lock--;

	if(!(PIND & KEY1 ))                     //potwierdzenie temperatury
		{
		   // Odblokowanie globalne przerwań
		   sei();
		   burning();
		}


}
int power()
{
	t=cel+cel_fract_bits/10;
	e_0=tk-t;

	// człon całkujący
	if(!windup_flag)ie+=((double)TP*ki/2)*((double)e_1+(double)e_0);
		else if(windup_flag)ie=0;

	// człon różniczkujący
	de=td*((double)e_0-(double)e_1)/(double)TP;

	//obliczanie mocy
	if(e_0>0)u=kp*(e_0+ie+de);
	else  u=kp*(e_0+de);

	//saturacja
	if(u>2400)
	{
		windup_flag=1;
		u=2400;
	}
	else if(u<0)
	{
		u=0;
	}
	else windup_flag=0;

	e_1=e_0;
	power_flag=0;

	//czas załączenia grzałki z dokładnością do 1/200 maksymalnej mocy
	return (200-round(u/12));
}
void burning()
{
        while(1)
        {
            if(!(PIND & KEY2 ))         // przycisk anulowania zadanego procesu
                {
                    cli();              //globalne wyłączenie przerwań
                    RLY_OFF;
                    lcd_cls();
                    lcd_locate(0,2);
                    lcd_str("New temp.");
                    temp_cnt=0;
                    measure_temp();     //pomiar temperatury
                    _delay_ms(1500);
                    ie=0;
                    e_1=0;
                    temp_cnt=1;
                    lcd_cls();
                    measure_temp();     //wczytanie temperatury
                    break;
                }

            if(temp_flag)measure_temp(); //pomiar lub wczytanie temperatury

            lcd_locate(0,0);
            lcd_str("T: ");
            lcd_locate(1,0);
            lcd_str("Aim T:");
            lcd_locate(1,7);
            lcd_int((int)tk);
            lcd_str("C ");

            if(power_flag)n=power();    // obliczanie mocy dla aktualnego uchybu

        }
    }

