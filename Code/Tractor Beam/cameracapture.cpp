#include "cameracapture.hpp"
#include <iostream>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;

CameraCapture::CameraCapture(CameraPtr pcam) {
    pCam = pcam;
}

CameraCapture::~CameraCapture() {
    ReleaseCamera();
}

bool CameraCapture::InitializeCamera() {
    try {
        // Initialize the camera hardware
        pCam->Init();

         // Access the GenICam node map for configuring camera settings
        INodeMap& nodeMap = pCam->GetNodeMap();
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        CEnumEntryPtr ptrContinuous = ptrAcquisitionMode->GetEntryByName("Continuous"); // Continuos so we can grab frames in a loop
        ptrAcquisitionMode->SetIntValue(ptrContinuous->GetValue());

        // Start acquiring images
        pCam->BeginAcquisition();

        return true;

    } catch (Spinnaker::Exception& e) {
        // Throw error if it didn't work
        std::cerr << "Camera initialization failed: " << e.what() << std::endl;
        return false;
    }
}

bool CameraCapture::CaptureAndSaveImage(const std::string& filename) {
    // Captures a single frame and saves it as an image file
    try {
        // Get the latest image from the camera
        pResultImage = pCam->GetNextImage();

        // Check for incomplete image
        if (pResultImage->IsIncomplete()) {
            std::cerr << "Incomplete image: "
                      << Image::GetImageStatusDescription(pResultImage->GetImageStatus()) << std::endl;
            return false;
        }
    
        // Save raw if no undistortion specified
        pResultImage->Save(filename.c_str());

        // Free the image from the camera buffer
        pResultImage->Release();

        return true;

    } catch (Spinnaker::Exception& e) { 
        // Throw error if it didn't work
        std::cerr << "Image capture failed: " << e.what() << std::endl;
        return false;
    }
}

// Stops acquisition and deinitializes the camera
void CameraCapture::ReleaseCamera() {
    try { // Only shut down if camera is valid and initialized
        if (pCam && pCam->IsValid() && pCam->IsInitialized()) {
            pCam->EndAcquisition();
            pCam->DeInit();
        }
    } catch (...) {
        // If shutdown already happened then ignore
    }
}
