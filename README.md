# FleaCV -- Using the [Flea3](https://www.flir.co.uk/products/flea3-usb3/) camera in [OpenCV](https://opencv.org/)

This demo show you how to use the [Flea3](https://www.flir.co.uk/products/flea3-usb3/) camera from Point Grey (now distributed by [Flir](https://www.flir.co.uk/)). I tested the code in GNU/Linux. There is no reason why it should not work with MS Windows. The code below

1. opens the camera,
2. retrieve the frames one by one,
3. converts them to an [OpenCV Mat](https://docs.opencv.org/master/d3/d63/classcv_1_1Mat.html),
4. displays them in a window, and
5. save them in a AVI video file.
