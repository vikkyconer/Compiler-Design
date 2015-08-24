// Revised parser

#include <stdio.h>
#include "lex.h"

void factor();
void term();
void expression();

statements() {
	//statements -> expression SEMI | expression SEMI statements

	while( !match(EOI)) {
		expression();

		if(match(SEMI))
			advance();
		else
			fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno);
	}
}

void expression() {
	//expression -> term expression'
	//expression' -> PLUS term expression' | epsilon

	if( !legal_lookahead(NUM_OR_ID, LP, 0))
		return;

	term();
	while( match( PLUS )) {
		advance();
		term();
	}
}

void term() {
	if( !legal_lookahead( NUM_OR_ID, LP, 0 ))
		return;

	factor();
	while( match( TIMES )) {
		advance();
		factor();
	}
}

void factor() {
	if( !legal_lookahead(NUM_OR_ID, LP, 0))
		return;

	if( match( NUM_OR_ID ))
		advance();

	else if( match( LP )) {
		advance();
		expression();
		if( match( RP ))
			advance();
		else
			fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno);
	}
	else
		fprintf(stderr, "%d: Number of identifier expected\n", yylineno);
}

#include <stdarg.h>
#define MAXFIRST 16
#define SYNCH	 SEMI

int legal_lookahead( first_arg )
int first_arg;

{
	va_list args;
	int tok;
	int lookaheads[MAXFIRST], *p = lookaheads, *current;
	int error_printed = 0;
	int rval = 0;

	va_start( args, first_arg);

	if( !first_arg ) {
		if( match( EOI ))
			rval = 1;
	}
	else {
		*p++ = first_arg;
		while( ( tok = va_arg( args, int)) && p < &lookaheads[MAXFIRST])
			*++p = tok;
		while( !match( SYNCH )) {
			for( current = lookaheads; current < p; current++)
				if( match( *current )) {
					rval = 1;
					goto exit;
				}

				if( !error_printed ) {
					fprintf(stderr, "Line %d: Syntax error\n", yylineno);
					error_printed = 1;
				}

				advance();
		}
	}

exit:
	va_end( args )
	return rval;
}