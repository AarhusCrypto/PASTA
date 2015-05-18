/*

Copyright (c) 2013, Rasmus Zakarias, Aarhus University
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software

must display the following acknowledgement:

This product includes software developed by Rasmus Winther Zakarias
at Aarhus University.

4. Neither the name of Aarhus University nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Rasmus Zakarias at Aarhus University
''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Rasmus Zakarias at Aarhus University BE
LIABLE FOR ANY, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Created: 2014-10-26

Author: Rasmus Winther Zakarias, rwl@cs.au.dk

Changes:
2014-10-26 14:27: Initial version created
*/


/*
 * Ring.h
 *
 * For large fields the overhead of calling through an interface 
 * is proportionally small enough that it doesn't matter.
 *
 * For small fields we want the metal right under our fingers and using 
 * this interface is not recommended.
 *
 *
 */
#ifndef RING_H
#define RING_H
#include <osal.h>

typedef struct _ring_element_ {

	/*!
	 * Modify this instance to be the mul-inverse of it self.
	 *
	 * \return RC_OK on success.
	 */
	RC(*inv)(void);

	/*!
	* Modify this instance to be the quotient with {other}.
	*
	* \param other - The other element to add
	*
	* \return RC_OK on success. Otherwise this element has not
	*         changed due to some error.
	*/
	RC(*div)(struct _ring_element_ * other);

	/*!
	* Modify this instance to be the difference with {other}.
	*
	* \param other - The other element to add
	*
	* \return RC_OK on success. Otherwise this element has not
	*         changed due to some error.
	*/
	RC(*sub)(struct _ring_elmement_ * other);

	/*!
	 * Modify this instance to be the sum with {other}.
	 *
	 * \param other - The other element to add 
	 *
	 * \return RC_OK on success. Otherwise this element has not
	 *         changed due to some error.
	 */
	RC (*add)(struct _ring_element_ * other);

	/*!
	 * Modify this instance to be the product with {other}.
	 *
	 * \param other - The other element to multiply
	 *
	 * \return RC_OK on success. Otherwise this element has not
	 *         changed due to some error.
	 */
	RC (*mul)(struct _ring_element_ * other);

	/*!
	 * Suppose {other} has an interpretation as an integer. 
	 * Modify this instance to be the element obtained by 
	 * multiplying this instance with it self {other}-times.
	 *
	 * \param other - The other element to exponentiate this 
	 *                element to.
	 *
	 * \return RC_OK on success. Otherwise this element has not 
	 *         changed due to some error .
	 */
	RC (*pow)(struct _ring_element_ * other);

	/*!
	 * Modify this instance to be the {i}th power.
	 *
	 * \param i - integer times to mul. this instance with itself
	 *
	 * \return RC_OK on success. Otherwise this element has not
	 *         changed due to some error .
	 */
	RC(*powi)(ull i);

	/*!
	 * Make a copy of this element.
	 *
	 * \param rc - optionally a return code can be obtained
	 *
	 * \return element to fresh pointer on success, NULL otherwise.
	 */
	struct _ring_element_ * (*copy)(RC * rc);
	void * impl;
} * RE;

typedef struct _ring_ {
	// rc optional
	RE(*zero)(RC * rc);
	// rc optional
	RE(*one)(RC * rc);
	// rc optional
	RE(*from_ull)(ull v, RC * rc);
	// rc optional, {str} is in base16
	RE(*from_cstr)(const char * str, RC * rc);
	void * impl;
} * Ring;

/*!
 * Create a Ring that approximates the Integers limitted by the 
 * memory of our machine.
 */
Ring TheIntegers_New(OE oe);
void TheIntegers_Destroy(Ring * r);

// --- Number theoretical support for TheIntegers

/*!
 * Return the next prime greater than {re} allocated
 * and managed in the context of {theintegers}.
 *
 * \param theintegers - context to create prime in
 * \param re          - offset for finde next prime
 *
 * \return a highly proble prime (accoding to gmp doc).
 *
 * see: https://gmplib.org/manual/Number-Theoretic-Functions.html
 */
RE TheIntegers_NextPrime(Ring theintegers, RE re);

/*!
 * Create a Ring where computation is done modulo p for 
 * some number p.
 *
 */
Ring Zp_New(OE oe, RE p);
void Zp_Destroy(Ring * r);

#endif