
#ifndef EXPRESSION_H
#define EXPRESSION_H

enum expType { eLiteral, eVariable, eFunction, eParent };
enum math_operation { mAdd, mSub, mMul, mDiv, mMod, mPow, mRoot};
enum math_function { fSin = 0, fCos, fTan, fASin, fACos, fATan, fAbs, fLog, fLn };

struct type_function_names {
	char fname[5];
	enum math_function fnumber;
}; 

struct type_variable {
	char variable;
	float value;
};

struct expression {
	enum expType type;
	union evalue {
		float literal_value;
		char var;
		struct sfunction {
			enum math_function func;
			struct expression * e;
		} function;
		struct schild {
			enum math_operation op;
			struct expression * e1;
			struct expression * e2;
		} child;
	} evalue;
};


struct expression * CreateExpressionFromString (const char * string);
float EvaluateExpression (struct expression * expr, struct type_variable * variables);
void DeleteExpression (struct expression * expr);

#endif

