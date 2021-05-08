int strobePin  = 6;    // Strobe Pin on the MSGEQ7
int resetPin   = 5;    // Reset Pin on the MSGEQ7
int outPin     = A0;   // Output Pin on the MSGEQ7
int level[7];          // An array to hold the values from the 7 frequency bands

int entry[7];
int sums[7];

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  // Define our pin modes
  pinMode      (strobePin, OUTPUT);
  pinMode      (resetPin,  OUTPUT);
  pinMode      (outPin,    INPUT);
 
  // Create an initial state for our pins
  digitalWrite (resetPin,  LOW);
  digitalWrite (strobePin, LOW);
  delay        (1);
 
  // Reset the MSGEQ7 as per the datasheet timing diagram
  digitalWrite (resetPin,  HIGH);
  delay        (1);
  digitalWrite (resetPin,  LOW);
  digitalWrite (strobePin, HIGH);
  delay        (1);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Cycle through each frequency band by pulsing the strobe.
  
  sample(25);
  for (int i = 0; i < 7; i++) {
    Serial.print       (level[i]);
    Serial.print       ("   ");
  }
 
  Serial.println ();  
}

void readLevels() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(100);                    // Delay necessary due to timing diagram
    entry[i] = analogRead(outPin);
    digitalWrite(strobePin, HIGH);
    delayMicroseconds(100);                    // Delay necessary due to timing diagram  
  }
}
void sample(int windowms) {
  long start = millis();
  int out[7];
  for (int i=0;i<7;i++) {
    sums[i]=0;
    out[i]=0;
  }
  int entries = 0;
  while (start + windowms > millis()) {
    readLevels();
    for (int i=0;i<7;i++) {
      sums[i] += entry[i];
    }
    entries++;
  }
  for (int j=0;j<7;j++) {
    out[j] = sums[j] / entries;
  }
  //level = out;
  memcpy(level,out,7);
}
