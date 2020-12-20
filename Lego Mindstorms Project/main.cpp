 
 
#include <hFramework.h>
#include <DistanceSensor.h>
#include <Lego_Touch.h>
#include <cmath>
#include <stdio.h>

using namespace hSensors;
using namespace hModules;

short parleft=0, parright=0;                              //wybor parkowania z lewej, prawej
bool state1=0, state2=0, przycisk, parkstart=0, kalibracja=0;         // statusy wci�ni�tych przycisk�w, pocz�tku szukania miejsca
bool przycisk_state=0, alarm=0;                         // statusy przycisku, alarmu
float naped_przelozenie = 0.0133333, distzero=0, distjeden=0;
       // cm/tick, odleglosc przed parkowaniem
short szerokosc, glebokosc, odl_krytyczna=6, strona;
// status czy ju� prakuje, czy jest w trakcie sprawdzania czy jest odpowiednia luka mi�dzy samochodami
bool parkowanie = 0, miejsce=0;  


DistanceSensor sensleft(hSens1);
DistanceSensor sensright(hSens4);
DistanceSensor sensback(hSens6);
DistanceSensor sensfront(hSens3);


void start() //DONE
{
	sys.setLogDev(&Serial);
	hLegoSensor_simple ls(hSens5);
	Lego_Touch sensor(ls);
	hLED1.off();
	hLED2.off();
	hLED3.off();
	do
	{
		if(hCfg.isPressed()){ kalibracja=1; hExt.pin1.write(1); }      
		       // funkcja wybierania parkowania z prawej strony
		if(!state1&&hBtn2.isPressed())                   // funkcja wybierania parkowania z prawej strony
		{
			state1=1;
			parright++;

			if(parright==3)
			{
				parright=0;
			}
			sys.delay(20);
		}

		if(!state2&&hBtn1.isPressed())                   // funkcja wybierania parkowania z lewej strony
		{
			state2=1;
			parleft++;

			if(parleft==3)
			{
				parleft=0;
			}
			sys.delay(20);
		}

		if(state1&&!hBtn2.isPressed())
		{
				state1=0;
				sys.delay(20);
		}
		if(state2&&!hBtn1.isPressed())
		{
				state2=0;
				sys.delay(20);
		}
		
		// przycisk potwierdzaj�cy wyb�r i rozpoczynaj�cy szukanie miejsca
		przycisk = sensor.isPressed();                  
		if(!przycisk_state&&przycisk)
		{
			parkstart=1;
			przycisk_state=1;
		}
		if(parleft) hLED3.on();
			else hLED3.off();
		if(parright) hLED1.on();
			else hLED1.off();
		if(parleft==2 ||parright==2) hLED2.on();
			else hLED2.off();		
	}
	while(!parkstart);
	sys.delay(20);

	 switch(parright)
        {
            case 1: szerokosc = 28;
                    glebokosc = 35;
					strona=-1;
                    break;
            case 2: szerokosc = 54;
                    glebokosc = 28;
					strona=-1;
					break;
        }
        if(strona!=-1)
		{	
			switch(parleft)
			{
				case 1: szerokosc = 28;
						glebokosc = 35;
						strona=1;
						break;
				case 2: szerokosc = 54;
						glebokosc = 28;
						strona=1;
						break;
			}
		}
	printf("%d\r\n", parright);
	printf("%d\r\n", parleft);
}

