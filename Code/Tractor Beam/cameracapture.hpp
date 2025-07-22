#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>
#include <opencv2/opencv.hpp>
#include <string>

class CameraCapture {
public:
    CameraCapture(Spinnaker::CameraPtr pcam);
    ~CameraCapture();

    bool InitializeCamera();                          // Initialize and start acquisition
    bool CaptureAndSaveImage(const std::string& filename); // Capture image and save to file
    void ReleaseCamera();                             // Stop acquisition and release camera

private:
    Spinnaker::CameraPtr pCam;           // Pointer to the camera
    Spinnaker::ImagePtr pResultImage;    // Last acquired raw image
    cv::Mat CurrentImage;                // Last converted OpenCV image
};


#endif //CAMERACAPTURE_H