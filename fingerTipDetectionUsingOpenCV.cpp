/*
 *Project Name : Fingertip Detection using OpenCV.
 *Developed in : Microsoft Visual Studio Community 2019
 *				 Microsoft Visual C++ 2019
 *				 Version-16.5.4
 *Author : Yogesh Patil
 *
 *Masters Student of Information Technology at SRH Hochschule Heidelberg.
 *Developed for Image Processing Project.
 *
 *Description: This project takes hand as input, detects hand using skin segmentation in HSV colorspace and detects the fingertips.
 */
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

/*------------------------------------------------------------------------------------------------------------------------------------
 * Functions -
 *------------------------------------------------------------------------------------------------------------------------------------
 * 1) int findMaxContourArea(vector<vector<Point> > contours);
 * This function is used to calculate contour areas and eventually find out a contour with Max area.
 * Return type is int as the function returns the position of contour with max area of contour.
 * Parameters:
 * contours		- datatype is vector<vector<Point> >.
 *				  All the found contours are passed to process the function. The declaration of function is written after main routine.
 *------------------------------------------------------------------------------------------------------------------------------------
 * 2) void fingerTipDetection(Mat img);
 * This function is used for the processing after the image has been segmented. All the processing is done with finding contours.
 *	a) We find Region of Interest with maximum contour area.
 *  b) Start drawing lines from the bottom.
 *  c) Check for number of contours and area.
 *  d) Draw Rectangles.
 *------------------------------------------------------------------------------------------------------------------------------------
 */
int findMaxContourArea(vector<vector<Point> > contours);
void fingerTipDetection(Mat img);

int threshold_value = 220;
int threshold_type = 0; //binary threshold
int const max_BINARY_value = 255;

Mat img, imgSegmented, frame;
Mat drawingMaxArea = Mat::zeros(img.rows, img.cols, CV_8UC1);

int main()
{
	VideoCapture cap = VideoCapture(0);

	int slider = 0;
	int slider_max = 255;

	namedWindow("Control", 1);

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars for controlling Hue Saturation Value
	cvCreateTrackbar("LowH", "Control", &iLowH, 255);
	cvCreateTrackbar("HighH", "Control", &iHighH, 255);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255);
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255);
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	for (;;)
	{
		cap >> frame;
		cap.read(frame);
		Mat imgHSV;
		cvtColor(frame, imgHSV, CV_BGR2HSV);

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgSegmented);

		//Morphological filters
		dilate(imgSegmented, imgSegmented, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgSegmented, imgSegmented, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		threshold(imgSegmented, img, threshold_value, max_BINARY_value, threshold_type);

		imshow("Segmentation", img);

		fingerTipDetection(img);

		waitKey(25);
	}
}
void fingerTipDetection(Mat img)
{
	std::vector<std::vector<cv::Point> > contours;
	findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	Mat drawing = Mat::zeros(img.rows, img.cols, CV_8UC1);

	int maxContourAreaLoc = findMaxContourArea(contours);
	drawContours(drawing, contours, maxContourAreaLoc, Scalar(255, 255, 255), CV_FILLED);

	std::vector<std::vector<cv::Point> > contoursMaxArea;	//contoursDrawing;
	drawingMaxArea = drawing.clone();	//drawingContours 

	int k = drawingMaxArea.rows;
	while (contoursMaxArea.size() <= 4 && k > 0)
	{
		line(drawingMaxArea, Point(0, k), Point(drawingMaxArea.cols, k), Scalar(0, 0, 0), 5, 8, 0);
		findContours(drawingMaxArea, contoursMaxArea, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
		k--;

		if (contoursMaxArea.size() <= 4)
		{
			vector<RotatedRect> minRect(contoursMaxArea.size());
			float scale = 0.34;
			for (size_t i = 0; i < contoursMaxArea.size(); i++)
			{
				if (contourArea(contoursMaxArea[i]) > 3900 && contourArea(contoursMaxArea[i]) < 4000 && contoursMaxArea[i].size() > 180)
				{
					drawContours(drawingMaxArea, contoursMaxArea, i, Scalar(255, 255, 255));

					minRect[i] = minAreaRect(Mat(contoursMaxArea[i]));
					Point2f rectPoints[4];
					minRect[i].points(rectPoints);

					//Reduce rectangle size. Adjust height/ width and centre
					if (minRect[i].size.height > minRect[i].size.width)
					{
						minRect[i].center = (rectPoints[1] + rectPoints[2]) / 2 + (rectPoints[0] - rectPoints[1]) / 6;
						minRect[i].size.height = (float)scale * (minRect[i].size.height);
					}
					else
					{
						minRect[i].center = (rectPoints[2] + rectPoints[3]) / 2 + (rectPoints[0] - rectPoints[3]) / 6;
						minRect[i].size.width = (float)scale * (minRect[i].size.width);
					}

					minRect[i].points(rectPoints);
					for (int j = 0; j < 4; j++)
						line(frame, rectPoints[j], rectPoints[(j + 1) % 4], Scalar(255, 0, 0), 1, 8);
				}
			}
		}
	}
	imshow("Fingers Seperated", drawingMaxArea);
	imshow("Output", frame);
}
int findMaxContourArea(vector<vector<Point> > contours)
{
	int maxContourAreaLoc = 0;
	double maxContourArea = 0;

	for (size_t i = 0; i < contours.size(); i++)
	{
		if (contourArea(contours[i], false) > maxContourArea)
		{
			maxContourArea = contourArea(contours[i], false);
			maxContourAreaLoc = i;
		}
	}
	return maxContourAreaLoc;
}
