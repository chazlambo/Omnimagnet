// example: one class, two objects
#include <iostream>

#include <vector>
#include "omnimagnet.hpp"
// #include "cameratrack.hpp"




// using namespace Spinnaker;
// using namespace Spinnaker::GenApi;
// using namespace Spinnaker::GenICam;

// using namespace std;

// class Rectangle {
//     int width, height;
//   public:
//     void set_values (int,int);
//     int area () {return width*height;}
// };

// void Rectangle::set_values (int x, int y) {
//   width = x;
//   height = y;
// }




// Eigen::Matrix3d R;
// // Find your Rotation Matrix
// Eigen::Vector3d T;
// // Find your translation Vector
// Eigen::Matrix4d Trans; // Your Transformation Matrix
// Trans.setIdentity();   // Set to Identity to make bottom row of Matrix 0,0,0,1
// Trans.block<3,3>(0,0) = R;
// Trans.rightCols<1>() = T;



// Custome pseudoinverse:
// Eigen::MatrixXd AshkanPseudoinverse(Eigen::MatrixXd A, double singularMinToMaxRatio)
// {
//     Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV ); // computes the SVD
//     Eigen::MatrixXd S_inv(svd.matrixV().cols(),svd.matrixU().rows());
//     S_inv.setZero();
//     for( int i = 0; i < std::min(A.rows(),A.cols()); i++)
//     {
//         double val = 0;
//         if( svd.singularValues()[i] > svd.singularValues()[0]*singularMinToMaxRatio )// threashold singular values anything less than 1/1000 of the max is set to 0
//             val = 1.0 / svd.singularValues()[i];
//         S_inv(i,i) = val;
//     }
//     Eigen::MatrixXd answer;
//     answer = svd.matrixV()*(S_inv*(svd.matrixU().transpose()));
//     return answer;
// };


// Eigen::MatrixXd Pseudoinverse(Eigen::MatrixXd A)
// {
//     Eigen::MatrixXd B = A.completeOrthogonalDecomposition().pseudoInverse();
//     return B;
// };
// Eigen::MatrixXd pseudoinverse(Eigen::MatrixXd A)
// {
//   // std::cout<< "U" <<std::endl << svd.matrixU()<< std::endl;
//   Eigen::MatrixXf  s = svd.singularValues();
//   // std::cout<< "s" <<std::endl << svd.singularValues()<< std::endl;
//   // std::cout<< "V" <<std::endl << svd.matrixV()<< std::endl;
//   // std::cout<< "VT" <<std::endl << svd.matrixV().adjoint()<< std::endl;
//   // std::cout<<sqrt(svd.matrixU().size())<<std::endl;
//   // std::cout<<sqrt(svd.matrixV().size())<<std::endl;

//   Eigen::MatrixXf S = Eigen::MatrixXf::Zero(sqrt(svd.matrixU().size()),sqrt(svd.matrixV().size()));
//   // std::cout << S << std::endl;
//   for (int i=0 ;i < s.size() ;i++){
//     // std::cout <<i << std::endl;
//     S(i,i) = s(i);
//   }
//   Eigen::MatrixXf S_1 = S.completeOrthogonalDecomposition().pseudoInverse();
//   // std::cout<< "S" <<std::endl << S<< std::endl;
//   // std::cout<< "S-1" <<std::endl << S.inverse()<< std::endl;
//       return (svd.matrixV() * S_1 * svd.matrixU().adjoint());
//   return svd.solve(i);
// };

// Eigen::MatrixXf pseudoinverse(Eigen::MatrixXf m){
//   Eigen::MatrixXf x(m.cols(),m.rows());
//   Eigen::MatrixXf i = Eigen::MatrixXf::Identity(m.cols(),m.cols());
//   std::cout<<"here"<<i;
//   x = m.colPivHouseholderQr().solve(i);
//   // x = m.colPivHouseholderQr().solve(i);
//   // std::cout<<"There";
//   // assert(i.isApprox(x*m));
//   return x;
// };



// omni_system[0].SetProp(18 , 121, 122, 132, 17, 1, 2, 3, true);
// omni_system[1].SetProp(18 , 121, 122, 132, 17, 4, 5, 6, true);
// omni_system[2].SetProp(18 , 121, 122, 132, 17, 7, 8, 9, true);




