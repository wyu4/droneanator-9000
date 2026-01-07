import socket
import threading
from time import time
from tkinter import *
from tkinter import ttk

DRONE_IP = "192.168.4.1"
DRONE_PORT = 57476
USER_PORT = 57476

PING_RATE = 1 # seconds
MAX_UPDATE_RATE = 3 # seconds

root = Tk()
status = StringVar(value="Disconnected")

parsed_data:dict[str, StringVar | dict[str, StringVar]] = {
    "outputs": [
        StringVar(value="0000"), StringVar(value="0000"), StringVar(value="0000"), StringVar(value="0000")
    ],
    "hoverOnly": StringVar(value="?"),
    "pidOutputs": {
        "pitch": StringVar(value="0000"),
        "roll": StringVar(value="0000"),
        "yaw": StringVar(value="0000")
    },
    "deltaTime": StringVar(value="00000ms"),
    "desired": {
        "throttle": StringVar(value="0000"),
        "pitch": StringVar(value="00"),
        "roll": StringVar(value="00"),
        "yaw": StringVar(value="00"),
        "arm": StringVar(value="0000")
    },
    "measured": {
        "pitch": StringVar(value="000"),
        "roll": StringVar(value="000"),
        "yaw": StringVar(value="00")
    },
    "preventThrottle": StringVar(value="0"),
    "disarmed": StringVar(value="0"),
    "timestamp": StringVar(value="000000.0ms")
}

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("0.0.0.0", USER_PORT))
sock.connect((DRONE_IP, USER_PORT))
sock.settimeout(0.1)
    
def connection():
    last_ping = 0
    last_update = 0
        
    while True:
        try:
            now = time()
            if (now - last_ping > PING_RATE):
                sock.sendto(b'heartbeat', (DRONE_IP, DRONE_PORT))
                last_ping = now
        
            raw, _ = sock.recvfrom(512)
            if raw:
                last_update = now
                message = raw.decode("utf-8")
                if (message.startswith("$data")):
                    data = message.removeprefix("$data ").split(" ")
                    if len(data) < 18: return
                    parsed_data["outputs"][0].set(data[0])
                    parsed_data["outputs"][1].set(data[1])
                    parsed_data["outputs"][2].set(data[2])
                    parsed_data["outputs"][3].set(data[3])
                    parsed_data["hoverOnly"].set(data[4])
                    parsed_data["pidOutputs"]["pitch"].set(data[5])
                    parsed_data["pidOutputs"]["roll"].set(data[6])
                    parsed_data["pidOutputs"]["yaw"].set(data[7])
                    parsed_data["deltaTime"].set(f'{data[8]}ms')
                    parsed_data["desired"]["throttle"].set(data[9])
                    parsed_data["desired"]["pitch"].set(data[10])
                    parsed_data["desired"]["roll"].set(data[11])
                    parsed_data["desired"]["yaw"].set(data[12])
                    parsed_data["desired"]["arm"].set(data[13])
                    parsed_data["measured"]["pitch"].set(data[14])
                    parsed_data["measured"]["roll"].set(data[15])
                    parsed_data["measured"]["yaw"].set(data[16])
                    parsed_data["preventThrottle"].set(data[17])
                    parsed_data["disarmed"].set(data[18])
                    parsed_data["timestamp"].set(f'{data[19]}ms')
            if ((now - last_update) > MAX_UPDATE_RATE):
                status.set("Disconnected")
            else:
                status.set("Connected")
        except socket.timeout:
            continue
        except Exception as e:
            print(f'An exception occurred in the connection loop: {str(e)}')

# Silently run the connection code in the background
threading.Thread(target=connection, daemon=True).start()
        
# GUI
root.title("Droneanator Pilot")

content = ttk.Frame(root, width=192, height=108, padding=10)
desired_frame = ttk.Frame(content, padding=5)
measured_frame = ttk.Frame(content, padding=5)
pid_frame = ttk.Frame(content, padding=5)
output_frame = ttk.Frame(content, padding=5)
footer_frame = ttk.Frame(content)

ttk.Label(content, anchor="center", textvariable=status, font=("Courier", 15, "bold")).grid(column=0, row=0, columnspan=4)

