#include "yoctobasic.h"

/**
 * CLEAR
 *
 * Clear the run time environment and start all over.
 * Like if called for the first time.
 */
void kw_clear()
{
	no_junk_eol(5);
	reset();
}

/**
 * END
 *
 * Ends execution.
 */
void kw_end(void)
{
	no_junk_eol(3);
	exit(0);
}

/**
 * FOR var = expression TO expression [STEP expression]
 *
 * Loop betweeen for/to with optional step.
 */
void kw_for(void)
{
	int var = pickup_variable(3);
	match('=');
	skip_whitespaces();
	if (*ip == '\n') {
		error("no value to variable");
	}

	int result = expression();
	variables[var] = result;

	skip_whitespaces();
	if (strncasecmp(ip, "to", 2) != 0) {
		error("missing to");
	}

	ip += 2;
	int to = expression();
	skip_whitespaces();

	int step = 1;
	if (strncasecmp(ip, "step", 4) == 0) {
		ip += 4;
		step = expression();
		if (step == 0) {
			error("cannot step 0");
		}
	}

	no_junk_eol(0);
	struct stack_item item;

	item.si_type = SI_FOR;
	item.variable = var;
	item.to = to;
	item.step = step;

	stack_push(&item);
}

/**
 * GOSUB
 *
 * Call subroutine (label).
 */
void kw_gosub(void)
{
	ip += 5;
	int label = expression();
	char *ptr = locate_label(label);
	no_junk_eol(0);

	struct stack_item i;
	i.si_type = SI_GOSUB;
	stack_push(&i);

	ip = ptr;
}

/**
 * GOTO
 *
 * Go unconditionally to label.
 */
void kw_goto(void)
{
	ip += 4;
	int label = expression();
	ip = locate_label(label);
}

/**
 * INPUT var [,var]*
 *
 * Read input from stdin. Input must be numbers.
 */
void kw_input(void)
{
	ip += 5;

	for (EVER) {
		skip_whitespaces();
		if (*ip == '\n') {
			line_number++;
			ip++;
			break;
		}

		int var = pickup_variable(0);
		int result;

		for (EVER) {
			int r = scanf("%d", &result);
			if (r == EOF) {
				error("huh? Gone?");
			}

			if (r == 0) {
				printf("?\n");
				continue;
			}

			variables[var] = result;
			break;
		}

		skip_whitespaces();
		if (*ip == ',') {
			match(',');
		}
	}
}

/**
 * IF expression <|<=|>|>=|<>|= expression THEN statement
 *
 * If statement.
 */
void kw_if(void)
{
	ip += 2;

	skip_whitespaces();
	int lhs = expression();
	int relop = relop_pickup();
	int rhs = expression();

	if (!relop_evaluate(relop, lhs, rhs)) {
		eol();
		return;
	}

	skip_whitespaces();
	if (strncasecmp(ip, "then", 4) != 0) {
		error("missing then to if");
	}

	ip += 4;
	skip_whitespaces();
	statement();
}

/**
 * LET var = expression
 *
 * Assign result of expression to var. Var must be 'a' <= var <= 'z'.
 */
void kw_let(void)
{
	int var = pickup_variable(3);
	match('=');
	skip_whitespaces();
	if (*ip == '\n') {
		error("no value to variable");
	}

	int result = expression();
	variables[var] = result;
	no_junk_eol(0);
}

/**
 * NEXT
 *
 * Next iteration of latest FOR.
 */
void kw_next(void)
{
	no_junk_eol(4);

	char *save_ip = ip;
	int save_line = line_number;

	struct stack_item item;
	stack_peek(&item);

	if (item.si_type != SI_FOR) {
		error("unbalanced stack - expected SI_FOR");
	}

	variables[item.variable] += item.step;
	int pop = 0;
	if (item.step > 0) {
		pop = variables[item.variable] > item.to;
	} else {
		pop = variables[item.variable] < item.to;
	}

	if (pop) {
		stack_pop(&item);
		ip = save_ip;
		line_number = save_line;
	}
}

/**
 * PRINT "string" | expression [, "string" | expression]*
 *
 * Print to stdio.
 */
void kw_print(void)
{
	ip += 5;
	for (EVER) {
		skip_whitespaces();
		if (*ip == '\n') {
			printf("\n");
			line_number++;
			ip++;
			break;
		}

		if (*ip == '"') {
			ip++;

			char *str = ip;
			int in_str = 1;
			for (; *ip != 0; ip++) {
				if (*ip == '\n') {
					error("unterminated string constant");
				}

				if (*ip == '"') {
					*ip = 0;
					printf("%s", str);
					*ip = '"';
					ip++;
					in_str = 0;
					break;
				}
			}

			if (in_str) {
				error("unterminated string constant");
			}
		} else {
			int value = expression();
			printf("%i", value);
		}

		skip_whitespaces();
		if (*ip == ',') {
			ip++;
		}
	}
}

/**
 * REM
 *
 * Remark - ignore rest of line.
 */
void kw_rem(void)
{
	ip += 3;
	eol();
}

/**
 * RETURN
 *
 * Return to line after calling GOSUB.
 */
void kw_return(void)
{
	no_junk_eol(6);

	struct stack_item i;
	stack_pop(&i);
	if (i.si_type != SI_GOSUB) {
		error("unbalanced stack");
	}
}