void parkowanie_prostopadle() // DONE 
{
	int i = 0;
	sys.setLogDev(&Serial);
	hLegoSensor_simple ls(hSens5);
	Lego_Touch sensor(ls);
	
	hExt.pin3.write(0);
	hExt.pin4.write(1);
	
	hMot3.setEncoderPolarity(Polarity::Reversed);  //changing encoder polarity (Polarity::Normal is default)
	hMot3.setMotorPolarity(Polarity::Normal); //changing motor polarity
	hMot4.setEncoderPolarity(Polarity::Reversed);  //changing encoder polarity (Polarity::Normal is default)	
	hMot4.setMotorPolarity(Polarity::Normal);
	
	for(i=35;i>0;i--)
	{
		while(alarm);
		hMot4.rotRel(-100*strona, 900, true, INFINITE);
	}

	for(i=18;i>0;i--)
	{	
		while(alarm);
		hMot3.rotRel(100, 370, true, INFINITE);
	}
	
	for(i=72;i>0;i--)
	{
		while(alarm); 
		hMot4.rotRel(100*strona, 900, true, INFINITE);
	}

	for(i=29;i>0;i--)
	{
		while(alarm);
		hMot3.rotRel(-100, 370, true, INFINITE);
	}
	for(i=40;i>0;i--)
	{
		while(alarm);
		hMot4.rotRel(-100*strona, 900, true, INFINITE);
	}

	for(i=18;i>0;i--)
	{
		while(alarm);
		hMot3.rotRel(-100, 370, true, INFINITE);
	}
	for(i=50;i>0;i--)
	{
		while(alarm);
		hMot3.rotRel(-distzero/50/naped_przelozenie, 370, true, INFINITE);
	}
	
	

	while(!sensor.isPressed()); // wyjazd z parkingu
	
	
	
	for(i=50;i>0;i--)
	{
		while(alarm);
		hMot3.rotRel(distzero/50/naped_przelozenie, 370, true, INFINITE);
	}
	for(i=18;i>0;i--)
	{
		while(alarm);
		hMot3.rotRel(100, 370, true, INFINITE);
	}
	for(i=40;i>0;i--)
	{
		while(alarm);
		hMot4.rotRel(100*strona, 900, true, INFINITE);
	}
	for(i=29;i>0;i--)
	{
		while(alarm);
		hMot3.rotRel(100, 370, true, INFINITE);
	}
		for(i=72;i>0;i--)
	{
		while(alarm); 
		hMot4.rotRel(-100*strona, 900, true, INFINITE);
	}
	for(i=18;i>0;i--)
	{	
		while(alarm);
		hMot3.rotRel(-100, 370, true, INFINITE);
	}
	for(i=39;i>0;i--)
	{
		while(alarm);
		hMot4.rotRel(100*strona, 900, true, INFINITE);
	}
	
	hExt.pin4.write(0);

	

}

void parkowanie_rownolegle() // DONE 
{
	int i = 0;
	sys.setLogDev(&Serial);
	hLegoSensor_simple ls(hSens5);
	Lego_Touch sensor(ls);
	
	hExt.pin3.write(0);
	hExt.pin4.write(1);

	hMot3.setEncoderPolarity(Polarity::Reversed);  //changing encoder polarity (Polarity::Normal is default)
	hMot3.setMotorPolarity(Polarity::Normal); //changing motor polarity
	hMot4.setEncoderPolarity(Polarity::Reversed);  //changing encoder polarity (Polarity::Normal is default)	
	hMot4.setMotorPolarity(Polarity::Normal);
	
	for(i=21+0.6*distzero;i>0;i--)					// trochę do przodu
	{
		while(alarm);
		hMot3.rotRel(100, 370, true, INFINITE);
	}
	for(i=35;i>0;i--)					// w lewo do oporu
	{
		while(alarm);
		hMot4.rotRel(100*strona, 900, true, INFINITE);
	}
	for(i=23+0.6*distzero;i>0;i--)					//do tylu
	{	
		while(alarm);
		hMot3.rotRel(-100, 370, true, INFINITE);
	}
	for(i=72;i>0;i--)					// w prawo do oporu
	{
		while(alarm);
		hMot4.rotRel(-100*strona, 900, true, INFINITE);
	}
	for(i=23+0.6*distzero;i>0;i--)					//do tylu
	{	
		while(alarm);
		hMot3.rotRel(-100, 370, true, INFINITE);
	}
		for(i=39;i>0;i--)					// w lewo na srodek
	{
		while(alarm);
		hMot4.rotRel(100*strona, 900, true, INFINITE);
	}
	for(i=7;i>0;i--)					// trochę do przodu
	{
		while(alarm);
		hMot3.rotRel(100, 370, true, INFINITE);
	}


	
	while(!sensor.isPressed()); // wyjazd z parkingu


	
	for(i=7;i>0;i--)					// trochę do przodu
	{
		while(alarm);
		hMot3.rotRel(-100, 370, true, INFINITE);
	}
	for(i=39;i>0;i--)					// w lewo na srodek
	{
		while(alarm);
		hMot4.rotRel(-100*strona, 900, true, INFINITE);
	}
	for(i=23+0.6*distzero;i>0;i--)					//do tylu
	{	
		while(alarm);
		hMot3.rotRel(100, 370, true, INFINITE);
	}
	for(i=72;i>0;i--)					// w prawo do oporu
	{
		while(alarm);
		hMot4.rotRel(100*strona, 900, true, INFINITE);
	}
	for(i=23+0.6*distzero;i>0;i--)					//do tylu
	{	
		while(alarm);
		hMot3.rotRel(100, 370, true, INFINITE);
	}
	for(i=35;i>0;i--)					// w lewo do oporu
	{
		while(alarm);
		hMot4.rotRel(-100*strona, 900, true, INFINITE);
	}
	
	hExt.pin4.write(0);



}

