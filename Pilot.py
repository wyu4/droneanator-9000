import socket
from time import time

DRONE_IP = "192.168.4.1"
DRONE_PORT = 57476
USER_PORT = 57476

lastPing = -1000
pingRate = 1 # seconds

displayData:dict[str, str | dict[str, str]] = {
    "outputs": [
        "0", "0", "0", "0"
    ],
    "hoverOnly": "0",
    "pidOutputs": {
        "pitch": "0.0",
        "roll": "0.0",
        "yaw": "0.0"
    },
    "deltaTime": "0",
    "desired": {
        "throttle": "0.0",
        "pitch": "0.0",
        "roll": "0.0",
        "yaw": "0.0",
        "arm": "0"
    },
    "measured": {
        "pitch": "0.0",
        "roll": "0.0",
        "yaw": "0.0"
    },
    "preventThrottle": "1",
    "disarmed": "0"
}

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("0.0.0.0", USER_PORT))
sock.connect(("192.168.4.2", USER_PORT))
sock.settimeout(0.1)

def send_heartbeat():
    sock.sendto(b'heartbeat', (DRONE_IP, DRONE_PORT))
    # print(f'Heartbeat sent to {DRONE_IP}:{DRONE_PORT}')

def parseData(raw:list[str]):
    displayData = {
        "outputs": [
            raw[0], raw[1], raw[2], raw[3]
        ],
        "hoverOnly": raw[4],
        "pidOutputs": {
            "pitch": raw[5],
            "roll": raw[6],
            "yaw": raw[7]
        },
        "deltaTime": f'{raw[8]}uS',
        "desired": {
            "throttle": raw[9],
            "pitch": raw[10],
            "roll": raw[11],
            "yaw": raw[12],
            "arm": raw[13]
        },
        "measured": {
            "pitch": raw[14],
            "roll": raw[15],
            "yaw": raw[16]
        },
        "preventThrottle": raw[17],
        "disarmed": raw[18]
    }
    print(str(displayData) + "\n\n\n\n\n\n\n\n")

while True:
    try:
        if (time() - lastPing > pingRate):
            send_heartbeat()
            lastPing = time()
    
        data, address = sock.recvfrom(512)
        if not data:
            continue
        message = data.decode("utf-8")
        if (message.startswith("$data")):
            parseData(message.removeprefix("$data ").split(" "))
            continue
    except socket.timeout:
        continue
    except KeyboardInterrupt:
            print("\nKeyboardInterrupt caught. Exiting gracefully.")
            break