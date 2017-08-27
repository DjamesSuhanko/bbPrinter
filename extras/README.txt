Topics
(A) How to generate documentation
    1 - calibratePixelsDistance
    2 - printRetangle
    3 - Print suggestion
    4 - Bibary image generator
(B) About Python scripts
    1 - Converting image to threshold map
    2 - Converting image to binary file
    3 - Dependences
(C) Support

(A) How to generate documentation:
You can   generate   the   documentation   running   Doxygen   program   against
"documentation.doxygen" file.
If you wish to see this library working in my project, you can visite my website:
http://www.dobitaobyte.com.br or visite the gitHub project in
http://www.dobitaobyte.com.br/impressora-com-arduino-opencv/ ‎

If you have doubts or you wish to know which  functions  are  available  in  this
library, you can edit both .cpp and .h files inside src/ directory. For each one,
you can see comments about the usage and what means each one of their parameters.

Usage examples:
In  examples  folder  there  are  a  few examples.  It's  enough  to  understand
everything you need to know. My suggestion is:
1 - Open calibratePixelsDistance first.
With this example, you can to find the  steps  needed  to  separate  the  pixels
correctly. In this example, you can test the pen drive too.

2 - Now, open printRetangle example and fire it. With this  simple  example  you
can  verify if everything is working fine (alignment, pen and so on).

3 - If you wish, open the lion example and print it. If you  are  capable
to print it correctly, congratz, your printer is live!

4 - Binary image generator:
Inside extras folder you can found a python script. That script uses  OpenCV  to
convert  image to black and white (prepareImageToMap.py).
There is a second python script (using OpenCV too) to generate a binary map file.
If you have a good black and white image or you  can convert one using any image
editor program, you can wish use only the second one (convertToMap.py).
My suggestion is use GIMP to do the convertion. GIMP is a free image manipulator
and have versions to Windows and Linux. If you use Linux, you know what  to  do.
If you are a Windows user, so you will need to download using a browser. You can
found the programa in https://www.gimp.org/downloads/ and  all  instructions  to
install it.

(B) About Python scripts
1 - Converting image to threshold map
The first thing you need to do is select an image to print. The better choice is
black and white image, but you can use anyone, included collored photos. But  if
you are using a collor image, the threshold  needs  be  adjusted  so,  edit  the
prepareImageToMap.py and change the threshold  limits to find the best condition
for you. Remember that print will use dots to make  printing,  so, your selected
image need to be solid enough to good results.
A lot of formats are supported and you need to pass input and output  images  as
parameter.

2 - Converting image to binary file
This script will generate a file with a lot of 0's and 1's. The 0's are void
points to print and the 1's are pixels to print.
Usage: ./convertToMap.py <file to map> <target.bin>. It's not really a binary
image, but it's like one.

(C) Support
If you need any support, I suggest you to join us in one or more facebook groups
where I'm admin. The default language is portuguese so, you will see  a  lot  of
portuguese posts, but don't worry, you can write in english or spanish and me or
anyone (or both) will answer you.

The groups are:

Arduino BR – https://www.facebook.com/groups/microcontroladorarduinobr/
Raspberry Pi BR – https://www.facebook.com/groups/raspberrybr/
Orange Pi BR – https://www.facebook.com/groups/OrangePiBR/
Odroid BR – https://www.facebook.com/groups/odroidBR/
Sistemas Embarcados BR – https://www.facebook.com/groups/SistemasEmbarcadosBR/
MIPS BR – https://www.facebook.com/groups/MIPSBR/
Do Bit ao Byte – https://www.facebook.com/groups/dobitaobyte/

I wish you a lot of fun!

Djames Suhanko.
