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
#include <comedilib.hpp>
#include <chrono>

Eigen::MatrixXd AshkanPseudoinverse(Eigen::MatrixXd A, double singularMinToMaxRatio);
Eigen::MatrixXd Pseudoinverse(Eigen::MatrixXd A);
double CurrentD2A(double current);
#endif //TYPE_H