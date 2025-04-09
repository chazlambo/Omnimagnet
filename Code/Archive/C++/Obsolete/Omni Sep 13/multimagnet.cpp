#include "multimagnet.hpp"


MultiMagnet::MultiMagnet(vector<OmniMagnet> omnisystem)
{
	omni_system = omnisystem;
};

MultiMagnet::MultiMagnet(){
};

void MultiMagnet::SetMagnet(int omniIndex, OmniMagnet omni_)
{
	omni_system[omniIndex] = omni_;

};

void MultiMagnet::AddMagnet(OmniMagnet omni_)
{
	omni_system.push_back (omni_);
};

void MultiMagnet::RemoveMagnet(int omniIndex)
{
	omni_system.erase(omniIndex);
};

void MultiMagnet::SetProp(int omniIndex, double wirewidth, double wirelenin, double wirelenmid,	double wirelenout, double coresize, int pinin, int pinmid, int pinout,bool estimate, comedi_t *card)
{
	omni_system[omniIndex].SetProp(double wirewidth, double wirelenin, double wirelenmid,	double wirelenout, double coresize, int pinin, int pinmid, int pinout,bool estimate, comedi_t *card);
};



Eigen::Matrix3d MultiMagnet::Pmap(POS pos_i, POS pos_j)
{
	Eigen::Vector3d dispVec = pos_i - pos_j;
	double vecNorm = dispVec.norm();
	Eigen::Vector3d unitDispVec = dispVec/vecNorm;
	result = (3*unitDispVec*unitDispVec.transpose() - Matrix3d::Identity())/(vecNorm^3);

	return result;
};

Eigen::MatrixXd MultiMagnet::Dmap(vector<OmniMagnet> omnisystem)
{
	numOmni = omnisystem.size();
	vector Pvec;
	vector Rvec;

	for (int i=0; i<numOmni; i++){
		Pvec = Pvec, omnisystem[i].GetPosition();
	}

	/*
	for (int i=0; i<numOmni; i++){
		Rvec = Rvec, //Something to pull core size from each omnimagnet
	}
	*/
	
	MatrixXd Dmat = MatrixXd::Zero(numOmni,numOmni);
	int k = 0;
	int j = 0;

	for (int i=0; i<numOmni; i++) {
		for (int j=0; j<numOmni, j++){
			if (i==j) {
				MatrixXd::Identity(3,3);
			} else {
				Matrix3d newMat = MultiMagnet::Pmap(Pvec[i], Pvec[j]);
			}
			Dmat //Figure out how to translate this from matlab

		}
	}
};
