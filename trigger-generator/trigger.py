#!/usr/bin/env -S python3
import sys
import serial

args = sys.argv[ 1: ]
if args:
    arg = 'byte=%s\n' % args[ 0 ]
    arg = arg.encode( 'utf8' )

    port = serial.Serial('/dev/cu.usbmodem3207101')
    port.write( arg )
    port.close()


