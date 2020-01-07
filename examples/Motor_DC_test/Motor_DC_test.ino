#include <motor_DC.h>

#define PRINT(A,B)	{Serial.print(A);Serial.print(B);};
#define PRINTLN(A,B)	{Serial.print(A);Serial.println(B);};
motor_DC x;

void setup() {
	Serial.begin(115200);
	Serial.println("start setup...");
	x.begin(5);
	pinMode(23,INPUT_PULLUP);
	Serial.println("setup ends.");
}

void loop() {
	// teste toute la dynamique
	
	for(uint8_t coef=100;coef>0;coef-=20) {
		x.setOutputCoefficient(coef);
		for(int32_t input=-512;input<512;input++) {
			PRINT("POSITION : ", input);
			PRINT("\tCOEF : ", coef);
			PRINT("\t VITESSE MOTEUR : ", (byte)(x.getSpeedValue(input)));
			PRINTLN("\t DIRECTION : ", x.getMotorDirection());
		}
		while(digitalRead(23));
	}
/*	Serial.println(x.getMotorSpeed());
	PRINTLN("dir=",x.getMotorDirection());
	delay(1);
*/
}
