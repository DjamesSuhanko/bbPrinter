#!/usr/bin/env python
# -*- coding: utf-8 -*-
''' You can adjust threshold below to better results. Try it with different values a few times'''
import cv2
import sys


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Please, inform an image!")
        sys.exit(1)

    in_im = sys.argv[1]
    IMAGE_WHITELIST = [
        'jpe',
        'webp',
        'sr',
        'ras',
        'tif',
        'png',
        'jpg',
        'bmp',
        'pgm',
        'dib',
        'pbm',
        'tiff',
        'jpeg',
        'ppm',
        'jp2']

    im_name, im_filetype = in_im.rsplit(".", 1)

    VIDEO_WHITELIST = ['avi', 'mp4', 'mkv', 'mjpeg', 'swf', 'webm']

    if len(sys.argv) == 3:
        out_im_path = sys.argv[2]
    else:
        out_im_path = im_name + "-out." + im_filetype

    if im_filetype in IMAGE_WHITELIST:
        im = cv2.imread(in_im, 0)
        ret, out_im = cv2.threshold(im, 98, 255, cv2.THRESH_BINARY)
        cv2.imwrite(out_im_path, out_im)

    elif im_filetype in VIDEO_WHITELIST:
        cap = cv2.VideoCapture(in_im)
        if not cap.isOpened():
            print("cannot open video")
            sys.exit(1)

        fps = cap.get(cv2.CAP_PROP_FPS)
        delay = int((1.0 / float(fps)) * 1000)

        runner.on_video_start()
        while (cap.isOpened()):
            ret, im = cap.read()
            if not ret:
                break

            out_im, allinfo = runner.run(im)
            cv2.imshow("Output - Press 'q' to exit", out_im)
            k = cv2.waitKey(delay)
            if k & 0xFFF == ord('q'):
                break
        runner.on_video_stop()

        cap.release()
        cv2.destroyAllWindows()
    sys.exit(0)
