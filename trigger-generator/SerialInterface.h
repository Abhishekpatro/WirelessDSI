#ifndef __SerialInterface_H__
#define __SerialInterface_H__
#include "Arduino.h"

/*
	The following example allows two global variables to be queried and/or set over the
	serial port::
	
		#include "SerialInterface.h"
		
		double gFoo = 0.0;
		String gBar = "Hello World";
		
		void setup()
		{
			Serial.begin( 9600 );
		}
		
		void loop()
		{
			if( StartCommands() )
			{
				if( ProcessVariableCommand( "foo", gFoo ) ) Serial.println( "changed foo" );
				if( ProcessVariableCommand( "bar", gBar ) ) Serial.println( "changed bar" );
				EndCommands();
			}
		}
	
	An optional third argument to `ProcessVariableCommand` can be `VARIABLE_READ_ONLY`,
	`VARIABLE_VERBOSE`, or the default value `VARIABLE_SILENT`.
	
	In this example, if you send the command `foo` over the serial port, the reply will
	be a JSON string associating the name `foo` with the current value of the corresponding
	global variable `gFoo`. 
	
	Provided the variable is not designated as `VARIABLE_READ_ONLY`, you can also set its
	value, e.g. by sending the command `foo = 2`.  (If you have designated the variable as
	`VARIABLE_VERBOSE` then this would also trigger a JSON output, as if you had queried
	`foo` immediately after setting it.)

	You can send the simple command `?` to receive a JSON output containing all the
	variables that are accessible in this manner.
*/

bool gCommandBackslash = false;
char gCommandQuote = '\0';
String gPartialCommand = "", gFullCommand = "";
int gListAllVariables = 0;

const char *
_ParseVariableCommand( const char *command, const char * variableName )
{
	if( !command || !*command ) return NULL;
	int lvn = strlen( variableName );
	if( strncmp( command, variableName, lvn ) != 0 ) return 0;
	command += lvn;
	while( isspace( *command ) ) command++;
	if( *command && *command++ != '=' ) return NULL;
	while( isspace( *command ) ) command++;
	return command;
}

