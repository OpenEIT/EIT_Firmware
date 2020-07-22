#!/usr/bin/python
import time
import os
import RPi.GPIO as GPIO
import subprocess
import logging
import logging.config
import datetime 

#logger = logging.getLogger(__name__)

shutdownbut = 3
scriptbut 	= 13

scriptLED 	= 19
error 		= 26
programming = 16
success 	= 6
sound 		= 5

ble_unique_id = "sudo openocd -f /home/pi/spectra/test/ble_id.cfg 2>&1"

program_bluetooth	= "sudo openocd -f /home/pi/spectra/test/ble.cfg 2>&1"
# program_mainchip	= "sudo ../../JLink/JLinkExe -device ADUCM350 -if SWD -speed 115.2 -jtagconf -1,-1 -autoconnect 1 -ExitonError 1 -CommanderScript /home/pi/spectra/hex/JLinkCommandFile.jlink"
#
program_mainchip	= "sudo /home/pi/JLink/JLinkExe -device ADUCM350 -if SWD -speed 115.2 -jtagconf -1,-1 -autoconnect 1 -ExitonError 1 -CommanderScript /home/pi/spectra/hex/JLinkCommandFile.jlink"

# dc 			= 95 # duty cycle (0-100) for PWM pin
# this is for the shutdown button. The LED is automatically tied to the TX pin so needs no software. 
GPIO.setmode(GPIO.BCM)

# GPIO.setup(shutdownbut, GPIO.IN, pull_up_down=GPIO.PUD_UP)
# GPIO.wait_for_edge(shutdownbut, GPIO.FALLING)

# this is the script start button i.e. the red button. 
GPIO.setup(scriptbut, GPIO.IN, pull_up_down = GPIO.PUD_UP)
# GPIO.wait_for_edge(13, GPIO.FALLING)
# This is the RED button LED. This turns it on for 3 seconds. 
GPIO.setup(scriptLED, GPIO.OUT)
GPIO.output(scriptLED, GPIO.LOW)

GPIO.setup(sound, GPIO.OUT)
GPIO.output(sound, GPIO.LOW)

GPIO.setup(error, GPIO.OUT)
GPIO.output(error, GPIO.LOW)

GPIO.setup(programming, GPIO.OUT)
GPIO.output(programming, GPIO.LOW)

GPIO.setup(success, GPIO.OUT)
GPIO.output(success, GPIO.LOW)

def get_ble_uniqueid(): 
	uniqueidstring = ""	
	
	ble_str = subprocess.check_output(ble_unique_id,shell=True,stderr=subprocess.STDOUT)
	idx = ble_str.find('0x10000060')
	deviceids = ble_str[idx:len(ble_str)].split()
	uniqueidstring = deviceids[1]+deviceids[2]
	print (uniqueidstring)

	return uniqueidstring

# def get_ble_mac_address(): 
# 	# This is the raspberry Pi's UART not the Bluetooth chip id or address. 
# 	serialno = '00'
# 	# using bluez tools 
# 	get_mac_address = 'bt-device -l' 

# 	mac_strings = ''	
# 	try:
# 		mac_strings = subprocess.check_output(get_mac_address,shell=True,stderr=subprocess.STDOUT)
# 	except subprocess.CalledProcessError as e:			
# 		raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

# 	print (mac_strings)
# 	testseq="MINDSEYE_UART"
# 	if testseq in mac_strings: 
# 		print ('bluetooth local found mindseye')
# 		# Now parse out the MAC ADDRESS
# 		# print ()
# 		lhs,rhs = mac_strings.split('MINDSEYE_UART',1)
# 		macaddress = rhs 
# 		line = macaddress
# 		serialno = line.replace("(","").replace(")","").replace(":","").replace("\n","").replace(" ","")

