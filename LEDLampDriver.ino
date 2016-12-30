/**
 * LED lamp driver.
 * Joe Desbonnet
 * jdesbonnet@gmail.com
 * Jan 2013.
 * 
 * Commands (all parameters in hex):
 * Dnn : program mode nn=0 off, nn=prog on
 * Prrggbb : instantly set PWM outputs for R,G,B channels
 * Srrggbbtttt : slew to rrggbb from current values in tttt ms
 * Frrggbbtttt : flash to color RGB for tttt milliseconds
 */
 
#define RED_PWM 9
#define BLUE_PWM 10
#define GREEN_PWM 11

#define SPO_RED  8

byte buf[16];
byte bufptr = 0;

byte current[4];
byte target[4];
byte start[4];


unsigned long slewStart = 0;
unsigned long slewTime = 3000;

byte slewMode = 0;

byte flashMode = 0;
unsigned long flashStart;
unsigned long flashTime;
unsigned long lastReading;

byte progMode = 0;
  
void setup() {
  pinMode(RED_PWM, OUTPUT);
  pinMode(GREEN_PWM, OUTPUT);
  pinMode(BLUE_PWM, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  
  Serial.println ("LED Lamp Driver 0.1");
    
  byte data;
  int i;
  long dt,d;
  int a0,a1;

  //current[0]=40;
  
  setPWM();
  
  for (;;) {
    if (Serial.available()) {
      data = Serial.read();
      buf[bufptr++] = data;
      if (data == 0x0d || data == 0x0a) {
        processCommand();
      }
      if (bufptr == 16) {
        processCommand();
      }
    }
    
    if (slewMode == 1) {
      dt = millis() - slewStart;
      for (i = 0; i < 4; i++) {
        d = target[i] - start[i];
        current[i] = start[i] + (dt*d)/slewTime;
      }
      setPWM();
      if (dt >= slewTime) {
        slewMode = 0;
      }
    }
    if (flashMode == 1) {
      dt = millis() - flashStart;
      if (dt > flashTime) {
        current[0]=0;
        current[1]=0;
        current[2]=0;
        setPWM();
        flashMode = 0;
      }
    }
    if (progMode == 1 && slewMode == 0) {
      startSlew(random(255),random(255),random(255),8000);
    }
    if (progMode == 2) {
      dt = millis() - lastReading;
      if (dt > 20) {
        a0 = analogRead(2);
        digitalWrite (SPO_RED,1);
        delayMicroseconds(500);
        a1 = analogRead(2);
        digitalWrite (SPO_RED,0);
        
        Serial.print (a0);
        Serial.print (" ");

        Serial.print (a1);
        Serial.print (" ");

        Serial.println (a1 - a0);
        current[0] = (a1-a0)>>2;
        current[1] = 0;
        current[2] = 0;
        setPWM();
        lastReading = millis();
      }
    }
         
  } // end for(;;) loop
  
}

void setPWM () {
  analogWrite(RED_PWM, current[0]);
  analogWrite(GREEN_PWM, current[1]);
  analogWrite(BLUE_PWM, current[2]);
}

void startFlash (byte r, byte g, byte b, unsigned long t) {
  Serial.println ("start flash");
  current[0] = r;
  current[1] = g;
  current[2] = b;
  setPWM();
  flashStart = millis();
  flashTime = t;
  flashMode = 1;
}

void startSlew (byte r, byte g, byte b, unsigned long t) {
      target[0] = r;
      target[1] = g;
      target[2] = b;
      slewTime = t;

      start[0] = current[0];
      start[1] = current[1];
      start[2] = current[2];
      slewStart = millis();
      slewMode = 1;
}


void processCommand () {
  switch (buf[0]) {
    
    case 'D':
      progMode = readHex8(1);
      Serial.println ("new prog mode");
      break;
      
    case 'F' :
      current[0] = readHex8(1);
      current[1] = readHex8(3);
      current[2] = readHex8(5);
      flashTime = readHex16(7);
      setPWM();
      flashStart = millis();
      flashMode = 1;
      break;
      
    case 'P' :
      current[0] = readHex8(1);
      current[1] = readHex8(3);
      current[2] = readHex8(5);
      setPWM();
      break;
    case 'R' :
      slewTime = readHex16(1);
      break;
      
    case 'S' :
      target[0] = readHex8(1);
      target[1] = readHex8(3);
      target[2] = readHex8(5);
      slewTime = readHex16(7);

      start[0] = current[0];
      start[1] = current[1];
      start[2] = current[2];
      slewStart = millis();
      slewMode = 1;
      break;
  }
  
  bufptr = 0;
  
}

int readHex8 (byte index) {
  byte v = 0;
  v = hexDigitToVal(buf[index]);
  if (v == -1) {
    return -1;
  }
  v <<= 4;
  
  v |= hexDigitToVal(buf[index+1]);
  return v;
}
int readHex16 (byte index) {
  return readHex8(index)<<8 | readHex8(index+2);
}


int hexDigitToVal (byte c) {
  
  if (c >= '0' && c <= '9') {
    return c -'0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else if ( c>= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }

  return -1;
}

