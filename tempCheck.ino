/* Odczyt z czytnika temperatury i wilgotnosci
 * Dokumentacja : DHT11.pdf
 */

#define ONE_LENGTH 52
#define PRINTBITS 0

const int pin = 4;

const unsigned long STARTSIGNALLENGTH = 20000;

void resetState();
void pushBit(int bitValue);
void sendResult();

char charBits[41];
int charBitsLength;

int result[5];

int isLow() {
  return LOW == digitalRead(pin);
}

int isHigh() {
  return HIGH == digitalRead(pin);
}

void makeHigh() {
  digitalWrite(pin,HIGH);
}

void makeLow() {
  digitalWrite(pin,LOW);
}


void setup() {
  Serial.begin(9600);
  Serial.println("Setup");

}


void loop() {
  delay(2000); // initial delay
  synchImpl();
  resetState();
}

void synchImpl() {
  
  pinMode(pin, OUTPUT); 
  makeLow(); // emit starting signal

  delayMicroseconds(STARTSIGNALLENGTH); 

  makeHigh();
  pinMode(pin,INPUT);
  
  while(!isLow());
  while(isLow());
  while(isHigh());

  for (int i=0;i<40;i++) {
    while(isLow());
    unsigned long start = micros();
    while(isHigh());
    unsigned long elapsed = micros() - start;
    pushBit(elapsed);
  }

  pinMode(pin,OUTPUT);
  makeHigh();
  sendResult();
}

void sendResult() {
  Serial.println("WYNIK:");
  if (PRINTBITS) {
    for (int i=0;i<40;i++) {
      if (i % 8 == 0) {
        Serial.println("-");
      }
      Serial.print((int)charBits[i]);
      Serial.print(" ");
      Serial.println(charBits[i] <= ONE_LENGTH ? "0" : "1");
      
    }
    Serial.println("-----");
  }
  for (int i=0;i<4;i++) {
    Serial.print(i);
    Serial.print(" ");
    Serial.println(result[i]);
  }

  Serial.print("checksum ");
  Serial.println(result[4]);

  int sum = 0;
  for (int i=0;i<4;i++) {
    sum += result[i];
  }

  if ( (sum & 0xFF) == result[4]) {
    Serial.println("[  OK  ] checksum");
  }
  else {
    Serial.println("[FAILED] checksum");
  }
}

void pushBit(int signalLength) {
  charBits[charBitsLength] = signalLength;
  
  result[charBitsLength/8] = result[charBitsLength/8] << 1 | (signalLength > ONE_LENGTH);
  
  charBitsLength++;
}


void resetState() {
  for (int i=0;i<41;i++) {
    charBits[i] = 0;
  }

  for (int i=0;i<5;i++) {
    result[i] = 0;
  }
  charBitsLength = 0; 
}

