/*
ad5933-test
    TESTED: printing values at the setup to see if I can transfer that code over here
    UNTESTED: printing the exact impedance value every time the sweep runs (is there a particular reason this wouldn't work?)
*/

#include <Wire.h>
#include "AD5933.h"

#define START_FREQ  (50000)//changed from 80k
#define FREQ_INCR   (1000)
#define NUM_INCR    (40)

double gain[NUM_INCR+1]={0.0000050097155570,0.0000049948835372,0.0000049356393814,0.0000049652414321,0.0000049635896682,0.0000049244956970,0.0000049835190773,0.0000050023765563,0.0000051153731346,0.0000049239583015,0.0000049417834281,0.0000049606566429,0.0000050327577590,0.0000049964413642,0.0000050312280654,0.0000050649309158,0.0000049902162551,0.0000049370827674,0.0000049673233032,0.0000050291275978,0.0000049746317863,0.0000049868030548,0.0000050402312278,0.0000050352439880,0.0000050188565254,0.0000049839534759,0.0000049638333320,0.0000050353717803,0.0000050216398239,0.0000050355634689,0.0000049993758201,0.0000049667105674,0.0000050571527481,0.0000050352439880,0.0000050075168609,0.0000049468603134,0.0000050579290390,0.0000049912109375,0.0000050134921073,0.0000050114769935,0.0000050429220199};
//double gain[NUM_INCR+1];
int phase[NUM_INCR+1];

union {
   byte array[8];
   double bigNum;
} myUnion;

void setup(void)
{
  // Begin I2C
  Serial.begin(115200);
  /* myUnion.bigNum = 10;
   for (int i = 0; i < sizeof(double); i++) {
   //Serial.println(i);
   Serial.print(myUnion.array[i]);   // Send to wherever
   if(i!=3){
     Serial.print(".");
   }
}*/
  long REF_RESIST=(long)40;
  Wire.begin();

  // Begin serial at 9600 baud for output
  
  

  // Perform initial configuration. Fail if any one of these fail.
  if (!(AD5933::reset() &&
        AD5933::setInternalClock(true) &&
        AD5933::setStartFrequency(START_FREQ) &&
        AD5933::setIncrementFrequency(FREQ_INCR) &&
        AD5933::setNumberIncrements(NUM_INCR) &&
        AD5933::setPGAGain(PGA_GAIN_X1)))
        {

            while (true) ;
        }

  // Perform calibration sweep
  /*if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR+1)){
    
    
  }
  else
    //Serial.println("Calibration failed...");
    ;*/
}

void loop(void)
{
  // Easy to use method for frequency sweep
  /*for(int i=0; i<41; i++){
      Serial.println(gain[i]);
    }*/ //why is it 0
  //frequencySweepEasy();

  // Delay
  //delay(5000);

  // Complex but more robust method for frequency sweep
  frequencySweepRaw();

  // Delay
  delay(5000); //safe to decrease?
}

// Easy way to do a frequency sweep. Does an entire frequency sweep at once and
// stores the data into arrays for processing afterwards. This is easy-to-use,
// but doesn't allow you to process data in real time.
void frequencySweepEasy() {
    // Create arrays to hold the data
    int real[NUM_INCR+1], imag[NUM_INCR+1];

    // Perform the frequency sweep
    if (AD5933::frequencySweep(real, imag, NUM_INCR+1)) {
      // Print the frequency data
      int cfreq = START_FREQ/1000;
      for (int i = 0; i < NUM_INCR+1; i++, cfreq += FREQ_INCR/1000) {
        // Print raw frequency data


        // Compute impedance
        double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
        double impedance = 1/(magnitude*gain[i]);

      }
      //Serial.println("Frequency sweep complete!");
    } else {
      //Serial.println("Frequency sweep failed...");
    }
}

// Removes the frequencySweep abstraction from above. This saves memory and
// allows for data to be processed in real time. However, it's more complex.
void frequencySweepRaw() {
    // Create variables to hold the impedance data and track frequency
    int real, imag, i = 0, cfreq = START_FREQ/1000;

    // Initialize the frequency sweep
    if (!(AD5933::setPowerMode(POWER_STANDBY) &&          // place in standby
          AD5933::setControlMode(CTRL_INIT_START_FREQ) && // init start freq
          AD5933::setControlMode(CTRL_START_FREQ_SWEEP))) // begin frequency sweep
         {
  
         }

    // Perform the actual sweep
    while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE) {
        // Get the frequency data for this frequency point
        if (!AD5933::getComplexData(&real, &imag)) {
 
        }

        // Print out the frequency data
        //Serial.print(cfreq);


        // Compute impedance
        double magnitude = sqrt(pow(real, 2) + pow(imag, 2));

        /*Serial.print(" Magnitude: ");
        Serial.print(magnitude);
        Serial.print(" Magnitude*Gain: ");
        Serial.print((magnitude*gain[i]));
        Serial.print(" Impedance: ");
        Serial.print(1/(magnitude*gain[i]));*/
        double impedance = 1/(magnitude*gain[i]);
  
         myUnion.bigNum = impedance;
        for (int i = 0; i < sizeof(double); i++) {
        //Serial.println(i);
        Serial.print(myUnion.array[i]);   // Send to wherever
        if(i!=3){
          Serial.print(".");
        }
      }

        // Increment the frequency
        i++;
        cfreq += FREQ_INCR/1000;
        AD5933::setControlMode(CTRL_INCREMENT_FREQ);
    }

    //Serial.println("Frequency sweep complete!");

    // Set AD5933 power mode to standby when finished
    if (!AD5933::setPowerMode(POWER_STANDBY))
      ;
}