void parkuj() //DONE
{   
	switch(parleft)
	{
		case 1:	sys.taskCreate(parkowanie_prostopadle);
				break;
		case 2: sys.taskCreate(&parkowanie_rownolegle);
				break;
	}
	if(strona==-1)	
	{
		switch(parright)
		{
			case 1:	sys.taskCreate(parkowanie_prostopadle);
					break;
			case 2: sys.taskCreate(&parkowanie_rownolegle);
					break;
		}
	}
					
}

void encoder()//DONE
{
	while (true)
	{
		Serial.printf("pos: %d\r\n", hMot3.getEncoderCnt()); 
		hLED1.toggle();
		sys.delay(1000);
	}
}

void encoder2()//DONE
{
	while (true)
	{
		Serial.printf("pos2: %d\r\n", hMot4.getEncoderCnt());
		hLED1.toggle();
		sys.delay(1000);
	}
}

void korekcja1()
{	
    // kolejno: zmienna pomocnicza odleg�o�ci pocz�tkowej, odleg�o�ci z lewej strony, r��nica odleg�o�ci (pochodna)
	float pocz=0, leftdist2=0, leftdist1=0, leftdist3=0,leftdist0=0, leftdiv=0;     
	bool poczatek_miejsca=0;
	hMot4.setEncoderPolarity(Polarity::Reversed);  
	hMot4.setMotorPolarity(Polarity::Normal);
		
	for(short i=10;i>=0;i--)                              // pomiar pocz�tkowej odleg�o�ci auta
	{			
				pocz=sensleft.getDistance();
				Serial.printf("Czujnik korekcja poczatek: %f\r\n", pocz);
				if(pocz==-1)
					{
						i++;
						continue;
					}
				leftdist1 = leftdist1 + pocz;
		
	}
	
	leftdist1=leftdist1/11; 
	leftdist0 = leftdist1;
	leftdist3 = leftdist1;

	for(;;)
	{	if(alarm)continue;                     // je�eli jest alarm lub sprawdzana luka to nie koryguje toru
		if(parkowanie)break; 
		
		if(miejsce && !poczatek_miejsca)
		{	
			leftdist1=0;
			for(short i=10;i>=0;i--)                              // pomiar pocz�tkowej odleg�o�ci auta
			{			
				pocz=sensleft.getDistance();
				Serial.printf("Czujnik korekcja miejsce: %f\r\n", pocz);
				if(pocz==-1)
					{
						i++;
						continue;
					}
				leftdist1 = leftdist1 + pocz;
			}

			poczatek_miejsca=1;
			leftdist1 = leftdist1/11; 
			leftdist3 = leftdist1;
			continue;
		}
		
		if(!miejsce && poczatek_miejsca)
		{
			leftdist1 = leftdist0;
			leftdist3 = leftdist1;
			poczatek_miejsca=0;
		}
		                            
		leftdist2=0;

		for(short i=6;i>=0;i--)
		{			
				pocz=sensleft.getDistance();
				
				// odrzucanie wynik�w -1 i zbyt r��ni�cych si� od ostatnich odleg�o�ci
				if(pocz==-1 || (i<5 && abs(pocz-leftdist2/(6-i))>7) || (i>4 && abs(pocz-leftdist3)>7)) 
					{
						i++;
						continue;
					}
				
				Serial.printf("Czujnik korekcja: %f\r\n", pocz);
				leftdist2 = leftdist2 + pocz;
		}
		if(parkowanie)break;      
	
		leftdist2=leftdist2/7;
		if(!miejsce)distjeden=leftdist2;
		leftdist3=leftdist2;
		leftdiv = (leftdist2 - leftdist1);                  // wylicza odchy�k� dystansu
		if(abs(leftdiv)>8 || (int)leftdiv==0)continue;                         // nie przyjmuje zbyt du�ych odchy�ek
		
		
		//Serial.printf("Div: %f\r\n", leftdiv);
		
		
		hMot4.rotRel((350+600/abs((int)leftdiv))*(int)leftdiv,1000, true, INFINITE);
		sys.delay(460);                                     // czas skr�cania
		hMot4.rotRel(-(350+600/abs((int)leftdiv))*(int)leftdiv, 1000, true, INFINITE);
			

	}
}
void korekcja2()
{	
    // kolejno: zmienna pomocnicza odleg�o�ci pocz�tkowej, odleg�o�ci z lewej strony, r��nica odleg�o�ci (pochodna)
	float pocz=0, leftdist2=0, leftdist1=0, leftdist3=0,leftdist0=0, leftdiv=0;     
	bool poczatek_miejsca=0;
	hMot4.setEncoderPolarity(Polarity::Reversed);  
	hMot4.setMotorPolarity(Polarity::Normal);
		
	for(short i=10;i>=0;i--)                              // pomiar pocz�tkowej odleg�o�ci auta
	{			
				pocz=sensright.getDistance();
				Serial.printf("Czujnik korekcja poczatek: %f\r\n", pocz);
				if(pocz==-1)
					{
						i++;
						continue;
					}
				leftdist1 = leftdist1 + pocz;
	}
	
	leftdist1=leftdist1/11; 
	leftdist0 = leftdist1;
	leftdist3 = leftdist1;

	for(;;)
	{	if(alarm)continue;                     // je�eli jest alarm lub sprawdzana luka to nie koryguje toru
		if(parkowanie)break; 
		
		if(miejsce && !poczatek_miejsca)
		{	
			leftdist1=0;
			for(short i=10;i>=0;i--)                              // pomiar pocz�tkowej odleg�o�ci auta
			{			
				pocz=sensright.getDistance();
				Serial.printf("Czujnik korekcja miejsce: %f\r\n", pocz);
				if(pocz==-1)
					{
						i++;
						continue;
					}
				leftdist1 = leftdist1 + pocz;
			}

			poczatek_miejsca=1;
			leftdist1 = leftdist1/11; 
			leftdist3 = leftdist1;
			continue;
		}
		
		if(!miejsce && poczatek_miejsca)
		{
			leftdist1 = leftdist0;
			leftdist3 = leftdist1;
			poczatek_miejsca=0;
		}
		                            
		leftdist2=0;

		for(short i=6;i>=0;i--)
		{			
				pocz=sensright.getDistance();
				
				// odrzucanie wynik�w -1 i zbyt r��ni�cych si� od ostatnich odleg�o�ci
				if(pocz==-1 || (i<5 && abs(pocz-leftdist2/(6-i))>7) || (i>4 && abs(pocz-leftdist3)>7)) 
					{
						i++;
						continue;
					}
				
				Serial.printf("Czujnik korekcja: %f\r\n", pocz);
				leftdist2 = leftdist2 + pocz;
		}
		if(parkowanie)break;      
	
		leftdist2=leftdist2/7;
		if(!miejsce)distjeden=leftdist2;
		leftdist3=leftdist2;
		leftdiv = (leftdist2 - leftdist1);                  // wylicza odchy�k� dystansu
		if(abs(leftdiv)>8 || (int)leftdiv==0)continue;                         // nie przyjmuje zbyt du�ych odchy�ek
		
		
		//Serial.printf("Div: %f\r\n", leftdiv);
		
			
		hMot4.rotRel((350+600/abs((int)leftdiv))*(int)leftdiv*strona,1000, true, INFINITE);
		sys.delay(460);                                     // czas skr�cania
		hMot4.rotRel(-(350+600/abs((int)leftdiv))*(int)leftdiv*strona, 1000, true, INFINITE);
			

	}
}

