#include "dpll.h"
#include <stdio.h>
#include <stdlib.h>

int dpll_inter(cnfform_t formula, assignment_t assign) { 
	/* First, check for forced clauses. If any are found, check if that
	 * leads to UNSAT. If so, return 0. If not, assign and recurse. If the
	 * recursion fails, rollback and return 0. If it succeeds, just
	 * return. */

	/* Then, check for unit clauses. If found, assign and recurse. If
	 * fail, rollback and return 0. If succeed, just return. */

	/* If neither works, apply the heuristic and recurse first on the
	 * suggested assignment, then its inverse. If neither works, rollback
	 * and return 0. If either works, DO NOT rollback, and return 1. */
	return 10;
}

int forced(cnfform_t * formula, assign_t * ret) {

	int i;

	if(!formula)
		return -1;

	for(i=0; i < formula->num_clauses; i++)
		if(1 == formula->clauses[i].num_lits) {
			if(ret) {
				ret->lit = formula->clauses[i].lits[0];
				ret->val = (ret->lit > 0) ? TRUE : FALSE;
			}
			return 1;
		}

	return 0;
}

int pure(cnfform_t * formula, assignment_t * ret) {

	return 10;
}

void pp_clause(clause_t * c) {

	int i;

	if (!c)
		return;

	printf("(");
	for(i=0; i < c->num_lits-1; i++)
		printf("%s%d v ", c->lits[i] < 0 ? "~":"",
				c->lits[i] < 0 ? -c->lits[i]: c->lits[i]);
	printf("%s%d)", c->lits[i] < 0 ? "~":"",
				c->lits[i] < 0 ? -c->lits[i]: c->lits[i]);
}

void pp_cnfform(cnfform_t * formula) {

	int i;

	if(!formula)
		return;

	if(!formula->num_clauses || !formula->clauses)
		return;

	for(i=0; i < formula->num_clauses - 1; i++) {
		pp_clause(formula->clauses + i);
		printf("  ^  ");
	}
	pp_clause(formula->clauses + i);

	printf("\n");
	return;
}

int init_cnfform(cnfform_t * formula, unsigned int num_vars,
		unsigned int num_clauses) {

	if(!formula)
		return -1;

	formula->num_vars = num_vars;
	formula->num_clauses = num_clauses;
	formula->clauses = (clause_t *) malloc(sizeof(clause_t)*num_clauses);
	if(!formula->clauses)
		return -1;

	return 0;
}

int init_clause(clause_t * c, unsigned int num_lits) {

	int i;

	if(!c)
		return -1;

	c->num_lits = num_lits;

	c->lits = (literal_t *) malloc(sizeof(literal_t)*c->num_lits);
	if(!c->lits)
		return -1;

	for(i=0; i < c->num_lits; i++)
		c->lits[i] = 0;

	return 0;
}

void pp_assign(assign_t * a) {

	char map[3] = "ft?"; /* THIS IS NOT A STRING! IT IS NOT NULL-TERMINATD!
			      */
	if(!a)
		return;

	printf("(%d = %c)", (int)a->lit, map[a->val]);

	return;
}

void pp_assignment(assignment_t * a) {

	int i;

	if(!a || !a->asss || !a->num_asss)
		return;

	for(i = 0; i < a->num_asss - 1; i++) {
		pp_assign(a->asss + i);
		printf(", ");
	}
	pp_assign(a->asss + i);
	printf("\n");
}

int init_assignment(assignment_t * a, unsigned int num_asss) {

	int i;

	if(!a)
		return -1;

	a->num_asss = num_asss;
	a->asss = (assign_t *) malloc(sizeof(assign_t)*num_asss);
	if(!a->asss)
		return -1;

	for(i=0; i<num_asss; i++) {
		(a->asss+i)->lit = 0;
		(a->asss+i)->val = UNDEF;
	}
}
