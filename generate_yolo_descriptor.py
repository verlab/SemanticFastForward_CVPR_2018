import sys
import numpy as np
import cv2

def is_cv2():
    # if we are using OpenCV 2, then our cv2.__version__ will start
    # with '2.'
    return check_opencv_version("2.")


def check_opencv_version(major, lib=None):
    # if the supplied library is None, import OpenCV
    if lib is None:
        import cv2 as lib

    # return whether or not the current OpenCV version matches the
    # major version number
    return lib.__version__.startswith(major)

argc = len(sys.argv)
classes = 80

def loadYoloData(yolo_lines):
    descriptors = np.zeros((framecount, classes), dtype=np.int16)
    i = 0
    while i < len(yolo_lines):
        objects = (int)((float)(yolo_lines[i].rsplit(',')[0]))
        current_frame = (int)((float)(yolo_lines[i].rsplit(',')[1]))
        i+=1
        for j in xrange(objects):
            obj_line = (yolo_lines[i+j].rsplit(','))
            yolo_class = (int)(obj_line[0])
            descriptors[current_frame, yolo_class] += 1

        i += objects
    
    return descriptors

if __name__ == '__main__':
    
    if argc < 3:
        print "USAGE ERROR: python generate_descriptor.py <video_path> <yolo_extraction> <desc_output>"
        exit()

    cap = cv2.VideoCapture(sys.argv[1])
    YoloLines = open(sys.argv[2], 'r').readlines()

    if is_cv2():
        framecount = (int)(cap.get(cv2.cv.CV_CAP_PROP_FRAME_COUNT))
    else:
        framecount = (int)(cap.get(cv2.CAP_PROP_FRAME_COUNT))
            
    descriptors = loadYoloData(YoloLines)

    np.savetxt(sys.argv[3], descriptors, delimiter=",", fmt='%i')

    print "Total objects: ", np.sum(descriptors)
    print "Done :)"
