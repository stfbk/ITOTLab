#!/usr/bin/env python3

import argparse
import asyncio
import numpy
import signal
import sys
import time

import json
import paho.mqtt.client as mqtt

"""
This program is used to provide access to the values (temperature, vibration and orientation)
acquired from the Arduino's sensors and made available through Modbus/TCP.

It is important to conform to the "Modbus jargon" and to make sure that the functions
used in this Python program and the ones used in the Arduino's Sketch are coherent.

The temperature is an analog input and the value is stored in two input registers with
offset 0.

The vibration and the orientation values (acceleration on X, Y and Z axis and their orientation)
are provided through other nine registers, with offset 2.
"""

from pyModbusTCP.client import ModbusClient

def sigint_handler(sig, frame):
	"""Handles the SIGINT signals and exits appropriately."""

	print("[+] Exiting...")
	sys.exit(0)

def write_slave(payload):
	"""Writes the emergency coil on the Modbus slave"""

	if payload == "1":
		c.write_single_coil(0,True)
		return

	c.write_single_coil(0,False)
 
def read_slave():
	"""Reads the registers from the Modbus slave
	
	This method reads the registers from the Modbus slave. Be careful that on the slave, the
	four 32bit float values hree 8bit values and the thave been converted into an uint16_t bit array,
	composed as follows:

	regs = [temp,	temp,
			acc_x,	acc_x,	acc_y,	acc_y,	acc_z,	acc_z,
			or_x,	or_y,	or_z]
	"""

	int_regs = c.read_input_registers(0, 11)

	v_dic = {"Temperature": "",
			"Acceleration (X,Y,Z)": "",
			"Orientation (X,Y,Z)": ""}

	if int_regs:
		regs=numpy.array(int_regs, numpy.int16)
		regs_float = regs[0:8]
		regs_float.dtype = numpy.float32

		v_dic['Temperature'] = "{}".format(regs_float[0])
		v_dic['Acceleration (X,Y,Z)'] = "{}".format(regs_float[1:4])
		v_dic['Orientation (X,Y,Z)'] = "{}".format(regs[8:])

		print("\tTemperature: {}".format(v_dic['Temperature']))
		print("\tAcceleration: (X,Y,Z): {}".format(v_dic['Acceleration (X,Y,Z)']))
		print("\tOrientation: (X,Y,Z): {}".format(v_dic['Orientation (X,Y,Z)']))
	else:
		print("[+] Slave read error!")

	return json.dumps(v_dic)

def on_connect(client, userdata, flags, rc):
	"""Handler for the on_connect MQTT event"""

	print("[+] Connected to MQTT broker with result code " + str(rc))
	print("[+] Subscribing to topics...")
	client.subscribe("FBKSensors")
	client.subscribe("FBKSensors_Alarm")

def on_message(client, userdata, msg):
	"""Handler for the on_message MQTT event """
	if msg.topic == "FBKSensors_Alarm":
		print("[+] Got MQTT message")
		payload = str(msg.payload.decode("utf-8"))
		write_slave(payload)

if __name__ == "__main__":

	# Register SIGINT signal handler
	signal.signal(signal.SIGINT, sigint_handler)

	# Parse arguments
	parser = argparse.ArgumentParser()
	parser.add_argument("-a", "--address",
						type=str,
						help="Server address(as IP:port)")
	parser.add_argument("-b", "--broker",
						type=str,
						help="MQTT broker address (as IP:port)")
	args = parser.parse_args()

	if 	args.address == None or args.broker == None:
		parser.print_help()
		sys.exit(1)

	address = args.address.split(":")
	if len(address) == 1:
		address.append(502)

	broker = args.broker.split(":")
	if len(broker) == 1:
		broker.append(1883)

	# Connect to the MQTT broker
	client = mqtt.Client("Raspberry", True, None, mqtt.MQTTv31)
	client.on_connect = on_connect
	client.on_message = on_message
	client.connect(broker[0], int(broker[1]))
	client.loop_start()

	# Main loop
	while True:
	
		print("[+] Reading Modbus/TCP from {}:{}".format(address[0], int(address[1])))
		c = ModbusClient(	host=address[0],
							port=address[1],
							auto_open=True,
							auto_close=True)

		regs = read_slave()
		client.publish("FBKSensors", regs)