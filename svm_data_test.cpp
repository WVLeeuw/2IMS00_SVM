// Author: Wouter Leeuw
// Based on the ABY testing framework
// Test file for SVM using real datasets

#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>

#include "kernel.cpp"

int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role, uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address, uint16_t* port, int32_t* test_op, std::string* file_name, std::string* cls_file, std::string* attr_file) {

	uint32_t int_role = 0, int_port = 0;
	
	parsing_ctx options[] ={ 
	{ (void*) &int_role, T_NUM, "r", "Role: 0/1", true, false},
	{ (void*) nvals, T_NUM, "n", "Number of elements for kernel computation, default: 128", false, false},
	{ (void*) bitlen, T_NUM, "b", "Bit-length, default 16", false, false},
	{ (void*) secparam, T_NUM, "s", "Symmetric Security Bits, default: 128", false, false},
	{ (void*) address, T_STR, "a", "IP-address, default: localhost", false, false},
	{ (void*) &int_port, T_NUM, "p", "Port, default: 7766", false, false},
	{ (void*) test_op, T_NUM, "t", "Single test, default:off", false, false},
	{ (void*) test_op, T_NUM, "f", "Training data file, default: None", false, false},
	{ (void*) test_op, T_NUM, "c", "Class data file, default: None", false, false},
	{ (void*) test_op, T_NUM, "a", "Attribute data file, default: None", false, false}
	};
	
	if (!parse_options(argcp, argvp, options, sizeof(options) / sizeof(parsing_ctx))) {
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
		std::cout << "Exiting" << std::endl;
		exit(0);
	}
	
	assert(int_role < 2);
	*role = (e_role) int_role;
	
	if (int_port != 0) {
		assert(int_port < 1 << (sizeof(uint16_t) * 8));
		*port = (uint16_t) int_port;
	}
	
	return 1;		
}

int main(int argc, char** argv) {
	e_role role;
	uint32_t bitlen = 16, secparam = 128, nthreads = 1;
	uint16_t port = 7766;
	std::string address = "127.0.0.1";
	int32_t test_op = -1;
	e_mt_gen_alg mt_alg = MT_OT;
	std::string file_name = "";
	std::string cls_file = "";
	std::string attr_file = "";
	
	// Test file reading
	std::vector<int> cls;	
	std::vector<std::vector<double>> data;
//	read_absenteeism(cls, data);
//	read_breast_cancer(cls, data);
	read_adult(cls, data);
//	read_balance_scale(cls, data);
	
	uint32_t nvals = static_cast<int>(data[0].size());
	
	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address, &port, &test_op, &file_name, &cls_file, &attr_file);
	
	seclvl seclvl = get_sec_lvl(secparam);

	// Compute kernels with ABY and train SVM
	test_svm_computation(role, address, port, seclvl, nvals, bitlen, nthreads, mt_alg, S_ARITH, cls, data);
	
	return 0;
}