style = ttk.Style()
style.configure(".", font=("Helvetica", 12))

content.grid(column=0, row=0)

desired_frame.grid(column=0, row=2)
ttk.Label(content, text="Desired", anchor="center", font=("Helvetica", 13, "bold")).grid(column=0, row=1)
ttk.Label(desired_frame, text="Throttle:", anchor="e").grid(column=0, row=1)
ttk.Label(desired_frame, text="Pitch:", anchor="e").grid(column=0, row=2)
ttk.Label(desired_frame, text="Yaw:", anchor="e").grid(column=0, row=3)
ttk.Label(desired_frame, text="Roll:", anchor="e").grid(column=0, row=4)
ttk.Label(desired_frame, text="Arm:", anchor="e").grid(column=0, row=5)
ttk.Label(desired_frame, anchor="w", textvariable=parsed_data["desired"]["throttle"], relief="sunken").grid(column=1, row=1)
ttk.Label(desired_frame, anchor="w", textvariable=parsed_data["desired"]["pitch"], relief="sunken").grid(column=1, row=2)
ttk.Label(desired_frame, anchor="w", textvariable=parsed_data["desired"]["yaw"], relief="sunken").grid(column=1, row=3)
ttk.Label(desired_frame, anchor="w", textvariable=parsed_data["desired"]["roll"], relief="sunken").grid(column=1, row=4)
ttk.Label(desired_frame, anchor="w", textvariable=parsed_data["desired"]["arm"], relief="sunken").grid(column=1, row=5)

measured_frame.grid(column=1, row=2)
ttk.Label(content, text="Measured", anchor="center", font=("Helvetica", 13, "bold")).grid(column=1, row=1)
ttk.Label(measured_frame, text="Pitch:", anchor="e").grid(column=0, row=1)
ttk.Label(measured_frame, text="Yaw:", anchor="e").grid(column=0, row=2)
ttk.Label(measured_frame, text="Roll:", anchor="e").grid(column=0, row=3)
ttk.Label(measured_frame, anchor="w", textvariable=parsed_data["measured"]["pitch"], relief="sunken").grid(column=1, row=1)
ttk.Label(measured_frame, anchor="w", textvariable=parsed_data["measured"]["yaw"], relief="sunken").grid(column=1, row=2)
ttk.Label(measured_frame, anchor="w", textvariable=parsed_data["measured"]["roll"], relief="sunken").grid(column=1, row=3)

pid_frame.grid(column=2, row=2)
ttk.Label(content, text="PID", anchor="center", font=("Helvetica", 13, "bold")).grid(column=2, row=1)
ttk.Label(pid_frame, text="Pitch:", anchor="e").grid(column=0, row=1)
ttk.Label(pid_frame, text="Yaw:", anchor="e").grid(column=0, row=2)
ttk.Label(pid_frame, text="Roll:", anchor="e").grid(column=0, row=3)
ttk.Label(pid_frame, anchor="w", textvariable=parsed_data["pidOutputs"]["pitch"], relief="sunken").grid(column=1, row=1)
ttk.Label(pid_frame, anchor="w", textvariable=parsed_data["pidOutputs"]["yaw"], relief="sunken").grid(column=1, row=2)
ttk.Label(pid_frame, anchor="w", textvariable=parsed_data["pidOutputs"]["roll"], relief="sunken").grid(column=1, row=3)

output_frame.grid(column=3, row=2)
ttk.Label(content, text="Motor Output", anchor="center", font=("Helvetica", 13, "bold")).grid(column=3, row=1)
ttk.Label(output_frame, anchor="center", textvariable=parsed_data["outputs"][3], relief="sunken").grid(column=1, row=0) #FL
ttk.Label(output_frame, anchor="center", textvariable=parsed_data["outputs"][0], relief="sunken").grid(column=1, row=1) #FR
ttk.Label(output_frame, anchor="center", textvariable=parsed_data["outputs"][2], relief="sunken").grid(column=2, row=0) #BL
ttk.Label(output_frame, anchor="center", textvariable=parsed_data["outputs"][1], relief="sunken").grid(column=2, row=1) #BR

footer_frame.grid(column=0, row=3)

root.mainloop()