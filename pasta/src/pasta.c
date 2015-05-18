/*
 * The PASTA protocol
 *
 * Work by Carsten Baum, Ivan Damgaard, Tomas Toft and Rasmus Zakarias
 */

#include <osal.h>
#include <utils/options.h>
#include <Ring.h>

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

static void run_pasta(OE oe, Map args) {
	Ring integers = TheIntegers_New(oe);
	Ring zp = 0;
	RE p = 0;
	RC rc = RC_OK;

	p = integers->from_cstr("123123123", &rc);

	zp = Zp_New(oe, p);
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