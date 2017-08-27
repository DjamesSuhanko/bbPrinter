#!/usr/bin/env python
from __future__ import print_function
import cv2
import sys
import numpy as np

def imageToBin():
    #check if calling with source file parameter and target file parameter
    if len(sys.argv) < 3:
        print("Usage: ",end='')
        print(sys.argv[0],end='')
        print(" <file to map> <target.px>")
        sys.exit(0)
    #create image object as grayscale image
    img = cv2.imread(sys.argv[1],cv2.COLOR_BGR2GRAY)
    #get image dimensions
    rows,cols = img.shape
    #check each pixel with tolerance little than 11 (0 is black, 255 is white)
    f = open(sys.argv[2],'a')
    for i in range(rows):
        for j in range(cols):
            if img[i][j] < 10:
                print('1',end='')
                f.write('1')
            else:
                print('0',end='')
                f.write('0')
        print("")
        f.write("\n")
    print(" ")
    f.close()

imageToBin()