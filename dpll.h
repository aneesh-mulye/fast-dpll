#ifndef __DPLL_H_INCLUDED__
#define __DPLL_H_INCLUDED__

typedef int literal_t;

typedef enum {
	FALSE,
	TRUE,
	UNDEF
} value_t;

typedef struct {
	literal_t lit;
	value_t val;
} assign_t;

typedef struct{
	unsigned int num_asss;
	assign_t *asss;
} assignment_t;

typedef struct {
	unsigned int num_lits;
	literal_t *lits;
} clause_t;

typedef struct {
	unsigned int num_vars;
	unsigned int num_clauses;
	clause_t *clauses;
} cnfform_t;


int init_clause(clause_t *, unsigned int);
int init_cnfform(cnfform_t *, unsigned int, unsigned int);
int init_assignment(assignment_t *, unsigned int);
void pp_assign(assign_t *);
void pp_assignment(assignment_t *);
void pp_cnfform(cnfform_t *);
void pp_clause(clause_t *);
int dpll(cnfform_t, assignment_t);
int dpll_inter(cnfform_t, assignment_t);
assignment_t purelits(cnfform_t);
/* Gives us the first forced assignment it encounters in the assignment.
 * Returns 0 if none is found, leaving the second argument unchanged.
 * Returns 1 if found, and in the second argument (if non-null) returns it.
 * Returns -1 on error/invalid input.
 */
int forced(cnfform_t *, assign_t *);
void simplify(cnfform_t *, assignment_t *);

#endif
