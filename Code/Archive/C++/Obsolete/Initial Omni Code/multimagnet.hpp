#include omnimagnet.cpp
#include <vector>
using std::vector;

class MultiMagnet {
	private:	
		vector<OmniMagnet> omni_system;
		int numOmni;

	public:
		MultiMagnet();
		MultiMagnet(vector<OmniMagnet> omni_system);
		void SetMagnet(int omniIndex, OmniMagnet omni_);
		void SetProp(int omniIndex, double wirewidth, double wirelenin, double wirelenmid,	double wirelenout, double coresize, int pinin, int pinmid, int pinout,bool estimate, comedi_t *card);
		void AddMagnet(OmniMagnet omni_);
		void RemoveMagnet(int omniIndex);
		Eigen::Matrix3d Pmap(POS pos1, POS pos2);
		Eigen::Matrix3d Fmap(POS pos1, POS pos2, DIP dip1);
		Eigen::MatrixXd Dmap(omni_system);
		Eigen::MatrixXd Mmap(omni_system);

};