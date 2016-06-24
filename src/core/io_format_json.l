%{
#define YY_INPUT(buf,result,max_size) \
	{ \
		int c = getchar(); \
		result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
	}

#include <math.h>
%}
DIGIT    [0-9]
%%

{DIGIT}+			{ return atoi(yytext); }
{DIGIT}+"."{DIGIT}*	{ return atof(yytext); }
true				{ return TBoolValue(true); }
false				{ return TBoolValue(false); }
null				{ return TNullValue(); }
undefined			{ return TUndefinedValue(); }
\{					{ return TObjectValue(); }
\}					{ }
\[					{ return TArrayValue(); }
\]					{ }
%%
