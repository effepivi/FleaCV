#ifndef Flea3Capture_h
#define Flea3Capture_h

#include <iostream> // For operator >>
#include <opencv2/opencv.hpp> // For OpenCV
#include <FlyCapture2.h> // For the Flea3 camera

class Flea3Capture {
public:
    Flea3Capture();
    Flea3Capture(int index);
    ~Flea3Capture();

    bool grab();
    bool isOpened() const;
    bool open(int index);
    bool read(cv::Mat& image) ;
    void release();
    bool retrieve(cv::Mat& image);

private:
    int m_index;
    FlyCapture2::PGRGuid m_guid;
    FlyCapture2::Camera m_camera;
    cv::Mat m_last_frame;
};

Flea3Capture& operator>>(Flea3Capture& aVideoCapture, cv::Mat &image);

#endif
