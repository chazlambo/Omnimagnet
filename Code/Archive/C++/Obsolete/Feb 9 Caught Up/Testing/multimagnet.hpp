#include "omnimagnet.hpp"
#include <vector>


class MultiMagnet {
	public:	
		std::vector<OmniMagnet> omni_system;
		int numOmni;
		Eigen::MatrixXd Dmat;
		Eigen::MatrixXd Mmat;
		Eigen::MatrixXd Amat;
		Eigen::MatrixXd Wmat;
		

	public:
		MultiMagnet();
		MultiMagnet(std::vector<OmniMagnet> omni_system);

		void SetSystem(std::vector<OmniMagnet> omni_system);
		void SetMagnet(int omniIndex, OmniMagnet omni_);
		void SetProp(int omniIndex, double wirewidth, double wirelenin, double wirelenmid,	double wirelenout, double coresize, int pinin, int pinmid, int pinout,bool estimate/*, comedi_t *card*/);
		void SetWmat(Eigen::MatrixXd WeightMatrix);
		void AddMagnet(OmniMagnet omni_);
		void RemoveMagnet(int omniIndex);
		Eigen::VectorXd Wrench2Current(WRENCH Zdes);
		void SetSystemCurrent(Eigen::VectorXd sysCur);

	public: //Private Functions
		double GetCoreSize(int omniIndex);
		MAP Pmap(POS pos1, POS pos2);
		MAP Fmap(POS pos1, POS pos2, DIP dip1);
		Eigen::Matrix3d skew(Eigen::Vector3d vec);

		void UpdateDmat();
		void UpdateMmat(bool UpdateOmni);
		void UpdateAmat(POS toolPos, DIP toolDip);

};