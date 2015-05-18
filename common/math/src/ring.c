#include <ring.h>
#include <coov4.h>
#include <mini-gmp.h>
#include <intkeymap.h>
#include <singlelinkedlist.h>

List SingleLinkedList_new(OE oe);


// ---------------------------------------------------
// The Integers implementation based on GMP
// ---------------------------------------------------
typedef struct _the_ints_ {
	OE oe;
	List allocated_items; // List<RE>
} * Ints;

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
	mpz_set_str(*relm->gmpelm, cstr,10);

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

// ---------------------------------------------------
// Zp implementation
// ---------------------------------------------------
static RE ZpE_New(Ring origin);
typedef struct _zp_ {
	OE oe;
	List allocated_elms; // List<RE<ZpE>>
	mpz_t * mod;
} *Zp;

static void ZpE_Destroy(RE * elm);
void Zp_Destroy(Ring * ring) {
	Ring r = 0;
	uint i = 0;
	Zp zp = 0;

	if (!ring) return;

	r = *ring;
	if (!r) return;

	zp = r->impl;
	if (!zp) return;

	COO_detach(r->from_cstr);
	COO_detach(r->from_ull);
	COO_detach(r->one);
	COO_detach(r->zero);

	for (i = 0; i < zp->allocated_elms->size(); ++i) {
		RE c = zp->allocated_elms->get_element(i);
		ZpE_Destroy(&c);
	}
	SingleLinkedList_destroy(&zp->allocated_elms);
	mpz_clear(*zp->mod);
	zp->oe->putmem(zp->mod);
	zp->oe->putmem(zp);
}

typedef struct _zp_elm_ {
	Ring origin;
	mpz_t * elm;
} *ZpE;

COO_DEF(RE, RC, zp_add, RE other) {
	ZpE impl = this->impl;
	Ring org = impl->origin;
	Zp zp = (Zp)org->impl;
	OE oe = zp->oe;
	ZpE ozpe = other->impl;
	mpz_t * res = oe->getmem(sizeof(*res));

	mpz_add(*res, *impl->elm, *ozpe->elm);
	oe->putmem(impl->elm);
	impl->elm = res;

	return RC_OK;
}}

COO_DEF(RE, RE, zp_cpy) {
	ZpE impl = this->impl;
	Ring org = impl->origin;
	Zp zp = (Zp)org->impl;
	OE oe = zp->oe;
	RE result = ZpE_New(org);
	ZpE rzp = result->impl;

	mpz_add_ui(*rzp->elm, *impl->elm, 0);

	return result;
}}

COO_DEF(RE, RC, zp_mul, RE other) {
	ZpE impl = this->impl;
	Ring org = impl->origin;
	Zp zp = (Zp)org->impl;
	OE oe = zp->oe;
	ZpE ozpe = other->impl;
	mpz_t * res = oe->getmem(sizeof(*res));

	mpz_mul(*res, *impl->elm, *ozpe->elm);
	oe->putmem(impl->elm);
	impl->elm = res;

	return RC_OK;

}}

COO_DEF(RE, RC, zp_pow, RE other) {
	ZpE impl = this->impl;
	Ring org = impl->origin;
	Zp zp = (Zp)org->impl;
	OE oe = zp->oe;
	ZpE ozpe = other->impl;
	mpz_t * res = oe->getmem(sizeof(*res));

	mpz_pow_ui(*res, *impl->elm, mpz_get_ui(*ozpe->elm));
	oe->putmem(impl->elm);
	impl->elm = res;

	return RC_OK;

}}

COO_DEF(RE, RC, zp_powi,ull i) {
	ZpE impl = this->impl;
	Ring org = impl->origin;
	Zp zp = (Zp)org->impl;
	OE oe = zp->oe;
	mpz_t * res = oe->getmem(sizeof(*res));

	mpz_pow_ui(*res, *impl->elm, i);
	oe->putmem(impl->elm);
	impl->elm = res;

	return RC_OK;

}}

static void set_zpelm_functions(RE re) {
	re->add = COO_attach(re, RE_zp_add);
	re->copy = COO_attach(re, RE_zp_cpy);
	re->mul = COO_attach(re, RE_zp_mul);
	re->pow = COO_attach(re, RE_zp_pow);
	re->powi = COO_attach(re, RE_zp_powi);
}

static void ZpE_Destroy(RE * elm) {
	RE r = 0;
	OE oe = 0;
	ZpE e = 0;
	Zp zp = 0;

	if (!elm) return;

	r = *elm;
	if (!r) return;

	e = r->impl;
	if (!e) return;

	zp = e->origin->impl;
	oe = zp->oe;

	COO_detach(r->add);
	COO_detach(r->copy);
	COO_detach(r->mul);
	COO_detach(r->pow);
	COO_detach(r->powi);

	mpz_clear(*e->elm);
	oe->putmem(e->elm);
	oe->putmem(e);
	oe->putmem(r);
}

static RE ZpE_New(Ring origin) {
	Zp zp = (Zp)origin->impl;
	OE oe = zp->oe;
	RE result = 0;
	ZpE elm = 0; 

	result = oe->getmem(sizeof(*result));
	if (!result) goto failure;

	elm = (ZpE)oe->getmem(sizeof(*elm));
	if (!elm) goto failure;
	
	result->impl = elm;
	set_zpelm_functions(result);
	zp->allocated_elms->add_element(result);

	return result;
failure:
	ZpE_Destroy(&result);
	return 0;
}

COO_DEF(Ring, RE, zp_from_cstr, const char * cstr) {
	Zp impl = (Zp)this->impl;
	RE result = ZpE_New(this);
	ZpE rzp = result->impl;

	mpz_set_str(*rzp->elm, cstr,10);
	return result;
}}

COO_DEF(Ring, RE, zp_from_ull, ull v) {
	Zp impl = (Zp)this->impl;
	RE result = ZpE_New(this);
	ZpE rzp = result->impl;

	mpz_set_ui(*rzp->elm, v);
	return result;
}}

COO_DEF(Ring, RE, zp_one) {
	Zp impl = (Zp)this->impl;
	RE result = ZpE_New(this);
	ZpE rzp = result->impl;

	mpz_set_ui(*rzp->elm, 1);
	return result;


}}

COO_DEF(Ring, RE, zp_zero) {
	Zp impl = (Zp)this->impl;
	RE result = ZpE_New(this);
	ZpE rzp = result->impl;

	mpz_set_ui(*rzp->elm, 0);
	return result;

}}

Ring Zp_New(OE oe, RE p) {
	Ring res = (Ring)oe->getmem(sizeof(*res));
	Zp zp = (Zp)oe->getmem(sizeof(*zp));
	TheIntsElm pi = (TheIntsElm)p->impl;

	res->from_cstr = COO_attach(res, Ring_zp_from_cstr);
	res->from_ull = COO_attach(res, Ring_zp_from_ull);
	res->one = COO_attach(res, Ring_zp_one);
	res->zero = COO_attach(res, Ring_zp_zero);
	res->impl = zp;
	zp->oe = oe;
	zp->allocated_elms = SingleLinkedList_new(oe);
	zp->mod = oe->getmem(sizeof(*zp->mod));
	mpz_mul_ui(*zp->mod, *pi->gmpelm, 1);
	return res;
}

