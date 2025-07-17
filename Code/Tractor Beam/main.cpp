#include <iostream>

#include <vector>
#include "omnimagnet.hpp"

#include "cameratrack.hpp"


using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;


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

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Setup for the Cameras: --- DON'T MODIFIY ---
    SystemPtr system = System::GetInstance();
    
    // Retrieve list of cameras from the system
    std::cout << "\nGetting Camera(s)\n";

    CameraList camList = system->GetCameras();
    const unsigned int numCameras = camList.GetSize();
    std::cout << "Number of cameras detected: " << numCameras << std::endl;
    if (numCameras == 0)
    {
        // Clear camera list before releasing system
        camList.Clear();
        // Release system
        system->ReleaseInstance();
        std::cout << "Not enough cameras!" << std::endl;
        std::cout << "Done! Press Enter to exit..." << std::endl;
        getchar();
        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // Defining the cameras
    std::cout << "Setup target tracking";
    Eigen::Vector3d targetPose;
    CameraTracker camTrack1(camList.GetByIndex(0), targetPose, 1, 30.0, "../camera_calibration.xml"); //Defines a tracker with a given camera * and 3 IDs for coordinate frame and an ID for the target and address to the calibration file,  
    cv::Mat Image = camTrack1.GetCurrentFrame();
    for (int i = 0; i<30; i++){
      camTrack1.UpdateTargetPose();
      // std::chrono::seconds dura( 1);
      // std::this_thread::sleep_for( dura );
    }

    // Test/Example Camera function
    bool target_marker = true;     //Show target marker
    bool origin_markers = false;   //Show orgin outlines 
    camTrack1.SaveCurrentFrame("frame.jpg",target_marker,origin_markers);
    std::cout << "\nTarget Position:\n" << camTrack1.GetTargetLocation() << "\n"; // print target location 
    std::cout << "\nTarget Orientation:\n" << camTrack1.GetTargetOrientaion() << "\n"; // print target location 

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


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN EXPERIMENT CODE
    // --  Run Omnimagnet Rotation -- 

    // Define Rotating Dipole Properties

    // Omnimagnet 1 (Left Upper)
    float dipole_strength_1 = 40;       //[Am^2] dipole strength
    Eigen::Vector3d dipole_axis_1;      // Dipole Initial Vector Position
    dipole_axis_1 << 0.0 , 0.0, 1.0;    // (Any axis in XY plane of table is fine)
    Eigen::Vector3d rotation_axis_1;    // Axis the Dipole Rotates About
    rotation_axis_1 << 0.0, 1.0, 0.0;   // Z-axis of table

    // Omnimagnet 5 (Left Lower)
    float dipole_strength_2 = 40;       //[Am^2] dipole strength
    Eigen::Vector3d dipole_axis_2;      // Dipole Initial Vector Position
    dipole_axis_2 << 1.0 , 0.0, 0.0;    // (Any axis in XY plane of table is fine)
    Eigen::Vector3d rotation_axis_2;    // Axis the Dipole Rotates About
    rotation_axis_2 << 0.0, 0.0, -1.0;  // Z-axis of table
    
    // Dipole Rotation Variables
    double freq = 15.0;                 // [Hz] frequency of rotation
    int cycle_time = 2000;              // [ms] run time for each cycle in the experiment
    double test_duration_minutes = 10;  // [min] Duration to run experiment for

    // Run test for duration
    int num_cycles = round(test_duration_minutes * 60 * 1000 / cycle_time / 2); 

    for(int i = 0; i < num_cycles; i++){
        // Print Current Cycle
        std::cout << "\n Cycle: " << i+1 << " / " << num_cycles << "\n";

        // Rotate Magnet 1
        omni_system[0].RotatingDipole(dipole_strength_1*dipole_axis_1, rotation_axis_1, freq, cycle_time);

        // Rotate Magnet 5
        omni_system[4].RotatingDipole(dipole_strength_2*dipole_axis_2, rotation_axis_2, freq, cycle_time);

        // Check for user input to end experiment
        if (!std::cin.eof() && std::cin.peek() != EOF) {
            std::string dummy;
            std::getline(std::cin, dummy); // consume whatever was entered
            std::cout << "\nInput detected. Stopping experiment...\n";
            break;
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    


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