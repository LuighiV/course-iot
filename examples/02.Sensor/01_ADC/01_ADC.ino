#define ADC_INPUT_PIN   15
#define ADC_RESOLUTION  12

#define MEAN_FILTER_LENGTH 3

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  
  //set the resolution to 12 bits (0-4096)
  analogReadResolution(ADC_RESOLUTION);
  //analogSetAttenuation(ADC_11db);
}

void loop() {
  // read the analog / millivolts value
  int sumValue = 0;
  int sumVolts = 0;

  for (int i=0; i<MEAN_FILTER_LENGTH; i++){
    sumValue += analogRead(ADC_INPUT_PIN);
    sumVolts += analogReadMilliVolts(ADC_INPUT_PIN);
    delay(100);
  }

  float analogValue = sumValue/MEAN_FILTER_LENGTH;
  float analogVolts = sumVolts/MEAN_FILTER_LENGTH;
  
  // print out the values you read:
  Serial.printf("ADC analog value = %.1f\n",analogValue);
  Serial.printf("ADC millivolts value = %.1f\n",analogVolts);
  
  delay(1000);  // delay in between reads for clear read from serial
}
