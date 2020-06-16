// Author: Wouter Leeuw
// SVM implementation based on OpenCV

class Support_Vector_Machine {
	// Initialization
	private:
	double lrate = 0.005;
	double threshold = 0.005;
	double w1 = 1;
	double b = 0;
	
	public:
	// Gives the loss for a data point
	double calc_loss(double &x1, int &y, double &w1, double &b) {
	
		double loss = 0;
		if(y==1) {
			loss = 1-(w1*x1 + b);
		} else {
			loss = 1+(w1*x1 + b);
		}
		if(loss < 0) {
			loss = 0;
		}
		return loss;
	}
	
	// Gives the cost at a training state
	double calc_cost(std::vector<double> &x1, std::vector<int> &y, double w1, double b, double &dw1, double &db) {
	
		int n = static_cast<int>(y.size());
		
		double cost = 0;
		dw1 = 0;
		db = 0;
		
		for(int i=0; i<n; i++) {
			double loss = calc_loss(x1[i], y[i], w1, b);
			cost += loss;
			if(loss > 0) {
				dw1 += (-x1[i] * y[i]);
				db += (-y[i]);
			}
		}
		
		cost /= n;
		dw1 /= n;
		db /= n;
		return cost;
	}
	
	void fit(std::vector<double> &x1, std::vector<int> &y) {
	
		// Initialization of the differences in weights and bias
		double dw1 = 0;
		double db = 0;
		
		int iter = 0;
		
		while(true) {
			double cost = calc_cost(x1, y, w1, b, dw1, db);
			if(iter % 1000 == 0) {
			std::cout << "Iteration: " << iter << ", cost = " << cost << ", weight update = " << dw1 << ", bias update = " << db << std::endl;
			}
			iter++;
			if(abs(dw1) < threshold && abs(db) < threshold || iter >= 1000000) {
				std::cout << "y = " << w1 << " * x1 + " << b << std::endl;
				break;
			}
			w1 -= lrate * dw1;
			b -= lrate * db;
		}
	}
	
	// Data is single value resulting from the kernel function
	// Not needed for project
	int predict(double &x1) {
		int pred = 0;
		if(x1 * w1 + b >= 1) {
			pred = 1;
		}
		std::cout << "Predicted class: " << pred << std::endl;
		return pred;
	}
};
