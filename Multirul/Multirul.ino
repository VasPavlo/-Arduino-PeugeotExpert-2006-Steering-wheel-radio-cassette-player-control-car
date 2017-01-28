// Author: Pavlo Vasylykiv, Email:grubdmail@gmail.com


#include <IRremote.h>

#define DELTA   0.01   // Погрешность в измерении приходящего сигнала, если показания скачут более чем на 5 едениц, то можно увеличить
#define ALLDOWN  447 //Все кнопки отпущены

#define BTN_VOLPLUS  buttonCode[17] 
#define BTN_VOLMINUS  buttonCode[15] 
#define BTN_NEXT  buttonCode[13] 
#define BTN_BACK  buttonCode[19]
#define BTN_MUTE  buttonCode[2]
#define BTN_MODE  buttonCode[1]

#define BTN_MEMONEXT  buttonCode[3]
#define BTN_MEMOBACK  buttonCode[18]

#define BTN_MEMONEXT_AND_BTN_VOLPLUS   buttonCode[12]
#define BTN_MEMONEXT_AND_BTN_VOLMINUS   buttonCode[13]

#define BTN_MEMONEXT_AND_BTN_NEXT   buttonCode[10]
#define BTN_MEMONEXT_AND_BTN_BACK   buttonCode[14]

//Multirul  resistance by pressing buttons
#define V_VOLPLUS  0.13
#define V_VOLMINUS  0.00

#define V_NEXT  0.28
#define V_BACK  0.55

#define V_MUTE  0.55
#define V_MODE  0.00

#define V_MEMONEXT  0.28
#define V_MEMOBACK  0.13
//----------------------------------------

IRsend irsend;

unsigned long buttonCode[] = { /* NEC format */
	0xFFA25D,0xFF629D,0xFFE21D,
	0xFF22DD,0xFF02FD,0xFFC23D,
	0xFFE01F,0xFFA857,0xFF906F,
	0xFF6897,0xFF9867,0xFFB04F,
	0xFF30CF,0xFF18E7,0xFF7A85,
	0xFF10EF,0xFF38C7,0xFF5AA5,
	0xFF42BD,0xFF4AB5,0xFF52AD };


int analogPin0 = 0; // Анлоговый вход для считывания напряжения с делителя напряжения
int analogPin1 = 1;

float Vout0 = 0; // Переменная для хранения значения напряжения в средней точки делителя (0-3.3)
float Vout1 = 0;


float R0 = 0; // Переменная для хранения значения резистора R0
float R1 = 0; //

int  LEDsMultiSteeringWheel = 6; // пин на которам висят светодиоды мультируля
bool isLightsInCarOnOff = false;


void setup() {

	pinMode(LEDsMultiSteeringWheel, OUTPUT);
	Serial.begin(14400);

	//После включения питания включается магнитолла и переходит на флешку потом рандомный трек
	delay(30000);  // 60000 = 1 min.
	irsend.sendNEC(BTN_VOLMINUS, 32);
	delay(220);
	irsend.sendNEC(BTN_MODE, 32);
	delay(220);

	for (int i = 0; i <= random(8); i++)
	{
		irsend.sendNEC(BTN_NEXT, 32);
		delay(220);
	}
	//-----------------------------------
}

