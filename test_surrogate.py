import os
import sys
import time
import struct
import threading

import serial    # python -m pip install pyserial

# Content from utils.py
def EncodeFakeCommand(payload):
    try:
        payload = payload.encode("utf8")
    except AttributeError:
        payload = bytes(payload)
    return bytes([len(payload)]) + payload

def EncodeFakeSignalBlock(payload):
    try:
        payload = payload.encode("utf8")
    except AttributeError:
        payload = bytes(payload)
    payload += b"\x0D\x0A"
    restOfHeader = b"\xFF" * 10
    blockSize = 4 + len(restOfHeader) + len(payload)
    return (
        b"\x23\x32"
        + struct.pack("<H", blockSize)
        + restOfHeader
        + payload
    )

# The original content of test_surrogate.py
portAddr = (
    sys.argv[1] if getattr(sys, "argv", [])[1:] else "COM5"
)

# baud=921600 data=8 parity=N stop=1 dtr=on rts=on
s = serial.Serial(
    port=portAddr,
    baudrate=921600,
    bytesize=8,
    parity="N",
    stopbits=1,
    timeout=None,
    xonxoff=False,
#    rtscts=True,        # ESP32 doesn't like this to be True
#    dsrdtr=True,        # ESP32 and ItsyBitsy M4 don't like this to be True (nor does the headset itself)
    write_timeout=None,
    inter_byte_timeout=None,
    exclusive=None,
)

mutex = threading.Lock()

def report(s):
    sys.stdout.write(s + "\n")
    try:
        sys.stdout.flush()
    except:
        pass

GO = [1]
def receiver():
    report('receiver starting')
    GO[:] = [1]
    packet_count = 0
    byte_count = 0
    start_time = time.time()
    while GO:
        with mutex:
            intro = ''
            if s.in_waiting >= 2: intro = s.read(2)
            if intro == b'#2':
                sizeBytes = s.read(2)
                size, = struct.unpack("<H", sizeBytes)
                payload = s.read(size - 4)
                #report(repr(intro + sizeBytes + payload))
                # Increment packet and byte counters
                packet_count += 1
                byte_count += size
                elapsed_time = time.time() - start_time
                if elapsed_time >= 1.0:  # Update the console every 1 second
                    report("Packets per second: %d, Bytes per second: %d" % (packet_count/elapsed_time, byte_count/elapsed_time))
                    # Reset counters and start time
                    packet_count = 0
                    byte_count = 0
                    start_time = time.time()
            elif intro:
                payload = intro
                while not payload.endswith(b'\n'): payload += s.read(1)
                report('Message from base station: ' + repr(payload))
        time.sleep(0.001)
    report("receiver ending")

def sender():
    try:
        while True:
            cmd = input("Command: ").strip()
            if cmd.lower() == "quit":
                break
            if not cmd:
                continue
            cmd = EncodeFakeCommand(cmd)
            report("sending %r" % cmd)
            with mutex:
                s.write(cmd)
                s.flush()
    except (KeyboardInterrupt, EOFError):
        pass

def stop():
    GO[:] = []

if __name__ == "__main__":
    receiver_thread = threading.Thread(target=receiver)
    receiver_thread.start()
    sender()
    stop()
