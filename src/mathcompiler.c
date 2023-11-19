#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "mathcompiler.h"

static void mceventloop(void);
static int mclineparser(const char *, size_t);
static int mchandledigit(char *, const char *, size_t, size_t);
static int mciscommand(const char *);
static ssize_t mcreadbracket(const char *, size_t, ssize_t);
static int mcpushdigit(const char *);
static int mcpushoperator(const char);
static void mcclearstack(void);
static int mcisdigit(const char *);
static float mccalculationloop(void);
static float calculate(const float, const char, const float);

/*
 * Holds the number of digits stored in stack
 */
unsigned int digitstackcount = 0;

/*
 * Holds floats used to calculate the result
 */
static float digitstack[DIGITSTACKMAX];

/*
 * Holds number of chars in operator stack
 */
unsigned int operatorstackcount = 0;

/*
 * Holds chars representing the operators
 */ 
static char operatorstack[OPERATORSTACKMAX];

int
mcinit(void)
{
	mceventloop();
	return 0;
}

/*
 * Loops forever, reading a string from stdin
 */
static void
mceventloop(void)
{
	size_t len;
	float result;
	char buf[512];

	for (;;) {
		result = 0;

		fgets(buf, sizeof(buf), stdin);
		
		if (*buf) {
			/* Remove the newline char */
			len = strlen(buf);
			if (buf[len] == '\n')
				buf[len--] = '\0';

			if (!mclineparser(buf, len)) {
				result = mccalculationloop();
				fprintf(stdout, "%f\n", result);
			}

			mcclearstack();
		}
	}
}

/*
 * Reads a line following an assumption of num op num op num
 * e.g 1 + 1 + 1, doing 5 ++ * 5  + 5 -- 5 would be invalid
 * Brackets are pre calculated and pushed to the stack
 */
static int 
mclineparser(const char * line, size_t len)
{
	ssize_t i;
	int isnum = 0;
	char * tmpnum = (char*) 0;

	/*
	 * Check for built in commands
	 */
	if (mciscommand(line))
		return 0;

	if (!(tmpnum = (char*) malloc(sizeof(*line)))) {
		fputs("Bad alloc tmpnum\n", stderr);
		return 1;
	}

	memset(tmpnum, 0, sizeof(*tmpnum));

	/*
	 * Read the string
	 */
	for (i = 0; (size_t) i < len; i++) {
		/*
		 * Ignore whitespace
		 */
		if (isspace(line[i]))
			;

		/*
		 * If we encounter a bracket
		 */
		else if (line[i] == '(') {
			if (isnum == 1) {
				fprintf(stderr, "Syntax error '%c'\n", line[i]);
				free(tmpnum);
				return 1;
			}

			if ((i = mcreadbracket(line, len, i)) < 0) {
				free(tmpnum);
				return 1;
			}

			isnum = 1;
		}

		/*
		 * When we find a number
		 */
		else if (isdigit(line[i])) {
			if (isnum == 1) {
				fprintf(stderr, "Syntax error '%c'\n", line[i]);
				free(tmpnum);
				return 1;
			}

			if (mchandledigit(tmpnum, line, len, (size_t) i)) {
				free(tmpnum);
				return 1;
			}

			memset(tmpnum, 0, sizeof(*tmpnum));
			isnum = 1;
		}

		/*
		 * Treat anything else as an operator
     */
		else {
			if (isnum == 0) {
				fprintf(stderr, "Syntax error '%c'\n", line[i]);
				return 1;
			}

			if (mcpushoperator(line[i])) {
				free(tmpnum);
				return 1;
			}

			isnum = 0;
		}
	}

	free(tmpnum);
	return 0;
}

/*
 * Scans part of string for numeric chars
 */
static int
mchandledigit(char * s, const char * l, size_t len, size_t index)
{
	while (index < len) {
		if (isdigit(l[index]) || l[index] == '.')
			strncat(s, &l[index++], 1);
		else {
			--index;
			break;
		}
	}

	if (mcpushdigit(s))
		return 1;

	return 0;
}

/*
 * Checks for built in commands
 */
