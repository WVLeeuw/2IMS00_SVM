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

#include "svm.cpp"

// Particularly designed for absenteeism
void read_absenteeism(std::vector<int> &cls, std::vector<std::vector<double>> &data) {
		
	std::string filename = "Datasets/Absenteeism/Absenteeism_at_work.csv";
	chdir("..");
	
	std::ifstream in(filename);
	
	// Variables in the dataset
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
	}
		
	std::cout << "File read." << std::endl;
	in.close();
}

void read_breast_cancer(std::vector<int> &cls, std::vector<std::vector<double>> &data) {
	std::string filename = "Datasets/breast_cancer/breast-cancer.data";
	chdir("..");
	
	std::ifstream in(filename);
	
	if(!in.is_open()) {
		std::cerr << "Cannot open the File: " << filename << std::endl;
	}
	
	// Variables in the dataset
	std::string recc, age, meno, tumor_size, inv_nodes, node_caps, breast, breast_quad, irradiat, deg_malig;
	
	double n_age, n_meno, n_tumor_size, n_inv_nodes, n_node_caps, n_deg_malig, n_breast, n_breast_quad, n_irradiat;
	int n_recc;
	
	std::string line;
	
	while(getline(in, line)) {
		std::stringstream ss(line);
		
		// Read as strings
		getline(ss, recc, ',');
		getline(ss, age, ',');
		getline(ss, meno, ',');
		getline(ss, tumor_size, ',');
		getline(ss, inv_nodes, ',');
		getline(ss, node_caps, ',');
		getline(ss, deg_malig, ',');
		getline(ss, breast, ',');
		getline(ss, breast_quad, ',');
		getline(ss, irradiat, ',');
		
		// Convert to double
		if(recc == "no-recurrence-events") n_recc = 0;
		if(recc == "recurrence-events") n_recc = 1;
		
		if(age == "10-19") n_age = 0;
		if(age == "20-29") n_age = 1;
		if(age == "30-39") n_age = 2;
		if(age == "40-49") n_age = 3;
		if(age == "50-59") n_age = 4;
		if(age == "60-69") n_age = 5;
		if(age == "70-79") n_age = 6;
		if(age == "80-89") n_age = 7;
		if(age == "90-99") n_age = 8;
			
		if(meno == "lt40") n_meno = 1;
		if(meno == "ge40") n_meno = 2;
		if(meno == "premono") n_meno = 0;
		
		if(tumor_size == "0-4") n_tumor_size = 0;
		if(tumor_size == "5-9") n_tumor_size = 1;
		if(tumor_size == "10-14") n_tumor_size = 2;
		if(tumor_size == "15-19") n_tumor_size = 3;
		if(tumor_size == "20-24") n_tumor_size = 4;
		if(tumor_size == "25-29") n_tumor_size = 5;
		if(tumor_size == "30-34") n_tumor_size = 6;
		if(tumor_size == "35-39") n_tumor_size = 7;
		if(tumor_size == "40-44") n_tumor_size = 8;
		if(tumor_size == "45-49") n_tumor_size = 9;
		if(tumor_size == "50-54") n_tumor_size = 10;
		if(tumor_size == "55-59") n_tumor_size = 11;
		
		if(inv_nodes == "0-2") n_inv_nodes = 0;
		if(inv_nodes == "3-5") n_inv_nodes = 1;
		if(inv_nodes == "6-8") n_inv_nodes = 2;
		if(inv_nodes == "9-11") n_inv_nodes = 3;
		if(inv_nodes == "12-14") n_inv_nodes = 4;
		if(inv_nodes == "15-17") n_inv_nodes = 5;
		if(inv_nodes == "18-20") n_inv_nodes = 6;
		if(inv_nodes == "21-23") n_inv_nodes = 7;
		if(inv_nodes == "24-26") n_inv_nodes = 8;
		if(inv_nodes == "27-29") n_inv_nodes = 9;
		if(inv_nodes == "30-32") n_inv_nodes = 10;
		if(inv_nodes == "33-35") n_inv_nodes = 11;
		if(inv_nodes == "36-39") n_inv_nodes = 12;
		
		if(node_caps == "yes") n_node_caps = 1;
		if(node_caps == "no") n_node_caps = 0;
		
		if(breast == "left") n_breast = 0;
		if(breast == "right") n_breast = 1;
		
		if(breast_quad == "left-up") n_breast_quad = 0;
		if(breast_quad == "left-low") n_breast_quad = 1;
		if(breast_quad == "right-up") n_breast_quad = 2;
		if(breast_quad == "right-low") n_breast_quad = 3;
		if(breast_quad == "central") n_breast_quad = 4;
		
		if(irradiat == "yes") n_irradiat = 1;
		if(irradiat == "no") n_irradiat = 0;
			
		n_deg_malig = stod(deg_malig);
		
		// Put data element in vector
		std::vector<double> el = {n_age, n_meno, n_tumor_size, n_inv_nodes, n_node_caps, n_deg_malig, n_breast, n_breast_quad, n_irradiat};
		cls.push_back(n_recc);
		data.push_back(el);
	}
	
	std::cout << "File read." << std::endl;
	in.close();
}

