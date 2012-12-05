#include "dpll.h"
#include <stdio.h>


int main_formula() {

	int lits = 3, clauses = 10, i;
	cnfform_t f;

	init_cnfform(&f, 7, 10); 
	for(i=0; i < f.num_clauses; i++)
		init_clause(f.clauses+i, lits);

	pp_cnfform(&f);

	return 0;
}

int main_partial() {

	int lits = 3, clauses = 10, i, j;
	cnfform_t f;
	reversal_t rev;

	init_cnfform(&f, 7, 10);

	for(i=0; i < f.num_clauses; i++)
		init_clause(f.clauses+i, lits);

	for(i=0; i < f.num_clauses; i++)
		for(j=0; j<lits; j++)
			f.clauses[i].lits[j] = (i*lits + j)%10 + 1;

	pp_cnfform(&f);

	partial_assign(&f, -1, &rev);

	pp_cnfform(&f);
	pp_reversal(&rev, f.num_clauses + rev.ext_clauses);

	return 0;
}

int main_reverse() {

	int lits = 3, clauses = 10, i, j;
	cnfform_t f;
	reversal_t rev;

	init_cnfform(&f, 7, 10);

	for(i=0; i < f.num_clauses; i++)
		init_clause(f.clauses+i, lits);

	for(i=0; i < f.num_clauses; i++)
		for(j=0; j<lits; j++)
			f.clauses[i].lits[j] = (i*lits + j)%10 + 1;

	pp_cnfform(&f);

	partial_assign(&f, 1, &rev);

	pp_cnfform(&f);
	pp_reversal(&rev, f.num_clauses + rev.ext_clauses);

	reverse_assign(&f, &rev);

	pp_cnfform(&f);

	return 0;
}

int main_reversefull() {

	int lits = 3, clauses = 5, i, j;
	int asgarr[3] = {-1, -2, -3};
	cnfform_t f;
	reversal_t *rev;

	init_cnfform(&f, 10, 10);

	for(i=0; i < f.num_clauses; i++)
		init_clause(f.clauses+i, lits);

	for(i=0; i < f.num_clauses; i++)
		for(j=0; j<lits; j++)
			f.clauses[i].lits[j] = (i*lits + j)%10 + 1;

	for(i=0; i < f.num_vars; i++)
		f.vars[i] = i+1;

	pp_cnfform(&f);

	partial_assignment(&f, asgarr, 3, &rev);

	pp_cnfform(&f);

	reverse_assignment(&f, rev, 3);

	pp_cnfform(&f);

	return 0;
}

int main_unitclauses() {

	int lits = 3, clauses = 5, i, j;
	int asgarr[3] = {-1, -2, -3};
	cnfform_t f;
	reversal_t *rev;
	literal_t *unitlits;
	unsigned int num_ul;

	init_cnfform(&f, 10, 10);

	for(i=0; i < f.num_clauses; i++)
		init_clause(f.clauses+i, lits);

	for(i=0; i < f.num_clauses; i++)
		for(j=0; j<lits; j++)
			f.clauses[i].lits[j] = (i*lits + j)%10 + 1;

	for(i=0; i < f.num_vars; i++)
		f.vars[i] = i+1;

	pp_cnfform(&f);

	partial_assignment(&f, asgarr, 3, &rev);

	pp_cnfform(&f);

	unitclauses(&f, &unitlits, &num_ul);
	printf("Number of unit clause assignments: %d\n", num_ul);
	for(i=0; i<num_ul; i++)
		printf("%d ", unitlits[i]);
	printf("\n");

	reverse_assignment(&f, rev, 3);

	pp_cnfform(&f);

	return 0;
}

int main_pureliterals() {

	int lits = 3, clauses = 5, i, j;
	int asgarr[3] = {1, 4, 7};
	cnfform_t f;
	reversal_t *rev;
	literal_t *purelits;
	unsigned int num_pl;

	init_cnfform(&f, 10, 10);

	for(i=0; i < f.num_clauses; i++)
		init_clause(f.clauses+i, lits);

	for(i=0; i < f.num_clauses; i++)
		for(j=0; j<lits; j++)
			f.clauses[i].lits[j] = (i*lits + j)%10 + 1;

	for(i=0; i < f.num_vars; i++)
		f.vars[i] = i+1;

	pp_cnfform(&f);

	partial_assignment(&f, asgarr, 3, &rev);

	pp_cnfform(&f);

	pureliterals(&f, &purelits, &num_pl);
	printf("Number of pure literals: %d\n", num_pl);
	for(i=0; i<num_pl; i++)
		printf("%d ", purelits[i]);
	printf("\n");

	reverse_assignment(&f, rev, 3);

	pp_cnfform(&f);

	return 0;
}

int main() {

	return main_pureliterals();
}
