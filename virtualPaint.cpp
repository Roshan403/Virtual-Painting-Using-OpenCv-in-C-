// Before to go with the code you need to include some of the Opencv lib
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat img;
Mat canvas(900, 900, CV_8UC3, Scalar(255, 255, 255));

VideoCapture cap(0);
vector<vector<int>> newPoints;
// to store all points

/////////////////////  COLOR VALUES ////////////////////////////////
// hmin, smin, vmin hmax, smax, vmax
// BGR,BL,WH
//  { 79,0,0,179,255,104 }
vector<vector<int>> myColors{{100, 125, 49, 121, 255, 255}, // blue
                             {78, 66, 66, 105, 255, 206},
                             {155, 88, 186, 178, 255, 255},
                             {0, 0, 243, 124, 73, 255}}; // Green
vector<Scalar> myColorValues{{255, 0, 0},                // Purple
                             {0, 255, 0},
                             {0, 0, 255},
                             {255, 255, 255}}; // Green
////////////////////////////////////////////////////////////////////

class DrawingArea
{
private:
    int xCor, yCor;

public:
    DrawingArea() : xCor(0), yCor(0) {}
    DrawingArea(int x, int y) : xCor(x), yCor(y) {}

    void plotDrawingArea()
    {
        Rect roi(10, 10, 880, 775);
        rectangle(canvas, roi, Scalar(0, 0, 0), 1.5, LINE_4);
        line(canvas, Point(xCor / 2 - 150, 100), Point(xCor / 2 + 174, 100), Scalar(0, 0, 0), 2);
        putText(canvas, "Drawing Area", Point(xCor / 2 - 150, 80), FONT_HERSHEY_DUPLEX, 1.5, Scalar(0, 0, 0), 1);
    }

    void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
    {
        vector<vector<int>> prevPoints;
        for (int i = 0; i < newPoints.size(); i++)
        {
            circle(canvas, Point(newPoints[i][0], newPoints[i][1]), 9, myColorValues[newPoints[i][2]], FILLED);
        }
    }

    void displayDrawingArea()
    {
        imshow("Canvas", canvas);
    }

    vector<vector<int>> findColor(Mat img)
    {
        Mat imgHSV;
        cvtColor(img, imgHSV, COLOR_BGR2HSV);
        for (int i = 0; i < myColors.size(); i++)
        {
            Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
            Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
            Mat mask;
            inRange(imgHSV, lower, upper, mask);

            Point myPoint = getContours(mask);
            if (myPoint.x != 0)
            {
                newPoints.push_back({myPoint.x, myPoint.y, i});
            }
        }
        return newPoints;
    }

    Point getContours(Mat image)
    {
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        drawContours(img, contours, -1, Scalar(255, 0, 255), 2);

        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boundRect(contours.size());

        Point myPoint(0, 0);

        for (int i = 0; i < contours.size(); i++)
        {
            int area = contourArea(contours[i]);
            cout << area << endl;

            string objectType;

            if (area > 900)
            {
                float peri = arcLength(contours[i], true);
                approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

                cout << conPoly[i].size() << endl;
                boundRect[i] = boundingRect(conPoly[i]);
                myPoint.x = boundRect[i].x + boundRect[i].width / 2;
                myPoint.y = boundRect[i].y;
            }
        }
        return myPoint;
    }
};

void main()
{

    int xCor = 900, yCor = 900;
    DrawingArea d1(xCor, yCor);
    d1.plotDrawingArea();
    while (true)
    {
        char key = waitKey(1);
        if (key == 'q' || key == 'Q')
        {
            break;
        }
        cap.read(img);

        newPoints = d1.findColor(img);
        d1.drawOnCanvas(newPoints, myColorValues);
        d1.displayDrawingArea();
        imshow("Image", img);
    }
}