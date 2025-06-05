
import socket
import struct
import os
import time
from Forza import Forza  # Assuming Forza class is defined in Forza.py

IP = '0.0.0.0'  # Receive any incoming UDP packet on this port
PORT = 1151  # Example port
ADDRESS = (IP, PORT)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(ADDRESS)
print(f"Listening for UDP packets on {IP}:{PORT}...")

forza = Forza('./fh5.dat')  # Assuming Forza class is defined in Forza.py
print(
    f"Offsets: RPM={forza.get_offset("CurrentEngineRpm")}, Speed={forza.get_offset("Speed")}, Gear={forza.get_offset("gear")}")

while True:
    data, addr = sock.recvfrom(512)  # FH5 packets are about 324 bytes
    # print("Packet length:", len(data))
    if len(data) < 324:
        continue
    # Offset:
    #   - Engine RPM: float at offset 16
    #   - Speed: float at offset 256
    #   - Gear: byte at offset 319

    rpm = forza.get_value(data, 'CurrentEngineRpm')
    speed = forza.get_value(data, 'Speed') * 3.6  # Convert m/s to km/h
    gear = forza.get_value(data, 'gear')
    # print("Gear:", gear)

    # for debugging, print the offsets

    print(f"RPM: {rpm:.0f}, Speed: {speed:.2f} km/h, Gear: {gear}")