int main () {


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Setup for the D2A card for Omni system: --- DON'T MODIFIY ---

    // //opening the D2A card:
    // int subdev = 0;     /* change this to your input subdevice */
    // int chan = 10;      /* change this to your channel */
    // int range = 16383;      /* more on this later */
    // int aref = AREF_GROUND;  more on this later 
    // unsigned int subdevice = 0;
    // int res;
    // comedi_t *D2A;
    // D2A = comedi_open("/dev/comedi0");
    //     if(D2A == NULL) {
    //     printf("did not work");
    //     comedi_perror("comedi_open");
    //     return 1;
    // }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Activating the amplifiers inhibits. Pins are 25&26   --- DON'T MODIFIY ---
    // int retval;
    // retval = comedi_data_write(D2A, subdev, 25, 0, AREF_GROUND, 16383.0*3/4);
    // retval = comedi_data_write(D2A, subdev, 26, 0, AREF_GROUND, 16383.0*3/4);
    // int rrr;


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Setup for the Cameras: --- DON'T MODIFIY ---
    // SystemPtr system = System::GetInstance();
    
    // // Retrieve list of cameras from the system
    // CameraList camList = system->GetCameras();
    // const unsigned int numCameras = camList.GetSize();
    // std::cout << "Number of cameras detected: " << numCameras << std::endl;
    // if (numCameras == 0)
    // {
    //     // Clear camera list before releasing system
    //     camList.Clear();
    //     // Release system
    //     system->ReleaseInstance();
    //     std::cout << "Not enough cameras!" << std::endl;
    //     std::cout << "Done! Press Enter to exit..." << std::endl;
    //     getchar();
    //     return -1;
    // }


    /////////////////////////////////////////////////////////////////////////////////
    // Defining the cameras
    // Eigen::Vector3d targetPose;
    // CameraTracker camTrack1(camList.GetByIndex(0), targetPose, 1, 30.0, "../camera_calibration.xml"); //Defines a tracker with a given camera * and 3 IDs for coordinate frame and an ID for the target and address to the calibration file,  
    // cv::Mat Image = camTrack1.GetCurrentFrame();
    // for (int i = 0; i<30; i++){
    //   camTrack1.UpdateTargetPose();
    //   // std::chrono::seconds dura( 1);
    //   // std::this_thread::sleep_for( dura );
    // }

    // bool target_marker = true;     //Show target marker
    // bool origin_markers = false;   //Show orgin outlines 
    // camTrack1.SaveCurrentFrame("frame.jpg",target_marker,origin_markers);
    // std::cout<<camTrack1.GetTargetLocation();
    /////////////////////////////////////////////////////////////////////////////////

    // std::cin>>rrr;
    //Defining the Omni_magnet systme
    int num_omni = 5;   // number of omnimagnets using in system. !! REQUIRED UPDATE !!
    std::vector<OmniMagnet> omni_system(num_omni); // defines 3 omnis use omni_system.SetProp(double wire_width, double wire_len_in, double wire_len_mid, double wire_len_out, double core_size, int pinin, int pinmid, int pinout,bool estimate, comedi *D2A).
    omni_system[0].SetProp(1.35/1000.0 , 121, 122, 132, 17, 2, 0, 18, true);       // Omni #1, left upper 
    // omni_system[1].SetProp(1.35/1000.0 , 121, 122, 132, 17, 3, 11, 19, true, D2A);      // Omni #2, center upper 
    // omni_system[2].SetProp(1.35/1000.0 , 121, 122, 132, 17, 4, 12, 20, true, D2A);      // Omni #3, right upper 
    // omni_system[3].SetProp(1.35/1000.0 , 121, 122, 132, 17, 5, 13, 21, true, D2A);      // Omni #4, right lower 
    // omni_system[4].SetProp(1.35/1000.0 , 121, 122, 132, 17, 6, 14, 22, true, D2A);      // Omni #5, left lower
    // omni_system[5].SetProp(1.35/1000.0 , 121, 122, 132, 17, 7, 15, 23, true, D2A);   // spare (potentially for Super Omni), DON'T TURN ON TILL CONNECTED

    // -- (EXAMPLE) Run Omnimagnet Rotation -- 
    // float freq = 1;          // [Hz] frequency of dipole rotation
    // float run_time = 1000;   // [ms] run time for rotating dipole
    // 
   // omni_system[0].RotatingDipole(dipole_strength*Eigen::Vector3d::UnitX(), -Eigen::Vector3d::UnitY(), freq, run_time); // rotates the dipole aling the given axis, with the given frequency for a given duration. (dipole, axis, frequency(Hz), duration(ms))
   // ---------------------- // 

    // -- (EXAMPLE) Run constant current to Omnimagnet --- 
    CUR current_;
    current_<< 0.0, 0.0, 10.0;
    // omni_system[4].SetCurrent(current_);
    int a;
    std::cin>>a;


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    std::cout<<".......................\n";







    //////////////////////////////////////////////////////////
    // Shutting down the system:  --- DON'T MODIFIY ---
    Eigen::Vector3d off;
    off << 0.0, 0.0, 0.0;
    for (auto &omni : omni_system) // access by reference to avoid copying
    {  
        // omni.SetCurrent(off);
    }
    //Shutting down the amplifiers inhibits. Pins are 25&26 
    // retval = comedi_data_write(D2A, subdev, 25, 0, AREF_GROUND, 16383.0*2/4);
    // retval = comedi_data_write(D2A, subdev, 26, 0, AREF_GROUND, 16383.0*2/4);
    // std::cout<<retval;
    //////////////////////////////////////////////////////////


  return 0;
}