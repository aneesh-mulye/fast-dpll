#include "dpll.h"
#include <stdio.h>
#include <stdlib.h>

void pp_clause(clause_t * c) {

	int i;

	if (!c)
		return;

	if(c->num_lits == 0) {
		printf("()");
		return;
	}
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

	if(!formula->clauses)
		return;

	printf("Variables: %d\n", formula->num_vars);
	if(!formula->num_vars)
		return;
	for(i=0; i < formula->num_vars -1 ; i++)
		printf("%d ", formula->vars[i]);
	printf("%d\n", formula->vars[i]);

	printf("Clauses: %d\n", formula->num_clauses);
	if(!formula->num_clauses)
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
	formula->vars = (literal_t *) malloc(sizeof(literal_t)*num_vars);
	if(!formula->vars) {
		free(formula->clauses);
		return -1;
	}

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

int partial_assign(cnfform_t * formula, literal_t lit, reversal_t * r) {

	int i, orignum;
	int first, last, lp; /* lp is literal position. */
	reversal_t temprev;
	clause_t tempclause;
	literal_t templit;

	if(!formula || !lit || !r)
		return -1;

	/* Check that the assigned variable is actually there. */
	if(-1 == haslit(formula, lit))
		return -1;
	/* Remove it from the formula. */
	templit = formula->vars[haslit(formula, lit)];
	formula->vars[haslit(formula, lit)] =
		formula->vars[formula->num_vars-1];
	formula->vars[formula->num_vars-1] = templit;
	formula->num_vars--;

	temprev.ext_clauses = 0;
	temprev.inc_clauses = malloc(sizeof(unsigned short int)*
			formula->num_clauses);
	if(!temprev.inc_clauses)
		return -1;
	temprev.swaps1 = malloc(sizeof(unsigned int)*formula->num_clauses);
	if(!temprev.swaps1) {
		free(temprev.inc_clauses);
		return -1;
	}
	temprev.swaps2 = malloc(sizeof(unsigned int)*formula->num_clauses);
	if(!temprev.swaps2) {
		free(temprev.inc_clauses);
		free(temprev.swaps1);
		return -1;
	}
	for(i=0; i<formula->num_clauses;i++)
		temprev.inc_clauses[i] = temprev.swaps1[i] = 
			temprev.swaps2[i] = 0;

	first = 0;
	last = formula->num_clauses-1;

	orignum = formula->num_clauses;
	for(i = 0; i < orignum; i++)
		if(-1 < (lp = inclausep(lit, formula->clauses + first))) {
			/* Switch the clauses... */
			tempclause = formula->clauses[first];
			formula->clauses[first] = formula->clauses[last];
			formula->clauses[last] = tempclause;
			/* ...remove the satisfied one from the formula... */
			formula->num_clauses--;
			/* ...record the swap... */
			temprev.swaps1[i] = first;
			temprev.swaps2[i] = last;
			/* ...and write that you have to the reversal list. */
			temprev.ext_clauses++;

			last--;
		} else if(-1 < (lp = inclausep(-lit, formula->clauses+first))){
			/* Switch the clauses... */
			tempclause = formula->clauses[first];
			formula->clauses[first] = formula->clauses[last];
			formula->clauses[last] = tempclause;
			/* ...swap the literals in the clause... */
			swapout(lp, formula->clauses+last);
			/* ...record the swap... */
			temprev.swaps1[i] = first;
			temprev.swaps2[i] = last;
			/* ...and record what you just did in the reversal
			 * list.
			 */
			temprev.inc_clauses[last] = 1;

			last--;
		}
		else {
			first++;
		}
	/* Copy out the reversal list... */
	*r = temprev;

	/* ...and you're done! */
	return 0;
}

int inclausep(literal_t lit, clause_t * clause) {

	int i;

	if(!clause)
		return -1;

	for(i=0; i<clause->num_lits; i++)
		if(lit == clause->lits[i])
			return i;

	return -1;
}

int swapout(int pos, clause_t * clause) {

	literal_t templit;

	if(!clause || pos >= clause->num_lits)
		return -1;

	templit = clause->lits[clause->num_lits-1];
	clause->lits[clause->num_lits-1] = clause->lits[pos];
	clause->lits[pos] = templit;

	clause->num_lits--;

	return 0;
}

void pp_reversal(reversal_t * rev, unsigned int formsize) {

	unsigned int i;

	if(!rev)
		return;

	printf("Extend formula by %d clauses.\n", rev->ext_clauses);

	for(i=0; i<formsize-1; i++)
		printf("%d ", rev->inc_clauses[i]);
	printf("%d\n", rev->inc_clauses[i]);

	return;
}

int reverse_assign(cnfform_t * formula, reversal_t * rev) {

	int i;
	clause_t tempclause;

	if(!formula || !rev)
		return -1;

	formula->num_clauses+=rev->ext_clauses;
	formula->num_vars++;

	for(i=0; i < formula->num_clauses; i++)
		formula->clauses[i].num_lits += rev->inc_clauses[i];

	for(i = formula->num_clauses - 1; i >= 0; i--)
		if(rev->swaps1[i] != rev->swaps2[i]) {
			tempclause = formula->clauses[rev->swaps1[i]];
			formula->clauses[rev->swaps1[i]] =
				formula->clauses[rev->swaps2[i]];
			formula->clauses[rev->swaps2[i]] = tempclause;
		}

	return 0;

}

int partial_assignment(cnfform_t * formula, literal_t * litlist,
		unsigned int numlits, reversal_t ** revlist) {

	unsigned int i;

	if(!formula || !litlist || !revlist || !numlits)
		return -1;

	*revlist = malloc(sizeof(reversal_t)*numlits);
	if(!*revlist)
		return -1;

	for(i = 0; i < numlits; i++)
		partial_assign(formula, litlist[i], *revlist + i);

	return 0;
}

int reverse_assignment(cnfform_t * formula, reversal_t * revlist,
		unsigned int numlits) {

	int i;

	if(!formula || !revlist || !numlits)
		return -1;

	for(i = numlits - 1; i >= 0; i--)
		reverse_assign(formula, revlist+i);

	return 0;
}

int haslit(cnfform_t * formula, literal_t lit) {

	int i;

	if(!formula)
		return -1;

	if(!formula->num_vars || !formula->vars)
		return -1;

	for(i=0; i<formula->num_vars; i++)
		if(formula->vars[i] == lit ||
				formula->vars[i] == -lit)
			return i;

	return -1;
}

int unitclauses(cnfform_t * formula, literal_t ** flits,
		unsigned int * num_lits) {

	unsigned int num_forced = 0, i;

	if(!formula || !flits || !num_lits)
		return -1;

	for(i=0; i<formula->num_clauses; i++)
		if(1 == formula->clauses[i].num_lits)
			num_forced++;

	if(!num_forced)
		return 0;

	*flits = malloc(sizeof(literal_t)*num_forced);
	if(!*flits)
		return -1;

	*num_lits = num_forced;
	num_forced = 0;

	for(i=0; i<formula->num_clauses; i++)
		if(1 == formula->clauses[i].num_lits)
			(*flits)[num_forced++] = formula->clauses[i].lits[0];

	return 0;
}

int pureliterals(cnfform_t * formula, literal_t ** plits,
		unsigned int * num_lits) {

	short int *status, *values;
	unsigned int num_pure=0, i, j;
	int tempind;
	if(!formula || !plits || !num_lits)
		return -1;

	status = (short int *) malloc((sizeof(short int)*formula->num_vars));
	if(!status)
		return -1;
	values = (short int *) malloc((sizeof(short int)*formula->num_vars));
	if(!values) {
		free(status);
		return -1;
	}

	for(i=0; i<formula->num_vars; i++)
		status[i] = 0;

	for(i=0; i < formula->num_clauses; i++)
		for(j=0; j < formula->clauses[i].num_lits; j++) {
			tempind = haslit(formula,
					formula->clauses[i].lits[j]);
			if(0 == status[tempind]) {
				values[tempind] =
				   (formula->clauses[i].lits[j] > 0) ? 1 : -1;
				status[tempind] = 1;
			} else if(1 == status[tempind] &&
			   (values[tempind]*formula->clauses[i].lits[j] < 0)) {
				status[tempind] = -1;
				values[tempind] = 0;
			}
		}

	for(i=0; i<formula->num_vars; i++)
		if(1 == status[i])
			num_pure++;

	*plits = (literal_t *) malloc((sizeof(literal_t)*num_pure));
	if(!*plits) {
		free(values);
		free(status);
		return -1;
	}
	*num_lits = num_pure;
	num_pure = 0;


	for(i=0; i<formula->num_vars; i++)
		if(1 == status[i])
			(*plits)[num_pure++] = formula->vars[i]*values[i];

	free(status);
	free(values);

	return 0;
}

int dpll_inner(cnfform_t * formula, literal_t **assignment,
		unsigned int * assigned) {

	if(!formula || !assignment)
		return -1;
}
