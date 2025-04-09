#ifndef TYPE_H
#define TYPE_H
/*****************************************************
type.h  (requires eigen library)  defines matrix types: 

    Inherits:
		eigen
Ver 1.0 by Ashkan July-2019		
*****************************************************/

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/SVD>
#include <Eigen/Dense>
#include <Eigen/QR> 
// #include <comedilib.hpp>
#include <chrono>

// Variable type definitions
typedef Eigen::Matrix3d AXIS_ROT;
typedef Eigen::Vector3d TEMP;
typedef Eigen::Vector3d POW;
typedef Eigen::Vector3d POS;
typedef Eigen::Vector3d CUR;
typedef Eigen::Vector3d CUR_DEN;
typedef Eigen::Vector3d D2A_PIN;
//typedef Eigen::Matrix3f orientation;
typedef Eigen::Matrix3d MAP;
typedef Eigen::Vector3d TARGET_LOC;
typedef Eigen::Vector3d DIP;
typedef Eigen::VectorXd WRENCH;





Eigen::MatrixXd AshkanPseudoinverse(Eigen::MatrixXd A, double singularMinToMaxRatio);
Eigen::MatrixXd Pseudoinverse(Eigen::MatrixXd A);

double CurrentD2A(double current) ;
#endif //TYPE_H