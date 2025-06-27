import tkinter as tk
from tkinter import ttk
import serial
import time

# Serial connection (update COM port)
try:
    arduino = serial.Serial('COM12', 9600, timeout=1)
    time.sleep(2)
except:
    print("⚠️ Could not connect to Arduino. Check COM port.")

# Function to send motor movement commands
def send_command(axis, value):
    try:
        value = int(float(value))  # First convert to float, then to int
        if value == 0:
            return  # No movement if slider is centered
        if arduino.is_open:
            command = f"{axis} {value}\n"
            arduino.write(command.encode())
    except ValueError:
        print(f"Invalid value received: {value}")

# Callback functions for each axis
def on_x_change(value):
    send_command('X', value)

def on_y_change(value):
    send_command('Y', value)

def on_z_change(value):
    send_command('Z', value)

# GUI setup
root = tk.Tk()
root.title("Stepper Motor Slider Controller")
root.geometry("400x400")

style = ttk.Style()
style.configure("TScale", sliderlength=20, troughcolor='gray', background='lightblue')

tk.Label(root, text="X Axis").pack(pady=5)
x_slider = ttk.Scale(root, from_=-200, to=200, orient='horizontal', command=on_x_change)
x_slider.pack(fill='x', padx=20)
x_slider.set(0)

tk.Label(root, text="Y Axis").pack(pady=5)
y_slider = ttk.Scale(root, from_=-200, to=200, orient='horizontal', command=on_y_change)
y_slider.pack(fill='x', padx=20)
y_slider.set(0)

tk.Label(root, text="Z Axis").pack(pady=5)
z_slider = ttk.Scale(root, from_=-200, to=200, orient='horizontal', command=on_z_change)
z_slider.pack(fill='x', padx=20)
z_slider.set(0)

def reset_sliders():
    x_slider.set(0)
    y_slider.set(0)
    z_slider.set(0)

tk.Button(root, text="Reset Sliders", command=reset_sliders).pack(pady=20)
tk.Button(root, text="Exit", command=root.destroy).pack()

root.mainloop()
