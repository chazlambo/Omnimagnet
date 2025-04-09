#ifndef OMNIMAGNET_H
#define OMNIMAGNET_H
/*****************************************************
OmniMagnet.h  (requires omnimagnet.cpp)  defines a class which
    takes in an Omnimagnet props: 
		OmniMagnet(double wire_guage, double wire_len_in, double wire_len_mid,	double wire_len_out, double core_size;);
	Inherits:
		type.hpp


Omnimangnet local frame: out:Z, mid:Y, in:X
Ver 1.0 by Ashkan July-2019		
*****************************************************/
#include "type.hpp" // defines the matrix types. 
class OmniMagnet {
	public:
		int subdev = 0;     /* change this to your input subdevice */
		int chan = 10;      /* change this to your channel */
		int range = 16383;      /* more on this later */
		// int aref = AREF_GROUND; /* more on this later */
		unsigned int subdevice = 0;
		int res;
		// comedi_t* card_;
		bool estimate_;
		double wire_width;
		double wire_len_in;
		double wire_len_mid;
		double wire_len_out;
		double core_size;
		double orientation_;

		//TYPEDEF CHANGES
		AXIS_ROT axis_rot_Z;
		TEMP temperature_;
		POW power_;
		POS position_;
		CUR current_;
		CUR_DEN current_density;
		D2A_PIN D2A_pin_number;
		// Eigen::Matrix3f orientation_;
		MAP mapping_;     // local maping 
		CUR max_current;
		// std::chrono::milliseconds ref_time;
		std::chrono::high_resolution_clock::time_point ref_time, current_time;


	public:
		OmniMagnet();
		OmniMagnet(double wire_width, double wire_len_in, double wire_len_mid, double wire_len_out, double core_size, int pinin, int pinmid, int pinout,bool estimate);
		void SetProp(double wire_width, double wire_len_in, double wire_len_mid, double wire_len_out, double core_size, int pinin, int pinmid, int pinout,bool estimate);
		
		//TYPEDEF CHANGES
		// void SetCurrent(current current_);
		void SetTemperature(TEMP temperature_);
		void SetPower(POW power_);
		void SetPosition(POS position);
		// void SetOrientation( orientation orientation_ );
		void SetOrientation( double orientation);
		void UpdateMapping();
		double GetOrientation();
		CUR GetCurrent();
		TEMP GetTemperature();
		POW GetPower();
		POS GetPosition();
		TARGET_LOC GetDipole(TARGET_LOC target_loc);
		MAP GetMapping();
		double GetCoreSize();
		DIP Dipole2Current(DIP dipole_);
		double max_dipole_mag;
		void RotatingDipole(DIP init_dipole, AXIS_ROT axis_rot, double freq, int dur);
};
#endif // OMNIMAGNET_H