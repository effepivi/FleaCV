#include <iostream> // For cout and cerr
#include <sstream> // For formatting error messages
#include <memory> // For smart pointer
#include <stdexcept> // For runtime exceptions
#include <opencv2/opencv.hpp> // For OpenCV
#include <FlyCapture2.h> // For the Flea3 camera

#include "Flea3Capture.h"

using namespace FlyCapture2;
using namespace cv;
using namespace std;


int main(int argc, char** argv)
{
    try
    {
        Flea3Capture camera(0);

        // Grab a new frame
        Mat frame;
        camera >> frame;
        cout << frame.cols << "x" << frame.rows << endl;

        int FPS = 50;
        VideoWriter video_output("../output.mp4", VideoWriter::fourcc('H','2','6','4'), FPS, Size(frame.cols,frame.rows));

        namedWindow("Webcam", WINDOW_GUI_EXPANDED); // Create a window

        while (waitKey(int(1000.0 / FPS)) != 27) // Exit when pressing <ESC>
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

            // Save the frame in the output file
            cvtColor(frame, frame, COLOR_GRAY2BGR);
            video_output << frame;

            // Display the image
            imshow("Webcam", frame);
        }

        // Release the video writer, the camera, and the window
        video_output.release(); // We are now done with the video output, stop it
        camera.release(); // Release the camera
        destroyAllWindows(); // Destroy all the created windows
    }
    catch (const exception& error)
    {
        cerr << error.what() << endl;
    }

    return 0;
}
