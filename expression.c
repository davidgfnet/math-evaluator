
// Math expression evaluator
// 24th March 2010
// david@davidgf.net
// 
// Unreadable code warning!

#include "expression.h"
#include <math.h>
#include <stdlib.h>

#define NUMBER_FUNCTIONS	9

float floatabs(float a) { if (a < 0) return -a; return a; }

struct type_function_names function_names[NUMBER_FUNCTIONS] = { 	{"sin",fSin},{"cos",fCos},{"tan",fTan},
									{"asin",fASin},{"acos",fACos},{"atan",fATan},
									{"abs",fAbs},{"log",fLog},{"ln",fLn} };

float (*function_pointers[9])(float a)  = { sinf, cosf, tanf, 0,0,0, floatabs, 0,0 };


// Just compares the n first bytes
int _mem_compare (const char * a, const char *b, int n) {
	while (n--) if (*(a++) != *(b++)) return -1;
	return 0;
}
// common strlen func. but implemented myself so we don't depend on libgcc
int _internal_strlen(const char * str) {
	int ret = 0;
	while (*(str++) != 0) ret++;
	return ret;
}
// Reads a literal from string just like 3.1416
float ReadLiteral(const char * string, int length) {
	float sign = 1;
	if (*string == '-') { sign = -1; string++; length--; }
	int i = 0;
	int p1 = 0, p2 = 0;
	while (i < length) {
		if (string[i] == '.') break;
		p1 = p1 * 10;
		p1 = p1 + (int)(string[i]-'0');
		i++;
	}
	if (string[i++] != '.') {
		float ret = p1;
		return sign*ret;
	}else{
		float div = 1;
		while (i < length) {
			p2 = p2 * 10;
			p2 = p2 + (int)(string[i]-'0');
			div *= 10;
			i++;
		}
		float ret = p1; float ret2 = p2;
		ret2 = ret2 / div;
		return sign*(ret+ret2);
	}
}

// Given a string finds the closer parenthesis
int SearchCloseParenthesis(const char * string, int length) {
	int i = 0, nump = 0;
	while (i < length) {
		if (string[i] == '(') nump++;
		else if (string[i] == ')') {
			if (nump == 0) return i;
			nump--;
		}
		i++;
	}
}
// Given a string finds the closer parenthesis but from right to left
int SearchCloseParenthesisInv(const char * string, int length) {
	int i = length-1, nump = 0;
	while (i >= 0) {
		if (string[i] == ')') nump++;
		else if (string[i] == '(') {
			if (nump == 0) return i;
			nump--;
		}
		i--;
	}
}

// Creates an expression into ret and returns the last used expression in the list
struct expression * CreateExpressionFromString_int (const char * string, int length, struct expression * ret) {
	int i,j,k;

	// First of all de-parenthise. Avoid excessive recursivity
	int changed = 1;
	while (changed) {
		changed = 0;
		if (length > 2) {
			if (string[0] == '(') {
			if (SearchCloseParenthesis(&string[1],length-1) == length-2) {
				string++;
				length-=2;
				changed = 1;
			}
			}
		}
	}

	// Just a literal?
	int literal = 1;
	for (i = 1; i < length; i++)
		if (!((string[i] >= '0' && string[i] <= '9') || string[i] == '.')) { literal = 0; break; }
	if (!((string[0] >= '0' && string[0] <= '9') || string[0] == '.' || string[0] == '-')) literal = 0;
	if (literal == 1) {
		ret->type = eLiteral;
		ret->evalue.literal_value = ReadLiteral(string,length);
		return ret+1;
	}

	// Is it just a variable? It can't be literal!
	if (length == 1) {
		ret->type = eVariable;
		ret->evalue.var = *string;
		return ret+1;
	}

	// Operators!

	// Operator priority!! First search for + -
	// Evaluate from right to left to avoid 6-2-2 being converted to 6-(2-2) just (6-2)-2
	for (i = length-1; i >= 0; i--) {
		if (string[i] == '+' || string[i] == '-') {  // We create two groups, all the stuff by now and the future stuff
			ret->type = eParent;
			if (string[i] == '+') ret->evalue.child.op = mAdd;
			else ret->evalue.child.op = mSub;

			ret->evalue.child.e1 = ret +1;
			ret->evalue.child.e2 = CreateExpressionFromString_int(string,i,ret->evalue.child.e1);
			return CreateExpressionFromString_int(&string[i+1],length-i-1,ret->evalue.child.e2);
		}
		if (string[i] == ')') {  // Maintain the parenthesis as a full expression, which we won't treat now
			j = SearchCloseParenthesisInv(string,i);
			i = j-1+1;  // Skip the entire block (+1 because next iteration will i--)
		}
	}

