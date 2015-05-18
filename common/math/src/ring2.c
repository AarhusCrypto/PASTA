#include <ring.h>
#include <coov4.h>
#include <mini-gmp.h>
#include <intkeymap.h>
#include <singlelinkedlist.h>

List SingleLinkedList_new(OE oe);
typedef struct _the_ints_ {
	OE oe;
	List allocated_items; // List<RE>
} *Ints;

typedef struct _the_ints_elm_ {
	Ints origin;
	mpz_t * gmpelm;
} *TheIntsElm;

COO_DEF(RE, RC, ints_add, RE other) {
	TheIntsElm impl = (TheIntsElm)this->impl;
	TheIntsElm othr = other->impl;

	mpz_t * res = impl->origin->oe->getmem(sizeof(*res));
	if (!res) return RC_NOMEM;

	mpz_add(*res, *(impl->gmpelm), *(othr->gmpelm));
	impl->origin->oe->putmem(impl->gmpelm);
	impl->gmpelm = res;

	return RC_OK;
}}

COO_DEF(RE, RC, ints_mul, RE other) {
	TheIntsElm impl = (TheIntsElm)this->impl;
	TheIntsElm othr = other->impl;

	mpz_t * res = impl->origin->oe->getmem(sizeof(*res));
	if (!res) return RC_NOMEM;

	mpz_mul(*res, *(impl->gmpelm), *(othr->gmpelm));
	impl->origin->oe->putmem(impl->gmpelm);
	impl->gmpelm = res;

	return RC_OK;

}}

COO_DEF(RE, RC, ints_pow, RE other) {
	TheIntsElm impl = (TheIntsElm)this->impl;
	TheIntsElm othr = other->impl;

	mpz_t * res = impl->origin->oe->getmem(sizeof(*res));
	if (!res) return RC_NOMEM;



	mpz_pow_ui(*res, *impl->gmpelm, mpz_get_ui(*othr->gmpelm));
	impl->origin->oe->putmem(impl->gmpelm);
	impl->gmpelm = res;

	return RC_OK;

}}

COO_DEF(RE, RC, ints_powi, ull i) {
	TheIntsElm impl = (TheIntsElm)this->impl;

	mpz_t * res = impl->origin->oe->getmem(sizeof(*res));
	if (!res) return RC_NOMEM;

	if (i > 0xFFFFFFFF) return RC_BAD_ARGS;

	mpz_pow_ui(*res, *impl->gmpelm, i);
	impl->origin->oe->putmem(impl->gmpelm);
	impl->gmpelm = res;

	return RC_OK;


}}

static void set_intselm_functions(RE re);


static void TheIntegers_Elm_Destroy(RE * re) {
	TheIntsElm elm = 0;
	RE r = 0;
	OE oe = 0;

	if (!re) return;

	r = *re;
	if (!r) return;

	elm = (TheIntsElm)r->impl;
	if (!elm) return;

	oe = elm->origin->oe;

	COO_detach(r->add);
	COO_detach(r->copy);
	COO_detach(r->mul);
	COO_detach(r->pow);
	COO_detach(r->powi);
	oe->putmem(elm->gmpelm);
	oe->putmem(elm);
	oe->putmem(r);
	*re = 0;
}

static RE TheIntegers_Elm_New(Ints impl) {
	RE re = impl->oe->getmem(sizeof(*re));
	TheIntsElm elm = impl->oe->getmem(sizeof(*elm));
	mpz_t gmpint = { 0 };

	re->impl = elm;
	elm->origin = impl;
	mpz_init_set_ui(gmpint, 0);

	set_intselm_functions(re);
	impl->allocated_items->add_element(re);

	return re;
}


COO_DEF(RE, RE, ints_copy, RC * rc) {
	TheIntsElm impl = (TheIntsElm)this->impl;
	TheIntsElm newe = 0;
	RE result = TheIntegers_Elm_New(impl->origin);

	newe = (TheIntsElm)result->impl;

	mpz_mul_ui(*newe->gmpelm, *impl->gmpelm, 1);
	return result;
}}

static void set_intselm_functions(RE re) {
	re->add = COO_attach(re, RE_ints_add);
	re->copy = COO_attach(re, RE_ints_copy);
	re->mul = COO_attach(re, RE_ints_mul);
	re->pow = COO_attach(re, RE_ints_pow);
	re->powi = COO_attach(re, RE_ints_powi);
}

COO_DEF(Ring, RE, ints_from_cstr, const char * cstr) {
	Ints impl = (Ints)this->impl;
	TheIntsElm relm = 0;

	RE result = TheIntegers_Elm_New(impl);
	if (!result) return 0;

	relm = (TheIntsElm)result->impl;
	mpz_set_str(*relm->gmpelm, cstr, 10);

	return result;
}}

COO_DEF(Ring, RE, ints_from_ull, ull v) {
	Ints impl = (Ints)this->impl;
	TheIntsElm relm = 0;

	RE result = TheIntegers_Elm_New(impl);
	if (!result) return 0;

	relm = (TheIntsElm)result->impl;
	mpz_set_ui(*relm->gmpelm, v);
	return result;
}}

COO_DEF(Ring, RE, ints_one) {
	Ints impl = (Ints)this->impl;
	TheIntsElm relm = 0;

	RE result = TheIntegers_Elm_New(impl);
	if (!result) return 0;

	relm = (TheIntsElm)result->impl;
	mpz_set_ui(*relm->gmpelm, 1);
	return result;
}}

COO_DEF(Ring, RE, ints_zero) {
	Ints impl = (Ints)this->impl;
	TheIntsElm relm = 0;

	RE result = TheIntegers_Elm_New(impl);
	if (!result) return 0;

	relm = (TheIntsElm)result->impl;
	mpz_set_ui(*relm->gmpelm, 0);
	return result;
}}

Ring TheIntegers_New(OE oe) {
	Ring ints = (Ring)oe->getmem(sizeof(*ints));
	Ints impl = (Ints)oe->getmem(sizeof(*impl));

	if (!ints) goto failure;
	if (!impl) goto failure;

	ints->from_cstr = COO_attach(ints, Ring_ints_from_cstr);
	ints->from_ull = COO_attach(ints, Ring_ints_from_ull);
	ints->one = COO_attach(ints, Ring_ints_one);
	ints->zero = COO_attach(ints, Ring_ints_zero);
	ints->impl = impl;
	impl->oe = oe;
	impl->allocated_items = SingleLinkedList_new(oe);
	return ints;
failure:
	TheIntegers_Destroy(&ints);
	return 0;
}

void TheIntegers_Destroy(Ring * ring) {
	Ring r = 0;
	Ints impl = 0;
	OE oe = 0;
	uint i = 0;

	if (!ring) return;

	r = *ring;
	if (!r) return;

	impl = (Ints)r->impl;
	if (!impl) return;

	for (i = 0; i < impl->allocated_items->size(); ++i) {
		RE cur = (RE)impl->allocated_items->get_element(i);
		TheIntegers_Elm_Destroy(&cur);
	}

	for (i = 0; i < impl->allocated_items->size(); ++i) {
		impl->allocated_items->rem_element(0);
	}

	COO_detach(r->from_cstr);
	COO_detach(r->from_ull);
	COO_detach(r->one);
	COO_detach(r->zero);
	SingleLinkedList_destroy(&impl->allocated_items);
	oe->putmem(impl);
}

Ring Zp_New(OE oe, RE p) {
	Ring res = (Ring)oe->getmem(sizeof(*res));
	return res;
}