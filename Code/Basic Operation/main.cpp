// example: one class, two objects
#include <iostream>

#include <vector>
#include "omnimagnet.hpp"


int main () {
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Setup for the D2A card for Omni system: --- DON'T MODIFIY ---

    //opening the D2A card:
    int subdev = 0;     /* change this to your input subdevice */
    int chan = 10;      /* change this to your channel */
    int range = 16383;      /* more on this later */
    int aref = AREF_GROUND; /* more on this later */
    unsigned int subdevice = 0;
    int res;
    comedi_t *D2A;
    D2A = comedi_open("/dev/comedi0");
        if(D2A == NULL) {
        printf("did not work");
        comedi_perror("comedi_open");
        return 1;
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Activating the amplifiers inhibits. Pins are 25&26   --- DON'T MODIFIY ---
    int retval;
    retval = comedi_data_write(D2A, subdev, 25, 0, AREF_GROUND, 16383.0*3/4);
    retval = comedi_data_write(D2A, subdev, 26, 0, AREF_GROUND, 16383.0*3/4);
    int rrr;

    /////////////////////////////////////////////////////////////////////////////////
    //Defining the Omni_magnet system
    int num_omni = 5;                   // number of omnimagnets using in system. !! REQUIRED UPDATE !!
    std::vector<OmniMagnet> omni_system(num_omni); // defines 3 omnis use omni_system.SetProp(double wire_width, double wire_len_in, double wire_len_mid, double wire_len_out, double core_size, int pinin, int pinmid, int pinout,bool estimate, comedi *D2A).
    
    omni_system[0].SetProp(1.35/1000.0 , 121, 122, 132, 17, 2, 0, 18, true, D2A); omni_system[0].UpdateMapping();      // Omni #1, left upper 
    omni_system[1].SetProp(1.35/1000.0 , 121, 122, 132, 17, 3, 11, 19, true, D2A); omni_system[1].UpdateMapping();     // Omni #2, center upper 
    omni_system[2].SetProp(1.35/1000.0 , 121, 122, 132, 17, 4, 12, 20, true, D2A); omni_system[2].UpdateMapping();     // Omni #3, right upper 
    omni_system[3].SetProp(1.35/1000.0 , 121, 122, 132, 17, 5, 13, 21, true, D2A); omni_system[3].UpdateMapping();     // Omni #4, right lower 
    omni_system[4].SetProp(1.35/1000.0 , 121, 122, 132, 17, 6, 14, 22, true, D2A); omni_system[4].UpdateMapping();     // Omni #5, left lower
    // omni_system[5].SetProp(1.35/1000.0 , 121, 122, 132, 17, 7, 15, 23, true, D2A); omni_system[5].UpdateMapping();  // spare (potentially for Super Omni), DON'T TURN ON TILL CONNECTED

    // -- (EXAMPLE) Run Omnimagnet Rotation -- 
    float dipole_strength = 30;         //[Am^2] dipole strength
    Eigen::Vector3d dipole_axis;        // dipole inital vector position
    dipole_axis << 1.0 , 0.0, 0.0;      
    Eigen::Vector3d rotation_axis;      // rotation axis 
    rotation_axis << 0.0, 1.0, 0.0; 
    double freq = 1.0;                  // [Hz] frequency of rotation
    int run_time = 10;               // [ms] run time of demo 

    // omni_system[3].RotatingDipole(dipole_strength*dipole_axis, -rotation_axis, freq, run_time); // rotates the dipole aling the given axis, with the given frequency for a given duration. (dipole, axis, frequency(Hz), duration(ms))
    // omni_system[0].RotatingDipole(dipole_strength*dipole_axis, -rotation_axis, freq, run_time); // rotates the dipole aling the given axis, with the given frequency for a given duration. (dipole, axis, frequency(Hz), duration(ms))

   // ---------------------- // 

    // -- (EXAMPLE) Run constant current to Omnimagnet --- 
    // Eigen::Vector3d current_;
    // current_<< 0.0, 0.0, 10.0;
    // omni_system[0].SetCurrent(current_);
    int a;       //user input turns off demo 
    std::cin>>a;


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    std::cout<<"\nDemo ended!!! Spin Test 3\n";


    //////////////////////////////////////////////////////////
    // Shutting down the system:  --- DON'T MODIFIY ---
    Eigen::Vector3d off;
    off << 0.0, 0.0, 0.0;
    for (auto &omni : omni_system) // access by reference to avoid copying
    {  
        omni.SetCurrent(off);
        std::cout <<"\nSystem Omnimagnet OFF\n";    //will repeat for each omnimagnet
    }
    std::cout << "\nZero current to all Omnimagnets\n";
    
    //Shutting down the amplifiers inhibits. Pins are 25&26 
    retval = comedi_data_write(D2A, subdev, 25, 0, AREF_GROUND, 16383.0*2/4);
    retval = comedi_data_write(D2A, subdev, 26, 0, AREF_GROUND, 16383.0*2/4);
    std::cout<<retval;
    //////////////////////////////////////////////////////////


  return 0;
}