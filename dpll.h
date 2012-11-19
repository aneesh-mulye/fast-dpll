typedef int literal_t;

typedef enum {
	TRUE,
	FALSE,
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

int dpll(cnfform_t, assignment_t);
int dpll_inter(cnfform_t, assignment_t);
assignment_t purelits(cnfform_t);
assign_t forced(cnfform_t);
void simplify(cnfform_t *, assignment_t *);
assign_t
