#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
using namespace cv;
using namespace std;

bool start_morph = false;
vector<Point> P1, Q1, P2, Q2;
Mat two;
Mat img1, img2, imga_x,imga_y,imgb_x,imgb_y,imga,imgb;
Mat map21,map12,finalPic;
int width, height;
double alpha;
double X_[4];//x1 y1 x2 y2
IplImage *new_image;
void mouse_event(int event, int x, int y, int flags, void* ustc)
{
	if (start_morph == false){
		if (event == EVENT_LBUTTONDOWN) {
			if (x <= 255 && x >= 0) { //img1
				P1.push_back(Point(x, y));
				cout << "size: " << P1.size() << " P1 = " << P1[P1.size()-1];
			}
			else if (x > 355 && x <= 610) {
				P2.push_back(Point(x - 355, y));
				cout << "size: " << P2.size() << " P2 = " << P2[P2.size() - 1];

			}
		}
		else if (event == EVENT_LBUTTONUP) {
			Point p = (355, 0);
			if (x <= 255 && x >= 0) {
				Q1.push_back(Point(x, y));
				cout << "size: " << Q1.size() << " Q1 = " << Q1[Q1.size() - 1];
				
				line(two, P1[P1.size() - 1], Q1[Q1.size() - 1], Scalar(255, 255, 255), 2, LINE_8);
			}
			else if (x > 355 && x <= 610) {
				Q2.push_back(Point(x - 355, y));
				cout << "size: " << Q2.size() << " Q2 = " << Q2[Q2.size() - 1];
				line(two, Point(P2[P2.size() - 1].x+355, P2[P2.size() - 1].y), Point(Q2[Q2.size() - 1].x + 355, Q2[Q2.size() - 1].y) ,Scalar(255, 255, 255), 2, LINE_8);
				
			}
			imshow("img", two);
		}
	}

}
double Getu(int x,int y,int i,bool flag)
{
	Point P = (0, 0), Q = (0, 0);
	if (flag == true) {//P1 Q1
		P = P1[i];
		Q = Q1[i];
		
	}
	else {//P2 Q2
		P = P2[i];
		Q = Q2[i];
		//cout << "P2=" << P2[i] << " Q2=" << Q2[i] << endl;
	}
	double X_P_x = 0.0, X_P_y = 0.0, Q_P_x = 0.0, Q_P_y = 0.0,u = 0.0,len = 0.0;
	X_P_x = x - P.x;
	X_P_y = y - P.y;
	Q_P_x = Q.x - P.x;
	Q_P_y = Q.y - P.y;
	len = sqrt((Q.x - P.x)*(Q.x - P.x) + (Q.y - P.y)*(Q.y - P.y));
	u = ((X_P_x * Q_P_x) + (X_P_y * Q_P_y)) / (len * len);
	return u;
}
double Getv(int x, int y, int i,bool flag)
{
	Point P = (0, 0), Q = (0, 0);
	if (flag == true) {//P1 Q1
		P = P1[i];
		Q = Q1[i];
	}
	else {//P2 Q2
		P = P2[i];
		Q = Q2[i];
	}
	double X_P_x = 0.0, X_P_y = 0.0, Q_P_x = 0.0, Q_P_y = 0.0, Perp_Q_x = 0.0,Perp_Q_y = 0.0,len = 0.0;
	X_P_x = x - P.x;
	X_P_y = y - P.y;
	Q_P_x = Q.x - P.x;
	Q_P_y = Q.y - P.y;
	Perp_Q_x = Q_P_y;
	Perp_Q_y = -Q_P_x;
	len = sqrt((Q.x - P.x)*(Q.x - P.x) + (Q.y - P.y)*(Q.y - P.y));
	double v = ((X_P_x * Perp_Q_x) + (X_P_y * Perp_Q_y)) / len;
	return v;
}
Point GetPoint(int x, int y, int i,double u, double v, bool flag)
{
	Point P = (0,0), Q = (0,0);
	if (flag == true) {//P1 Q1
		P = P1[i];
		Q = Q1[i];
	}
	else {//P2 Q2
		P = P2[i];
		Q = Q2[i];
	}
	double Perp_Q_x = 0.0, Perp_Q_y = 0.0,len = 0.0;
	Perp_Q_x = Q.y - P.y;
	Perp_Q_y = -(Q.x - P.x);
	len = sqrt((Q.x - P.x)*(Q.x - P.x) + (Q.y - P.y)*(Q.y - P.y));
	Point p;
	p.x = P.x + u * (Q.x - P.x) + ((v * Perp_Q_x) / len);
	p.y = P.y + u * (Q.y - P.y) + ((v * Perp_Q_y) / len);
	return p;
}
double GetWeight(int x, int y, int i , double u,double v,bool flag)
{
	Point P = (0, 0), Q = (0, 0);
	if (flag == true) {//P1 Q1
		P = P1[i];
		Q = Q1[i];
	}
	else {//P2 Q2
		P = P2[i];
		Q = Q2[i];
	}
	int p = 0;
	int a = 1;
	int b = 2;
	double d = 0.0, len = 0.0;
	//if( x == 0 && y == 0)
	//	cout << P << " " << Q << endl;
	len = sqrt((Q.x - P.x)*(Q.x - P.x) + (Q.y - P.y)*(Q.y - P.y));
	
	if (u > 1.0)
	{
		d = sqrt((x - Q.x) * (x - Q.x) + (y - Q.y) * (y - Q.y));
	}
	else if (u < 0)
	{
		d = sqrt((x - P.x) * (x - P.x) + (y - P.y) * (y - P.y));
	}
	else {
		d = abs(v);
	}
	/*if (x == 253 && y == 100)
	{
		cout << "d:" << d << endl;
		cout << "len :" << len << endl;
	}*/
	double weight = pow(pow(len, p) / (a + d), b);
	//if (x == 253 && y == 100) cout << "weight:" << weight << endl;
	return weight;
}

