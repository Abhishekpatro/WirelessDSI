#include "SerialInterface.h"
#include "TriggerBoxV2.h"

#define POWER_INDICATOR_LED SPARE_LED_PINS[ 0 ]

int gTriggerByte;

void Output( int x )
{
  for( int outputBit = 0; outputBit < 8; outputBit++, x >>= 1 )
    digitalWrite( PARALLEL_PORT_OUTPUT_PINS[ outputBit ], ( x & 1 ) ? HIGH : LOW );
}


void setup()
{
  for( int repeat = 0; repeat < 1; repeat++ )
  {
    for( int outputBit = 0; outputBit < 8; outputBit++ )
    {
      int pin = PARALLEL_PORT_OUTPUT_PINS[ outputBit ];
      if( pin >= 0 ) pinMode( pin, OUTPUT );
      if( pin >= 0 ) digitalWrite( pin, HIGH );
      delay( 200 );
      if( pin >= 0 ) digitalWrite( pin, LOW );
    }
  }
  Serial.begin( 38400 );
  if( POWER_INDICATOR_LED >= 0 ) { pinMode( POWER_INDICATOR_LED, OUTPUT ); digitalWrite( POWER_INDICATOR_LED, HIGH ); }
  gTriggerByte = 0;
}


void loop()
{
  if( StartCommands() )
  {
    if( ProcessVariableCommand( "byte", gTriggerByte ) ) Output( gTriggerByte );
    EndCommands();
  }  
}
