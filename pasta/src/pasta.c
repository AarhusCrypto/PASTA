/*
 * The PASTA protocol
 *
 * Work by Carsten Baum, Ivan Damgaard, Tomas Toft and Rasmus Zakarias
 */

#include <osal.h>
#include <utils/options.h>
#include <Ring.h>
#include <rnd.h>

static bool check_args(OE oe, Map args) {

	if (args->size() != 0) {
		uint i = 0;
		for (i = 0; i < args->size(); ++i) {
			List keys = args->get_keys(i);
			oe->p("Unexpected arg: \"%s\"",(char*)keys->get_element(i));
		}
		return False;
	}

	return True;
}

static void random_base10_string(OE oe,Rnd rnd, byte * b, uint lb) {
	byte * buffer = oe->getmem(lb);
	uint i = 0;
	//char cs[] = {"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
	char cs[] = {"0123456789"};
	rnd->rand(buffer, lb);

	// bias to 0=250,1=251,...,5=255
	for (i = 0; i < lb; ++i) {
		b[i] = cs[buffer[i] % 10];
	}
	b[lb - 1] = 0;
}

static void run_pasta(OE oe, Map args) {
	Ring integers = TheIntegers_New(oe);
	Ring zp = 0;
	Rnd random = LibcWeakRandomSource_New(oe);
	RE p = 0, q = 0;
	RE a = 0, b = 0, c = 0;
	RE N = 0;
	RE pmin1 = 0, qmin1 = 0;
	RE phi = 0;
	
	RC rc = RC_OK;
	byte rand_buf[257] = { 0 }; // log_2(10) > 3 thus we have > 128*3 bits 

	random_base10_string(oe, random, rand_buf, sizeof(rand_buf)-1);
	p = TheIntegers_NextPrime(integers, integers->from_cstr(rand_buf, &rc));

	random_base10_string(oe, random, rand_buf, sizeof(rand_buf)-1);
	q = TheIntegers_NextPrime(integers, integers->from_cstr(rand_buf, &rc));

	N = integers->one(0);
	N->mul(p);
	N->mul(q);
	
	pmin1 = p->copy(0);
	pmin1->sub(integers->one(0));
	qmin1 = q->copy(0);
	qmin1->sub(integers->one(0));


	zp = Zp_New(oe, N);
	

	Zp_Destroy(&zp);
	TheIntegers_Destroy(&integers);
}

int main(int c, char ** a) {

	OE oe = OperatingEnvironment_New();
	Map args = Options_New(oe, c, a);

	oe->print("The PASTA protocol Aarhus University all rights reserve.\n");
	
	if (!check_args(oe,args)) return -1;

	run_pasta(oe,args);

	OperatingEnvironment_Destroy(&oe);
	return 0;
}