#define _START_VARIABLE_PROCESSOR( TYPE, QUOTE ) \
	bool \
	ProcessVariableCommand( const char * variableName, TYPE & var, VariableWriteMode writeMode=VARIABLE_SILENT ) \
	{ \
		const char * command = _ParseVariableCommand( gFullCommand.c_str(), variableName ); \
		if( gListAllVariables ) { Serial.print( ( gListAllVariables++ == 1 ) ? "{" : ", " ); Serial.print( "\"" ); Serial.print( variableName ); Serial.print( "\": " ); Serial.print( QUOTE ); Serial.print( var ); Serial.print( QUOTE ); } \
		if( !command ) return false; \
		if( !*command ) { Serial.print( "{\"" ); Serial.print( variableName ); Serial.print( "\": " ); Serial.print( QUOTE ); Serial.print( var ); Serial.print( QUOTE ); Serial.println( "}" ); Serial.flush(); gFullCommand = ""; return false; } \
		if( writeMode == VARIABLE_READ_ONLY ) { Serial.print( "{\"SERIAL_COMMAND_ERROR\": \"cannot change the '" ); Serial.print( variableName ); Serial.println( "' variable because it is read-only\"}" ); Serial.flush(); gFullCommand = ""; return false; } \
		char *remainder = NULL; \
		TYPE val;
		//
		// conversion of const char * command to TYPE val happens here between _START_VARIABLE_PROCESSOR and _END_VARIABLE_PROCESSOR macros
		//
#define _END_VARIABLE_PROCESSOR( TYPE, QUOTE ) \
		while( remainder && isspace( *remainder ) ) remainder++; \
		if( remainder && *remainder ) { Serial.print( "{\"SERIAL_COMMAND_ERROR\": \"failed to interpret argument as type '" #TYPE "' when setting the '" ); Serial.print( variableName ); ; Serial.println( "' variable\"}" ); Serial.flush(); gFullCommand = ""; return false; } \
		var = val; \
		if( writeMode == VARIABLE_VERBOSE ) { Serial.print( "{\"" ); Serial.print( variableName ); Serial.print( "\": " ); Serial.print( QUOTE ); Serial.print( var ); Serial.print( QUOTE ); Serial.println( "}" ); Serial.flush(); } \
		gFullCommand = ""; \
		return true; \
	}

typedef enum
{
	VARIABLE_READ_ONLY = 0,
	VARIABLE_SILENT    = 1,
	VARIABLE_VERBOSE   = 2
} VariableWriteMode;

const char *
StartCommands( void )
{
	char c = '\0';
	if( !Serial.available() || ( ( c = Serial.read() ) == ';' && !gCommandQuote ) )
	{
		if( !c && !gPartialCommand.length() ) return NULL;
		gFullCommand = gPartialCommand;
		gFullCommand.trim();
		if( gFullCommand == "?" ) { gListAllVariables = 1; gFullCommand = ""; }
		gPartialCommand = "";
		gCommandBackslash = false;
		gCommandQuote = '\0';
		return gFullCommand.c_str();
	}
	bool escape = ( c == '\\' && !gCommandBackslash );
	if( gCommandBackslash )
	{
		if(      c == 'n' ) c = '\n';
		else if( c == 'r' ) c = '\r';
		else if( c == 't' ) c = '\t';
	}
	if( !escape && ( !isspace( c ) || gPartialCommand.length() ) ) gPartialCommand += c;
	if( !gCommandQuote && ( c == '\'' || c == '"' ) ) gCommandQuote = c;
	else if( gCommandQuote && c == gCommandQuote && !gCommandBackslash ) gCommandQuote = '\0';
	gCommandBackslash = escape;
	return NULL;
}

_START_VARIABLE_PROCESSOR( int, "" )            val = strtol(  command, &remainder, 10 );     _END_VARIABLE_PROCESSOR( int, "" )
_START_VARIABLE_PROCESSOR( unsigned int, "" )   val = strtoul( command, &remainder, 10 );     _END_VARIABLE_PROCESSOR( unsigned int, "" )
_START_VARIABLE_PROCESSOR( short, "" )          val = strtol(  command, &remainder, 10 );     _END_VARIABLE_PROCESSOR( short, "" )
_START_VARIABLE_PROCESSOR( unsigned short, "" ) val = strtoul( command, &remainder, 10 );     _END_VARIABLE_PROCESSOR( unsigned short, "" )
_START_VARIABLE_PROCESSOR( long, "" )           val = strtol(  command, &remainder, 10 );     _END_VARIABLE_PROCESSOR( long, "" )
_START_VARIABLE_PROCESSOR( unsigned long, "" )  val = strtoul( command, &remainder, 10 );     _END_VARIABLE_PROCESSOR( unsigned long, "" )
_START_VARIABLE_PROCESSOR( float, "" )          val = ( float )strtod( command, &remainder ); _END_VARIABLE_PROCESSOR( float, "" )
_START_VARIABLE_PROCESSOR( double, "" )         val = strtod( command, &remainder );          _END_VARIABLE_PROCESSOR( double, "" )
_START_VARIABLE_PROCESSOR( String, "\"" )
	char quote = *command++;
	if( quote != '"' && quote != '\'' ) remainder = &quote;
	else
	{
		val = command;
		int trimmedLength;
		for( trimmedLength = val.length(); trimmedLength > 0; trimmedLength-- )
			if( !isspace( val.charAt( trimmedLength - 1 ) ) ) break;
		if( trimmedLength < ( int )val.length() ) val = val.substring( 0, trimmedLength );
		if( val.charAt( val.length() - 1 ) != quote ) remainder = &quote;
		else val = val.substring( 0, val.length() - 1 );
	}
_END_VARIABLE_PROCESSOR( String, "\"" )

bool ProcessCommand( const char * command )
{
	if( gFullCommand != command ) return false;
	gFullCommand = "";
	return true;
}

bool
EndCommands( void )
{
	if( gListAllVariables ) { Serial.println( "}" ); Serial.flush(); gListAllVariables = 0; }
	if( gFullCommand.length() ) { Serial.println( "{\"SERIAL_COMMAND_ERROR\": \"failed to recognize command\"}" ); Serial.flush(); gFullCommand = ""; return true; }
	return false;
}
#endif // __SerialInterface_H__
