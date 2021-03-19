#include <iostream>
#include <sstream>

#include "Flea3Capture.h"

using namespace FlyCapture2;
using namespace cv;
using namespace std;


//---------------------------
Flea3Capture::Flea3Capture():
//---------------------------
    m_index(-1)
//---------------------------
{}


//------------------------------------
Flea3Capture::Flea3Capture(int index):
//------------------------------------
    m_index(-1)
//------------------------------------
{
    open(index);
}


//---------------------------
Flea3Capture::~Flea3Capture()
//---------------------------
{
    release();
}


//-----------------------
bool Flea3Capture::grab()
//-----------------------
{
    if (m_index < 0) return false;

    // Retrieve an image
    Image rawImage;
    FlyCapture2::Error error_status = m_camera.RetrieveBuffer(&rawImage);
    if (error_status != PGRERROR_OK)
    {
        cerr << error_status.GetDescription() << endl;
        m_last_frame = Mat();
        return false;
    }

    // Convert the raw image
    Image convertedImage;
    error_status = rawImage.Convert(PIXEL_FORMAT_MONO8, &convertedImage);
    if (error_status != PGRERROR_OK)
    {
        cerr << error_status.GetDescription() << endl;
        m_last_frame = Mat();
        return false;
    }

    // There is a frame
    if (convertedImage.GetRows() && convertedImage.GetCols())
    {
        // Convert to OpenCV
        m_last_frame = Mat(convertedImage.GetRows(), convertedImage.GetCols(), CV_8UC1);
        for (int row = 0; row < convertedImage.GetRows(); ++row)
        {
            for (int col = 0; col < convertedImage.GetCols(); ++col)
            {
                m_last_frame.at<unsigned char>(Point(col, row)) = *convertedImage(row, col);
            }
        }

        return true;
    }

    // There is no a frame or there was an error
    m_last_frame = Mat();
    return false;
}


//---------------------------------
bool Flea3Capture::isOpened() const
//---------------------------------
{
    return (m_index >= 0);
}


//--------------------------------
bool Flea3Capture::open(int index)
//--------------------------------
{
    if (index != m_index) release();

    if (!isOpened())
    {
        BusManager bus_manager;
        unsigned int number_of_cameras;

        // Retrieve the number of cameras
        FlyCapture2::Error error_status = bus_manager.GetNumOfCameras(&number_of_cameras);
        if (error_status != PGRERROR_OK)
        {
            cerr << error_status.GetDescription() << endl;
            return false;
        }

        // The camera exists
        if (number_of_cameras > index)
        {
            error_status = bus_manager.GetCameraFromIndex(index, &m_guid);
            if (error_status != PGRERROR_OK)
            {
                cerr << error_status.GetDescription() << endl;
                return false;
            }
        }
        // The camera does not exist
        else
        {
            stringstream error_message;
            error_message << "FATAL ERROR" << endl;
            error_message << "Number of cameras detected: " << number_of_cameras << endl;
            error_message << "Camera ID " << index << " does not exist" << endl;
            error_message << "The program will terminate.";

            cerr << error_message.str() << endl;

        }

        // Connect to a camera
        error_status = m_camera.Connect(&m_guid);
        if (error_status != PGRERROR_OK)
        {
            cerr << error_status.GetDescription() << endl;
            return false;
        }

        // Get the camera configuration
        FC2Config config;
        error_status = m_camera.GetConfiguration(&config);
        if (error_status != PGRERROR_OK)
        {
            cerr << error_status.GetDescription() << endl;
            return false;
        }

        // Set the number of driver buffers used to 10.
        config.numBuffers = 10;

        // Set the camera configuration
        error_status = m_camera.SetConfiguration(&config);
        if (error_status != PGRERROR_OK)
        {
            cerr << error_status.GetDescription() << endl;
            return false;
        }

        // Start capturing images
        error_status = m_camera.StartCapture();
        if (error_status != PGRERROR_OK)
        {
            cerr << error_status.GetDescription() << endl;
            return false;
        }

        // Read the first frame and throw it away
        // (the data is rubbish)
        Image rawImage;
        error_status = m_camera.RetrieveBuffer(&rawImage);
        if (error_status != PGRERROR_OK)
        {
            cerr << error_status.GetDescription() << endl;
            return false;
        }

        m_index = index;
    }

    return true;
}


bool Flea3Capture::read(cv::Mat& image)
{
    // The camera is open
    if (isOpened())
    {
        // Grab the frame
        if (grab())
        {
            // There is a frame
            if (m_last_frame.cols && m_last_frame.rows)
            {
                image = m_last_frame;
                return true;
            }
        }
    }

    // There is no a frame or there was an error
    image = Mat();
    return false;
}


void Flea3Capture::release()
{
    if (isOpened())
    {
        // Stop capturing images
        FlyCapture2::Error error_status = m_camera.StopCapture();
        if (error_status != PGRERROR_OK)
        {
            cerr << error_status.GetDescription() << endl;
            return;
        }

        // Disconnect the camera
        error_status = m_camera.Disconnect();
        if (error_status != PGRERROR_OK)
        {
            cerr << error_status.GetDescription() << endl;
            return;
        }

        // Reset the index
        m_index = -1;
    }
}


//-----------------------------------------
bool Flea3Capture::retrieve(cv::Mat& image)
//-----------------------------------------
{
    // The camera is open
    if (isOpened())
    {
        // There is a frame
        if (m_last_frame.cols && m_last_frame.rows)
        {
            image = m_last_frame;
            return true;
        }
    }

    // There is no a frame or there was an error
    image = Mat();
    return false;
}


Flea3Capture& operator>>(Flea3Capture& aVideoCapture, Mat &image)
{
    aVideoCapture.read(image);
    return aVideoCapture;
}
