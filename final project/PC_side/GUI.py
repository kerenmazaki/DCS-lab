import tkinter as tk
from tkinter import filedialog
import serial as ser
import threading
import queue
import math

class StateManager:
    def __init__(self, root):
        self.root = root
        self.mode = "neutral"
        self.last_x, self.last_y = 200, 200
        self.current_state = None
        self.canvas = None
        self.mode_label = None
        self.calibration_label = None

        # Queue for thread-safe communication
        self.queue = queue.Queue()

        # Serial communication setup
        self.serial = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS,
                                 parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                                 timeout=1)
        self.serial.reset_input_buffer()
        self.serial.reset_output_buffer()

        # Start serial reading thread
        self.thread = threading.Thread(target=self.read_from_serial, daemon=True)
        self.thread.start()

        # Create the main application window
        self.create_main_window()

        # Start processing the queue
        self.process_queue()

        #center the window
        self.center_window(300, 200)

    def create_main_window(self):
        self.root.title("Main Menu")
        tk.Button(self.root, text="Manual control of motor based machine", command=self.state1_func).pack(pady=10)
        tk.Button(self.root, text="Joystick based PC painter", command=self.state2_func).pack(pady=10)
        tk.Button(self.root, text="Stepper Motor Calibration", command=self.state3_func).pack(pady=10)
        tk.Button(self.root, text="Script Mode", command=self.state4_func).pack(pady=10)

    def read_from_serial(self):
        while True:
            if self.serial.in_waiting > 0:
                line = self.serial.read_until(b'\n').decode("ascii").strip()
                if "\x00" in line:
                    pass
                elif line: # Check if line is not empty
                    self.queue.put(line)

    def process_queue(self):
        try:
            while True:
                line = self.queue.get_nowait()
                print("recieved ",line)
                self.handle_mcu_data(line)
        except queue.Empty:
            pass
        finally:
            self.root.after(100, self.process_queue)

    def handle_mcu_data(self, data):

        if data.startswith("2"):  # Painter data
            _, action, angle = data.split(',')
            angle = float(angle)  # Convert angle to float
            if self.canvas:
                # Define the radius for movement (you can adjust this value as needed)
                radius = 10

                # Adjust angle for clockwise direction with 0 degrees pointing left
                angle_rad = math.pi - angle
                #angle_rad = math.radians(adjusted_angle)

                # Calculate new position based on angle and radius
                dx = radius * math.cos(angle_rad)
                dy = -radius * math.sin(angle_rad)
                
                # Compute the new position
                new_x = self.last_x + dx
                new_y = self.last_y + dy

                # Clamp positions to stay within canvas boundaries
                new_x = max(0, min(new_x, self.canvas.winfo_width()))
                new_y = max(0, min(new_y, self.canvas.winfo_height()))

                # Remove the old cursor
                self.canvas.delete(self.cursor_id)

                # Draw a new cursor at the new position
                self.cursor_id = self.canvas.create_oval(new_x - 2, new_y - 2, new_x + 2, new_y + 2, fill="red")


                # Draw on the canvas based on the action
                if action == 'w':  # Write
                    self.mode = "write"
                    self.canvas.create_line(self.last_x, self.last_y, new_x, new_y, fill="black", width=2)
                elif action == 'd':  # Delete
                    self.mode = "delete"
                    self.canvas.create_rectangle(self.last_x, self.last_y, new_x, new_y, outline="white", fill="white")
                elif action == 'n':  # Neutral
                    self.mode = "neutral"
                # Update the last position
                self.last_x, self.last_y = new_x, new_y
                self.mode_label.config(text=f"Current mode: {self.mode}")
                

        elif data.startswith("3"):  # Calibration data
            _, steps, step_angle = data.split(',')
            if self.calibration_label:
                # if step_angle > 2*math.pi:
                #     step_angle -= 2*math.pi
                # elif step_angle < 0:
                #     step_angle += 2*math.pi

                self.calibration_label.config(text=f"Steps per round: {steps}\nAngle: {(float(step_angle)*180/math.pi):.3f}°")
       
        elif data.startswith("4"):  # Calibration data
            _, motor_angle = data.split(',')
            if self.motor_angle:
                self.motor_angle.config(text=f"Motor Angle: {(float(motor_angle)*180/math.pi):.3f}°")

    def send_to_serial(self, data):
        self.serial.write(bytes(data + '\n', 'ascii'))
        print('sent: ', (data))

    def center_window(self, width, height):
        """Centers the window on the screen and sets the size"""
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()

        x = (screen_width // 2) - (width // 2)
        y = (screen_height // 2) - (height // 2)

        self.root.geometry(f"{width}x{height}+{x}+{y}")
    
    def done_function(self,state_menu):
        state_menu.destroy()
        self.last_x, self.last_y = 200, 200
        self.send_to_serial("STATE0")

    def toggle_calibration(self):
        """Toggle between start and stop calibration states"""
        if not hasattr(self, 'calibration_button') or self.calibration_button is None:
            print("Calibration button not initialized")
            return
        
        if self.calibration_button["text"] == "Start Calibration":
            self.send_to_serial("1START")
            self.calibration_button.config(text="Stop Calibration")
        else:
            self.send_to_serial("1STOP")
            self.calibration_button.config(text="Start Calibration")
            
    def state1_func(self):
        """Manual control of motor based machine"""
        self.send_to_serial("STATE1")
        self.current_state = 1
        state1_menu = tk.Toplevel(self.root)
        state1_menu.title("Manual Motor Control")

        tk.Label(state1_menu, text="Use joystick to control motor angle (0-360 degrees)").pack(pady=10)

        # Create a button with initial text "start calibration"
        self.calibration_button = tk.Button(state1_menu, text="Start Calibration", command=self.toggle_calibration)
        self.calibration_button.pack(pady=10)

        tk.Button(state1_menu, text="Done", command=lambda: self.done_function(state1_menu)).pack(pady=10)


    def state2_func(self):
        """Joystick based PC painter"""
        self.send_to_serial("STATE2")
        self.current_state = 2
        state2_painter = tk.Toplevel(self.root)
        state2_painter.title("PC Painter")
        
        self.mode = "write"  # Initial mode
        self.mode_label = tk.Label(state2_painter, text=f"Current mode: {self.mode}")
        self.mode_label.pack(pady=10)
        
        self.canvas = tk.Canvas(state2_painter, width=400, height=400, bg="white")
        self.canvas.pack()
        
        tk.Label(state2_painter, text="Use joystick to control the painter").pack(pady=10)
        tk.Label(state2_painter, text="Press joystick to cycle modes: Write -> Erase -> Neutral").pack(pady=5)
        
        tk.Button(state2_painter, text="Done", command= lambda: self.done_function(state2_painter)).pack(pady=10)

        self.cursor_id = self.canvas.create_oval(self.last_x - 2, self.last_y - 2, self.last_x + 2, self.last_y + 2, fill="red")

    def state3_func(self):
        """Stepper Motor Calibration"""
        self.send_to_serial("STATE3")
        self.current_state = 3
        state3_menu = tk.Toplevel(self.root)
        state3_menu.title("Stepper Motor Calibration")
        
        self.calibration_label = tk.Label(state3_menu, text="Waiting for calibration data...")
        self.calibration_label.pack(pady=10)
        
        tk.Label(state3_menu, text="Press joystick to start/stop calibration").pack(pady=10)
        tk.Button(state3_menu, text="Done", command=lambda: self.done_function(state3_menu)).pack(pady=10)

    def state4_func(self):
        """Script Mode"""
        self.send_to_serial("STATE4")
        self.current_state = 4
        state4_menu = tk.Toplevel(self.root)
        state4_menu.title("Script Mode")

        self.motor_angle = tk.Label(state4_menu, text="Waiting for stepper_deg Function")
        self.motor_angle.pack(pady=10)
        
        for script_no in range(1, 4):
            script_frame = tk.Frame(state4_menu)
            script_frame.pack(pady=10)
            
            tk.Button(script_frame, text=f"Load Script {script_no}", command=lambda s=script_no: self.load_script(s)).pack(side=tk.LEFT, padx=5)
            tk.Button(script_frame, text=f"Run Script {script_no}", command=lambda s=script_no: self.send_to_serial(f"4R,{s}")).pack(side=tk.LEFT, padx=5)
        
        tk.Button(state4_menu, text="Done", command=lambda: self.done_function(state4_menu)).pack(pady=10)

    def load_script(self, script_no):
        filename = tk.filedialog.askopenfilename(filetypes=[("Text files", "*.txt")])
        if filename:
            with open(filename, 'r') as file:
                script_content = file.read()
            hex_script = self.convert_script_to_hex(script_content)
            self.send_to_serial(f"4L,{script_no},{hex_script}\n")

    def convert_script_to_hex(self, script):
            hex_commands = []
            for line in script.split('\n'):
                parts = line.strip().split(maxsplit=1)
                if not parts:
                    continue
                command = parts[0].lower()
                if command == 'inc_lcd':
                    hex_commands.append(f"01{int(parts[1]):02X}")
                elif command == 'dec_lcd':
                    hex_commands.append(f"02{int(parts[1]):02X}")
                elif command == 'rra_lcd':
                    hex_commands.append(f"03{ord(parts[1]):02X}")
                elif command == 'set_delay':
                    hex_commands.append(f"04{int(parts[1]):02X}")
                elif command == 'clear_all_leds':
                    hex_commands.append("05")
                elif command == 'stepper_deg':
                    hex_commands.append(f"06{int(parts[1]):02X}")
                elif command == 'stepper_scan':
                    left, right = map(int, parts[1].split(','))
                    hex_commands.append(f"07{left:02X}{right:02X}")
                elif command == 'sleep':
                    hex_commands.append("08")
            return ','.join(hex_commands)

# Create the main application window
root = tk.Tk()
app = StateManager(root)

# Run the application
root.mainloop()