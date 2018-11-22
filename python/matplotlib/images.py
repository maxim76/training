import cv2

# Read and show image using cv2
img = cv2.imread('t-72.jpg', cv2.IMREAD_COLOR)
cv2.imshow('image',img)
cv2.waitKey(0)
cv2.destroyAllWindows()


# show image using pyplot
from matplotlib import pyplot as plt
# OpenCV follows BGR order, while matplotlib likely follows RGB order.
# So when display an image loaded in OpenCV using pylab functions, need to convert it into RGB mode.
img2 = img[:,:,::-1]
plt.imshow(img2)
plt.xticks([]), plt.yticks([])  # to hide tick values on X and Y axis
plt.show()
