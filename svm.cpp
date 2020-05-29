#include<iostream>
#include<vector>

#include "ABY/src/abycore/circuit/circuit.h"
#include "ABY/src/abycore/circuit/arithmeticcircuits.h"
#include "ABY/src/abycore/aby/abyparty.h"
#include "ABY/src/abycore/sharing/sharing.h"
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>

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

int32_t test_kernel_computation(e_role role, const std::string& address, uint16_t port, seclvl seclvl, uint32_t nvals, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing) {
	
	/* Step 1: Create ABYParty object */
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg);
	
	/* Step 2: Create sharings */
	std::vector<Sharing*>& sharings = party->GetSharings();
	
	/* Step 3: Create circuit */
	ArithmeticCircuit* circ = 
		(ArithmeticCircuit*) 
		sharings[sharing]->GetCircuitBuildRoutine();
	
	/* Step 4: Create share objects for SVM (or kernel) */
	share *s_x1, *s_x2, *s_y, *s_out;
	
	/* Step 5: Fill the property vectors */
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
	
	/* Step 6: To Do: Copy x1 and x2 into respective share object 	using PutINGate() and PutDummyINGate() for other parties input. */
	s_x1 = circ->PutSIMDINGate(nvals, x1_vals.data(), 16, SERVER);
	s_x2 = circ->PutSIMDINGate(nvals, x2_vals.data(), 16, CLIENT);
	
	/* Step 7: To Do: Call the build method for building the circuit */
	s_out = build_kernel_circuit(s_x1, s_x2, nvals, 32, (ArithmeticCircuit*) circ);
	
	/* Step 8: To Do: PutOUTGate based on role of server and client. */
	s_out = circ->PutOUTGate(s_out, ALL);
	
	/* Step 9: To Do: Execute the circuit using the ABYParty object. */
	party->ExecCircuit();
	
	/* Step 10: To Do: Type cast the value for output */
	output = s_out->get_clear_value<uint16_t>();
	
	std::cout << "\nCircuit Result: " << output;
	std::cout << "\nVerification Result: " << v_sum << std::endl;
	
	delete s_x1;
	delete s_x2;
	// delete y;
	delete party;
	
	return 0;
}