double bounder(double *x) {
	if (x[0] < 0) x[0] = 0;
	if (x[1] < 0) x[1] = 0;
	if (x[2] < 0) x[2] = 0;
	if (x[3] < 0) x[3] = 0;
	if (x[0] > 255) x[0] = 255;
	if (x[1] > 189) x[1] = 189;
	if (x[2] > 255) x[2] = 255;
	if (x[3] > 189) x[3] = 189;
	return *x;
}
CvScalar CombineImg(Mat img, double X, double Y)
{
	IplImage *image;
	image = &IplImage(img);
	int x_floor = (int)X;
	int y_floor = (int)Y;
	int x_ceil = x_floor + 1;
	int y_ceil = y_floor + 1;
	double a = X - x_floor;
	double b = Y - y_floor;
	if (x_ceil >= width - 1)
		x_ceil = width - 1;
	if (y_ceil >= height - 1)
		y_ceil = height - 1;
	CvScalar output_scalar;
	CvScalar leftdown = cvGet2D(image, y_floor, x_floor);
	CvScalar lefttop = cvGet2D(image, y_ceil, x_floor);
	CvScalar rightdown = cvGet2D(image, y_floor, x_ceil);
	CvScalar righttop = cvGet2D(image, y_ceil, x_ceil);
	for (int i = 0; i < 4; i++) {
		output_scalar.val[i] = (1 - a)*(1 - b)*leftdown.val[i] + a*(1 - b)*rightdown.val[i] + a*b*righttop.val[i] + (1 - a)*b*lefttop.val[i];
	}
	return output_scalar;
}
void wrap()
{
	alpha = 0.5;
	Mat image1 = imread("women.jpg");
	Mat image2 = imread("cheetah.jpg");
	double XSum_x1, XSum_y1, XSum_x2, XSum_y2, WSum1, WSum2, NewU1 = 0.0, NewV1 = 0.0;
	double NewU2 = 0.0, NewV2 = 0.0;
	double weight = 0.0;
	Point Newp1, Newp2;
	imga_x.create(image1.size(), CV_32F);
	imga_y.create(image1.size(), CV_32F);
	imgb_x.create(image1.size(), CV_32F);
	imgb_y.create(image1.size(), CV_32F);
	
	map21.create(image1.size(), image1.type());
	map12.create(image1.size(), image1.type());
	finalPic.create(image1.size(),image1.type());
	for (int x = 0; x < 255; x++)
	{
		for (int y = 0; y < 189; y++)
		{
			XSum_x1 = 0.0; XSum_x2 = 0.0;
			XSum_y1 = 0.0; XSum_y2 = 0.0;
			WSum1 = 0.0; WSum2 = 0.0;
			weight = 0.0;
			NewU1 = 0.0; NewU2 = 0.0;
			NewV1 = 0.0; NewV2 = 0.0;
			Newp1 = Point(0, 0); 
			Newp2 = Point(0, 0);
			for (int k = 0; k < 4; k++)
				X_[k] = 0.0;
			for (int i = 0; i < P1.size(); i++)
			{
				//img1
				NewU1 = Getu(x,y,i,true);
				NewV1 = Getv(x, y, i,true);
				Newp1 = GetPoint(x, y, i,NewU1,NewV1,false);
				weight = GetWeight(x,y,i, NewU1,NewV1,true);
				
				XSum_x1 = XSum_x1 + Newp1.x * weight;
				XSum_y1 = XSum_y1 + Newp1.y * weight;
				WSum1 = WSum1 + weight;
				//img2
				NewU2 = Getu(x, y, i, false);
				NewV2 = Getv(x, y, i, false);
				Newp2 = GetPoint(x, y, i, NewU2, NewV2, true);
				weight = GetWeight(x, y, i, NewU2, NewV2, false);
				
				XSum_x2 = XSum_x2 + Newp2.x * weight;
				XSum_y2 = XSum_y2 + Newp2.y * weight;
				WSum2 = WSum2 + weight;
			}
			
			X_[0] = XSum_x1 / WSum1;
			X_[1] = XSum_y1 / WSum1;
			X_[2] = XSum_x2 / WSum2;
			X_[3] = XSum_y2 / WSum2;

			bounder(X_);
			/*if (x > 100 && y > 100)
			{
				cout<< "a[" << X_[0] << ", " << X_[1] << "]" << endl;
				cout<< "b[" << X_[2] << ", " << X_[3] << "]" << endl;
			}*/
			//if (x > 100 && y > 100)cout << "[" << X_[0] <<", "<< X_[1] << "]" <<endl;
			
			imga_x.at<float>(Point(x, y)) = float(X_[0] * 0.75 + x * 0.25);
			imga_y.at<float>(Point(x, y)) = float(X_[1] * 0.75 + y * 0.25);
			imgb_x.at<float>(Point(x, y)) = float(X_[2] * 0.75 + x * 0.25);
			imgb_y.at<float>(Point(x, y)) = float(X_[3] * 0.75 + y * 0.25);
			/*if (x > 200 && y > 100) {
				cout << "x1:" << imga_x.at<float>(Point(x, y)) << endl;
				cout << "y1:" << imga_y.at<float>(Point(x, y)) << endl;
				cout << "x2:" << imgb_x.at<float>(Point(x, y)) << endl;
				cout << "y2:" << imgb_y.at<float>(Point(x, y)) << endl;
			}*/
		}
		
	}

	remap(image2,map21,imga_x,imga_y, INTER_LINEAR);
	remap(image1,map12,imgb_x,imgb_y, INTER_LINEAR);

	cv::addWeighted(map12, 0.5, map21, 0.5, 0.0, finalPic);
	
	//cvShowImage("window", new_image);
	//imshow("window1", map21);
	//imshow("window2", map12);
	imshow("window3", finalPic);
	return;
	
}
int main()
{
	
	img1 = imread("women.jpg");
	img2 = imread("cheetah.jpg");

	namedWindow("img");

	width = img1.cols + img2.cols + 100;

	//img1,img2 length = 255, middle length = 100
	height = max(img1.rows, img2.rows);
	cout << height;
	two = Mat::zeros(Size(width, height), img1.type());
	Rect r1(0, 0, img1.cols, img1.rows);
	Rect r2(0, 0, img2.cols, img2.rows);
	rectangle(two, Point(255, 0), Point(355, height), Scalar(255, 255, 255), 100, 1, 0);
	//putText(two, "MORPHING", Point(260, height / 2), FONT_HERSHEY_COMPLEX, 0.5, Scalar(0, 0, 255));
	r2.x = img1.cols + 100;
	img1.copyTo(two(r1));
	img2.copyTo(two(r2));

	imshow("img", two);
	//line(img1, Point(0, 0), Point(10, 10), Scalar(255, 255, 255), 2, LINE_8);
	//imshow("image", img1);
	//imshow("image2", img2);

	cvSetMouseCallback("img", mouse_event, NULL);
	while (1) {
		int key = cvWaitKey(0);
		if (key == 'q')
		{
			break;
		}
		else if (key == 'm') {
			cout << "1";
			start_morph = true;
			wrap();
			
		}	
	}
	return 0;

}
