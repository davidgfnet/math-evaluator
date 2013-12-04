
#include <stdio.h>
#include <stdlib.h>
#include "expression.h"

// Just a little regression for the tests

int main() {
	struct expression * ret = CreateExpressionFromString("(1-(1*5*(3*(9-3*1-3)/4)*2/3)-1*cos(3.1416))");
	printf("%f\n",EvaluateExpression(ret,NULL));
	DeleteExpression(ret);

	ret = CreateExpressionFromString("2*2*(4*3)+2*2-3*1-1*6-5*6*(5-6+1)*(-1)");
	printf("%f\n",EvaluateExpression(ret,NULL));
	DeleteExpression(ret);

	ret = CreateExpressionFromString("1/2/3/4/5+(4*1-(8*6*(-5))/3/2)");
	printf("%f\n",EvaluateExpression(ret,NULL));
	DeleteExpression(ret);

	ret = CreateExpressionFromString("(((5*6))/(2*((2-1-1+1))))");
	printf("%f\n",EvaluateExpression(ret,NULL));
	DeleteExpression(ret);


	struct type_variable vlist[3];
	vlist[0].variable = 'b';
	vlist[0].value = 1.6f;
	vlist[1].variable = 'c';
	vlist[1].value = -3.0f;
	vlist[2].variable = 0;

	ret = CreateExpressionFromString("(((5*c))/(2*((2-b-1+1))))");
	printf("%f\n",EvaluateExpression(ret,&vlist[0]));
	DeleteExpression(ret);

	return 0;
}


