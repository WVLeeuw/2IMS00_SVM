// Author: Wouter Leeuw
// File with all functions needed for SVM

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include <cstdlib>

#include "ABY/src/abycore/circuit/circuit.h"
#include "ABY/src/abycore/circuit/arithmeticcircuits.h"
#include "ABY/src/abycore/aby/abyparty.h"
#include "ABY/src/abycore/sharing/sharing.h"
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>

// N.B. if dataset is known, no_samples, attr_per_sample and no_testing can be defined beforehand

// To do: Check whether all data is double (or also int). Double can store either one so it is the safest option.
class Support_Vector_Machine{
	// Initialization
	public:
	double lrate = 0.005;
	double threshold = 0.001;
	double w1 = 1;
	double b = 0;
	
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
		bool opt = false;
		
		while(!opt) {
			double cost = calc_cost(x1, y, w1, b, dw1, db);
			std::cout << "Iteration: " << iter << " cost = " << cost << " weight update = " << dw1 << " bias update = " << db << std::endl;
			iter++;
			if(abs(dw1) < threshold && abs(db) < threshold) {
			std::cout << "y = " << w1 << "* x1 + " << b << std::endl;
			break;
			}
			w1 -= lrate * dw1;
			b -= lrate * db;
			if(iter >= 100000) {
				opt = true;
			}
		}
	}
	
	// Data is single value resulting from the kernel function
	int predict(double &x1, double &w1, double &b) {
		int pred = 0;
		if(x1 * w1 + b >= 1) {
			pred = 1;
		}
		return pred;
	}
};

void to_categorical() {

}

void discretize() {

}

void populate_data(std::vector<std::vector<int>> &data, std::map<std::string, int> &classmap, std::vector<std::map<std::string, int>> attrimap, std::string c, std::vector<std::string> attrs) {

	std::vector<int> apair;
	apair.push_back(classmap[c]);
	int attr_values_num = 0;
	for(int i = 0; i < attrs.size(); i++) {
		apair.push_back(attr_values_num + attrimap[i][attrs[i]]);
		attr_values_num += attrimap[i].size();
	}
	std::vector<std::vector<int>> newarr(1, apair);
	data.insert(data.end(), newarr.begin(), newarr.end());
}

void read_data(std::string filename, std::vector<std::vector<int>>& data, std::map<std::string, int> class_map, std::vector<std::map<std::string, int>>& attr_map, int n_samples) {

	std::ifstream in(filename);
	if (!in) {
		std::cerr << "Cannot open File: " << filename << std::endl;
		return;
	}
	
	std::string c, line, attr;
	std::cout << "Reading file" << std::endl;
	while (getline(in, line)) {
		std::istringstream iss(line);
		iss >> c;
		std::vector<std::string> attrs;
		while (iss >> attr) {
			attrs.push_back(attr);
		}
		populate_data(data, class_map, attr_map, c, attrs);
	}
	std::cout << "File has been read" << std::endl;
}

// Computes the kernel given two shares of vectors
share* build_kernel_circuit(share* s_x1, share* s_x2, uint32_t nvals, uint32_t bitlen, ArithmeticCircuit* ac) {

	// pairwise multiplication
	s_x1 = ac->PutMULGate(s_x1, s_x2);
	
	// split SIMD gate to separate wires
	s_x1 = ac->PutSplitterGate(s_x1);
	
	// add up individual multiplication resuls
	for(int i = 1; i < nvals; i++) {
		s_x1->set_wire_id(0, ac->PutADDGate(s_x1->get_wire_id(0), s_x1->get_wire_id(i)));
	}
	
	// discard all wires, except the addition result
	s_x1->set_bitlength(1);
	
	return s_x1; // kernel result
}

// Test computation of the kernel function
int32_t test_kernel_computation(e_role role, const std::string& address, uint16_t port, seclvl seclvl, uint32_t nvals, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing) {
	
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg);
	
	std::vector<Sharing*>& sharings = party->GetSharings();
	
	ArithmeticCircuit* circ = 
		(ArithmeticCircuit*) 
		sharings[sharing]->GetCircuitBuildRoutine();
	
	share *s_x1, *s_x2, *s_y, *s_out;
	
	uint16_t x1, x2;
	uint16_t output, v_sum = 0;
	
	std::vector<uint16_t> x1_vals(nvals);
	std::vector<uint16_t> x2_vals(nvals);
	
	uint32_t i;
	srand(time(NULL));
	
	for (i=0; i < nvals; i++) {
		x1 = rand();
		x2 = rand();
		
		v_sum += x1 * x2;
		
		x1_vals[i] = x1;
		x2_vals[i] = x2;
	}
	
	s_x1 = circ->PutSIMDINGate(nvals, x1_vals.data(), 16, SERVER);
	s_x2 = circ->PutSIMDINGate(nvals, x2_vals.data(), 16, CLIENT);
	
	s_out = build_kernel_circuit(s_x1, s_x2, nvals, 32, (ArithmeticCircuit*) circ);
	
	s_out = circ->PutOUTGate(s_out, ALL);

	party->ExecCircuit();
	
	output = s_out->get_clear_value<uint16_t>();
	
	std::cout << "\nCircuit Result: " << output;
	std::cout << "\nVerification Result: " << v_sum << std::endl;
	
	delete s_x1;
	delete s_x2;
	delete party;
	
	return 0;
}

int main() {
	std::vector<double> x1 = {35, 27, 19, 25, 26, 45, 46, 48, 47, 29, 27, 28, 27, 30, 28, 23, 27, 18};
	std::vector<int> y = {-1, -1, -1, -1, -1, 1, 1, 1, 1, -1, 1, -1, -1, -1, -1, -1, -1, -1};
	
	Support_Vector_Machine svm;
	
	svm.fit(x1, y);

	return 0;
}
