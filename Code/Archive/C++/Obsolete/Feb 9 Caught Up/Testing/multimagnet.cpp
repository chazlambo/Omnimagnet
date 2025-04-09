#include <iostream>
#include "multimagnet.hpp"


MultiMagnet::MultiMagnet(std::vector<OmniMagnet> omnisystem)
{
	omni_system = omnisystem;
	numOmni = omni_system.size();
};

MultiMagnet::MultiMagnet(){

};

void MultiMagnet::SetSystem(std::vector<OmniMagnet> omnisystem)
{
	omni_system = omnisystem;
};


void MultiMagnet::SetMagnet(int omniIndex, OmniMagnet omni_)
{
	omni_system[omniIndex] = omni_;
};

void MultiMagnet::AddMagnet(OmniMagnet omni_)
{
	omni_system.push_back(omni_);
	numOmni = omni_system.size();
};

void MultiMagnet::RemoveMagnet(int omniIndex)
{
	omni_system.erase(omni_system.begin()+omniIndex);
	numOmni = omni_system.size();
};

void MultiMagnet::SetProp(int omniIndex, double wirewidth, double wirelenin, double wirelenmid,	double wirelenout, double coresize, int pinin, int pinmid, int pinout,bool estimate/*, comedi_t *card*/)
{
	//omni_system[omniIndex].SetProp(double wirewidth, double wirelenin, double wirelenmid,	double wirelenout, double coresize, int pinin, int pinmid, int pinout,bool estimate, comedi_t *card);
};

double MultiMagnet::GetCoreSize(int omniIndex)
{
	double result = omni_system[omniIndex].GetCoreSize();
	return result;
};

Eigen::Matrix3d MultiMagnet::skew(Eigen::Vector3d vec)
/*Creates a skew symmetric matrix of an input vector*/
{
	Eigen::Matrix3d result;
	result <<    0.0, -vec[2],  vec[1],
			  vec[2],     0.0, -vec[0],
			 -vec[1],  vec[0],     0.0;
	return result;
};

MAP MultiMagnet::Pmap(POS pos_i, POS pos_j)
/* Generates the (3X3) P mapping matrix for two given position vectors*/
/* Equation 1 of the Omnimagnet System paper*/
{
	Eigen::Vector3d dispVec = pos_i - pos_j;
	double vecNorm = dispVec.norm();
	Eigen::Vector3d normDir = dispVec/vecNorm;
	Eigen::Matrix3d result = (3.0*normDir*normDir.transpose() - Eigen::Matrix3d::Identity())/(pow(vecNorm,3.0));

	return result;
};

MAP MultiMagnet::Fmap(POS pos_i, POS pos_j, DIP dip_i)
/* Generates the (3X3) P mapping matrix for two given position vectors and a given dipole vector*/
/* Equation 4 of the Omnimagnet System paper*/
{
	Eigen::Vector3d dispVec = pos_i - pos_j;
	double vecNorm = dispVec.norm();
	Eigen::Vector3d normDir = dispVec/vecNorm;
	Eigen::Matrix3d result = (dip_i*normDir.transpose() + normDir*dip_i.transpose() + normDir.dot(dip_i)*(Eigen::Matrix3d::Identity()-5.0*normDir*normDir.transpose()))/(pow(vecNorm,4));

	return result;
};


void MultiMagnet::UpdateDmat()
/* Generates the D matrix*/
/* Equation 11 of the Omnimagnet System paper*/
{

	Dmat = Eigen::MatrixXd::Zero(numOmni*3,numOmni*3);
	Eigen::Matrix3d newMat;
	double core;
	double Rcoeff;

	for (int i=0; i<numOmni; i++) {
		for (int j=0; j<numOmni; j++){
			if (i==j) {
				newMat = Eigen::MatrixXd::Identity(3,3);
			} else {
				newMat = MultiMagnet::Pmap(omni_system[i].GetPosition(), omni_system[j].GetPosition());
				Rcoeff = -1*(pow(omni_system[i].GetCoreSize(),3.0));
				newMat = Rcoeff*newMat;
			}	
			Dmat.block(3*i,3*j,3,3) = newMat;
		}
	}

};


void MultiMagnet::UpdateMmat(bool UpdateOmni)
/* Updates the M matrix*/
/* Equation 12 of the Omnimagnet System paper*/
{
	if(UpdateOmni){
		for(int i=0;i<numOmni;i++){
			omni_system[i].UpdateMapping();
		}
	}

	Mmat = Eigen::MatrixXd::Zero(numOmni*3,numOmni*3);
	Eigen::Matrix3d newMat;

	for (int i=0; i<numOmni; i++) {
		Mmat.block(3*i,3*i,3,3) = omni_system[i].GetMapping();
	}

};

void MultiMagnet::UpdateAmat(POS toolPos, DIP toolDip)
/* Updates the A matrix*/
/* Equation 12 of the Omnimagnet System paper*/
{
	Amat = Eigen::MatrixXd::Zero(6,numOmni*3);
	Eigen::Matrix3d skewed = MultiMagnet::skew(toolDip);
	Eigen::MatrixXd Smat(6,6);
	Smat.topLeftCorner(3,3) = skewed;
	Smat.topRightCorner(3,3) = Eigen::Matrix3d::Zero();
	Smat.bottomLeftCorner(3,3) = Eigen::Matrix3d::Zero();
	Smat.bottomRightCorner(3,3) = 3.0*Eigen::Matrix3d::Identity();

	Eigen::MatrixXd Tmat = Eigen::MatrixXd::Zero(6,3*numOmni);

	for (int i=0; i<numOmni; i++) {
		Tmat.block(0,3*i,3,3) = MultiMagnet::Pmap(toolPos,omni_system[i].GetPosition());
		Tmat.block(3,3*i,3,3) = MultiMagnet::Fmap(toolPos,omni_system[i].GetPosition(), toolDip);
	}

	Amat = 1e-7*Smat*Tmat;
};

void MultiMagnet::SetWmat(Eigen::MatrixXd WeightMatrix)
/*Sets the positive-definite symmetric weighting matrix*/

{
	if (WeightMatrix.rows() == numOmni*3 && WeightMatrix.cols() == numOmni*3) {
		WeightMatrix = WeightMatrix.array().pow(0.5);
		WeightMatrix = WeightMatrix.inverse();
		Wmat = WeightMatrix;
	}
	else {
		std::cout << "Inputted matrix does not fit the proper dimensions" << std::endl;
		std::cout << "Required size is " << numOmni*3 << "x" << numOmni*3 << std::endl;
	}
};



Eigen::VectorXd MultiMagnet::Wrench2Current(WRENCH Zdes)
{
	Eigen::MatrixXd newMat = Amat*Dmat.transpose()*Mmat*Wmat;
	newMat = newMat.completeOrthogonalDecomposition().pseudoInverse();
	Eigen::VectorXd current = Wmat * newMat * Zdes;
	return current;
};

void MultiMagnet::SetSystemCurrent(Eigen::VectorXd sysCur)
{
	for(int i=0; i<numOmni; i++){
		CUR newcurrent = {sysCur[3*i],sysCur[3*i+1],sysCur[3*i+2]};
		std::cout << i << std::endl << newcurrent << std::endl;
		//omni_system[i].SetCurrent(newcurrent);
	}
};
