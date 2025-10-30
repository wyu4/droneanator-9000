import socket
import time

DRONE_IP = "192.168.4.1"
DRONE_PORT = 57476
USER_PORT = 57476

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("0.0.0.0", USER_PORT))
sock.settimeout(0.1)

def send_heartbeat():
    sock.sendto(b'heartbeat', (DRONE_IP, DRONE_PORT))
    print(f'Heartbeat sent to {DRONE_IP}:{DRONE_PORT}')
        
send_heartbeat()

while True:
    try:
        data, address = sock.recvfrom(1024)
        if not data:
            continue
        message = data.decode("utf-8")
        print(message)
    except socket.timeout:
        continue
    except KeyboardInterrupt:
            print("\nKeyboardInterrupt caught. Exiting gracefully.")
            break