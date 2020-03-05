    /*     Simple Stepper Motor Control Exaple Code
     *      
     *  by Dejan Nedelkovski, www.HowToMechatronics.com
     *  
     */
    // defines pins numbers

#include "Arduino.h"
#include "motor_DC.h"


motor_DC::motor_DC(uint8_t Polar, uint8_t range, uint32_t SamplingPeriod,uint32_t DeadBand, uint8_t mark):
	Polarity(Polar),
	SamplingPeriod(SamplingPeriod),
	Joystick_dead_band(DeadBand),
	_mark(mark)
{
	// cleanup
	this->sumSamples =0;
	this->MaxOutputSpeedCoef=100;
	this->MaxValue=0;
	this->output_speed=0;
	this->output_direction = FORWARD;
	this->_range =0;
	// évite le débordement du calcul par 2^n -1 si n=32
	for(uint8_t i=0;i<range;i++)
		this->_range |= (1<<i);	// allume les bits jusqu'à range-1

	this->MaxValue = _range;
	this->reference = 512;
	this->min = -512;
	this->max = 512;
}

void motor_DC::begin() {
	begin(this->SamplingPeriod);
}

void motor_DC::begin(uint32_t Period) {
	this->SamplingPeriod = Period;
	this->nbSamples=0;
	this->PreviousRefreshTime = millis();
	this->initialized_ = true;
	PRINTLN("MaxValue=",this->MaxValue);
}

void motor_DC::setOutputCoefficient(uint8_t coefficient){
	this->MaxOutputSpeedCoef = constrain(coefficient,0,100);

	this->MaxValue = int32_t((float)(this->_range)*(float)this->MaxOutputSpeedCoef/100.0f); 
	PRINT("\nrange=",byte(this->_range));
	PRINTLN("\tMaxValue=",this->MaxValue);
}

void motor_DC::setForwardCompensationCoef(uint8_t coefficient){
	this->_CompensationCoef = constrain(coefficient,0,100);
}

void motor_DC::setSamplingPeriod(uint32_t SamplingPeriod) {
	this->SamplingPeriod = SamplingPeriod;
}

void motor_DC::setCenterPoint(int32_t ref) {

	this->reference = ref;
	this->min = -ref;
	this->max = DYN-ref;
}

/*
void motor_DC::setJoystickPolarity(uint8_t polarity) {
	this->Polarity = polarity;
}

*/
uint32_t motor_DC::getMotorSpeed(int32_t value) {
	if(!initialized_) begin(SamplingPeriod);

	// Ajoute tous les échantillons
	sumSamples+=value;
	nbSamples++;
	if(millis() > (PreviousRefreshTime+SamplingPeriod)) {
		PRINT("Nbsamples :",this->nbSamples);
		PRINT("\tsumsamples :",this->sumSamples);
		PreviousRefreshTime = millis();
		sumSamples/=nbSamples;
		sumSamples=constrain(sumSamples,this->min,this->max);		// limite l'extrusion 
		PRINT("\tsample moyen:",this->sumSamples);
		setSpeedValue(sumSamples);
		nbSamples=0;
		sumSamples=0;
		// A ce point là, la variable output_speed a été mise à jour
	}
	return output_speed;
}

void motor_DC::setSpeedValue(int32_t input){
	int32_t tmp_output;
	uint8_t tmp_direction;
	//PRINT("joystick input =",input);
    if (input>=(int32_t)Joystick_dead_band) {
    	// positif et au-dessus de la bande morte
    	tmp_output = map(input,Joystick_dead_band,this->max,0, this->MaxValue);
    	// PRINTLN(" Positif : intermédiaire 1 ",this->output_speed);

    	// OUTPUT est compris entre 0 et 2^range-1

    	// Applique le coefficient de sortie
	//	tmp_output=(tmp_output*MaxOutputSpeedCoef) / 100;
		tmp_direction = FORWARD;
		tmp_output =(tmp_output*this->_CompensationCoef)/100;
    	// Si le mode Revert est actif il faut inverser la sortie
    	if(this->Polarity == BACKWARD_WHEN_POSITIVE) {
    		tmp_output = this->_range - tmp_output;
    		tmp_direction = BACKWARD;
	    	// PRINTLN(" Positif : intermédiaire 2 ",this->output_speed);
	    	// PRINTLN(" Positif : intermédiaire 2 - dir=",this->output_direction);
    	}
    } else if (input<=-(int32_t)Joystick_dead_band) {

    	// négatif et au-dessous de la bande morte
		tmp_output = map(input,this->min,-Joystick_dead_band,this->MaxValue,0);
/*    	PRINT(" Négatif : intermédiaire 1 ",tmp_output);
		PRINT("\t","");
*/
		if(this->Polarity == FORWARD_WHEN_POSITIVE) {
			tmp_direction = BACKWARD;
			tmp_output = (tmp_output*this->_CompensationCoef)/100;
			// Ajoute l'offset pour une sortie 
			tmp_output = this->_range - tmp_output;
	    	// PRINTLN(" Négatif : intermédiaire 2 ",this->output_speed);
		} else {
			// on ne change pas l'offset et la direction est avant
			tmp_direction = FORWARD;
			tmp_output = tmp_output;
	    	// PRINT(" Négatif : intermédiaire 3 ",this->output_speed);
	    	// PRINTLN(" Négatif : intermédiaire 3 - dir =",this->output_direction);
		}
    } else {
    	// PRINTLN(" Nul","");    	
    	// dans la bande morte, vitesse nulle
    	tmp_direction=FORWARD;
    	tmp_output=0;
    }

    // Finally apply only if it isn't a marker value
    if(tmp_output!=_mark) {
    	this->output_speed = tmp_output;
    	this->output_direction = tmp_direction;
    }
}

uint8_t motor_DC::getMotorDirection() {
	return this->output_direction;
}

// debug helper functions
#ifdef DEBUG

uint32_t motor_DC::getSpeedValue(uint32_t val){
	setSpeedValue((int32_t)val);
	return this->output_speed;
}
#endif