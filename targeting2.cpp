/*
*  targeting2.cpp
*
*  Created on: Feb 16, 2013
*      Author: Alec Robinson
*
* Allows a user to select pixels samples and have similar pixels detected.
* Also will now find the centers of various shapes. 
*
*/
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <stdio.h>

using namespace cv;
using namespace std;

bool findblobs = false;

bool editcurrent = false; //is true just after the user selects a pixel. when true, the program will get the rgb values of the selected pixel

int thresh = 30; 
int catnumx, catnumy; //x and y of latest pixel selected by the user
int currentpix = 0; //number of selected pixels
int bee[50], jee[50], are[50]; //rgb values of selected pixels (at the time of selection)

bool findcenter = true;

void my_mouse_callback( int event, int x, int y, int flags, void* param );

VideoCapture capturen; 
Mat frame; //mat of the original image
Mat newframe; //mat of the threshholded (threshheld?) image
Mat newerframe;
ofstream outfile;
namespace {

  void blobfind(Mat &theimage, int b, int g, int r, float &x, float &y)
  {
    int xrunning = 0, yrunning = 0;
    int found = 0;
    
    int labels [theimage.rows][theimage.cols];
    int foundblobs = 0;
    /*
    for(int i = 0; i < theimage.rows; i++)
      {
	for(int j = 0; j < theimage.cols; j++)
	  {
	    int currentleast = 10;
	    if (theimage.at<cv::Vec3b>(i,j)[0] == 255)
	      {
		int currentleast = 10;
		for (int w = -1; w < 1; w++)
		  {
		    for (int e = -1; e < 2; e++)
		      {
			if (labels [i+w][j+e] < currentleast && labels [i+w][j+e] > 0)
			  {
			    labels [i][j] = labels [i+w][j+e];
			    currentleast = labels [i][j];
			  }
		      }
		  }
	      }
	  }
      }



	      }
	    else
	      {
		labels[i][j] = 0;
	      }
	  }
      }
    */

    //*
    bool foundfirst = false;
    for(int i = 0; i < theimage.rows && !foundfirst; i++)
      {
	for(int j = 0; j < theimage.cols && !foundfirst; j++)
	  {
	    if (theimage.at<cv::Vec3b>(i,j)[0] == 255)
	      {
		theimage.at<cv::Vec3b>(i,j)[0] = b;
		theimage.at<cv::Vec3b>(i,j)[1] = g;
		theimage.at<cv::Vec3b>(i,j)[2] = r;
		xrunning += i;
		yrunning += j;
		found++;
		foundfirst = true;
	      }
	  }
      }
    
    //* Around 0.8 fps
    int newdeclared = 1;
    while (newdeclared > 0)
      {
	newdeclared = 0;
	for(int i = 0; i < theimage.rows; i++)
	  {

	    for(int j = 0; j < theimage.cols; j++)
	      {

		if (theimage.at<cv::Vec3b>(i,j)[0] == b &&
		    theimage.at<cv::Vec3b>(i,j)[1] == g &&
		    theimage.at<cv::Vec3b>(i,j)[2] == r)
		  {
		    for (int k = -1; k < 2; k++)
		      {
			
			for (int l = -1; l < 2; l++)
			  {
			    if (i+k > -1 && i+k < theimage.rows && j+l > -1 && j+l < theimage.cols)
			      {
				if (theimage.at<cv::Vec3b>(i+k,j+l)[0] == 255)
				  {
				    theimage.at<cv::Vec3b>(i+k,j+l)[0] = b;
				    theimage.at<cv::Vec3b>(i+k,j+l)[1] = g;
				    theimage.at<cv::Vec3b>(i+k,j+l)[2] = r;
				    theimage.at<cv::Vec3b>(i,j)[0] = b+1;
				    newdeclared++;
				    xrunning += i+k;
				    yrunning += j+l;
				    found++;
				  }
			      }
			  }
		      }
		  }
	      }
	  }
      }
    

    /* Way less than 0.001 fps
    int newdeclared = 1;
    while (newdeclared > 0)
      {
	newdeclared = 0;
	for(int i = 0; i < theimage.rows; i++)
	  {
	
	    for(int j = 0; j < theimage.cols; j++)
	      {
		
		if (theimage.at<cv::Vec3b>(i,j)[0] == 255 &&
		    theimage.at<cv::Vec3b>(i,j)[1] == 255 &&
		    theimage.at<cv::Vec3b>(i,j)[2] == 255)
		  {
		    for (int k = 0; k < 2; k++)
		      {
			
			for (int l = -1; l < 2; l++)
			  {
			    if (i+k > -1 && i+k < theimage.rows && j+l > -1 && j+l < theimage.cols)
			      {
				if (theimage.at<cv::Vec3b>(i+k,j+l)[0] == b &&
				    theimage.at<cv::Vec3b>(i+k,j+l)[1] == g &&
				    theimage.at<cv::Vec3b>(i+k,j+l)[2] == r)
				  {
				    theimage.at<cv::Vec3b>(i+k,j+l)[0] = b;
				    theimage.at<cv::Vec3b>(i+k,j+l)[1] = g;
				    theimage.at<cv::Vec3b>(i+k,j+l)[2] = r;
				    //theimage.at<cv::Vec3b>(i,j)[0] = b+1;
				    newdeclared++;
				    xrunning += i+k;
				    yrunning += j+l;
				    found++;
				  }
			      }
			  }
		      }
		  }
	      }
	  }
      }
    */

    if (found > 0)
      {
	x = xrunning/found;
	y = yrunning/found;
      }
  }


