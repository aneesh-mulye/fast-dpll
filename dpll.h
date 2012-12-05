#ifndef __DPLL_H_INCLUDED__
#define __DPLL_H_INCLUDED__

typedef int literal_t;

typedef struct {
	unsigned int num_lits;
	literal_t *lits;
} clause_t;

typedef struct {
	unsigned int num_vars;
	literal_t *vars; /* All positive, by convention. */
	unsigned int num_clauses;
	clause_t *clauses;
} cnfform_t;

typedef struct {
	unsigned int ext_clauses;
	unsigned short int *inc_clauses;
	unsigned int *swaps1, *swaps2;
} reversal_t;


int init_clause(clause_t *, unsigned int);
int init_cnfform(cnfform_t *, unsigned int, unsigned int);

void pp_cnfform(cnfform_t *);
void pp_clause(clause_t *);
void pp_reversal(reversal_t *, unsigned int);

int partial_assign(cnfform_t *, literal_t, reversal_t *);
int partial_assignment(cnfform_t *, literal_t *, unsigned int, reversal_t **);
int reverse_assign(cnfform_t *, reversal_t *);
int reverse_assignment(cnfform_t *, reversal_t *, unsigned int);

int inclause(literal_t, clause_t *);
int swapout(int, clause_t *);
int haslit(cnfform_t *, literal_t);

int unitclauses(cnfform_t *, literal_t **, unsigned int *);
int pureliterals(cnfform_t *, literal_t **, unsigned int *);

int dpll_inner(cnfform_t *, literal_t **, unsigned int);
int dpll(cnfform_t *, literal_t **);
#endif
