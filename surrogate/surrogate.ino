
// https://learn.adafruit.com/introducing-adafruit-itsybitsy-m4/setup
// File -> Preferences -> Additional Boards Manager URLs:  https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
// Tools -> Board -> Boards Manager:  install "Adafruit SAMD Boards"
// Tools -> Board -> Adafruit SAMD Boards -> Adafruit ItsyBitsy M4 (SAMD51)
// 

int gRunning = 0;
unsigned long gLastOutputTime;

#define RED_LED 13  // This is true for the ItsyBitsy M4 ( https://learn.adafruit.com/introducing-adafruit-itsybitsy-m4 )

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 921600 );
  gLastOutputTime = micros();
  pinMode( RED_LED, OUTPUT );
  digitalWrite( RED_LED, HIGH );
}


void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite( RED_LED, gRunning ? HIGH : LOW ); // START to turn on red LED, STOP to turn it off
  if( Serial.available() )
  {
     char nBytes = Serial.read();
     String cmd;
     while( cmd.length() < nBytes ) { char c = Serial.read(); cmd += c; }
     if( cmd == "START" ) gRunning = 1;
     if( cmd == "STOP" ) gRunning = 0;
  }
  if( gRunning && micros() - gLastOutputTime > 30000 )
  {
    gLastOutputTime = micros();
    Serial.write( "#2" );             // intro
    Serial.write( "\x1a\x02" );       // little-endian unsigned short encoding the number of remaining bytes in this packet 
    Serial.write( "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" );  // of which the first ten are reserved
    Serial.write( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    Serial.write( "\r\n" );          // ... and the last two have to be the accepted outro
    Serial.flush();
  }
}