void loop()
{
	getLightsInCarOnOff();

	unsigned long tempData = getButtonClick();

	unsigned long delayTime = 120;

		if (tempData == BTN_VOLPLUS)
		{
			irsend.sendNEC(BTN_VOLPLUS, 32);
			Serial.println("BTN_VOLPLUS");
			multiSteeringWheelButtonsBlink(); // мигание кнопками на руле
		}
		if (tempData == BTN_VOLMINUS)
		{
			irsend.sendNEC(BTN_VOLMINUS, 32);
			Serial.println("BTN_VOLMINUS");

			multiSteeringWheelButtonsBlink();
		}


		if (tempData == BTN_NEXT)
		{
			irsend.sendNEC(BTN_NEXT, 32);
			Serial.println("BTN_NEXT");

			multiSteeringWheelButtonsBlink();
		}
		if (tempData == BTN_BACK)
		{
			irsend.sendNEC(BTN_BACK, 32);
			Serial.println("BTN_BACK");
			multiSteeringWheelButtonsBlink();
		}
	

		if (tempData == BTN_MODE)
		{
			irsend.sendNEC(BTN_MODE, 32);
			Serial.println("BTN_MODE");
			multiSteeringWheelButtonsBlink();
		}
		if (tempData == BTN_MUTE)
		{
			irsend.sendNEC(BTN_MUTE, 32);
			Serial.println("BTN_MUTE");
			multiSteeringWheelButtonsBlink();
		}


		//-------------------------------
			if (tempData == BTN_MEMONEXT_AND_BTN_VOLPLUS)
			{
				irsend.sendNEC(BTN_MEMONEXT_AND_BTN_VOLPLUS, 32);
				Serial.println("BTN_MEMONEXT_AND_BTN_VOLPLUS");
				multiSteeringWheelButtonsBlink();
			}
			if (tempData == BTN_MEMONEXT_AND_BTN_VOLMINUS)
			{
				irsend.sendNEC(BTN_MEMONEXT_AND_BTN_VOLMINUS, 32);
				Serial.println("BTN_MEMONEXT_AND_BTN_VOLMINUS");
				multiSteeringWheelButtonsBlink();
			}

			//-------------------------------
			//-------------------------------

			if (tempData == BTN_MEMONEXT_AND_BTN_BACK)
			{
				irsend.sendNEC(BTN_MEMONEXT_AND_BTN_BACK, 32);
				Serial.println("BTN_MEMONEXT_AND_BTN_BACK");
				multiSteeringWheelButtonsBlink();
			}
			if (tempData == BTN_MEMONEXT_AND_BTN_NEXT)
			{
				irsend.sendNEC(BTN_MEMONEXT_AND_BTN_NEXT, 32);
				Serial.println("BTN_MEMONEXT_AND_BTN_NEXT");
				multiSteeringWheelButtonsBlink();
			}
			//-------------------------------			



		/*if (tempData == BTN_MEMONEXT)
		{
			irsend.sendNEC(BTN_MEMONEXT, 32);
			Serial.println("BTN_MEMONEXT");

			multiSteeringWheelButtonsBlink();
		}
		if (tempData == BTN_MEMOBACK)
		{
			irsend.sendNEC(BTN_MEMOBACK, 32);
			Serial.println("BTN_MEMOBACK");

			multiSteeringWheelButtonsBlink();
		}*/
		
}


void multiSteeringWheelButtonsBlink() // мигнуть кнопками на руле ( если фары машины включены то погасить светодиоды на руле на некоторое время, если фары включены то наоборот )
{
	int durationCombustionLEDs = 120;
	if (isLightsInCarOnOff)
	{
		digitalWrite(LEDsMultiSteeringWheel, LOW);
		delay(durationCombustionLEDs);
		digitalWrite(LEDsMultiSteeringWheel, HIGH);
	}
	else
	{
		digitalWrite(LEDsMultiSteeringWheel, HIGH);
		delay(durationCombustionLEDs);
		digitalWrite(LEDsMultiSteeringWheel, LOW);
	}
}

bool getLightsInCarOnOff() //  Получение состояни включены  фары или выключены (НЕ реализовано, по умолчанию выключены)
{
	//TODO: Need implement.
	isLightsInCarOnOff = false; 
	digitalWrite(LEDsMultiSteeringWheel, LOW);
	return false;
}