void read_adult(std::vector<int> &cls, std::vector<std::vector<double>> &data) {
	std::string filename = "Datasets/Adult/adult_discretized.data";
	chdir("..");
	
	std::ifstream in(filename);
	
	if(!in.is_open()) {
		std::cerr << "Cannot open the File: " << filename << std::endl;
	}
	
	// Variables in the dataset
	std::string income, age, work_cls, fnlwgt, edu, edu_num, mar, occ, rel, race, sex, cap_gain, cap_loss, hpw, native;
	double n_age, n_work_cls, n_fnlwgt, n_edu, n_edu_num, n_mar, n_occ, n_rel, n_race, n_sex, n_cap_gain, n_cap_loss, n_hpw, n_native;
	int n_income;
	
	std::string line;
	
	while(getline(in, line)) {
		std::stringstream ss(line);
		
		// Read as strings
		getline(ss, income, ' ');
		getline(ss, age, ' ');
		getline(ss, work_cls, ' ');
		getline(ss, fnlwgt, ' ');
		getline(ss, edu, ' ');
		getline(ss, edu_num, ' ');
		getline(ss, mar, ' ');
		getline(ss, occ, ' ');
		getline(ss, rel, ' ');
		getline(ss, race, ' ');
		getline(ss, sex, ' ');
		getline(ss, cap_gain, ' ');
		getline(ss, cap_loss, ' ');
		getline(ss, hpw, ' ');
		getline(ss, native, ' ');
		
		if(income == "<=50k") n_income = 0;
		if(income == ">50k") n_income = 1;
		
		if(work_cls == "Private") n_work_cls = 0;
		if(work_cls == "Self-emp-not-inc") n_work_cls = 1;
		if(work_cls == "Self-emp-inc") n_work_cls = 2;
		if(work_cls == "Federal-gov") n_work_cls = 3;
		if(work_cls == "Local-gov") n_work_cls = 4;
		if(work_cls == "State-gov") n_work_cls = 5;
		if(work_cls == "Without-pay") n_work_cls = 6;
		if(work_cls == "Never-worked") n_work_cls = 7;
		
		if(edu == "Bachelors") n_edu = 0;
		if(edu == "Some-college") n_edu = 1;
		if(edu == "11th") n_edu = 2;
		if(edu == "HS-grad") n_edu = 3;
		if(edu == "Prof-school") n_edu = 4;
		if(edu == "Assoc-acdm") n_edu = 5;
		if(edu == "Assoc-voc") n_edu = 6;
		if(edu == "9th") n_edu = 7;
		if(edu == "7th-8th") n_edu = 8;
		if(edu == "12th") n_edu = 9;
		if(edu == "Masters") n_edu = 10;
		if(edu == "1st-4th") n_edu = 11;
		if(edu == "10th") n_edu = 12;
		if(edu == "Doctorate") n_edu = 13;
		if(edu == "5th-6th") n_edu = 14;
		if(edu == "Preschool") n_edu = 15;
		
		if(occ == "Tech-support") n_occ = 0;
		if(occ == "Craft-repair") n_occ = 1;
		if(occ == "Other-service") n_occ = 2;
		if(occ == "Sales") n_occ = 3;
		if(occ == "Exec-managerial") n_occ = 4;
		if(occ == "Prof-specialty") n_occ = 5;
		if(occ == "Handles-cleaners") n_occ = 6;
		if(occ == "Machine-op-inspct") n_occ = 7;
		if(occ == "Adm-clerical") n_occ = 8;
		if(occ == "Farming-fishing") n_occ = 9;
		if(occ == "Transport-moving") n_occ = 10;
		if(occ == "Priv-house-serve") n_occ = 11;
		if(occ == "Protective-serve") n_occ = 12;
		if(occ == "Armed-Forces") n_occ = 13;
		
		if(rel == "Wife") n_rel = 0;
		if(rel == "Own-child") n_rel = 1;
		if(rel == "Husband") n_rel = 2;
		if(rel == "Not-in-family") n_rel = 3;
		if(rel == "Other-relative") n_rel = 4;
		if(rel == "Unmarried") n_rel = 5;
		
		if(race == "White") n_race = 0;
		if(race == "Asian-Pac-Islander") n_race = 1;
		if(race == "Amer-Indian-Eskimo") n_race = 2;
		if(race == "Other") n_race = 3;
		if(race == "Black") n_race = 4;
		
		if(sex == "Female") n_sex = 0;
		if(sex == "Male") n_sex = 1;
		
		if(native == "United-States") n_native = 0;
		if(native == "Cambodia") n_native = 1;
		if(native == "England") n_native = 2;
		if(native == "Puerto-Rico") n_native = 3;
		if(native == "Canada") n_native = 4;
		if(native == "Germany") n_native = 5;
		if(native == "Outlying-US(Guam-USVI-etc)") n_native = 6;
		if(native == "India") n_native = 7;
		if(native == "Japan") n_native = 8;
		if(native == "Greece") n_native = 9;
		if(native == "South") n_native = 10;
		if(native == "China") n_native = 11;
		if(native == "Cuba") n_native = 12;
		if(native == "Iran") n_native = 13;
		if(native == "Honduras") n_native = 14;
		if(native == "Philippines") n_native = 15;
		if(native == "Italy") n_native = 16;
		if(native == "Poland") n_native = 17;
		if(native == "Jamaica") n_native = 18;
		if(native == "Vietnam") n_native = 19;
		if(native == "Mexico") n_native = 20;
		if(native == "Portugal") n_native = 21;
		if(native == "Ireland") n_native = 22;
		if(native == "France") n_native = 23;
		if(native == "Dominican-Republic") n_native = 24;
		if(native == "Laos") n_native = 25;
		if(native == "Ecuador") n_native = 26;
		if(native == "Taiwan") n_native = 27;
		if(native == "Haiti") n_native = 28;
		if(native == "Columbia") n_native = 29;
		if(native == "Hungary") n_native = 30;
		if(native == "Guatemala") n_native = 31;
		if(native == "Nicaragua") n_native = 32;
		if(native == "Scotland") n_native = 33;
		if(native == "Thailand") n_native = 34;
		if(native == "Yugoslavia") n_native = 35;
		if(native == "El-Salvador") n_native = 36;
		if(native == "Trinidad&Tobago") n_native = 37;
		if(native == "Peru") n_native = 38;
		if(native == "Hong") n_native = 39;
		if(native == "Holand-Netherlands") n_native = 40;
		
		n_age = stod(age);
		n_fnlwgt = stod(fnlwgt);
		n_edu_num = stod(edu_num);
		n_cap_gain = stod(cap_gain);
		n_cap_loss = stod(cap_loss);
		n_hpw = stod(hpw);
		
		// Put data element in vector
		std::vector<double> el = {n_age, n_work_cls, n_fnlwgt, n_edu, n_edu_num, n_mar, n_occ, n_rel, n_race, n_sex, n_cap_gain, n_cap_loss, n_hpw, n_native};
		cls.push_back(n_income);
		data.push_back(el);
	}
	
	std::cout << "File read." << std::endl;
	in.close();
}

