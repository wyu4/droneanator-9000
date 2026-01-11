import socket
import threading
import queue
from time import time
from tkinter import *
from tkinter import ttk
import matplotlib
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib.figure import Figure
matplotlib.use("TkAgg")

DRONE_IP = "192.168.4.1"
DRONE_PORT = 57476
USER_PORT = 57476

PING_RATE = 1 # seconds
MAX_DISCONNECT = 3 # seconds

root = Tk()
status = StringVar(value="Disconnected")
content = ttk.Frame(root, width=192, height=108, padding=10)

parsed_data:dict[str, StringVar | dict[str, StringVar]] = {
    "outputs": [
        StringVar(value="0"), StringVar(value="0"), StringVar(value="0"), StringVar(value="0")
    ],
    "hoverOnly": StringVar(value="0"),
    "pidOutputs": {
        "pitch": StringVar(value="0"),
        "roll": StringVar(value="0"),
        "yaw": StringVar(value="0")
    },
    "deltaTime": StringVar(value="0"),
    "desired": {
        "throttle": StringVar(value="0"),
        "pitch": StringVar(value="0"),
        "roll": StringVar(value="0"),
        "yaw": StringVar(value="0"),
        "arm": StringVar(value="0")
    },
    "measured": {
        "pitch": StringVar(value="0"),
        "roll": StringVar(value="0"),
        "yaw": StringVar(value="0")
    },
    "preventThrottle": StringVar(value="0"),
    "disarmed": StringVar(value="0"),
    "timestamp": StringVar(value="0")
}
    
opened_graph = -1
    
class PIDGraph:
    MAX_SIZE = 200
    def __init__(self, master:Frame, name:str, y_label:str, id:int):
        self.name = name
        self.master = master
        self.id = id
        self.figure = Figure(figsize=(6, 4), dpi=100)
        self.ax = self.figure.add_subplot(111)
        self.ax.set_title(name)
        self.ax.set_xlabel("Time (s)")
        self.ax.set_ylabel(y_label)
        self.measured, = self.ax.plot([], [], label="Measured", color="red")
        self.setpoint, = self.ax.plot([], [], label="Setpoint", color="green")
        self.ax.legend()
        self.canvas = FigureCanvasTkAgg(self.figure, master=master)
        self.canvas.get_tk_widget().pack(fill="both", expand=True)
        self.queue = queue.Queue()
        
        self.time_data = []
        self.measured_data = []
        self.setpoint_data = []
        
    def queue_data(self, measured:str, setpoint:str, time:str):
        if opened_graph != self.id:
            return
        try:
            self.queue.put_nowait((float(measured), float(setpoint), float(time)))
        except queue.Full:
            print(f'Graph "{self.name}"\'s queue is full.')
        except TypeError as et:
            print(f'Graph "{self.name}" could not convert raw data to float: {str(et)}')
            return
      
    def update(self):
        updated = False
        
        try:
            while True:
                measured, setpoint, time = self.queue.get_nowait()
                
                time_size = len(self.time_data)
                
                if time_size > 0 and time <= self.time_data[time_size-1]: # Delete data if graph restarts (moves back in time)
                    self.measured_data.clear()
                    self.setpoint_data.clear()
                    self.time_data.clear()
                
                self.measured_data.append(measured)
                self.setpoint_data.append(setpoint)
                self.time_data.append(time)
                
                updated = True
                
                if time_size > self.MAX_SIZE:
                    self.measured_data.pop(0)
                    self.setpoint_data.pop(0)
                    self.time_data.pop(0)
        except queue.Empty:
            pass
        except queue.Full:
            print(f'Could not update graph "{self.name}": queue is full.')
        except Exception as e:
            print(f'Could not update graph "{self.name}": {str(e)}')
        if updated:
            self.measured.set_data(self.time_data, self.measured_data)
            self.setpoint.set_data(self.time_data, self.setpoint_data)

            self.ax.relim()
            self.ax.autoscale_view()
            
            self.canvas.draw_idle()
        
graph_notebook = ttk.Notebook(content)
        
pitch_graph_frame = ttk.Frame(content)
pitch_graph = PIDGraph(pitch_graph_frame, "Pitch", "Measured Pitch (deg)", 0)