void szukanie_miejsca1()
{	
	
	hMot3.setEncoderPolarity(Polarity::Reversed);  
	hMot3.setMotorPolarity(Polarity::Normal); 
	
	float leftdist2=0, leftdist1=0, leftdist3=0, leftdiv=0, pocz;
	
	//warto�� enkodera gdy znalaz� luk� mi�dzy samochodami, iterator pomiaru niesko�czono�ci -1
	short encoder_zero=0,  j=0; 
	bool nieskonczonosc=0;                  // flaga wskazania odleg�o�ci niesko�czonej

	hMot3.resetEncoderCnt();
	
	leftdist1 = sensleft.getDistance();     // dystans pocz�tkowy
		
		
		
		for(;;)
			{
				leftdist1 = sensleft.getDistance();
				if(leftdist1!=-1)break;
			}
        
       
       
					
	for(;;)
	{	if(alarm)continue;                  // hamuje gdy jest alarm
		j=0;
		hMot3.setPower(300);

		
	//rozpatrywanie, czy niesko�czono�� jest przypadkowa, je�eli nie to stawia flag� niesko�czono�ci
			leftdist2=0;
			for(short i=2;i>=0;i--)
			{		
						
					pocz=sensleft.getDistance();
					if(pocz==-1)
					{	
						Serial.printf("Sz. msc Nieskonczonosc !!! \n");
						i++;
						continue;
					}
							
							/*if(pocz==-1)
							{
								while(pocz==-1 || pocz-leftdist1>40)
								{ 	pocz= sensleft.getDistance();
									Serial.printf("Czujnik petla nieskonczonosci: %f\r\n", leftdist2);
									j++;
									if(j==3){ nieskonczonosc=1; break; }	
								}
							}
							if(nieskonczonosc) break;*/
					

					Serial.printf("Czujnik : %f\r\n", pocz);
					leftdist2 = leftdist2 + pocz;
			}
			
	leftdist2 = leftdist2/3; 		
	
	
	if(!nieskonczonosc)leftdiv = (leftdist2 - leftdist1); // oblicza skok odleg�o�ci od przeszkody
	
	if ( !miejsce ) // procedury gdy nie znalaz� luki
	{
		hExt.pin3.write(0);

			if(!nieskonczonosc  && ( leftdiv>glebokosc || leftdist2-distjeden>glebokosc || leftdist2-leftdist3>glebokosc )) // czy jest skok odleg�o�ci od przeszkody o 35cm
		{	Serial.printf("Jestem w sprawdzaniu miejsca 1");
			encoder_zero = hMot3.getEncoderCnt();
			//Serial.printf("%d\r\n", encoder_zero);
			//hMot3.resetEncoderCnt();
			distzero = distjeden;								// BYLO leftdist1
			miejsce=1;
			leftdist1=leftdist2;
			continue;
		}

		if (nieskonczonosc ) // czy jest niesko�czono��
		{
			Serial.printf("Jestem w sprawdzaniu miejsca 2");
			encoder_zero = hMot3.getEncoderCnt();
			//Serial.printf("%d\r\n", encoder_zero);
			//hMot3.resetEncoderCnt();
			distzero = distjeden;                     // BYLO leftdist1
			miejsce=1;
			nieskonczonosc=0;
			continue;
		}
	}

	if(miejsce) // procedury gdy znalaz� luk� i musi sprawdzi� czy jest wystarczaj�co d�uga
	{
		hExt.pin3.write(1);
	    	//czy po 25 cm jest nadal niesko�czono��
			if(nieskonczonosc && (hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie>szerokosc ) /// wczesniej 25
			{
				Serial.printf("szer miejsca = %f", (hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie);
				Serial.printf("Jestem w sprawdzaniu miejsca 3");
				hMot3.stop();
				odl_krytyczna=3;
				sys.taskCreate(&parkuj);
				parkowanie = 1;
				break;
			}
			
			else if(nieskonczonosc)
			{
				nieskonczonosc=0; 
				Serial.printf("Jestem w sprawdzaniu miejsca 4");
				continue;
			}
			
			//czy po 25cm jest nadal przeszkoda wi�ksza ni� 35 od pocz�tkowej odleg�o�ci
			else if((hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie>szerokosc && leftdist2-distzero>glebokosc )
			{
				Serial.printf("szer miejsca = %f", (hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie);
				Serial.printf("Jestem w sprawdzaniu miejsca 5");
				hMot3.stop();
				parkowanie = 1;
				odl_krytyczna=3;
				sys.taskCreate(&parkuj);
				break;
			}
			
			// czy nast�pi� skok o 35 cm w drug� stron�
			else if(-leftdiv>glebokosc || leftdist3-leftdist2>glebokosc)
			{	
				Serial.printf("szer miejsca3 = %f", (hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie);
				miejsce=0;
				if((hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie>szerokosc)
				{
					Serial.printf("Jestem w sprawdzaniu miejsca 6");
					hMot3.stop();
					parkowanie = 1;
					odl_krytyczna=3;
					sys.taskCreate(&parkuj);
					break;
				}

			}
	}

		if(!nieskonczonosc){ leftdist3=leftdist1;	leftdist1=leftdist2; }
		sys.delay(20);
		
	}
}

void szukanie_miejsca2()
{	
	
	hMot3.setEncoderPolarity(Polarity::Reversed);  
	hMot3.setMotorPolarity(Polarity::Normal); 
	
	float leftdist2=0, leftdist1=0, leftdist3=0, leftdiv=0, pocz;
	
	//warto�� enkodera gdy znalaz� luk� mi�dzy samochodami, iterator pomiaru niesko�czono�ci -1
	short encoder_zero=0,  j=0; 
	bool nieskonczonosc=0;                  // flaga wskazania odleg�o�ci niesko�czonej

	hMot3.resetEncoderCnt();
	
	leftdist1 = sensright.getDistance();     // dystans pocz�tkowy
		
		
		
		for(;;)
			{
				leftdist1 = sensright.getDistance();
				if(leftdist1!=-1)break;
			}
        
       
       
					
	for(;;)
	{	if(alarm)continue;                  // hamuje gdy jest alarm
		j=0;
		hMot3.setPower(300);

		
	//rozpatrywanie, czy niesko�czono�� jest przypadkowa, je�eli nie to stawia flag� niesko�czono�ci
			leftdist2=0;
			for(short i=2;i>=0;i--)
			{		
						
					pocz=sensright.getDistance();
					if(pocz==-1)
					{	
						Serial.printf("Sz. msc Nieskonczonosc !!! \n");
						i++;
						continue;
					}
							
							/*if(pocz==-1)
							{
								while(pocz==-1 || pocz-leftdist1>40)
								{ 	pocz= sensleft.getDistance();
									Serial.printf("Czujnik petla nieskonczonosci: %f\r\n", leftdist2);
									j++;
									if(j==3){ nieskonczonosc=1; break; }	
								}
							}
							if(nieskonczonosc) break;*/
					

					Serial.printf("Czujnik : %f\r\n", pocz);
					leftdist2 = leftdist2 + pocz;
			}
			
	leftdist2 = leftdist2/3; 		
	
	
	if(!nieskonczonosc)leftdiv = (leftdist2 - leftdist1); // oblicza skok odleg�o�ci od przeszkody
	
	if ( !miejsce ) // procedury gdy nie znalaz� luki
	{
		hExt.pin3.write(0);

			if(!nieskonczonosc  && ( leftdiv>glebokosc || leftdist2-distjeden>glebokosc || leftdist2-leftdist3>glebokosc )) // czy jest skok odleg�o�ci od przeszkody o 35cm
		{	Serial.printf("Jestem w sprawdzaniu miejsca 1");
			encoder_zero = hMot3.getEncoderCnt();
			//Serial.printf("%d\r\n", encoder_zero);
			//hMot3.resetEncoderCnt();
			distzero = distjeden;								// BYLO leftdist1
			miejsce=1;
			leftdist1=leftdist2;
			continue;
		}

		if (nieskonczonosc ) // czy jest niesko�czono��
		{
			Serial.printf("Jestem w sprawdzaniu miejsca 2");
			encoder_zero = hMot3.getEncoderCnt();
			//Serial.printf("%d\r\n", encoder_zero);
			//hMot3.resetEncoderCnt();
			distzero = distjeden;                     // BYLO leftdist1
			miejsce=1;
			nieskonczonosc=0;
			continue;
		}
	}

	if(miejsce) // procedury gdy znalaz� luk� i musi sprawdzi� czy jest wystarczaj�co d�uga
	{
		hExt.pin3.write(1);
	    	//czy po 25 cm jest nadal niesko�czono��
			if(nieskonczonosc && (hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie>szerokosc ) /// wczesniej 25
			{
				Serial.printf("szer miejsca = %f", (hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie);
				Serial.printf("Jestem w sprawdzaniu miejsca 3");
				hMot3.stop();
				odl_krytyczna=3;
				sys.taskCreate(&parkuj);
				parkowanie = 1;
				break;
			}
			
			else if(nieskonczonosc)
			{
				nieskonczonosc=0; 
				Serial.printf("Jestem w sprawdzaniu miejsca 4");
				continue;
			}
			
			//czy po 25cm jest nadal przeszkoda wi�ksza ni� 35 od pocz�tkowej odleg�o�ci
			else if((hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie>szerokosc && leftdist2-distzero>glebokosc )
			{
				Serial.printf("szer miejsca = %f", (hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie);
				Serial.printf("Jestem w sprawdzaniu miejsca 5");
				hMot3.stop();
				parkowanie = 1;
				odl_krytyczna=3;
				sys.taskCreate(&parkuj);
				break;
			}
			
			// czy nast�pi� skok o 35 cm w drug� stron�
			else if(-leftdiv>glebokosc || leftdist3-leftdist2>glebokosc)
			{	
				Serial.printf("szer miejsca3 = %f", (hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie);
				miejsce=0;
				if((hMot3.getEncoderCnt()-encoder_zero)*naped_przelozenie>szerokosc)
				{
					Serial.printf("Jestem w sprawdzaniu miejsca 6");
					hMot3.stop();
					parkowanie = 1;
					odl_krytyczna=3;
					sys.taskCreate(&parkuj);
					break;
				}

			}
	}

		if(!nieskonczonosc){ leftdist3=leftdist1;	leftdist1=leftdist2; }
		sys.delay(20);
		
	}
}

void awaryjne_hamowanie()
{
	sys.setLogDev(&Serial);
	hLegoSensor_simple ls(hSens5);
	Lego_Touch sensor(ls);
	
    // odleg�o�� z ty�u i przodu
	int backdist=0, frontdist=0; 
	
	// flagi oznaczaj�ce wci�ni�cie przycisku, kranc�wek; flagi oznaczaj�ce pochodzenie alarmu
	bool przycisk = 0, krancowka = 0, alarmprzycisk=0, alarmsens=0, alarmkrancowka=0;
	hExt.pin5.setIn_pd();
	sys.delay(20);

	while(1)
	{
		backdist= sensback.getDistance();
		frontdist= sensfront.getDistance();
		
		
		if(!alarm)                                                                  //w��czanie alarm�w
		{
			
			if(!krancowka && !hExt.pin5.read())                                     // alarm od kra�c�wek
			{
				krancowka=1;
				hMot3.stop();
				hMot4.stop();
				alarm=1;
				alarmkrancowka=1;
				sys.delay(20);
			}
			if( ( (backdist<odl_krytyczna ||frontdist<odl_krytyczna ) && backdist!=-1 && frontdist!=-1))    // alarm od czujnik�w odleg�osci
				{
					hMot3.stop();
					hMot4.stop();
					alarm=1;
					alarmsens=1;
				}
			}
		else if(alarm)                                                              // wy��czanie alarmu
		{
			hExt.pin2.write(1); 
			
			if(hExt.pin5.read() && krancowka && !alarmprzycisk && !alarmsens)
			{
				alarm=0;
				krancowka=0;
				alarmkrancowka=0;
				hExt.pin2.write(0); 
			}
	
			if( (backdist>odl_krytyczna && frontdist>odl_krytyczna ) && !alarmprzycisk && !alarmkrancowka)
			{
				alarm=0;
				alarmsens=0;
				hExt.pin2.write(0); 
			}
		}
		
		
		przycisk = sensor.isPressed();
		if(przycisk_state&& !przycisk)
		{
			sys.delay(20);
			przycisk_state=0;		
		}
		if(!przycisk_state && przycisk  && !alarmkrancowka  && !alarmsens)          // alarm od przycisku
		{
			przycisk_state=1;
			hMot3.stop();
			hMot4.stop();
			alarm^=1;
			if(!alarm)hExt.pin2.write(0); 
			alarmprzycisk^=1;
			sys.delay(20);
		}
	}
}


void hMain()
	{	
		hMot4.setEncoderPolarity(Polarity::Reversed);  	
		hMot4.setMotorPolarity(Polarity::Normal); 
		hExt.pin1.setOut();
		hExt.pin1.write(0); 
		hExt.pin2.setOut();
		hExt.pin2.write(0);  
		hExt.pin3.setOut();
		hExt.pin3.write(0);  
		hExt.pin4.setOut();
		hExt.pin4.write(0);  
		// pozycjonowanie k��
		

		start(); // ustalanie sposobu parkowania
		if(kalibracja)
		{
			hMot4.rotRel(7000, 800, true, INFINITE); // do oporu w prawo (a� zadzia�a sprz�g�o przeci��eniowe)
			hMot4.rotRel(-720*6.8, 800, true, INFINITE); // do po�owy w lewo
			hExt.pin1.write(0); 
		}
		 // alarmy od czujnik�w odleg�o�ci, kra�c�wek i przycisku
		sys.taskCreate(&awaryjne_hamowanie,3);
		
		// korygowanie toru w przypadku kr�tej drogi
		if(strona==1)
		{
			sys.taskCreate(&korekcja1); 
			sys.taskCreate(&szukanie_miejsca1);
		}
		if(strona==-1)
		{
			sys.taskCreate(&korekcja2); 
			sys.taskCreate(&szukanie_miejsca2);

		}


		while(true)
		{
			
		
		}

	} 
 
