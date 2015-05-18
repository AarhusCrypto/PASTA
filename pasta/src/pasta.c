/*
 * The PASTA protocol
 *
 * Work by Carsten Baum, Ivan Damgaard, Tomas Toft and Rasmus Zakarias
 */

#include <osal.h>
#include <utils/options.h>
#include <Ring.h>
#include <rnd.h>
#include <encoding/hex.h>

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

static void random_base16_string(OE oe,Rnd rnd, byte * b, uint lb) {
	byte * buffer = oe->getmem(lb);
	uint i = 0;
	//char cs[] = {"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
	char cs[] = {"0123456789abcdef"};
	rnd->rand(buffer, lb);

	for (i = 0; i < lb; ++i) {
		b[i] = cs[buffer[i] % 16];
	}
	b[lb - 1] = 0;
}

static void bytes_as_base16(byte * in, uint lin, byte * out) {
	bs2hs(in, out, lin);
}

static void base16_as_bytes(byte * in, uint lin, byte * out) {
	hs2bs(in, out, lin);
}

static void run_pasta(OE oe, Map args) {
	Ring integers = TheIntegers_New(oe);
	Ring zp = 0;
	Ring zphi = 0;
	Rnd random = LibcWeakRandomSource_New(oe);
	RE p = 0, q = 0;
	RE a = 0, b = 0, c = 0;
	RE N = 0;
	RE pmin1 = 0, qmin1 = 0;
	RE phi = 0;
	
	RC rc = RC_OK;
	byte rand_buf[257] = { 0 }; // 1024 bits
	RE e = 0, d = 0;

	// ------
	// Example doing plain RSA
	// ------

	// generate pseudo random  p
	random_base16_string(oe, random, rand_buf, sizeof(rand_buf)-1);
	p = TheIntegers_NextPrime(integers, integers->from_cstr(rand_buf, &rc));

	// generate pseudo random  q
	random_base16_string(oe, random, rand_buf, sizeof(rand_buf)-1);
	q = TheIntegers_NextPrime(integers, integers->from_cstr(rand_buf, &rc));

	// N = p * q
	N = integers->zero(0);
	N->add(p);
	N->mul(q);
	
	// compute phi(N)
	pmin1 = p->copy(0);
	pmin1->sub(integers->one(0));
	qmin1 = q->copy(0);
	qmin1->sub(integers->one(0));
	phi = pmin1->copy(0);
	phi->mul(qmin1);

	// compute private exponent from public exponent 65537
	zphi = Zp_New(oe, phi);
	e = zphi->from_cstr("65537", 0);
	d = e->copy(0);
	d->inv(0);

	// compute ciphertext - Plain RSA encryption
	zp = Zp_New(oe, N);
	{
		RE plaintext = 0;
		RE message = 0;
		RE ciphertext = 0;
		byte msg[] = {"deadbeefdeadbeef\0"};
		byte plx[17] = { 0 };
		message = zp->from_cstr(msg, 0);
		ciphertext = message->copy(0);
		ciphertext->pow(e);
		plaintext = ciphertext->copy(0);
		plaintext->pow(d);
		if (zp->to_cstr(plaintext, plx, 17) == RC_OK) {
			oe->print("Decryption successful.");
		}
		else {
			oe->print("Decryption failed");
		};
	}

	// clean up - all elements from a Ring are cleaned too
	// when the Ring is destroyed.
	Zp_Destroy(&zp);
	Zp_Destroy(&zphi);
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