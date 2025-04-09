#include "multimagnet.hpp"


MultiMagnet::MultiMagnet(vector<OmniMagnet> omnisystem)
{
	omni_system = omnisystem;
	numOmni = omnisystem.size();
};

MultiMagnet::MultiMagnet(){
};

void MultiMagnet::SetMagnet(int omniIndex, OmniMagnet omni_)
{
	omni_system[omniIndex] = omni_;

};

void MultiMagnet::AddMagnet(OmniMagnet omni_)
{
	omni_system.push_back (omni_);		//Adds an omnimagnet to the end of the omnisystem vector
	numOmni = omnisystem.size();
};

void MultiMagnet::RemoveMagnet(int omniIndex)
{
	omni_system.erase(omniIndex);		//Removes an omnimagnet from the omnisystem vector
	numOmni = omnisystem.size();
};

void MultiMagnet::SetProp(int omniIndex, double wirewidth, double wirelenin, double wirelenmid,	double wirelenout, double coresize, int pinin, int pinmid, int pinout,bool estimate, comedi_t *card)
{
	omni_system[omniIndex].SetProp(double wirewidth, double wirelenin, double wirelenmid,	double wirelenout, double coresize, int pinin, int pinmid, int pinout,bool estimate, comedi_t *card);
	//Sets the properties of the specified omnimagnet within the omnisystem vector
};



Eigen::Matrix3d MultiMagnet::Pmap(POS pos_i, POS pos_j)	//General Pmap generation function using Eq. 1 in Multimagnet paper
{
	Eigen::Vector3d dispVec = pos_i - pos_j;
	double vecNorm = dispVec.norm();
	double normDir = dispVec.normalize();
	result = (3*normDir*normDir.transpose() - Matrix3d::Identity())/(vecNorm^3);

	return result;
};

Eigen::MatrixXd MultiMagnet::Fmap(POS pos_i, POS pos_j, DIP dip_i)
{
	Eigen::Vector3d dispVec = pos_i - pos_j;
	double normDir = dispVec.normalize();
	result = dip_i*normDir.transpose() + normDir*dip_i.transpose() + (normDir*dip_i)(Matrix3d::Identity()-5*normDir*normDir.transpose());

	return result;
};

Eigen::MatrixXd MultiMagnet::Dmap(vector<OmniMagnet> omnisystem) //Returns |D in Eq. 11
{
	
	MatrixXd Dmat = MatrixXd::Zero(numOmni*3,numOmni*3);


	for (int i=0; i<numOmni; i++) {
		for (int j=0; j<numOmni, j++){
			if (i==j) {
				Matrix3d newMat = MatrixXd::Identity(3,3);
			} else {
				Matrix3d Pmat = MultiMagnet::Pmap(omnisystem[i].GetPosition(), omnisystem[j].GetPosition());
				R = -1*omnisystem[i].GetCoreSize();
				Matrix3d newMat = R^3*Pmat;
			}
			Dmat.block((0+i*3),(0+j*3),3,3) = newMat;
		}
	}

	return Dmat;
};




Eigen::MatrixXd MultiMagnet::Mmap(vector<OmniMagnet> omnisystem) //Returns |M in Eq. 12
{
	int numOmni = omnisystem.size();

	MatrixXd Mmat = MatrixXd::Zero(numOmni*3, numOmni*3);

	for (int i=0; i<numOmni; i++) {
		Matrix3d newMat = omnisystem[i].GetMapping();
		Mmat.block((0+i*3),(0+i*3),3,3) = newMat;
	}

	return Mmat;
};

Eigen::Matrix3d MultiMagnet::Skew(Vector3d v)
{
	Eigen::Matrix3d result <<   0  , -v[2],  v[1],
						       v[2],   0  , -v[0],
						      -v[1],  v[0],   0  ;

	return result;
};