# 	return serialno 
# Found UART: MINDSEYE_UART [C1:25:CA:10:0A:0B]
def program_spectra():

	result = False
	# default logfilename. 
	
	#log_dir = os.path.join(os.path.normpath(os.getcwd() + os.sep + os.pardir), 'logs')
	#log_fname = os.path.join(log_dir, 'log_file_name.log')
	# Create log file, with serial number ID for device. 
	#programmingflash(3)

	serialno = get_ble_uniqueid()
	if len(serialno) > 1: 
		logfilename = serialno+".txt"
	else: 
		logfilename = "problemboards"+".txt"
	print (serialno)
	f = open(logfilename,"a")
	f.write('Flashed at: %s\n' %datetime.datetime.now())

	ble_result = ''	
	try:
		ble_result = subprocess.check_output(program_bluetooth,shell=True,stderr=subprocess.STDOUT)
		#f.write(ble_result)
		#serialno = get_ble_mac_address()
		#logfilename = serialno+".txt"
		#logging.basicConfig(filename=logfilename, level=logging.INFO)
		#logging.info(ble_result.decode('utf-8'))
		#programmingflash(3)
	except subprocess.CalledProcessError as e:
		GPIO.output(error, GPIO.HIGH)
		GPIO.output(success, GPIO.LOW)
		GPIO.output(programming, GPIO.LOW)
		f.write('bluetooth failed\n')
		print ('bluetooth failed')
		tone3(500)
		makesound(500)
		tone2(500)
		#cmd = 'omxplayer --no-keys /home/pi/spectra/test/sounds/bluetooth_connect_error.mp3 &'
		#error_info = subprocess.check_output(cmd,shell=True,stderr=subprocess.STDOUT)				
		print ("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))
		main()
		#logging.info(error_info.decode('utf-8'))
		#raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))
	#programmingflash(3)	
	#logging.basicConfig( level=logging.INFO)
	#logging.basicConfig(filename=logfilename,format='%(asctime)s - %(message)s', level=logging.INFO)
	# f_handler = logging.FileHandler(logfilename)
	# f_handler.setLevel(logging.ERROR)
	# f_format = logging.Formatter('%(asctime)s - %(message)s')
	# f_handler.setFormatter(f_format)
	# logger.addHandler(f_handler)

	adu_result = ''
	try:
		adu_result = subprocess.check_output(program_mainchip,shell=True,stderr=subprocess.STDOUT)
		#programmingflash(3)
	except subprocess.CalledProcessError as e:
		GPIO.output(error, GPIO.HIGH)
		GPIO.output(success, GPIO.LOW)
		GPIO.output(programming, GPIO.LOW)
		f.write('adu failed\n')
		tone3(500)
		makesound(500)
		tone2(500)
		print ('adu failed')
		print ("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))
		main()
		#cmd = 'omxplayer --no-keys /home/pi/spectra/test/sounds/mainchip_connect_error.mp3 &'
		#subprocess.check_output(cmd,shell=True,stderr=subprocess.STDOUT)	
		#raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))
	#programmingflash(3)
	# print (ble_result)	
	#print (adu_result)	
	testseq="Verified OK"
	if testseq in ble_result: 
		print ('ble succeeded')
		result = True
		#serialno = get_ble_mac_address()
		f.write("ble succeeded\n")
		#logger.error('ble succeeded')
	else:
		GPIO.output(error, GPIO.HIGH)
		print ('ble failed')
		f.write("ble failed\n")
		#logger.error('ble failed')

	adutestseq="/home/pi/spectra/hex/openeit.hex"
	if adutestseq in adu_result: 
		print ('adu succeeded')		
		f.write("adu succeeded\n")
		#logger.error('adu succeeded')
	else:
		print ('adu failed')
		f.write("adu failed\n")
		if result == True: # if bluetooth worked but adu didn't then the whole thing failed. 
			result = False
	f.close() 
		#logger.error('adu failed')
	return result

def LEDflash(n):
	if n == 1:
		return 1
	else: 
		GPIO.output(scriptLED, GPIO.HIGH) # Turn on
		time.sleep(0.2)                  # Sleep for 1 second
		GPIO.output(scriptLED, GPIO.LOW)  # Turn off
		time.sleep(0.2)       
		n = n - 1           # 
		return LEDflash(n)

def programmingflash(n):
	if n == 1:
		return 1
	else: 
		GPIO.output(programming, GPIO.HIGH) # Turn on
		time.sleep(0.2)                  # Sleep for 1 second
		GPIO.output(programming, GPIO.LOW)  # Turn off
		time.sleep(0.2)       
		n = n - 1           # 
		return programmingflash(n)

def makesound(n):
	if n == 1:
		return 1
	else: 
		GPIO.output(sound, GPIO.HIGH) # Turn on
		time.sleep(0.0005)                  # Sleep for 1 second
		GPIO.output(sound, GPIO.LOW)  # Turn off
		time.sleep(0.0005)       
		n = n - 1           # 
		return makesound(n)

def tone2(n): 
	if n == 1:
		return 1
	else: 
		GPIO.output(sound, GPIO.HIGH) # Turn on
		time.sleep(0.001)                  # Sleep for 1 second
		GPIO.output(sound, GPIO.LOW)  # Turn off
		time.sleep(0.001)       
		n = n - 1           # 
		return tone2(n)

def tone3(n): 
	if n == 1:
		return 1
	else: 
		GPIO.output(sound, GPIO.HIGH) # Turn on
		time.sleep(0.00005)                  # Sleep for 1 second
		GPIO.output(sound, GPIO.LOW)  # Turn off
		time.sleep(0.00005)       
		n = n - 1           # 
		return tone3(n)

# middle A5 = 880Hz B5 = 987
notes = {"A":880,"B":987,"C":523,"D":587,"E":659,"F":698,"G":783}


def main():

	try:
		while 1:

			if GPIO.input(scriptbut): # button is released
				print ('flash')
				GPIO.output(scriptLED, GPIO.LOW)
				# Flash the LED to show we are ready to test. 	
				LEDflash(3)
			else: # button is pressed:
				print ('starting test sequence')
				GPIO.output(scriptLED, GPIO.HIGH)
				GPIO.output(programming, GPIO.HIGH)
				GPIO.output(success, GPIO.LOW)
				GPIO.output(error, GPIO.LOW)

				#serial = get_ble_mac_address()
				#print (serial)
				result = program_spectra()
				if not result: 
					print ('Device Failed')
					GPIO.output(error, GPIO.HIGH)
					GPIO.output(success, GPIO.LOW)
					tone3(500)
					makesound(500)
					tone2(500)

				else: 
					print ('Device Succeeded')
					GPIO.output(success, GPIO.HIGH)
					GPIO.output(error, GPIO.LOW)
					#tone2(500)
					makesound(500)
					tone3(500)

				GPIO.output(programming, GPIO.LOW)  # Turn off
				GPIO.output(scriptLED, GPIO.LOW)

	except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:

		print ('user forced exit')
		GPIO.cleanup() # cleanup all GPIO


# if__name__== "__main__"
main()