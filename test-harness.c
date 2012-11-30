#include "dpll.h"

int main_assignment() {

	assignment_t a;

	init_assignment(&a, 10);
	pp_assignment(&a);

	return 0;
}

int main_formula() {

	int lits = 3, clauses = 10, i;
	cnfform_t f;

	init_cnfform(&f, 7, 10);

	for(i=0; i < f.num_clauses; i++)
		init_clause(f.clauses+i, lits);

	pp_cnfform(&f);

	return 0;
}

int main() {

	return main_assignment();
}