void read_balance_scale(std::vector<int> &cls, std::vector<std::vector<double>> &data) {
	std::string filename = "Datasets/balance_scale/balance-scale.data";
	chdir("..");
	
	std::ifstream in(filename);
	
	if(!in.is_open()) {
		std::cerr << "Cannot open the File: " << filename << std::endl;
	}
	
	// Variables in the dataset
	std::string lr, l_w, l_d, r_w, r_d;
	double n_l_w, n_l_d, n_r_w, n_r_d;
	int n_lr;
	
	std::string line;
	
	while(getline(in, line)) {
		std::stringstream ss(line);
		
		// Read as strings
		getline(ss, lr, ' ');
		getline(ss, l_w, ' ');
		getline(ss, l_d, ' ');
		getline(ss, r_w, ' ');
		getline(ss, r_d, ' ');
		
		if (lr == "L") n_lr = 0;
		if (lr == "R") n_lr = 1;
		
		n_l_w = stod(l_w);
		n_l_d = stod(l_d);
		n_r_w = stod(r_w);
		n_r_d = stod(r_d);
		
		// Put data element in vector
		std::vector<double> el = {n_l_w, n_l_d, n_r_w, n_r_d};
		cls.push_back(n_lr);
		data.push_back(el);
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

// Test computation of the kernel function
int32_t test_svm_computation(e_role role, const std::string& address, uint16_t port, seclvl seclvl, uint32_t nvals, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, std::vector<int> cls, std::vector<std::vector<double>> data) {
	
	std::vector<uint32_t> to_fit;
	
	std::vector<uint32_t> x1;
	uint32_t output = 0;
	
	uint32_t data_vals = data.size();
	
	std::cout << "Data contains " << data_vals << " rows and " << nvals << " attributes." << std::endl;
	
	std::cout << "Splitting data..." << std::endl;
	
	for (int i=0; i < data_vals; i++) {
		int arr[data[i].size()];
		std::copy(data[i].begin(), data[i].end(), arr);
		
		// Split data
		for (int j=0; j < nvals; j++) {
			if (arr[j] > 1) {
				arr[j] = arr[j]/2;
			}
		}
		
		// Convert to vector
		std::vector<int> split_vec;
		for (int k : arr) {
			split_vec.push_back(k);
		}
		
		std::vector<uint32_t> split_p1(split_vec.begin(), split_vec.end());
		std::vector<uint32_t> split_p2(split_vec.begin(), split_vec.end());
		
		// ABY
		ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg);
	
		std::vector<Sharing*>& sharings = party->GetSharings();
	
		ArithmeticCircuit* circ = 
		(ArithmeticCircuit*) 
		sharings[sharing]->GetCircuitBuildRoutine();
		
		share *s1, *s2, *s_out;
		
		s1 = circ -> PutSIMDINGate(nvals, split_p1.data(), 16, SERVER);
		s2 = circ -> PutSIMDINGate(nvals, split_p2.data(), 16, CLIENT);
		
		s1 = circ -> PutADDGate(s1, s2);
		
		s_out = build_kernel_circuit(s1, s1, nvals, 16, circ);
		
		s_out = circ -> PutOUTGate(s_out, ALL);
		
		party->ExecCircuit();
	
		uint32_t out_bitlen, out_nvals, *out_vals;
	
		s_out->get_clear_value_vec(&out_vals, &out_bitlen, &out_nvals);
		std::cout << "Circuit result: " << *out_vals << std::endl;
		to_fit.push_back(*out_vals);
		
		delete party;
	}
	
	// Copy attributes to array
//	int attr_arr[data_vals];
//	std::copy(to_fit.begin(), to_fit.end(), attr_arr);
	
	// Copy class labels to array
//	int cls_arr[data_vals];
//	std::copy(cls.begin(), cls.end(), cls_arr);
	
	std::vector<double> attr(to_fit.begin(), to_fit.end());
	basic_Support_Vector_Machine svm;
	svm.fit(attr, cls);
	
	return 0;
}
