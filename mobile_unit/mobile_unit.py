import sys
import time
import websocket
import struct
import select
import threading

import serial     # pip install pyserial
import wdt        # from https://github.com/SequentMicrosystems/wdt-rpi.git

def report( msg ):
    stream = sys.stdout
    stream.write( '%s\n' % msg )
    try: stream.flush()
    except: pass

def handle_serial_data(connection, ws):
    intro = connection.read(2)
    if intro == b'\x23\x32':
        body_size_encoded = connection.read(2)
        bytes_to_read = struct.unpack('<H', body_size_encoded)[0] - 4
        #report( 'reading block of total size %d bytes' % ( bytes_to_read + 4 ) )
        complete_block = intro + body_size_encoded + connection.read(bytes_to_read)
        #report( repr( complete_block ) )
        if complete_block[-2:] == b'\x0D\x0A':
            ws.send_binary(complete_block)
        else:
            report("Data from headset did not end with expected block end code:\n%r" % complete_block)
    else:
        report("Data from headset did not start with block start code")

def handle_websocket_data(data, connection):
    while data:
        #report(repr(data))
        if data[0] != 0:
            n = data[0]
            if len(data) >= n + 1:
                command = data[:n+1]
                data = data[n+1:]
                report( 'forwarding command %r' % command )
                connection.write(command)
                connection.flush()
            else:
                break
        else:
            triggerByte = data[1]
            # TODO: set GPIO pins accordingly
            data = data[2:]


NEXT_POWER_CHECK = [0]
def retry(func, param=-1):
    r = 3
    ret = -1
    while ret < 0 and r > 0:
        time.sleep(0.1)
        if param != -1:
            ret = func(param)
        else:
            ret = func()
        r -= 1
    return ret

def power_check():
	if time.time() < NEXT_POWER_CHECK[0]: return
	NEXT_POWER_CHECK[:] = [time.time() + 3]
	vIn = retry(wdt.getVin)
	vBattery = retry(wdt.getVbat)
	vPi = retry(wdt.getVrasp)
	report( 'Line in: %.3fV;  Battery: %.3fV; Voltage to Pi: %.3fV' % ( vIn, vBattery, vPi ) )
	if 0.0 <= vIn < 3.0 and 0.0 <= vBattery < 3.0:
		report("Main power is disconnected and the battery has run down - the system is shutting down!")
		retry(wdt.setPeriod, 30)  # update the period with your maximum time to shutdown
		from subprocess import call
		#call("sudo shutdown -h now", shell=True) # TODO: uncomment


if __name__ == '__main__':

    # Set up the connection to the Itsy Bitsy M4 surrogate
    #connection = serial.Serial("/dev/ttyACM0", 921600)
    connection = serial.Serial(
          port="/dev/ttyACM0",
          baudrate=921600,
          bytesize=8,
          parity="N",
          stopbits=1,
          timeout=None,
          xonxoff=False,
          rtscts=True,        # ESP32 doesn't like this to be True
          dsrdtr=False,        # ESP32 and ItsyBitsy M4 don't like this to be True - nor does the headset itself!
          write_timeout=None,
          inter_byte_timeout=None,
          exclusive=None,
    )

    # WebSocket connection
    #websocket_server = "ws://10.6.3.243:80"  # NCAN WiFi
    #websocket_server = "ws://192.168.1.145:80"  # Danish home wifi
    websocket_server = "ws://192.168.4.13:80"  # direct access-point mode
    ws = websocket.WebSocket()
    ws.connect(websocket_server)

    report("Connected to Websocket server")

    try:
        while True:
            # Read data from the headset
            if connection.in_waiting:
                handle_serial_data(connection, ws)

            # Check for data from the WebSocket
            if ws.connected and ws.sock:
                ready, _, _ = select.select([ws.sock], [], [], 0)
                if ws.sock in ready:
                    websocket_data = ws.recv() # TODO: waits here
                    # report("Received data from WebSocket server:", websocket_data)
                    handle_websocket_data(websocket_data, connection)
            power_check() # TODO: this takes 300ms so needs to be in a background thread

            time.sleep(0.001)  # Add a small delay to avoid excessive CPU usage

    except KeyboardInterrupt:
        ws.close()
        connection.close()
        report("Program terminated.")