roll_graph_frame = ttk.Frame(content)
roll_graph = PIDGraph(roll_graph_frame, "Roll", "Measured Roll (deg)", 1)

yaw_graph_frame = ttk.Frame(content)
yaw_graph = PIDGraph(yaw_graph_frame, "Yaw", "Measured Yaw (deg/s)", 2)

graph_notebook.add(pitch_graph_frame, text="Pitch")
graph_notebook.add(roll_graph_frame, text="Roll")
graph_notebook.add(yaw_graph_frame, text="Yaw")
graph_notebook.add(ttk.Label(content, text="No graphs selected."), text="None")
graph_notebook.select(3)

def update_graphs():
    global opened_graph
    opened_graph = graph_notebook.index(graph_notebook.select())
    pitch_graph.update()
    roll_graph.update()
    yaw_graph.update()
    root.after(30, update_graphs)
    
def connection():
    last_ping = 0
    last_connect = 0
    
    success = False
    while not success:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.bind(("0.0.0.0", USER_PORT))
            sock.connect((DRONE_IP, USER_PORT))
            sock.settimeout(0.1)
            success = True
        except Exception as e:
            print(f"Could not start socket: {str(e)}")
            success = False
        
    while True:
        now = time()
        try:
            if (now - last_ping > PING_RATE):
                sock.sendto(b'heartbeat', (DRONE_IP, DRONE_PORT))
                last_ping = now
        
            raw, _ = sock.recvfrom(512)
            if raw:
                message = raw.decode("utf-8")
                status.set("Connected")
                last_connect = now
                if (message.startswith("$data")):
                    data = message.removeprefix("$data ").split(" ")
                    if len(data) >= 18:
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
                        parsed_data["timestamp"].set(f'{data[19]}s')
                        pitch_graph.queue_data(data[14], data[10], data[19])
                        roll_graph.queue_data(data[15], data[11], data[19])
                        yaw_graph.queue_data(data[16], data[12], data[19])
        except socket.timeout:
            if (now - last_connect) > MAX_DISCONNECT:
                status.set("Disconnected")
            continue
        except Exception as e:
            print(f'An exception occurred in the connection loop: {str(e)}')

# Silently run the connection code in the background
threading.Thread(target=connection, daemon=True).start()
        
# GUI
root.title("Droneanator Pilot")
desired_frame = ttk.Frame(content, padding=20)
measured_frame = ttk.Frame(content, padding=20)
pid_frame = ttk.Frame(content, padding=20)
output_frame = ttk.Frame(content, padding=20)
footer_frame = ttk.Frame(content)
hover_frame = ttk.Frame(footer_frame, padding=10)
throttleoff_frame = ttk.Frame(footer_frame, padding=10)
disarm_frame = ttk.Frame(footer_frame, padding=10)
deltatime_frame = ttk.Frame(footer_frame, padding=10)
timestamp_frame = ttk.Frame(footer_frame, padding=10)

for i in range(4):
    content.columnconfigure(i, weight=1)
for i in range(5):
    footer_frame.columnconfigure(i, weight=1)


ttk.Label(content, anchor="center", textvariable=status, font=("Courier", 15, "bold")).grid(column=0, row=0, columnspan=5)

style = ttk.Style()
style.configure(".", font=("Helvetica", 12))

content.grid(column=0, row=0)
graph_notebook.grid(column=0, row=2)
desired_frame.grid(column=1, row=2, sticky="N")
ttk.Label(desired_frame, text="Desired", anchor="center", font=("Helvetica", 13, "bold")).grid(column=0, row=0, columnspan=2)
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

measured_frame.grid(column=2, row=2, sticky="N")
ttk.Label(measured_frame, text="Measured", anchor="center", font=("Helvetica", 13, "bold")).grid(column=0, row=0, columnspan=2)
ttk.Label(measured_frame, text="Pitch:", anchor="e").grid(column=0, row=1)
ttk.Label(measured_frame, text="Yaw:", anchor="e").grid(column=0, row=2)
ttk.Label(measured_frame, text="Roll:", anchor="e").grid(column=0, row=3)
ttk.Label(measured_frame, anchor="w", textvariable=parsed_data["measured"]["pitch"], relief="sunken").grid(column=1, row=1)
ttk.Label(measured_frame, anchor="w", textvariable=parsed_data["measured"]["yaw"], relief="sunken").grid(column=1, row=2)
ttk.Label(measured_frame, anchor="w", textvariable=parsed_data["measured"]["roll"], relief="sunken").grid(column=1, row=3)
ttk.Label(measured_frame).grid(column=0, row=4, columnspan=2)
ttk.Label(measured_frame).grid(column=0, row=5, columnspan=2)

