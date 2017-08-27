import sendImageToArduino

uno = sendImageToArduino.imageSender(uport="/dev/ttyACM2")

uno.sendImage("/home/djames/lobo.bin")