static int
mciscommand(const char * s)
{
	int i;

	for (i = 0; i < cmdmapmax; i++) {
		/* TODO fix issue with quitasdads being valid */
		if (!strncmp(s, cmdmap[i].str, strlen(cmdmap[i].str)))
			cmdmap[i].fn(s);
	}

	return 0;
}

static ssize_t
mcreadbracket(const char * s, size_t len, ssize_t open)
{
	int valid = 0;
	ssize_t close = open;
	char * exp = (char*) 0;

	/* Find the closing bracket */
	while ((size_t) close < len) {
		if (s[close] == ')') {
			valid = 1;
			break;
		} else {
			close++;
		}
	}

	if (!valid) {
		fputs("No closing bracket in statment\n", stderr);
		return -1;
	}

	if (!(exp = (char*) malloc(sizeof(*s)))) {
		fputs("Bad alloc exp\n", stderr);
		return -1;
	}

	memset(exp, 0, sizeof(*exp));

	/* Catonate new expression */
	while (++open < close)
		strncat(exp, &s[open], 1);

	if (mclineparser(exp, strlen(exp)))
		return -1;

	free(exp);
	return (ssize_t) close;
}

/*
 * Converts string to float and pushes it to number stack
 */
static int 
mcpushdigit(const char * s)
{
	float n;

	/*
	 * Make sure the string can be converted
	 */
	if (!mcisdigit(s))
		return 1;

	n = (float) strtod(s, (char**) 0);

	if (digitstackcount < DIGITSTACKMAX)
		digitstack[digitstackcount++] = n;

	else {
		fputs("Digit stack is full\n", stderr);
		return 1;
	}

	return 0;
}

static int 
mcpushoperator(const char c)
{
	switch (c) {
		case '+':
		case '-':
		case '*':
		case 'x':
		case 'X':
		case '/':
		case '^':
		case '%':
			break;
		default:
			fprintf(stderr, "%c is not an operator\n", c);
			return 1;
	}

	if (operatorstackcount < OPERATORSTACKMAX)
		operatorstack[operatorstackcount++] = c;
	else {
		fputs("Operator stack is full\n", stderr);
		return 1;
	}

	return 0;
}

/*
 * Resets stack values and counts
 */
static void
mcclearstack(void)
{
	while (digitstackcount > 0)
		digitstack[digitstackcount--] = 0;

	while (operatorstackcount > 0)
		operatorstack[operatorstackcount--] = (char) 0;
}

/*
 * Checks that a string can be converted into a float 
 */
static int
mcisdigit(const char * s)
{
	int dc = 0;

	/* Scan first element seperately */
	if (!isdigit(s[0])) {
		if (s[0] == '.')
			dc++;

		if (s[0] != '+' || s[0] != '-') {
			fprintf(stderr, "%s is not numeric\n", s);
			return 0;
		}
	}

	while (*++s) {
		if (!*s)
			break;

		if (*s == '.')
			dc++;

		else if (*s == '+' || *s == '-') {
			fprintf(stderr, "%s is not numeric\n", s);
			return 0;
		}

		else if (!isdigit(*s)) {
			fprintf(stderr, "%s is not numeric\n", s);
			return 0;
		}
	}

	if (dc >= 2) {
		fprintf(stderr, "%s is not numeric\n", s);
		return 0;
	}

	return 1;
}

static float
mccalculationloop(void)
{
	unsigned int i, k;
	float active, result;

	active = result = 0;

	for (i = 0, k = 0; i < digitstackcount; i++) {
		active = digitstack[i];

		if (i == 0)
			result = calculate(active, operatorstack[k++], digitstack[++i]);
		else
			result = calculate(result, operatorstack[k++], active);
	}

	return result;
}

static float
calculate(const float n1, const char o, const float n2)
{
	switch (o) {
		case '+':
			return (n1 + n2);
		case '-':
			return (n1 - n2);
		case '*':
		case 'x':
		case 'X':
			return (n1 * n2);
		case '/':
			if (n1 == 0 || n2 == 0) {
				fputs("Division by 0\n", stderr);
				return 0;
			}
			return (n1 / n2);
		case '^':
			return (float) pow(n1, n2);
		case '%':
			return (float) ((long) n1 % (long) n2);
	}

	return 0;
}
