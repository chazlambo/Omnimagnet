#include <iostream>
#include "cameratrack.hpp"

CameraTracker::CameraTracker(){
    // Will add latter
};

CameraTracker::CameraTracker(CameraPtr pcam, const Eigen::Vector3d& pose_, int TargetID_, double TargetSize_, std::string calibrationAddress){
    TargetID = TargetID_;
    TargetSize = TargetSize_;
    CameraTracker::ReadCalirbation(calibrationAddress);
    pCam = pcam;
    Initial();
};

CameraTracker::~CameraTracker(){
    // Will add latter
    // pCam->DeInit();
};

Eigen::Vector3d CameraTracker::GetTargetLocation(){
    UpdateTargetPose();  
    return Target_location;
};

Eigen::Vector3d CameraTracker::GetTargetOrientaion(){
    UpdateTargetPose();  
    return Target_orientation;
};

void CameraTracker::UpdateTargetPose(){
    UpdateFrame();
    cv::aruco::detectMarkers(CurrentImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
    if (markerIds[0] != TargetID){
        std::cout<<"\n ================= Warning, target's id is not match =================\n";
    }
    cv::aruco::estimatePoseSingleMarkers(markerCorners, TargetSize, cameraMatrix, distCoeffs, rvecs, tvecs);
    rvec = rvecs[0];
    tvec = tvecs[0];
    Target_location << tvec[0], tvec[1], tvec[2];
    Target_orientation << rvec[0], rvec[1], rvec[2];
    // std::cout<<tvec<<"\n";
};

void CameraTracker::SaveCurrentFrame(std::string fileName, bool coordinate, bool markers){
    UpdateTargetPose();
    cv::Mat outputImage;
    CurrentImage.copyTo(outputImage);
    if (markers){
        cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
    }
    if (coordinate){   
        // std::cout<<rvec;
        cv::aruco::drawAxis(outputImage, cameraMatrix, distCoeffs, rvec, tvec, 20);
    }
    cv::imwrite(fileName, outputImage);
};

void CameraTracker::ReleaseCamera(){
    //Memory problem fix this!!!!!!!!!!!!!!!!
    pCam->DeInit();
};

cv::Mat CameraTracker::GetCurrentFrame(){
    UpdateFrame();
    return CurrentImage;
};

void CameraTracker::UpdateFrame(){
    pResultImage = pCam->GetNextImage();
    pResultImage->Release();
    if (pResultImage->IsIncomplete())
        {
            // Retrieve and print the image status description
            std::cout << "Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
                 << "..." << std::endl
                 << std::endl;
        }
        else
        {
            const size_t width = pResultImage->GetWidth();
            const size_t height = pResultImage->GetHeight();
            pResultImage = pResultImage->Convert(PixelFormat_BGR8, Spinnaker::IPP);
            unsigned int XPadding = pResultImage->GetXPadding();
            unsigned int YPadding = pResultImage->GetYPadding();
            unsigned int rowsize = pResultImage->GetWidth();
            unsigned int colsize = pResultImage->GetHeight();
            CurrentImage = cv::Mat(colsize + YPadding, rowsize + XPadding, CV_8UC3, pResultImage->GetData(), pResultImage->GetStride());
            // pResultImage->Release();
        }

};

int CameraTracker::Initial(){
    int result;
    try
    {
        // Errors (in case camera doesn't work)
        // Retrieve TL device nodemap and print device information
        INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
        result = PrintDeviceInfo(nodeMapTLDevice);
        // Initialize camera
        pCam->Init();
        // Retrieve GenICam nodemap
        INodeMap& nodeMap = pCam->GetNodeMap();
        // pCam->DeInit();
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
        {
            std::cout << "Unable to set acquisition mode to continuous (enum retrieval). Aborting..." << std::endl << std::endl;
            return -1;
        }
        // Retrieve entry node from enumeration node
        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
        {
            std::cout << "Unable to set acquisition mode to continuous (entry retrieval). Aborting..." << std::endl << std::endl;
            return -1;
        }
        // Retrieve integer value from entry node
        const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
        // Set integer value from entry node as new value of enumeration node
        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
        std::cout << "Acquisition mode set to continuous..." << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    pCam->BeginAcquisition();
    parameters = cv::aruco::DetectorParameters::create();
    dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
};

void CameraTracker::ReadCalirbation(std::string fileName){
    cv::FileStorage fs2(fileName, cv::FileStorage::READ);
    fs2["camera_matrix"] >> cameraMatrix;
    fs2["distortion_coefficients"] >> distCoeffs;
    fs2.release();
};

int CameraTracker::PrintDeviceInfo(INodeMap& nodeMap)
{
    std::cout << std::endl << "*** DEVICE INFORMATION ***" << std::endl << std::endl;
    try
    {
        FeatureList_t features;
        const CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
        if (IsAvailable(category) && IsReadable(category))
        {
            category->GetFeatures(features);
            for (auto it = features.begin(); it != features.end(); ++it)
            {
                const CNodePtr pfeatureNode = *it;
                std::cout << pfeatureNode->GetName() << " : ";
                CValuePtr pValue = static_cast<CValuePtr>(pfeatureNode);
                std::cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
                std::cout << std::endl;
            }
        }
        else
        {
            std::cout << "Device control information not available." << std::endl;
        }
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}