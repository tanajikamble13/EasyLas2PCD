#include <iostream>
#include <cstdlib>
#include <liblas/liblas.hpp>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/common/common.h>
#include <fstream>      // std::fstream
using namespace std;
typedef pcl::PointXYZRGBL PointT;

int main (int argc, char *argv[])
{
        string filePath;
	cout << "===================================================================" << endl;
	cout << "LAS2PCD - Converts .las point clouds into PCL-friendly format .pcd" << endl;
	cout << "ver 0.1 - 14 June 2019" << endl;
	cout << "(c) Tanaji Kamble" << endl;
	cout << "DCIL, India" << endl;
	cout << "contact: tanajikamble13@gmail.com" << endl;
	cout << "https://github.com/tanajikamble13" << endl;
	cout << "===================================================================" << endl;
        cout << "sample input: ./las2pcd  rgb.las true"<< endl;
	cout << endl;

	// cout << "Enter full .las file path: (or you can also drag the file here)" << endl;
    
	// getline(cin, filePath);
	// Edited to make fully command-line:
	filePath=argv[1];

    std::cerr << "INFO : Loading : " << filePath << std::endl;
    
    // instancing a new PCL pointcloud object
    pcl::PointCloud<PointT> cloud;

    // Opening  the las file
    std::ifstream ifs(filePath.c_str(), std::ios::in | std::ios::binary);

	// Safeguard against opening failure
    if(ifs.fail()) 
	{
        std::cerr << "ERROR : Can't open file : " << filePath <<std::endl;
        return 1;
    }

    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs); // reading las file
    unsigned long int nbPoints=reader.GetHeader().GetPointRecordsCount();
    int count = nbPoints * 0.025;
    cout << " Header \n " << reader.GetHeader() << endl;
	// Fill in the cloud data
	cloud.width    = nbPoints;				// This means that the point cloud is "unorganized"
	cloud.height   = 1;						// (i.e. not a depth map)
	cloud.is_dense = false;
	cloud.points.resize (cloud.width * cloud.height);

	cout << "INFO : " << cloud.points.size () << " points detected in " << filePath << endl;

	int i=0;				// counter
	uint16_t r1, g1, b1;	// RGB variables for .las (16-bit coded)
	int r2, g2, b2;			// RGB variables for converted values (see below)
	uint32_t rgb;			// "packed" RGB value for .pcd

	while(reader.ReadNextPoint()) 
	{
		// get XYZ information
        cloud.points[i].x = float (reader.GetPoint().GetX());
        cloud.points[i].y = float (reader.GetPoint().GetY());
        cloud.points[i].z = float (reader.GetPoint().GetZ());
        cloud.points[i].label= int (reader.GetPoint().GetClassification().GetClass());
        //cloud->points[i].intensity = (reader.GetPoint().GetIntensity());
        r1= reader.GetPoint().GetColor().GetRed();
        g1= reader.GetPoint().GetColor().GetGreen();
        b1= reader.GetPoint().GetColor().GetBlue();
        r2 = ceil(((float)r1 / 65536)*(float)256);
        g2 = ceil(((float)g1 / 65536)*(float)256);
        b2 = ceil(((float)b1 / 65536)*(float)256);
        rgb = ((int)r2) << 16 | ((int)g2) << 8 | ((int)b2);
        cloud.points[i].rgb = *reinterpret_cast<float*>(&rgb);
        if (i % count == 0)
            std::cout << "Point number :" <<  i  << "\t\t  x: " << reader.GetPoint().GetX() << "  y: " << reader.GetPoint().GetY() 
                      << "  z: " << reader.GetPoint().GetZ() << "  RGB: " << r2 << "Classification "
                      << cloud.points[i].label<< "\n"; // cloud.points[i].rgb
				
        i++; // ...moving on
     }
        
    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(255, 255, 255);
    //pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGBL> rgb(cloud.makeShared());
   // viewer->addPointCloud(cloud.makeShared(), "cloud");
    viewer->addPointCloud(cloud.makeShared());

	while (!viewer->wasStopped())
	{
		viewer->spin();
		boost::this_thread::sleep(boost::posix_time::microseconds(10));
	}
	
	// Allows output file to be set:
    std::string fileToRead (argv[1]);
    std::string fileToSave = fileToRead + ".pcd";
    //If you want to open saved pcd file in text format, save in binary format
	pcl::io::savePCDFileBinary (fileToSave, cloud); //savePCDFileASCII, alternative format to binary	
	std::cerr << "Saved " << cloud.points.size () << " data points to pointcloud.pcd." << std::endl;
    // use loop for multiple file
    bool TXT=false;
    TXT=argv[2];
    if(TXT)
    {
        std::fstream fs; 
        //int j=1;
        //std::stringstream ss; 
        //ss << j << ".txt"; 
        fs.open("1.txt", std::fstream::out); 
        for (size_t i = 0; i < cloud.points.size (); ++i) 
        {
         fs << cloud.points[i].x << "\t" 
            << cloud.points[i].y << "\t"
            << cloud.points[i].z << "\t"
            << cloud.points[i].r << "\t" 
            << cloud.points[i].g << "\t"
            << cloud.points[i].b << "\n";
        } 
        std::cout << "point cloud saved in text format : "<< std::endl;
        fs.close(); 
     }
   return (0);
}