	// Second round, now search for * /
	// Evaluate from right to left because div has special unparenthised cases! 6/2/2 = 1.5. Just as (6/2)/2
	for (i = length-1; i >= 0; i--) {
		if (string[i] == '*' || string[i] == '/') {  // We create two groups, all the stuff by now and the future stuff
			ret->type = eParent;
			if (string[i] == '*') ret->evalue.child.op = mMul;
			else ret->evalue.child.op = mDiv;

			ret->evalue.child.e1 = ret +1;
			ret->evalue.child.e2 = CreateExpressionFromString_int(string,i,ret->evalue.child.e1);
			return CreateExpressionFromString_int(&string[i+1],length-i-1,ret->evalue.child.e2);
		}
		if (string[i] == ')') {  // Maintain the parenthesis as a full expression, which we won't treat now
			j = SearchCloseParenthesisInv(string,i);
			i = j-1+1;  // Skip the entire block (+1 because next iteration will i--)
		}
	}

	// Third round, just search for pow ^
	for (i = 0; i < length; i++) {
		if (string[i] == '(') {  // Maintain the parenthesis as a full expression, which we won't treat now
			j = SearchCloseParenthesis(&string[i+1],length -i -1);
			i = i+1+ j+1;  // Skip the entire block
		}
		if (string[i] == '^') {  // We create two groups, all the stuff by now and the future stuff
			ret->type = eParent;
			ret->evalue.child.op = mPow;
			
			ret->evalue.child.e1 = ret +1;
			ret->evalue.child.e2 = CreateExpressionFromString_int(string,i,ret->evalue.child.e1);
			return CreateExpressionFromString_int(&string[i+1],length-i-1,ret->evalue.child.e2);
		}
	}

	// This only leaves us to think we have a function
	for (i = 0; i < length; i++) {
		if (string[i] == '(') { // Search the func name
			for (j = 0; j < NUMBER_FUNCTIONS; j++) {
				if (_mem_compare(string,function_names[j].fname,_internal_strlen(function_names[j].fname)) == 0) {
					ret->type = eFunction;
					ret->evalue.function.func = function_names[j].fnumber;
					ret->evalue.function.e = ret+1;
					return CreateExpressionFromString_int(&string[i+1],length-i-1-1,ret->evalue.function.e);
				}
			}
		}
	}
}


struct expression * CreateExpressionFromString (const char * string) {  // Initial run, remove all the blanks
	char * good_expr = (char*)malloc(_internal_strlen(string)+1);
	char * tptr = good_expr;

	// Remove the spaces for better work and speed
	int len = 0;
	while (*string != 0) {
		if (*string != ' ') {
			*(tptr++) = *string;
			len++;
		}
		string++;
	}

	// Calculate the number of nodes that will be necessary so we cant create a big buffer
	// This is the sum of the number of operators, literals, functions and variables
	int i; int num_nodes = 0;
	for (i = 0; i < len; i++) {
		switch (good_expr[i]) {
		case '+': case '-': case '*': case '/': case '%': case '^':
			num_nodes++; break;
		default:
			if ((good_expr[i] >= '0' && good_expr[i] <= '9') || good_expr[i] == '.') {
				num_nodes++;
				while ((good_expr[i+1] >= '0' && good_expr[i+1] <= '9') || good_expr[i+1] == '.') i++;
			}
			if (good_expr[i] >= 'a' && good_expr[i] <= 'z') {
				num_nodes++;
				while (good_expr[i+1] >= 'a' && good_expr[i+1] <= 'z') i++;
			}
			break;
		}
	}
	struct expression * elist = (struct expression *)malloc((num_nodes)*sizeof(struct expression));
	//printf("Estimated heap memory usage: %d bytes\n",(num_nodes)*sizeof(struct expression)+_internal_strlen(string)+1);

	CreateExpressionFromString_int(&good_expr[0],len,elist);
	free(good_expr);
	return elist;
}

float EvaluateExpression (struct expression * expr, struct type_variable * variables) {
	switch (expr->type) {
	case  eLiteral:
		return expr->evalue.literal_value;
		break;
	case eVariable:
		while (variables->variable != 0) {
			if (expr->evalue.var == variables->variable)
				return variables->value;
			variables++;
		}
		return 0; // Some handling needed here
		break;
	case eFunction:
		return function_pointers[expr->evalue.function.func](EvaluateExpression(expr->evalue.function.e,variables));
		break;
	case eParent:
		switch (expr->evalue.child.op) {
			case mAdd:
				return EvaluateExpression(expr->evalue.child.e1,variables)+EvaluateExpression(expr->evalue.child.e2,variables); break;
			case mSub:
				return EvaluateExpression(expr->evalue.child.e1,variables)-EvaluateExpression(expr->evalue.child.e2,variables); break;
			case mMul:
				return EvaluateExpression(expr->evalue.child.e1,variables)*EvaluateExpression(expr->evalue.child.e2,variables); break;
			case mDiv:
				return EvaluateExpression(expr->evalue.child.e1,variables)/EvaluateExpression(expr->evalue.child.e2,variables); break;
			case mMod:
				return 0; break;
			case mPow:
				return 0; break;
			default: return 0; break;
		};
		break;
	default: return 0; break;
	};
}

void DeleteExpression (struct expression * expr) {
	free(expr);
}

