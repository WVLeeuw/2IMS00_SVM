// Author: Wouter Leeuw
// File with all functions needed for SVM

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include <cstdlib>
#include <unistd.h>

#include "ABY/src/abycore/circuit/circuit.h"
#include "ABY/src/abycore/circuit/arithmeticcircuits.h"
#include "ABY/src/abycore/aby/abyparty.h"
#include "ABY/src/abycore/sharing/sharing.h"
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>

// N.B. if dataset is known, no_samples, attr_per_sample and no_testing can be defined beforehand

// SVM class
class Support_Vector_Machine{
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
			std::cout << "Iteration: " << iter << " cost = " << cost << " weight update = " << dw1 << " bias update = " << db << std::endl;
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

// Currently particularly designed for absenteeism
void read_file(std::string &filename, std::vector<int> &cls, std::vector<std::vector<double>> &data) {
		
	chdir("..");
	
	std::ifstream in(filename);
	
	std::string id, reason, month, day, season, expense, distance, time, age, workload, target, failure, educ, son, drink, smoke, pet, weight, height, bmi, absent_hours;
	
	double n_id, n_reason, n_month, n_day, n_season, n_expense, n_distance, n_time, n_age, n_workload, n_target, n_failure, n_educ, n_son, n_drink, n_smoke, n_pet, n_weight, n_height, n_bmi, n_absent_hours;
	
	std::string line;
	
	if(!in.is_open()) {
		std::cerr << "Cannot open the File: " << filename << std::endl;
	}

	while(getline(in, line)) {
		std::stringstream ss(line);
		
		// Read as strings
		getline(ss, id, ';');		
		getline(ss, reason, ';');
		getline(ss, month, ';');
		getline(ss, day, ';');
		getline(ss, season, ';');
		getline(ss, expense, ';');
		getline(ss, distance, ';');
		getline(ss, time, ';');
		getline(ss, age, ';');
		getline(ss, workload, ';');
		getline(ss, target, ';');
		getline(ss, failure, ';');
		getline(ss, educ, ';');
		getline(ss, son, ';');
		getline(ss, drink, ';');
		getline(ss, smoke, ';');
		getline(ss, pet, ';');
		getline(ss, weight, ';');
		getline(ss, height, ';');
		getline(ss, bmi, ';');
		getline(ss, absent_hours, ';');
		
		// Convert to double
		n_id = stod(id);
		n_reason = stoi(reason);
		n_month = stod(month);
		n_day = stod(day);
		n_season = stod(season);
		n_expense = stod(expense);
		n_distance = stod(distance);
		n_time = stod(time);
		n_age = stod(age);
		n_workload = stod(workload);
		n_target = stod(target);
		n_failure = stod(failure);
		n_educ = stod(educ);
		n_son = stod(son);
		n_drink = stod(drink);
		n_smoke = stod(smoke);
		n_pet = stod(pet);
		n_weight = stod(weight);
		n_height = stod(height);
		n_bmi = stod(bmi);
		n_absent_hours = stod(absent_hours);
		
		// Put data element in vector
		std::vector<double> el = {n_id, n_month, n_day, n_season, n_expense, n_distance, n_time, n_age, n_workload, n_target, n_failure, n_educ, n_son, n_drink, n_smoke, n_pet, n_weight, n_height, n_bmi, n_absent_hours};
		cls.push_back(n_reason);
		data.push_back(el);		
		std::cout << "Read ID: " << el[0] << std::endl;
	}
		
	std::cout << "File read." << std::endl;
	in.close();
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
	
	uint32_t x1, x2;
	uint32_t output, v_sum = 0;
	
	std::vector<uint32_t> x1_vals(nvals);
	std::vector<uint32_t> x2_vals(nvals);
	
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

//int main() {
//
//}
