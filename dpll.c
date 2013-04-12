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
#ifdef DEBUG
			printf("Switching %d ", first);
			pp_clause(formula->clauses+first);
			printf(" with %d ", last);
			pp_clause(formula->clauses+last);
			printf("\n");
#endif
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
			/* Switch the clauses... 
			tempclause = formula->clauses[first];
			formula->clauses[first] = formula->clauses[last];
			formula->clauses[last] = tempclause;
			*/
			/* ...swap the literals in the clause... */
			swapout(lp, formula->clauses+first);
			/* ...record the swap... */
			/*
			temprev.swaps1[i] = first;
			temprev.swaps2[i] = last;
			*/
			/* ...and record what you just did in the reversal
			 * list.
			 */
			temprev.inc_clauses[first] = 1;

			first++;
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

	unsigned int num_forced = 0, i, j, consistent;
	literal_t *temp_flits;

	if(!formula || !flits || !num_lits)
		return -1;

	for(i=0; i<formula->num_clauses; i++)
		if(1 == formula->clauses[i].num_lits)
			num_forced++;

#ifdef DEBUG
	if(!num_forced)
		printf("No unit clauses.\n");
#endif
	if(!num_forced)
		return 0;

	temp_flits = malloc(sizeof(literal_t)*num_forced);
	if(!temp_flits)
		return -1;

	/* *num_lits = num_forced; */
	num_forced = 0;

	for(i=0; i<formula->num_clauses; i++)
		if(1 == formula->clauses[i].num_lits)
			temp_flits[num_forced++] = formula->clauses[i].lits[0];

	/* Make this a consistent assignment; no self-conflicting
	 * assignments are allowed.
	 */

	for(i=0; i < num_forced; i++)
		for(j=i+1; j < num_forced; j++)
			if(temp_flits[i] == temp_flits[j] ||
					temp_flits[i] == -temp_flits[j])
				temp_flits[j] = 0;

	consistent = 0;
	for(i=0; i<num_forced; i++)
		if(temp_flits[i])
			consistent++;
	*flits = (literal_t *) malloc(j*sizeof(literal_t));
	if(!*flits) {
		free(temp_flits);
		return -1;
	}
	*num_lits = consistent;
	j = 0;
	for(i=0; i<num_forced; i++)
		if(temp_flits[i])
			(*flits)[j++] = temp_flits[i];

	free(temp_flits);

#ifdef DEBUG
	printf("%d unit clauses: ", j);
	for(i = 0; i<j; i++)
		printf("%d ", (*flits)[i]);
	printf("\n");
#endif
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
#ifdef DEBUG
			if(-1 == tempind) {
				printf("PROBLEM on %d at index %d, %d!\n",
						formula->clauses[i].lits[j],
						i, j);
				pp_cnfform(formula);
				pp_clause(formula->clauses+i);
				printf("\n");
			}
#endif
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
#ifdef DEBUG
	printf("%d pure literals: ", num_pure);
	for(i = 0; i<num_pure; i++)
		printf("%d ", (*plits)[i]);
	printf("\n");
#endif
	return 0;
}

