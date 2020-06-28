#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>
using namespace std;
using namespace cv;

int findMaxContourArea(vector<vector<Point> > contours);

Mat createPoints(Mat img);
Mat drawRectangle(Rect r, Mat img);
bool checkDensity(Mat img, Point p);
void contourCreation(Mat img);
void contourRectangle(Point arr, int size, int j);
//void contoursDrawingRectangle(vector<vector<Point>> contoursDrawing);
void contoursDrawingRectangle(std::vector<std::vector<cv::Point> > contoursDrawing);

int threshold_value = 220;
int threshold_type = 0; //binary threshold
int const max_BINARY_value = 255;

Mat img, imgSegmented, frame;
Mat drawingContours = Mat::zeros(img.rows, img.cols, CV_8UC1);

int main()
{

	//for working with images
	Mat img = imread("inputImg1.png");
	cvtColor(img, imgSegmented, CV_BGR2GRAY);
	//cvtColor(frame, img, CV_BGR2HSV);
	
	//inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgSegmented);

	//filter
	//Canny(frame, cannyOp, 30, 90, 3);	//0,100 //30,60 //100,200 //0,176
	//GaussianBlur(imgSegmented, imgSegmented, Size(7, 7), 1.5, 1.5);	//used to blur the image/frame
	dilate(imgSegmented, imgSegmented, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(imgSegmented, imgSegmented, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	threshold(imgSegmented, imgSegmented, threshold_value, max_BINARY_value, threshold_type);
	//imshow("check", imgSegmented);
	Scalar intensity;
	while (1)
	{
		contourCreation(imgSegmented);
		waitKey(25);
	}

	//for working with webcam
	/*VideoCapture cap = VideoCapture(0);

	int slider = 0;
	int slider_max = 255;

	namedWindow("Control", 1);
	//namedWindow("CannyControl", 2);

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars for controlliing Hue Saturation Value
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
		//frame = imread("hand.png");
		Mat imgHSV;
		//Mat imgOp;
		cvtColor(frame, imgHSV, CV_BGR2HSV);

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgSegmented);

		//filter
		//Canny(frame, cannyOp, 30, 90, 3);	//0,100 //30,60 //100,200 //0,176
		//GaussianBlur(imgSegmented, imgSegmented, Size(7, 7), 1.5, 1.5);	//used to blur the image/frame
		dilate(imgSegmented, imgSegmented, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgSegmented, imgSegmented, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		threshold(imgSegmented, img, threshold_value, max_BINARY_value, threshold_type);

		//imshow("canny op", cannyOp);
		imshow("Original", frame);
		//imshow("Output", img);
		imshow("Segmentation", img);

		//img = createPoints(img);
		contourCreation(img);

		//imshow("Line", img);
		waitKey(25);
	}*/
}

Mat createPoints(Mat img)
{
	//4 points to be found. 
	/*
	1) top -> i, j should be lowest
	2) side_L -> j lowest
	3) side_R -> j highest
	4) bottom -> img.rows
	*/
	Point top, bottom;
	Scalar intensity;
	Point side_R[2];
	Point side_L[2];

	int count = 0;
	side_L[0] = Point(img.cols, img.rows);
	for (long i = 0; i < img.rows; i++)
	{
		for (long j = 0; j < img.cols; j++)
		{
			intensity = img.at<uchar>(Point(j, i));
			if (intensity.val[0] > 200)
			{
				side_R[1] = Point(j, i);
				side_L[1] = Point(j, i);
				if (count == 0)
				{
					if (checkDensity(img, Point(j, i)))
					{
						top = Point(j, i);
						count++;
						//cout << top <<"top\n";
						bottom = Point(j, img.rows);
						//cout << bottom;
					}
				}
				if (side_R[1].x > side_R[0].x)
				{
					side_R[0] = side_R[1];
					//cout << " R" << side_R[0];
				}
				if (side_L[1].x < side_L[0].x)
				{
					//cout << "inside";
					side_L[0] = side_L[1];
					//cout << " L" << side_L[0] << "\n";
				}
			}
		}
	}
	//rectangle width 
	int width = side_R[0].x - side_L[0].x;
	//rectangle height
	int height = img.rows - top.y;
	//to find co-ordinate of vertex

	Point vertex = Point(side_L[0].x, top.y);
	//cout << vertex << "Vertex";

	Rect r = Rect(vertex.x, vertex.y, width, height);
	return drawRectangle(r, img);
}
Mat drawRectangle(Rect r, Mat img)
{
	rectangle(img, r, Scalar(255, 0, 0), 1, 8, 0);
	//imshow("Rectangle", img);

	//crop image based on roi
	Mat crop = img(r);
	//imshow("Cropped image", crop);
	return crop;
}
bool checkDensity(Mat img, Point p)
{
	Scalar checkIntensityV, checkIntensityHL, checkIntensityHR;
	for (int i = 0; i < 5; i++)
	{
		checkIntensityV = img.at<uchar>(Point(p.x, (p.y + i)));
		checkIntensityHL = img.at<uchar>(Point(p.x - 5 + i, p.y + i + 5));
		checkIntensityHR = img.at<uchar>(Point(p.x + 5 - i, p.y + i + 5));
		if (checkIntensityV.val[0] > 200 && checkIntensityHL.val[0] > 200 && checkIntensityHR.val[0] > 200)
		{
			continue;
		}
		else
			return false;
		return true;
	}
	//cout << p.x << " " << p.y;
}
void contourCreation(Mat img)
{
	//cout << "inside contour image" << endl;
	std::vector<std::vector<cv::Point> > contours;
	findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	Mat drawing = Mat::zeros(img.rows, img.cols, CV_8UC1);

	int maxContourAreaLoc = findMaxContourArea(contours);
	drawContours(drawing, contours, maxContourAreaLoc, Scalar(255, 255, 255), CV_FILLED);

	//contoursDrawingRectangle(contours);			//test********************************
	/*vector<RotatedRect> minRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contours[i]));
	//	cout << drawing[i].size() << endl;
	}

	/// Draw contours + rotated rects
	//Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		// contour
		drawContours(drawing, contours, i, Scalar(0, 255, 0), 1, 8);
		// rotated rectangle
		Point2f rect_points[4];
		minRect[i].points(rect_points);
		for (int j = 0; j < 4; j++)
			line(drawing, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 255), 1, 8);
	}
	*/
	//************************************************************************************ to be removed.. only for testing of rectangles around contours


	std::vector<std::vector<cv::Point> > contoursDrawing;
	drawingContours = drawing.clone();
	//findContours(drawingContours, contoursDrawing, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
	//cout << contoursDrawing.size()<< " contours size"<<endl;
	//drawContours(drawingContours, contoursDrawing, 0, Scalar(255, 255, 255));
	
	//vector<vector<Point> > contours_poly(contours.size());

	int i = drawingContours.rows;
	while (contoursDrawing.size() < 4 && i > 0)
	{
		//cout << "Drawing line\t" << contoursDrawing.size()<<endl;
		line(drawingContours, Point(0, i), Point(drawingContours.cols, i), Scalar(0, 0, 0), 5, 8, 0);
		findContours(drawingContours, contoursDrawing, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
		i--;
	}
	if (contoursDrawing.size() == 4)
	{
		vector<RotatedRect> minRect(contoursDrawing.size());
		Rect2d r;
		float scale = 0.34;
		//circle(drawing, Point(175,175), 10, Scalar(255, 255, 0), 1, 8);
		for (size_t i = 0; i < contoursDrawing.size(); i++)
		{
			vector<Rect2f> boundRect(contoursDrawing.size());
			minRect[i] = minAreaRect(Mat(contoursDrawing[i]));
			circle(drawingContours, minRect[i].center, 5, Scalar(255, 255, 0), 1, 8);

			boundRect[i] = boundingRect(Mat(contoursDrawing[i]));
			//cout << boundRect[i].tl() << " " <<boundRect[i].br() << endl;			
			r.br() = boundRect[i].br();
			r.tl() = boundRect[i].tl();
			r.height = boundRect[i].height;
			r.width = boundRect[i].width;

			//rectangle(drawingContours, r, Scalar(0, 255, 255), 1, 8);
			//cout << boundRect[i];
			drawContours(drawingContours, contoursDrawing, i, Scalar(255, 255, 255), 1);
			rectangle(drawing, Point(boundRect[i].tl()), Point(boundRect[i].br()), Scalar(255,0,0), 2, 8, 0);
		}
	}

	imshow("DrawingContours", drawingContours);
	imshow("contours", drawing);
	//cout << contours.size() << "contour Size";
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
void contourRectangle(Point arr, int size, int j)
{
	vector<Rect> boundRect(size);
	for (int i = 0; i < size; i++)
	{
		//approxPolyDP(contours, contours_poly[i], 3, true);
		//boundRect[i] = boundingRect();
		rectangle(drawingContours, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 255), 2);
	}
}
//void contoursDrawingRectangle(vector<vector<Point>> contoursDrawing)
void contoursDrawingRectangle(std::vector<std::vector<cv::Point> > contoursDrawing)
{
	vector<RotatedRect> minRect(contoursDrawing.size());

	for (int i = 0; i < contoursDrawing.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contoursDrawing[i]));
		cout << contoursDrawing[i].size() << endl;
	}

	/// Draw contours + rotated rects
	//Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	for (int i = 0; i < contoursDrawing.size(); i++)
	{
		// contour
		drawContours(drawingContours, contoursDrawing, i, Scalar(0, 255, 0), 1, 8);
		// rotated rectangle
		Point2f rect_points[4];
		minRect[i].points(rect_points);
		for (int j = 0; j < 4; j++)
			line(drawingContours, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 255), 1, 8);
	}
	//imshow("DrawingContours", drawingContours);
}