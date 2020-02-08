    /*     Simple Stepper Motor Control Exaple Code
     *      
     *  by Dejan Nedelkovski, www.HowToMechatronics.com
     *  
     */
    // defines pins numbers

#ifndef LIB_MOTOR_DC
#define LIB_MOTOR_DC

#define DYN 1023

//#define DEBUG

//#define PRINT_DBG

#ifdef PRINT_DBG
#define PRINT(A,B)  {Serial.print(A);Serial.print(B);};
#define PRINTLN(A,B)    {Serial.print(A);Serial.println(B);};
#else
#define PRINT(A,B)  {}
#define PRINTLN(A,B) {}
#endif

enum 
{
    FORWARD_WHEN_POSITIVE=0,
    BACKWARD_WHEN_POSITIVE
};

enum {
    FORWARD=0,
    BACKWARD
};

class motor_DC
{
public:
    // Constructor
	motor_DC(uint8_t Polar=FORWARD_WHEN_POSITIVE, uint8_t range=8, uint32_t SamplingPeriod = 5, uint32_t DeadBand=20, uint8_t mark=170);
	void begin(uint32_t Period);
    void begin(void);

    uint32_t getMotorSpeed(int32_t value);
    uint8_t  getMotorDirection();
    
#ifdef DEBUG
    uint32_t getSpeedValue(uint32_t val);
#endif    
    void setOutputCoefficient(uint8_t coefficient);
    void setSamplingPeriod(uint32_t SamplingPeriod);

    void setCenterPoint(int32_t ref);
    
    /* Controle la sortie de vitesse :
        false : sortie filtrée
        true  : sortie directe provenant du joystick (bypass du filtrage)
    */
private:

    // does begin has been called yet ?
    bool initialized_ =false;
    // internal getter
    void  setSpeedValue(int32_t joystick_input);
    
    /* Permet d'inverser le sens de calcul de direction et vitesse du joystick 
    Par exemple pousser le joystick vers le haut est négatif alors que c'est la marche avant*/
    uint8_t Polarity= FORWARD_WHEN_POSITIVE;
    ;
    /* valeur de la direction : par convention
        0 : MARCHE AVANT
        1 : MARCHE ARRIERE
    */
    uint8_t output_direction;

    /* Calcul de la vitesse */
    uint32_t output_speed;
    
    /* range : uint8_t exprimant le nombre de bits pour le calcul de la vitesse*/
    uint32_t _range;  // number of bits, for DC motor set to 8 bits

    int32_t reference;
    int32_t min;
    int32_t max;
    /* afin d"='éviter de alculer à chaque le max, on lecalcule et on le stocke*/
    uint32_t MaxValue;

    /* revert_speed : Boolean speed must be revert when in backward
    false : 0 est toujours la vitesse la plus faible quelque soit le sens
    true : si le sens est avant, 0 est la vitesse la plus faible
           si le sens est arrière, 2^(range)-1 est la vitesse la plus faible
    */
    bool revert_speed=false; 
    
    /* période d'échantillonnage min pour calculer la vitesse en (ms) */
    uint32_t SamplingPeriod = 5;

    /* date de l'ancienne acquisition */
    uint64_t PreviousRefreshTime=0;

    /* Valeur cumulée de l'entrée pour filtrage*/
    int64_t sumSamples=0;

    /* Nombre d'échantillons cumulées */
    uint16_t nbSamples;

    /* Applique un coefficient directement sur la sortie, compris entre 0 et 100
    0 : met la sortie vitesse au plus bas (0 ou 2^range-1)
    100 : Met la sortie vitesse au plus haut (0 ou 2^range-1)
    */
    uint8_t MaxOutputSpeedCoef = 0;

    /* zone autour de la référence 0 qui évite de fournir une vitesse.
    Dans cette zone, la vitesse est considérée nulle (soit 0, soit 2^range-1)
    Valeur exprimée en LSB d'entrée
    */
    uint32_t Joystick_dead_band;
    /*
    Valeur à filtrer 
    */
    uint8_t _mark;
};

#endif
