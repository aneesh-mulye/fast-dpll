#include "dpll.h"

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
}
