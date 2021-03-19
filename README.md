# FleaCV -- Using the [Flea3](https://www.flir.co.uk/products/flea3-usb3/) camera in [OpenCV](https://opencv.org/)

This demo show you how to use the [Flea3](https://www.flir.co.uk/products/flea3-usb3/) camera from Point Grey (now distributed by [Flir](https://www.flir.co.uk/)). I tested the code in GNU/Linux. There is no reason why it should not work with MS Windows. The code below

1. Opens the camera,
2. Retrieve the frames one by one,
3. Converts them to an [OpenCV Mat](https://docs.opencv.org/master/d3/d63/classcv_1_1Mat.html),
4. Displays them in a window, and
5. Save them in a AVI video file.

Requirements:

- [OpenCV](https://opencv.org/): To use OpenCV.
- [CMake](https://www.cmake.org/): To build the program.
- [FlyCapture SDK](https://www.flir.co.uk/support-center/iis/machine-vision/downloads/spinnaker-sdk-flycapture-and-firmware-download/) by [Flir](https://www.flir.co.uk/): Use use the [Flea3](https://www.flir.co.uk/products/flea3-usb3/) camera.

You can consult:

- The Jupyter notebook: [README.ipynb](README.ipynb), or
- This class I wrote to mimic [OpenCV](https://opencv.org/)'s [VideoCapture](https://docs.opencv.org/master/d8/dfe/classcv_1_1VideoCapture.html#a9ac7f4b1cdfe624663478568486e6712): [Flea3Capture.h](Flea3Capture.h)

In OpenCV, you'll use something like this to use the default camera:

```cpp
VideoCapture camera(0);

while (waitKey(0) != 27) // Exit when pressing <ESC>
{
    // Grab a new frame
    Mat frame;
    camera >> frame;

    // Make sure everything went well
    if (frame.empty())
    {
        camera.release(); // Release the camera
        throw runtime_error("OpenCV cannot grab a new frame from the camera, the program will terminate");
    }

    // Display the image
    imshow("Image from camera", frame);
}
```

With my class, just change `VideoCapture camera(0);` into `Flea3Capture camera(0);`. Job done: Your [Flea3](https://www.flir.co.uk/products/flea3-usb3/) camera from Point Grey or [Flir](https://www.flir.co.uk/) will work with your [OpenCV](https://opencv.org/) code! See [Flea2CV.cxx](Flea2CV.cxx) for a demo program that grab frames from the camera, display them in a window and save a video file.
