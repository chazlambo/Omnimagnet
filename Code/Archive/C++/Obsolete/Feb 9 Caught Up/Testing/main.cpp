#include <iostream>
#include "multimagnet.hpp"

int main () {
	Eigen::IOFormat CleanFmt(4,0, ", ", "\n", "[", "]", "\n", "\n");
	int num_omni = 5;
	std::vector<OmniMagnet> omnisystem(num_omni);
	POS toolPos;
	DIP toolDip;
	WRENCH zdes(6);

	for(int i=0;i<num_omni;i++){
		double j = i+1;
		omnisystem[i].SetProp(j,j,j,j,j,j,j,j, true);
		omnisystem[i].SetPosition({j,j,j});
		omnisystem[i].mapping_ << j, j, j, j, j, j, j, j, j;
	}


	toolPos << 1.0, 2.0, 3.0;
	toolDip << 4.0, 5.0, 6.0;
	zdes << 10,10,0,10,0,0;


	MultiMagnet multi(omnisystem);


	//CLASS DEFINE AND EDITING TESTING
	/*
	OmniMagnet omni2(7,7,7,7,7,7,7,7, true);
	omni2.SetPosition({7,7,7});
	OmniMagnet omni3(8,8,8,8,8,8,8,8, true);
	omni3.SetPosition({8,8,8});
	multi.AddMagnet(omni2);
	multi.SetMagnet(0,omni3);
	multi.RemoveMagnet(3);
	for (int i = 0; i<multi.numOmni; i++){
			std::cout << multi.omni_system[i].position_.format(CleanFmt) << std::endl;
	}
	*/

	//PMAP AND FMAP TESTING
	//std::cout << omnisystem[4].position_.format(CleanFmt) << std::endl;
	//std::cout << omnisystem[1].position_.format(CleanFmt) << std::endl;
	//Eigen::Matrix3d pmap = multi.Pmap(toolPos,omnisystem[0].position_);
	//std::cout << pmap.format(CleanFmt) << std::endl;
	//Eigen::Matrix3d fmap = multi.Fmap(omnisystem[0].position_,omnisystem[1].position_,toolDip);
	//std::cout << fmap.format(CleanFmt) << std::endl;

	//DMAT TESTING
	//multi.UpdateDmat();
	//std::cout << multi.Dmat.format(CleanFmt) << std::endl;

	//MMAT TESTING
	//multi.UpdateMmat(false);
	//std::cout << multi.Mmat.format(CleanFmt) << std::endl;

	//SKEW TESTING
	//Eigen::Matrix3d test = multi.skew(toolPos);
	//std::cout << test.format(CleanFmt) << std::endl;

	//AMAT TESTING
	//multi.UpdateAmat(toolPos,toolDip);
	//std::cout << multi.Amat.format(CleanFmt) << std::endl;


	//WMAT TESTING
	/*
	Eigen::MatrixXd weightMat;
	//weightMat = Eigen::MatrixXd::Identity(3,3);
	//multi.SetWmat(weightMat);
	//std::cout << multi.Wmat.format(CleanFmt) << std::endl;
	weightMat = 4.0*Eigen::MatrixXd::Identity(15,15);
	multi.SetWmat(weightMat);
	std::cout << multi.Wmat.format(CleanFmt) << std::endl;
	std::cout << weightMat.format(CleanFmt) << std::endl;
	weightMat = weightMat.array().pow(0.5);
	std::cout << weightMat.format(CleanFmt) << std::endl;
	weightMat = weightMat.inverse();
	std::cout << weightMat.format(CleanFmt) << std::endl;
	*/
	

	//FINAL TEST
	multi.UpdateDmat();

	multi.UpdateMmat(false);

	multi.UpdateAmat(toolPos,toolDip);

	Eigen::MatrixXd weightMat = Eigen::MatrixXd::Identity(15,15);
	multi.SetWmat(weightMat);

	Eigen::VectorXd sysCur;
	sysCur = multi.Wrench2Current(zdes);
	std::cout << sysCur.format(CleanFmt) << std::endl;

	multi.SetSystemCurrent(sysCur);


}
