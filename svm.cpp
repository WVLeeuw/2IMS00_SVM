// Author: Wouter Leeuw
// SVM implementation based on OpenCV

/*
#include "opencv/modules/core/include/opencv2/core.hpp"
#include "opencv/modules/imgproc/include/opencv2/imgproc.hpp"
#include "opencv/modules/imgcodecs/include/opencv2/imgcodecs.hpp"
#include "opencv/modules/highgui/include/opencv2/highgui.hpp"
#include "opencv/modules/ml/include/opencv2/ml.hpp"

using namespace cv;
using namespace cv::ml;
*/

class basic_Support_Vector_Machine {
	// initializaiton
	private:
	double lrate = 0.005;
	double threshold = 0.005;
	double w1 = 1;
	double b = 0;
	double MAX_ITER = 1000000;
	
	public:
	// Gives the loss for a data point
	double calc_loss(int &x1, int &y, double &w1, double &b) {
	
		double loss = 0;
		
		if (y==1) {
			loss = 1 - (w1 * x1 + b);
		} else {
			loss = 1 + (w1 * x1 + b);
		}
		
		if (loss < 0) {
			loss = 0;
		}
		
		return loss;
	}
	
	// Gives the cost at a training state
	double calc_cost(std::vector<int> &x1, std::vector<int> &y, double w1, double b, double &dw1, double &db) {
		
		int n = static_cast<int>(y.size());
		
		double cost = 0;
		dw1 = 0;
		db = 0;
		
		for (int i = 0; i < n; i++) {
			double loss = calc_loss(x1[i], y[i], w1, b);
			cost += loss;
			if (loss > 0) {
				dw1 += (-x1[y] * y[i]);
				db += (-y[i]);
			}
		}
		
		cost /= n;
		dw1 /= n;
		db /= n;
		return cost;
	}
	
	void fit(std::vector<int> &x1, std::vector<int> &y) {
	
		// Initialization of weight and bias updates
		double dw1 = 0;
		double db = 0;
		
		int iter = 0;
		
		while(true) {
			double cost = calc_cost(x1, y, w1, b, dw1, db);
			if (iter % 1000 == 0) {
				std::cout << "Iteration: " << iter << " cost = " << cost << " weight update = " << dw1 << " bias update = " db << std::endl;
			}
			if (abs(dw1) < threshold && abs(db) < threshold || iter >= MAX_ITER) {
				std::cout << "y = " << w1 << " * x1 + " << b << std::endl;
				break; 
			}
			w1 -= lrate * dw1;
			b -= lrate * db;
		}		
	}
}

/*
class OpenCV_Support_Vector_Machine {
	// Initialization
	Ptr<SVM> svm = SVM::create();
	svm -> setType(SVM::C_SVC);
	svm -> setKernel(SVM::LINEAR);
	svm -> setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
		
	void fit(int[] &x1, int[] &y) {
	
		Mat labelsMat((int)sizeof(y), 1, CV_32SC1, y);
		Mat trainingData((int)sizeof(x1), 1, CV_32FC1, x1);
		
		Ptr<TrainData> td = TrainData::create(trainingDataMat, ROW_SAMPLE, labelsMat);
		svm -> train(td);
		
		Vec3b green(0, 255, 0), blue(255, 0, 0);
		// Show decision regions given by SVM
		for (int i = 0; i < image.rows; i++) {
			for (int j = 0; j < image.cols; j++) {
				Mat sampleMat = (Mat_<float>(1, 2) << j, i);
				float response = svm -> predict(sampleMat);
				
				if (response == 1) image.at<Vec3b>(i, j) = green;
				else if (response == -1) image.at<Vec3b>(i, j)=blue;
			}
		}
		
		// Show training data
		int thickness = -1;
		int lineType = 8;
		circle(image, Point(501, 10), 5, Scalar(0, 0, 0), thickness, lineType);
		circle(image, Point(255, 10), 5, Scalar(255, 255, 255), thickness, lineType);
		circle(image, Point(501, 255), 5, Scalar(255, 255, 255), thickness, lineType);
		circle(image, Point(10, 501), 5, Scalar(255, 255, 255), thickness, lineType);
		
		// Show support vectors
		thickness = 2;
		lineType = 8;
		Mat sv = svm -> getSupportVecotrs();
		
		for (int i = 0; i < sv.rows; i++) {
			const float* v = sv.ptr<float>(i);
			circle(image, Point((int)v[0], (int)v[1]), 6, Scalar(128, 128, 128), thickness, lineType);
		}
		
		imshow("SVM Simple Example", image);
		waitKey(0);		
	}
};
*/
