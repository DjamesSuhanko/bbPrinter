#!/usr/bin/env python
import serial
import os
import time
import sys

class imageSender(object):
    def __init__(self, uport='/dev/ttyACM0',uspeed=9600):
        self.port    = uport
        self.speed   = uspeed

        self.arduino = serial.Serial(self.port, self.speed, timeout=None)

        if not self.arduino.isOpen():
            print("Could not open port. Exiting...")
            exit(-1)

    def sendImage(self,fileToSend):
        if not os.path.isfile(fileToSend):
            print("File doesn't exists. Do you sure that passed full path and correct filename?")
            return
        print("Sending file to printer...")

        with open(fileToSend) as f:
            content = f.readlines()

        line_number = 1
        of_lines    = len(content)
        for line in content:
            if (len(line) < 1):
                print("NULL")
                return

            status = ''
            line = line
            self.arduino.write(line)
            print("Sending: %s" % line)
            #self.arduino.flushInput()
            #self.arduino.flushOutput()
            while not status == 'go\n':
                #self.arduino.flushInput()
                #self.arduino.flushOutput()
                status = self.arduino.read_until('\n')
                data_left = self.arduino.inWaiting()
                #status += self.arduino.read_all()
                print("Waiting for status...")
                print("Status: %s" % status)
                print("line number: %s " % line_number)
                print("Total lines: %s" % of_lines)

            line_number += 1
            status = ""
            print("Going to next line")

            timer = 0
        self.arduino.close()
