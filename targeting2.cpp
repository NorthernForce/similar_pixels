/*
*  targeting.cpp
*
*  Created on: Feb 13, 2013
*      Author: Alec Robinson
*
* Allows a user to select pixels samples and have similar pixels detected.
* Also will now find the centers of various shapes. 
*
*/
#include "/home/pi/OpenCV-2.4.3/modules/highgui/include/opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <stdio.h>

using namespace cv;
using namespace std;

bool editcurrent = false; //is true just after the user selects a pixel. when true, the program will get the rgb values of the selected pixel

int thresh = 30; 
int catnumx, catnumy; //x and y of latest pixel selected by the user
int currentpix = 0; //number of selected pixels
int bee[99], jee[99], are[99]; //rgb values of selected pixels (at the time of selection)

bool findcenter = true;
int runningx, runningy, avrgx, avrgy; //for averaging; running total of x values, y values, and the calculated averages

void my_mouse_callback( int event, int x, int y, int flags, void* param );

VideoCapture capturen; 
Mat frame; //mat of the original image
Mat newframe; //mat of the threshholded (threshheld?) image

namespace {


  void targetinginit ()
  {
    namedWindow("Original Image", CV_WINDOW_KEEPRATIO);
    namedWindow("Threshold Image", CV_WINDOW_KEEPRATIO);
    
       
    cvCreateTrackbar( "Threshold", "Threshold Image", &thresh, 100, NULL );
  }

  int targetingprocess ()
  {

    for (;;) {
	  	
            
      capturen >> newframe;

	    	    
	  if (editcurrent) //reads the rgb of the newly selected pixel
	      {
		int z = currentpix-1;
		bee[z] = newframe.at<cv::Vec3b>(catnumx,catnumy)[0];
		jee[z] = newframe.at<cv::Vec3b>(catnumx,catnumy)[1];
		are[z] = newframe.at<cv::Vec3b>(catnumx,catnumy)[2];
		editcurrent = false;
	      }
	      

	    int found = 0; //number of matches found (used for averaging)
	    runningx = 0;
	    runningy = 0;
	    for(int i = 0; i < frame.rows; i++)
	    {
	      for(int j = 0; j < frame.cols; j++)
		{
		  int thisbee = newframe.at<cv::Vec3b>(i,j)[0];
		  int thisjee = newframe.at<cv::Vec3b>(i,j)[1];
		  int thisare = newframe.at<cv::Vec3b>(i,j)[2];
		  int matches = 0;
		  for(int h = 0; h <= currentpix; h++)
		    {

		      if(catnumx+catnumy != 0) if ((thisbee<bee[h]+thresh && thisbee>bee[h]-thresh) && (thisjee<jee[h]+thresh && thisjee>jee[h]-thresh) && (thisare<are[h]+thresh && thisare>are[h]-thresh)) //if it's a match
			{
			  newframe.at<cv::Vec3b>(i,j)[0] = 255; //set to white
			  newframe.at<cv::Vec3b>(i,j)[1] = 255;
			  newframe.at<cv::Vec3b>(i,j)[2] = 255;
			  if (findcenter && matches==0) {runningx += i; runningy += j; found++;}
			  matches++;
			}
		     
		    }
		  if (matches == 0)
			{
			  newframe.at<cv::Vec3b>(i,j)[0] = 0;
			  newframe.at<cv::Vec3b>(i,j)[1] = 0;
			  newframe.at<cv::Vec3b>(i,j)[2] = 0;
			}
		}
		  
	    
	    }
	    if (findcenter)
	      {  
		if (found>0){avrgx = runningx/found; //average the x
		  avrgy = runningy/found;} //average the y
		runningx = 0;
		runningy = 0;
		Point thecenter = Point(avrgy, avrgx);
		ellipse(newframe, thecenter, Size(20,20), 0, 0, 360, Scalar(255,0,238), 2,8); //draw circle
	      }
	    imshow("Threshold Image", newframe);
	    capturen >> frame;
	    if (findcenter) { Point thecenter = Point(avrgy, avrgx); ellipse(frame, thecenter, Size(20,20), 0, 0, 360, Scalar(255,0,238), 2,8);}

            imshow("Original Image", frame);
	    IplImage ipl = frame;
	    cvSetMouseCallback( "Original Image", my_mouse_callback, (void*) &ipl); //get mouse input
	   
            char key = (char)waitKey(5); //get keyboard input
	    if (key == 'q') break;
	    if (key == 'c') {findcenter = !findcenter; runningx = 0; runningy = 0; avrgx = 0; avrgy = 0;}
	}
        return 0;
	

    }


  }


  void my_mouse_callback( int event, int x, int y, int flags, void* param ){
	IplImage* image = (IplImage*) param;

	switch( event ){
	
		case CV_EVENT_LBUTTONDOWN:
		  catnumx = y;
		  catnumy = x;
		  currentpix++;
		  editcurrent = true;
		  break;

	        case CV_EVENT_RBUTTONDOWN:
		  for (int g = 0; g <= currentpix; g++)
		    {
		      catnumx = 0;
		      catnumy = 0;
		    }
		  currentpix = 0;
		  break;
	}
  }





  

int main(int ac, char** av) {
 


     cout << "Left click to collect samples, right click to clear them.\n";
     cout << "'c' to toggle center finding.\n";
     cout << "Press q to quit, but only if you really mean it.\n";

    std::string arg = "0";
    VideoCapture capture(arg); 
    if (!capture.isOpened())
        capture.open(atoi(arg.c_str()));
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 240); //this line and the following line is necessary only for the raspberry pi. if you were to delete these on the pi, you would get timeout errors
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 320);
    if (!capture.isOpened()) {
      cerr << "Failed to open a video device or video file!\n" << endl;
        return 1;
    }
    capturen = capture;
    targetinginit();
    return targetingprocess();
}

