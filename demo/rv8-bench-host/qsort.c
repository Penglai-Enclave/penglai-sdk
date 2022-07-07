/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef int		 cmp_t(const void *, const void *);

static inline char	*med3(char *, char *, char *, cmp_t *);
static inline void	 swapfunc(char *, char *, int, int, int);

#define	MIN(a, b)	((a) < (b) ? a : b)

/*
 * Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
 */
#define	swapcode(TYPE, parmi, parmj, n) {		\
	long i = (n) / sizeof (TYPE);			\
	TYPE *pi = (TYPE *) (parmi);		\
	TYPE *pj = (TYPE *) (parmj);		\
	do { 						\
		TYPE	t = *pi;		\
		*pi++ = *pj;				\
		*pj++ = t;				\
	} while (--i > 0);				\
}

#define	SWAPINIT(TYPE, a, es) swaptype_ ## TYPE =	\
	((char *)a - (char *)0) % sizeof(TYPE) ||	\
	es % sizeof(TYPE) ? 2 : es == sizeof(TYPE) ? 0 : 1;

static inline void
swapfunc( char *a, char *b, int n, int swaptype_long, int swaptype_int)
{
	if (swaptype_long <= 1)
		swapcode(long, a, b, n)
	else if (swaptype_int <= 1)
		swapcode(int, a, b, n)
	else
		swapcode(char, a, b, n)
}

#define	swap(a, b)					\
	if (swaptype_long == 0) {			\
		long t = *(long *)(a);			\
		*(long *)(a) = *(long *)(b);		\
		*(long *)(b) = t;			\
	} else if (swaptype_int == 0) {			\
		int t = *(int *)(a);			\
		*(int *)(a) = *(int *)(b);		\
		*(int *)(b) = t;			\
	} else						\
		swapfunc(a, b, es, swaptype_long, swaptype_int)

#define	vecswap(a, b, n)				\
	if ((n) > 0) swapfunc(a, b, n, swaptype_long, swaptype_int)

static inline char *
med3(char *a, char *b, char *c, cmp_t *cmp)
{
	return cmp(a, b) < 0 ?
	       (cmp(b, c) < 0 ? b : (cmp(a, c) < 0 ? c : a ))
	      :(cmp(b, c) > 0 ? b : (cmp(a, c) < 0 ? a : c ));
}

void
qsort(void *a, size_t n, size_t es, cmp_t *cmp)
{
	char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
	size_t d, r;
	int cmp_result;
	int swaptype_long, swaptype_int, swap_cnt;

loop:	SWAPINIT(long, a, es);
	SWAPINIT(int, a, es);
	swap_cnt = 0;
	if (n < 7) {
		for (pm = (char *)a + es; pm < (char *)a + n * es; pm += es)
			for (pl = pm;
			     pl > (char *)a && cmp(pl - es, pl) > 0;
			     pl -= es)
				swap(pl, pl - es);
		return;
	}
	pm = (char *)a + (n / 2) * es;
	if (n > 7) {
		pl = a;
		pn = (char *)a + (n - 1) * es;
		if (n > 40) {
			d = (n / 8) * es;
			pl = med3(pl, pl + d, pl + 2 * d, cmp);
			pm = med3(pm - d, pm, pm + d, cmp);
			pn = med3(pn - 2 * d, pn - d, pn, cmp);
		}
		pm = med3(pl, pm, pn, cmp);
	}
	swap(a, pm);
	pa = pb = (char *)a + es;

	pc = pd = (char *)a + (n - 1) * es;
	for (;;) {
		while (pb <= pc && (cmp_result = cmp(pb, a)) <= 0) {
			if (cmp_result == 0) {
				swap_cnt = 1;
				swap(pa, pb);
				pa += es;
			}
			pb += es;
		}
		while (pb <= pc && (cmp_result = cmp(pc, a)) >= 0) {
			if (cmp_result == 0) {
				swap_cnt = 1;
				swap(pc, pd);
				pd -= es;
			}
			pc -= es;
		}
		if (pb > pc)
			break;
		swap(pb, pc);
		swap_cnt = 1;
		pb += es;
		pc -= es;
	}
	if (swap_cnt == 0) {  /* Switch to insertion sort */
		for (pm = (char *)a + es; pm < (char *)a + n * es; pm += es)
			for (pl = pm;
			     pl > (char *)a && cmp(pl - es, pl) > 0;
			     pl -= es)
				swap(pl, pl - es);
		return;
	}

	pn = (char *)a + n * es;
	r = MIN(pa - (char *)a, pb - pa);
	vecswap(a, pb - r, r);
	r = MIN(pd - pc, pn - pd - es);
	vecswap(pb, pn - r, r);
	if ((r = pb - pa) > es)
		qsort(a, r / es, es, cmp);
	if ((r = pd - pc) > es) {
		/* Iterate rather than recurse to save stack space */
		a = pn - r;
		n = r / es;
		goto loop;
	}
/*		qsort(pn - r, r / es, es, cmp);*/
}

#define ARRAY_SIZE 50000

int compare (const void *a, const void *b)
{
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return *ia  - *ib;
}

int qsort_main()
{
	int *arr = malloc(ARRAY_SIZE * sizeof(int));
	int val = 1;
	//clock_t start, finish;

	//start = clock();
	for (size_t i = 0; i < ARRAY_SIZE; i++) {
		arr[i] = val;
		val = ((val * 8191) << 7) ^ val;
	}
	qsort(arr, ARRAY_SIZE, sizeof(int), compare);

	//finish = clock();
	//printf("Total costs %f ms\n", (float)(finish- start) / (CLOCKS_PER_SEC/1000));
	//printf("%u\n", arr[ARRAY_SIZE-1]);
	return 0;
}

int main(){
	unsigned long begin_cycle, end_cycle;
	begin_cycle =dd_get_cycle();
  qsort_main();
	end_cycle =dd_get_cycle();
	printf("[QSort] takes: %lu\n", end_cycle-begin_cycle);
	return 0;
}

