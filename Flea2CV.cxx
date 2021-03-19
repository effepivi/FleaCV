#include <iostream> // For cout and cerr
#include <sstream> // For formatting error messages
#include <memory> // For smart pointer
#include <stdexcept> // For runtime exceptions
#include <chrono> // For timestamps
#include <opencv2/opencv.hpp> // For OpenCV
#include <FlyCapture2.h> // For the Flea3 camera

#include "Flea3Capture.h"

using namespace FlyCapture2;
using namespace cv;
using namespace std;
using namespace std::chrono;

inline double clockToSeconds(clock_t ticks)
{
    return (double(ticks) / double(CLOCKS_PER_SEC));
}


inline int estimateFPS(Flea3Capture& aCamera)
{
    int frame_counter = 0;
    int interval = 250;
    milliseconds start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());

    while (frame_counter < interval)
    {
        // Grab a new frame
        Mat frame;
        aCamera >> frame;

        // Make sure everything went well
        if (frame.empty())
        {
            aCamera.release(); // Release the camera
            throw runtime_error("OpenCV cannot grab a new frame from the camera, the program will terminate");
        }

        // Save the frame in the output file
        cvtColor(frame, frame, COLOR_GRAY2BGR);

        ++frame_counter;
    }

    milliseconds end = duration_cast< milliseconds >(system_clock::now().time_since_epoch());

    milliseconds delta_time = end - start;

    // I had - 10 to account for display and storing in the VideoWriter
    return int(1000.0 * frame_counter / delta_time.count()) - 10;
}

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
        {
            stringstream error_message;
            error_message << "Wrong number of command line arguments" << endl;
            error_message << "Usage: " << endl;
            error_message << "\t" << argv[0] << " output_video_file.mp4" << endl << endl;
            error_message << "The program will terminate.";

            throw runtime_error(error_message.str());
        }

        Flea3Capture camera(0);

        // Grab a new frame
        Mat frame;
        camera >> frame;
        int FPS = estimateFPS(camera);
        cout << "Size of a frame: " << frame.cols << "x" << frame.rows << endl;

        cout << "Estimated FPS: " << FPS << endl;
        if (FPS < 10) FPS = 10;

        VideoWriter video_output(argv[1], VideoWriter::fourcc('M','J','P','G'), FPS, Size(frame.cols,frame.rows));

        namedWindow("Flir Flea3 Camera", WINDOW_GUI_EXPANDED); // Create a window


        while (waitKey(int(1000. / FPS)) != 27) // Exit when pressing <ESC>
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
            imshow("Flir Flea3 Camera", frame);
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
