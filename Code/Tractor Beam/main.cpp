#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

// File Saving
#include <filesystem>
#include <fstream>

// Main includes
#include "omnimagnet.hpp"
#include "cameracapture.hpp"

using namespace std;
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std::filesystem;


// Thread Declarations
std::atomic<bool> stop_requested(false);
void inputThread();
void captureThread(CameraCapture& cam,
                   std::ofstream& image_log_file,
                   const std::string& image_output_dir,
                   int timelapse_interval_ms);



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

    // Setup for file saving
    std::ofstream image_log_file;

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
    // MAIN EXPERIMENTAL SETUP

    // Omnimagnet 1 Properties (Left Upper)
    float dipole_strength_1 = 40;       //[Am^2] dipole strength
    Eigen::Vector3d dipole_axis_1;      // Dipole Initial Vector Position
    dipole_axis_1 << 0.0 , 0.0, 1.0;    // (Any axis in XY plane of table is fine)
    Eigen::Vector3d rotation_axis_1;    // Axis the Dipole Rotates About
    rotation_axis_1 << 0.0, 1.0, 0.0;   // Z-axis of table

    // Omnimagnet 5 Properties (Left Lower)
    float dipole_strength_2 = 40;       //[Am^2] dipole strength
    Eigen::Vector3d dipole_axis_2;      // Dipole Initial Vector Position
    dipole_axis_2 << 1.0 , 0.0, 0.0;    // (Any axis in XY plane of table is fine)
    Eigen::Vector3d rotation_axis_2;    // Axis the Dipole Rotates About
    rotation_axis_2 << 0.0, 0.0, -1.0;  // Z-axis of table
    
    // Dipole Rotation Variables
    double freq = 15.0;                 // [Hz] frequency of rotation
    int cycle_time = 2000;              // [ms] run time for each cycle in the experiment
    double test_duration_minutes = 15;  // [min] Duration to run experiment for
    int num_cycles = round(test_duration_minutes * 60 * 1000 / cycle_time / 2); 

    // Timelapse Settings
    bool save_data = true;                      // Toggle timelapse
    int timelapse_interval_ms = 2000;           // Time between photos [ms]
    string experiment_name = "experiment_F3";   // Creates subfolder in /output

    // If camera enabled
    CameraCapture cam(camList.GetByIndex(0));
    if (save_data && !cam.InitializeCamera()) {
        cerr << "Failed to initialize camera.\n";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DOCUMENT EXPERIMENTAL SETUP
    
    // Get starting timestamp
    auto start_time = std::chrono::system_clock::now();
    std::time_t start_time_t = std::chrono::system_clock::to_time_t(start_time);

    // Create output directory
    string base_output_dir = "../output";
    string experiment_output_dir = base_output_dir + "/" + experiment_name;
    string image_output_dir = experiment_output_dir + "/images";



    // Saves a .txt file with all the experimental setup variable values for later reference
    if (save_data) {
        
        // Create directory
        filesystem::create_directories(experiment_output_dir);
        filesystem::create_directories(image_output_dir);

        //
        image_log_file.open(experiment_output_dir + "/" + experiment_name + "_image_log.csv");
        if (!image_log_file.is_open()) {
            std::cerr << "Failed to open image_log.csv\n";
        } else {
            image_log_file << "filename,timestamp\n";
        }

        // Build metadata filename
        string setup_filename = experiment_output_dir + "/" + experiment_name + "_setup_info.txt";
        ofstream setup_file(setup_filename);

        // Get start time

        if (setup_file.is_open()) {
            // Starting Timestamp
            setup_file << "// Experiment Start Time: " << std::ctime(&start_time_t) << "\n";

            // Omnimagnet 1 Properties (Left Upper)
            setup_file << "// Omnimagnet 1 Properties (Left Upper)\n";
            setup_file << "float dipole_strength_1 = " << dipole_strength_1 << ";\n";
            setup_file << "Eigen::Vector3d dipole_axis_1 = [" << dipole_axis_1.transpose() << "];\n";
            setup_file << "Eigen::Vector3d rotation_axis_1 = [" << rotation_axis_1.transpose() << "];\n\n";

            // Omnimagnet 5 Properties (Left Lower)
            setup_file << "// Omnimagnet 5 Properties (Left Lower)\n";
            setup_file << "float dipole_strength_2 = " << dipole_strength_2 << ";\n";
            setup_file << "Eigen::Vector3d dipole_axis_2 = [" << dipole_axis_2.transpose() << "];\n";
            setup_file << "Eigen::Vector3d rotation_axis_2 = [" << rotation_axis_2.transpose() << "];\n\n";

            // Dipole Rotation Variables
            setup_file << "// Dipole Rotation Variables\n";
            setup_file << "double freq = " << freq << ";\n";
            setup_file << "int cycle_time = " << cycle_time << ";\n";
            setup_file << "double test_duration_minutes = " << test_duration_minutes << ";\n";
            setup_file << "int num_cycles = " << num_cycles << ";\n\n";

            // If camera enabled
            setup_file << "// Timelapse Settings\n";
            setup_file << "bool save_data = true;\n";
            setup_file << "int timelapse_interval_ms = " << timelapse_interval_ms << ";\n";
            setup_file << "string experiment_name = \"" << experiment_name << "\";\n";

            setup_file.close();
        } else {
            cerr << "Failed to create " << setup_filename << "\n";
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN EXPERIMENTAL LOOP
    static auto last_capture = chrono::steady_clock::now();

    // Experiment tracking variables
    bool early_exit = false;
    int actual_cycles = 0;

    // Start threads
    std::thread inputT(inputThread);
    std::thread captureT(captureThread,
                     std::ref(cam),
                     std::ref(image_log_file),
                     std::ref(image_output_dir),
                     timelapse_interval_ms);


    for(int i = 0; i < num_cycles && !stop_requested; i++){
        // Print Current Cycle
        std::cout << "\n Cycle: " << i+1 << " / " << num_cycles << "\n";

        // Rotate Magnet 1
        omni_system[0].RotatingDipole(dipole_strength_1*dipole_axis_1, rotation_axis_1, freq, cycle_time);

        // Rotate Magnet 5
        omni_system[4].RotatingDipole(dipole_strength_2*dipole_axis_2, rotation_axis_2, freq, cycle_time);

        // Update experiment data variables
        actual_cycles = i + 1;  // Track how many cycles were actually completed

        // Check for user input to end experiment
        if (stop_requested) {
            early_exit = true;
        break;
    }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN LOOP EXIT - SHUTDOWN

    //////////////////////////////////////////////////////////
    // Join or detach threads
    stop_requested = true;
    inputT.detach();
    captureT.join();

    //////////////////////////////////////////////////////////
    // Log end of experiment data
    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> total_duration = end_time - start_time;

    std::ofstream summary_file(experiment_output_dir + "/" + experiment_name +  "_experiment_summary.txt");
    if (summary_file.is_open()) {
        summary_file << "Total Duration: " << total_duration.count() << " seconds\n";
        summary_file << "Experiment exited early: " << (early_exit ? "Yes" : "No") << "\n";
        summary_file << "Cycles completed: " << actual_cycles << " / " << num_cycles << "\n";
        summary_file.close();
    } else {
        std::cerr << "Failed to write experiment_summary.txt\n";
    }

    //////////////////////////////////////////////////////////
    // Shutting down the camera system:
    if (save_data) {
        cam.ReleaseCamera();
        image_log_file.close();
    }

    //////////////////////////////////////////////////////////
    // Shutting down the omnimagnet system:  --- DON'T MODIFIY ---
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

// This thread waits for the user to press Enter to stop the experiment early
void inputThread() {
    std::cin.get();         // Blocks until enter is pressed
    stop_requested = true;  // Signals all threads to stop
}

// This thread handles background camera capture at a fixed interval
void captureThread(CameraCapture& cam,
                   std::ofstream& image_log_file,
                   const std::string& image_output_dir,
                   int timelapse_interval_ms)
{
    int img_id = 0;                                         // Counter for naming files
    auto last_capture = std::chrono::steady_clock::now();   // Timestamp of last capture

    while (!stop_requested) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_capture).count();

        // Check if enough time has passed to take the next image
        if (elapsed >= timelapse_interval_ms) {
            // Construct filename
            std::string filename = image_output_dir + "/frame_" + std::to_string(img_id++) + ".jpg";
            
            // Try to capture and save the image
            if (cam.CaptureAndSaveImage(filename)) {
                std::cout << "Captured image: " << filename << "\n";

                // Timestamp logging
                std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                char timebuf[32];
                std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
                image_log_file << filename << "," << timebuf << "\n";
            } else {
                std::cerr << "Image capture failed.\n";
            }

            // Update last capture timestamp
            last_capture = now;
        }

        // Sleep briefly to prevent the loop from consuming too much CPU
        // Set to half the interval time arbitrarily
        std::this_thread::sleep_for(std::chrono::milliseconds(timelapse_interval_ms/2)); // Avoid tight loop
    }
}