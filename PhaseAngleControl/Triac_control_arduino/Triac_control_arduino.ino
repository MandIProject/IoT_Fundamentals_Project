int detect = 0;
int value=0;
int last_CH1_state = 0;

void setup() {
  /*
   * Port registers allow for lower-level and faster manipulation of the i/o pins of the microcontroller on an Arduino board. 
   * The chips used on the Arduino board (the ATmega8 and ATmega168) have three ports:
     -B (digital pin 8 to 13)
     -C (analog input pins)
     -D (digital pins 0 to 7)   
  //All Arduino (Atmega) digital pins are inputs when you begin...
  */  
   
  PCICR |= (1 << PCIE0);    //enable PCMSK0 scan                                                 
  PCMSK0 |= (1 << PCINT0);  //Set pin D2 trigger an interrupt on state change. Input from optocoupler
  pinMode(3,OUTPUT);        //Define D3 as output for the triac driver pulse
  Serial.begin(9600);
}

void loop() {
   //AC frequency is 50Hz, so period is 20ms. We want to control the power
   //of each half period, so the maximum is 10ms or 10,000us.
   if(Serial.available()){
    value = map(Serial.read(),255,150,1000,5000); //mapped between 1ms to 5ms after reading from the serial COM
   }
   
    if (detect)
    {
      delayMicroseconds(value); //This delay controls the power
      digitalWrite(3,HIGH);
      delayMicroseconds(100); //each pulse has on time 100us
      digitalWrite(3,LOW);
      detect=0;
    } 
}




//This is the interruption routine
//----------------------------------------------

ISR(PCINT0_vect){
  /////////////////////////////////////               //Input from optocoupler
  if(PINB & B00000001){                               //We make an AND with the pin state register, We verify if pin 2 is HIGH???
    if(last_CH1_state == 0){                          //If the last state was 0, then we have a state change...
      detect=1;                                       //We have detected a state change!
    }
  }
  else if(last_CH1_state == 1){                       //If pin 2 is LOW and the last state was HIGH then we have a state change      
    detect=1;                                         //We have detected a state change!
    last_CH1_state = 0;                               //Store the current state into the last state for the next loop
    }
}