int dpll_inner(cnfform_t * formula, literal_t **assignment,
		unsigned int assigned) {

	literal_t *plit = 0, *uclause = 0, heur = 0;
	reversal_t *purerev = 0, *unitrev = 0, heurrev;
	unsigned int pureadv = 0, unitadv = 0;
	int tempresult = 0, i = 0, heurresult = 0;
	int baseheurwork = 0;
	if(!formula || !assignment)
		return -1;

	if(!assigned) {
		*assignment = malloc((sizeof(literal_t)*formula->num_vars));
		if(!*assignment)
			return -1;
	}

#ifdef DEBUG
	pp_cnfform(formula);
#endif
	/* Check if it turns out that we're at the end of the chain;
	 * allows for `blind calls'.
	 */
	if(1 == issat(formula))
		return 1;
	if(1 == isunsat(formula))
		return 0;

	/* First the pure literals. */
	pureliterals(formula, &plit, &pureadv);
	if(pureadv) {
		baseheurwork = 1;
		partial_assignment(formula, plit, pureadv, &purerev);
		for(i=0; i<pureadv; i++)
			(*assignment)[assigned++] = plit[i];
	}
	unitclauses(formula, &uclause, &unitadv);
	if(unitadv) {
		baseheurwork = 1;
		partial_assignment(formula, uclause, unitadv, &unitrev);
		for(i=0; i<unitadv; i++)
			(*assignment)[assigned++] = uclause[i];
	}
#ifdef DEBUG
	if(1 == unitadv)
		pp_reversal(unitrev, formula->num_clauses+unitrev[0].ext_clauses);
#endif

	/* Not required again - free now. */
	if(plit) free(plit);
	if(uclause) free(uclause);

	if(1 == baseheurwork) {
		/* Continue the unit clause cascade. */
		tempresult = dpll_inner(formula, assignment, assigned);
		assigned -= (unitadv + pureadv);
		if(unitadv)
			reverse_assignment(formula, unitrev, unitadv);
		if(pureadv)
			reverse_assignment(formula, purerev, pureadv);
		for(i=0; i<pureadv; i++)
			cleanrev(purerev+i);
		for(i=0; i<unitadv; i++)
			cleanrev(unitrev+i);
		free(purerev);
		free(unitrev);
		return tempresult;
	}

	/* The basic heuristics turned up empty.
	 * Recurse. Using the custom heuristic. */
	if(-1 == heuristic(formula, &heur))
		return -1;
	/* First, try the heuristic value. */
	partial_assign(formula, heur, &heurrev);
	(*assignment)[assigned++] = heur;
	heurresult = dpll_inner(formula, assignment, assigned);
	if(-1 == heurresult)
		return -1;
	if(1 == heurresult) {
		/* It worked! Let's free all the reversals and return. */
		reverse_assign(formula, &heurrev);
		if(unitadv)
			reverse_assignment(formula, unitrev, unitadv);
		if(pureadv)
			reverse_assignment(formula, purerev, pureadv);
		for(i=0; i<pureadv; i++)
			cleanrev(purerev+i);
		for(i=0; i<unitadv; i++)
			cleanrev(unitrev+i);
		cleanrev(&heurrev);
		free(purerev);
		free(unitrev);
		return 1;
	}
	/* Didn't work. Reverse... */
	reverse_assign(formula, &heurrev);
	cleanrev(&heurrev);
	assigned--;
	/* ...and try the other one. */
	partial_assign(formula, -heur, &heurrev);
	(*assignment)[assigned++] = heur;
	heurresult = dpll_inner(formula, assignment, assigned);
	if(-1 == heurresult)
		return -1;
	/* Reverse all things - IN REVERSE ORDER - and return. */
	reverse_assign(formula, &heurrev);
	if(unitadv)
		reverse_assignment(formula, unitrev, unitadv);
	if(pureadv)
		reverse_assignment(formula, purerev, pureadv);

	for(i=0; i<pureadv; i++)
		cleanrev(purerev+i);
	for(i=0; i<unitadv; i++)
		cleanrev(unitrev+i);
	cleanrev(&heurrev);
	free(purerev);
	free(unitrev);

	return heurresult;
}

int issat(cnfform_t * formula) {

	if(!formula)
		return -1;

	if(0 == formula->num_clauses)
		return 1;

	return 0;
}

int isunsat(cnfform_t * formula) {

	int i;

	if(!formula)
		return -1;

	for(i=0; i<formula->num_clauses; i++)
		if(0 == formula->clauses[i].num_lits)
			return 1;

	return 0;
}

int cleanrev(reversal_t * rev) {

	if(!rev)
		return -1;

	free(rev->inc_clauses);
	free(rev->swaps1);
	free(rev->swaps2);

	return 0;
}

int heuristic(cnfform_t * formula, literal_t * heur) {

	if(!formula || !heur)
		return -1;

	*heur = formula->vars[0];

	return 0;
}

int dpll(cnfform_t *formula, literal_t ** assignment) {

	return dpll_inner(formula, assignment, 0);
}

int init_3cnfform_file(cnfform_t * formula, char *path) {

	FILE *inf;
	char abc[81], j1[10], j2[10];
	int vars, clauses, i = 0;
	literal_t l1, l2, l3;

	if(!formula || !path)
		return -1;

	inf = fopen(path, "r");

	while(!feof(inf)) {
		fgets(abc, 81, inf);
		if('p' == abc[0]) {
			/* Specifier; init the formula. */
			sscanf(abc, "%s %s %d %d", j1, j2, &vars, &clauses);
			init_cnfform(formula, vars, clauses);
			/* And the vars. */
			for(i = 0; i<formula->num_vars; i++)
				formula->vars[i]= i+1;
			for(i=0; i<formula->num_clauses; i++)
				init_clause(formula->clauses + i, 3);
			i = 0;
		}
		if('-' == abc[0] || isdigit(abc[0]) || ' ' == abc[0]) {
			/* Clause; put it into the formula. */
			sscanf(abc, "%d %d %d", formula->clauses[i].lits,
					formula->clauses[i].lits+1,
					formula->clauses[i].lits+2);
			i++;
		}
		if('%' == abc[0])
			break;
	}

	fclose(inf);

}