  void targetinginit ()
  {
    cout << "Left click to collect samples, right click to clear them.\n";
    cout << "'c' to toggle center finding.\n";
    cout << "Press q to quit, but only if you really mean it.\n";

    outfile.open("bgrout.txt", ios::trunc);

    std::string arg = "1";
    VideoCapture capture(arg); 
    if (!capture.isOpened())
        capture.open(atoi(arg.c_str()));
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 240); //this line and the following line is necessary only for the raspberry pi. if you were to delete these on the pi, you would get timeout errors
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 320);
    if (!capture.isOpened()) {
      cerr << "Failed to open a video device or video file!\n" << endl;
    }
    capturen = capture;

    namedWindow("Original Image", CV_WINDOW_KEEPRATIO);
    namedWindow("Threshold Image", CV_WINDOW_KEEPRATIO);
    
       
    cvCreateTrackbar( "Threshold", "Threshold Image", &thresh, 200, NULL );
  }

  int targetingprocess ()
  {

    while (1)
      {
	clock_t t;
	t = clock();

	capturen >> newframe;
	
	
	if (editcurrent) //reads the rgb of the newly selected pixel
	  {
	    
	    int z = currentpix-1;
	    bee[z] = newframe.at<cv::Vec3b>(catnumx,catnumy)[0];
	    jee[z] = newframe.at<cv::Vec3b>(catnumx,catnumy)[1];
	    are[z] = newframe.at<cv::Vec3b>(catnumx,catnumy)[2];	
	    
	    
	    editcurrent = false;
	  }
	
	
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
		    
		    if(catnumx+catnumy != 0) 
		      if ((thisbee<bee[h]+thresh && thisbee>bee[h]-thresh) && (thisjee<jee[h]+thresh && thisjee>jee[h]-thresh) && (thisare<are[h]+thresh && thisare>are[h]-thresh)) //if it's a match
			{
			  newframe.at<cv::Vec3b>(i,j)[0] = 255; //set to white
			  newframe.at<cv::Vec3b>(i,j)[1] = 255;
			  newframe.at<cv::Vec3b>(i,j)[2] = 255;
			  
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
	
	
	erode(newframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	erode(newerframe, newerframe, Mat());
	erode(newerframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	dilate(newerframe, newerframe, Mat());
	
	
	float topx, topy, bottomx1, bottomy1, bottomx2, bottomy2;
	
	if (findblobs) 
	  {
	    blobfind(newerframe, 0, 240, 0, topx, topy);
	    //blobfind(newerframe, 240, 0, 0, bottomx1, bottomy1);
	    //blobfind(newerframe, 0, 0, 240, bottomx2, bottomy2);

	    ellipse( newerframe,
		 Point( topy, topx ),
		 Size( 22, 22 ),
		 360,
		 0,
		 360,
		 Scalar( 102, 0, 255 ),
		 5,
		 2 );

	    ellipse( frame,
		 Point( topy, topx ),
		 Size( 22, 22 ),
		 360,
		 0,
		 360,
		 Scalar( 102, 0, 255 ),
		 5,
		 2 );

	    ellipse( newerframe,
		 Point( bottomy1, bottomx1 ),
		 Size( 22, 22 ),
		 360,
		 0,
		 360,
		 Scalar( 0, 208, 255 ),
		 5,
		 2 );

	    ellipse( frame,
		 Point( bottomy1, bottomx1 ),
		 Size( 22, 22 ),
		 360,
		 0,
		 360,
		 Scalar( 0, 208, 255 ),
		 5,
		 2 );

	    ellipse( newerframe,
		 Point( bottomy2, bottomx2 ),
		 Size( 22, 22 ),
		 360,
		 0,
		 360,
		 Scalar( 0, 55, 255 ),
		 5,
		 2 );

	    ellipse( frame,
		 Point( bottomy2, bottomx2 ),
		 Size( 22, 22 ),
		 360,
		 0,
		 360,
		 Scalar( 0, 55, 255 ),
		 5,
		 2 );


	  }
	
        


	imshow("Threshold Image", newerframe);
	capturen >> frame;
	
	
	imshow("Original Image", frame);
	IplImage ipl = frame;
	cvSetMouseCallback( "Original Image", my_mouse_callback, (void*) &ipl); //get mouse input
	
	char key = (char)waitKey(5); //get keyboard input
	if (key == 'q') break;
	if (key == 'b') findblobs = !findblobs;
	if (key == 's')
	  {
	    outfile.close();
	    outfile.open("bgrout.txt", ios::trunc);
	    outfile << thresh << "\n";
	    for (int z = 0; z < currentpix; z++)
	      {
		outfile << bee[z] << "\n";
		outfile << jee[z] << "\n";
		outfile << are[z] << "\n";
	      }
	  }

	t = clock() - t;
	printf ("Getting %f frames per second.\n",1/(((float)t)/CLOCKS_PER_SEC));
	
      }
    outfile.close();
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
		  //outfile.close();
		  //outfile.open("bgrout.txt", ios::trunc);

		  for (int g = 0; g <= currentpix; g++)
		    {
		      catnumx = 0;
		      catnumy = 0;
		    }
		  currentpix = 0;
		  break;
		  /*
		 case 'e':
		  erodeit = !erodeit;
		  break;
		  */
	}
  }





  

int main() {
 
    targetinginit();
    return targetingprocess();
}