unsigned long getButtonClick()
{
	Vout0 = (3.3 / 1023.0) * analogRead(analogPin0); // Вычисляем напряжение в средней точки делителя (0-3.3)
	//R0 = 820 / ((3.3 / Vout0) - 1); // Вычисляем сопротивление R0 (10000 это значение R0 10 кОм) 
	//Serial.print("Voltage0: "); // 
	//Serial.println(Vout0); // Напряжения в средней точки делителя (0-3.5) для справки
	//Serial.print("R0: "); // 
	//Serial.println(R0); // Значение сопротивления R0

	Vout1 = (3.3 / 1023.0) * analogRead(analogPin1); // Вычисляем напряжение в средней точки делителя (0-3.3)
	//R1 = 820 / ((3.3 / Vout1) - 1); // Вычисляем сопротивление R1 (10000 это значение R1 10 кОм) 
	//Serial.print("Voltage1: "); // 
	//Serial.println(Vout1); // Напряжения в средней точки делителя (0-3.3) для справки
	//Serial.print("R1: "); // 
	//Serial.println(R1); // Значение сопротивления R1
		


	
	//-------------------------------

	if ((Vout0 >= (V_MEMONEXT - DELTA) && Vout0 <= (V_MEMONEXT + DELTA))   &&  (Vout1 >= (V_NEXT - DELTA) && Vout1 <= (V_NEXT + DELTA)))
	{
		return BTN_MEMONEXT_AND_BTN_NEXT;
	}
	if ((Vout0 >= (V_MEMONEXT - DELTA) && Vout0 <= (V_MEMONEXT + DELTA))   &&   (Vout1 >= (V_BACK - DELTA) && Vout1 <= (V_BACK + DELTA)))
	{
		return BTN_MEMONEXT_AND_BTN_BACK;
	}
	//-------------------------------


	//-------------------------------
	if (Vout0 >= (V_MEMONEXT - DELTA) && Vout0 <= (V_MEMONEXT + DELTA) && Vout1 >= (V_VOLPLUS - DELTA) && Vout1 <= (V_VOLPLUS + DELTA))
	{		
		return BTN_MEMONEXT_AND_BTN_VOLPLUS;
		//return ALLDOWN;
	}
	if (Vout0 >= (V_MEMONEXT - DELTA) && Vout0 <= (V_MEMONEXT + DELTA) && Vout1 == V_VOLMINUS)
	{		
		return BTN_MEMONEXT_AND_BTN_VOLMINUS;		
	}
	//-------------------------------

	//// ---- MEMO ----
	//if (Vout0 >= (V_MEMONEXT - DELTA) && Vout0 <= (V_MEMONEXT + DELTA))
	//{
	//	return BTN_MEMONEXT;
	//}
	//if (Vout0 >= ( V_MEMOBACK - DELTA) && Vout0 <= (V_MEMOBACK + DELTA))
	//{
	//	return BTN_MEMOBACK;
	//}

	


	if (Vout1 >= (V_VOLPLUS - DELTA)  && Vout1 <= (V_VOLPLUS + DELTA))
	{		
		return BTN_VOLPLUS;
	}
	if (Vout1 == V_VOLMINUS)
	{		
		return BTN_VOLMINUS;
	}

	//-------------------
	if (Vout1 >= (V_NEXT - DELTA) && Vout1 <= (V_NEXT + DELTA))
	{
		return BTN_NEXT;
	}
	if (Vout1 >= (V_BACK - DELTA) && Vout1 <= (V_BACK + DELTA))
	{
		return BTN_BACK;
	}

	//-------------------
	if (Vout0 == V_MODE)
	{
		return BTN_MODE;
	}
	if (Vout0 >= (V_MUTE - DELTA) && Vout0 <= (V_MUTE + DELTA))
	{
		return BTN_MUTE;
	}

	//// ---- MEMO ----
	//if (Vout0 >= (V_MEMONEXT - DELTA) && Vout0 <= (V_MEMONEXT + DELTA))
	//{
	//	return BTN_MEMONEXT;
	//}
	//if (Vout0 >= ( V_MEMOBACK - DELTA) && Vout0 <= (V_MEMOBACK + DELTA))
	//{
	//	return BTN_MEMOBACK;
	//}

	
	return ALLDOWN;	
}