pid_frame.grid(column=3, row=2, sticky="N")
ttk.Label(pid_frame, text="PID", anchor="center", font=("Helvetica", 13, "bold")).grid(column=0, row=0, columnspan=2)
ttk.Label(pid_frame, text="Pitch:", anchor="e").grid(column=0, row=1)
ttk.Label(pid_frame, text="Yaw:", anchor="e").grid(column=0, row=2)
ttk.Label(pid_frame, text="Roll:", anchor="e").grid(column=0, row=3)
ttk.Label(pid_frame, anchor="w", textvariable=parsed_data["pidOutputs"]["pitch"], relief="sunken").grid(column=1, row=1)
ttk.Label(pid_frame, anchor="w", textvariable=parsed_data["pidOutputs"]["yaw"], relief="sunken").grid(column=1, row=2)
ttk.Label(pid_frame, anchor="w", textvariable=parsed_data["pidOutputs"]["roll"], relief="sunken").grid(column=1, row=3)
ttk.Label(pid_frame).grid(column=0, row=4, columnspan=2)
ttk.Label(pid_frame).grid(column=0, row=5, columnspan=2)

output_frame.grid(column=4, row=2, sticky="N")
ttk.Label(output_frame, text="Motor Output", anchor="center", font=("Helvetica", 13, "bold")).grid(column=0, row=0, columnspan=2)
ttk.Label(output_frame, anchor="center", textvariable=parsed_data["outputs"][3], relief="sunken").grid(column=0, row=1) # FL
ttk.Label(output_frame, anchor="center", textvariable=parsed_data["outputs"][0], relief="sunken").grid(column=1, row=1) # FR
ttk.Label(output_frame, anchor="center", textvariable=parsed_data["outputs"][2], relief="sunken").grid(column=0, row=2) # BL
ttk.Label(output_frame, anchor="center", textvariable=parsed_data["outputs"][1], relief="sunken").grid(column=1, row=2) # BR



footer_frame.grid(column=0, row=3, columnspan=5)
hover_frame.grid(column=0)
ttk.Label(hover_frame, text="Hover Mode", anchor="center", font=("Helvetica", 12, "bold")).grid(column=0, row=0)
ttk.Label(hover_frame, anchor="center", textvariable=parsed_data["hoverOnly"], relief="sunken").grid(column=0, row=1)
throttleoff_frame.grid(column=1, row=0)
ttk.Label(throttleoff_frame, text="Throttle Off", anchor="center", font=("Helvetica", 12, "bold")).grid(column=0, row=0)
ttk.Label(throttleoff_frame, anchor="center", textvariable=parsed_data["preventThrottle"], relief="sunken").grid(column=0, row=1)
disarm_frame.grid(column=2, row=0)
ttk.Label(disarm_frame, text="Disarmed", anchor="center", font=("Helvetica", 12, "bold")).grid(column=0, row=0)
ttk.Label(disarm_frame, anchor="center", textvariable=parsed_data["disarmed"], relief="sunken").grid(column=0, row=1)
deltatime_frame.grid(column=3, row=0)
ttk.Label(deltatime_frame, text="ΔTime", anchor="center", font=("Helvetica", 12, "bold")).grid(column=0, row=0)
ttk.Label(deltatime_frame, anchor="center", textvariable=parsed_data["deltaTime"], relief="sunken").grid(column=0, row=1)
timestamp_frame.grid(column=4, row=0)
ttk.Label(timestamp_frame, text="Timestamp", anchor="center", font=("Helvetica", 12, "bold")).grid(column=0, row=0)
ttk.Label(timestamp_frame, anchor="center", textvariable=parsed_data["timestamp"], relief="sunken").grid(column=0, row=1)

update_graphs()
root.mainloop()