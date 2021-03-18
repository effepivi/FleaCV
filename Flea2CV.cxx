#include <iostream> // For cout and cerr
#include <sstream> // For formatting error messages
#include <memory> // For smart pointer
#include <stdexcept> // For runtime exceptions
#include <opencv2/opencv.hpp> // For OpenCV
#include <FlyCapture2.h> // For the Flea3 camera

using namespace FlyCapture2;
using namespace cv;
using namespace std;

//-----------------------------------
PGRGuid getFlea3Id(int aCameraId = 0)
//-----------------------------------
{
    PGRGuid guid;
    BusManager bus_manager;
    unsigned int number_of_cameras;

    // Retrieve the number of cameras
    FlyCapture2::Error error_status = bus_manager.GetNumOfCameras(&number_of_cameras);
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }

    // The camera exists
    if (number_of_cameras > aCameraId)
    {
        error_status = bus_manager.GetCameraFromIndex(aCameraId, &guid);
        if (error_status != PGRERROR_OK)
        {
            throw runtime_error(error_status.GetDescription());
        }
    }
    // The camera does not exist
    else
    {
        stringstream error_message;
        error_message << "FATAL ERROR" << endl;
        error_message << "Number of cameras detected: " << number_of_cameras << endl;
        error_message << "Camera ID " << aCameraId << " does not exist" << endl;
        error_message << "The program will terminate.";
        throw runtime_error(error_message.str());
    }

    return guid;
}

//------------------------------------------------------
void openFlea3Camera(Camera& aCamera, int aCameraId = 0)
//------------------------------------------------------
{
    // Get the ID
    PGRGuid guid = getFlea3Id(aCameraId);

    // Connect to a camera
    FlyCapture2::Error error_status = aCamera.Connect(&guid);
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }

    // Get the camera configuration
    FC2Config config;
    error_status = aCamera.GetConfiguration(&config);
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }

    // Set the number of driver buffers used to 10.
    config.numBuffers = 10;

    // Set the camera configuration
    error_status = aCamera.SetConfiguration(&config);
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }

    // Start capturing images
    error_status = aCamera.StartCapture();
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }

    // Read the first frame and throw it away
    // (the data is rubbish)
    Image rawImage;
    error_status = aCamera.RetrieveBuffer(&rawImage);
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }
}

//--------------------------------------
void releaseFlea3Camera(Camera& aCamera)
//--------------------------------------
{
    // Stop capturing images
    FlyCapture2::Error error_status = aCamera.StopCapture();
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }

    // Disconnect the camera
    error_status = aCamera.Disconnect();
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }
}

//---------------------------
Mat getFrame(Camera& aCamera)
//---------------------------
{
    // Retrieve an image
    Image rawImage;
    const int k_numImages = 10;

    FlyCapture2::Error error_status = aCamera.RetrieveBuffer(&rawImage);
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }

    // Convert the raw image
    Image convertedImage;
    error_status = rawImage.Convert(PIXEL_FORMAT_MONO8, &convertedImage);
    if (error_status != PGRERROR_OK)
    {
        throw runtime_error(error_status.GetDescription());
    }

    // Convert to OpenCV
    Mat opencv_image(convertedImage.GetRows(), convertedImage.GetCols(), CV_8UC1);
    for (int row = 0; row < convertedImage.GetRows(); ++row)
    {
        for (int col = 0; col < convertedImage.GetCols(); ++col)
        {
            opencv_image.at<unsigned char>(Point(col, row)) = *convertedImage(row, col);

        }
    }

    return opencv_image;
}


int main()
{
    try
    {
        Camera camera;
        openFlea3Camera(camera, 0);

        namedWindow("Webcam", WINDOW_GUI_EXPANDED); // Create a window

        while (waitKey(1) != 27) // Exit when pressing <ESC>
        {
            // Grab a new frame
            Mat frame = getFrame(camera);

            // Make sure everything went well
            if (frame.empty())
            {
                releaseFlea3Camera(camera);
                throw runtime_error("OpenCV cannot grab a new frame from the camera, the program will terminate");
            }

            // Display the image
            imshow("Webcam", frame);
        }

        // Release the camera and window
        releaseFlea3Camera(camera);
        destroyAllWindows(); // Destroy all the created windows
    }
    catch (const exception& error)
    {
        cerr << error.what() << endl;
    }

    return 0;
}
