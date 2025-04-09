#ifndef CAMTR_H
#define CAMTR_H
/*****************************************************
type.h  (requires eigen library)  defines matrix types: 

    Inherits:
		eigen
		Spinnaker
		Opencv
Ver 1.0 by Ashkan July-2019		
*****************************************************/

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/SVD>
#include <Eigen/Dense>
#include <Eigen/QR> 
#include <comedilib.hpp>
#include <chrono>
#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>
#include <SpinVideo.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <thread>
#include "type.hpp"
// #include <iostream>
// #include <sstream>
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Eigen;

class CameraTracker {
	private:
		CameraPtr pCam = nullptr;     /* pointer to the camera */
		int result = 0;
		int error = 0;
		double TargetSize;			// Marker's size in (mm)
		int TargetID;				// Marker's id for the Origin 
		Eigen::Vector3d Target_location;	//Pose of the target [X,Y,Z]
		Eigen::Vector3d Target_orientation;	//Pose of the target [roll, pitch, yaw]
		cv::Mat cameraMatrix, distCoeffs;   //Camera's calibration prameters
		ImagePtr pResultImage; 			//PointGray Image 
		cv::Mat CurrentImage; 			//Opencv Image
		//Dict of the IDs for the targets:
		std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
        cv::Ptr<cv::aruco::DetectorParameters> parameters;
        cv::Ptr<cv::aruco::Dictionary> dictionary;
        std::vector<cv::Vec3d> rvecs, tvecs; 
        cv::Vec3d rvec;
	    cv::Vec3d tvec;
		void ReadCalirbation(std::string fileName);
		int Initial();		
	public:
		CameraTracker();
		~CameraTracker();
		CameraTracker(CameraPtr pcam, const Eigen::Vector3d& pose_, int TargetID_, double TargetSize_,std::string calibrationAddress); 
					//( pointer to camera   , pointer target pose, Target id, Size of the target in (mm), calibration file name)   
		
		Eigen::Vector3d GetTargetLocation(); // Returns the target's pose (x,y,theta) in the coordinate frame defined by the other markers
		Eigen::Vector3d GetTargetOrientaion(); // Returns the target's pose (x,y,theta) in the coordinate frame defined by the other markers
		void UpdateTargetPose();	// Updates the target's pose (x,y,theta) in the coordinate frame defined by the other markers
		void ReleaseCamera();	// Releases the camera (use it when you are done)! 
		void SaveCurrentFrame(std::string fileName, bool coordinate, bool markers); // Updates and saves the current frame to the filename. You can draw the coordiante for the marker and also add the markers ID for ALL OF THE IDs.   
		cv::Mat GetCurrentFrame();	//Returns the current frame from the camera (Type : opencv Mat)
		int PrintDeviceInfo(INodeMap& nodeMap);	//Prints the device (use it to be sure the camera is detected.)
		void UpdateFrame(); //Updates the frame

};
#endif // CAMTR_H
