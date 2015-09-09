#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <sysexits.h>

using namespace std;

#include "pai_types.h"
#include "pai_tok.h"
#include "pai_toklist.h"

#include "pai_ic.h"

extern int verbose;

void
IC::IC_Tokenize(void)
{
	int i;

	if (verbose)
		TokListPrint();

	/*
	 * <WARUNKI POCZATKOWE>
	 */
	ExpectString("<WARUNKI");
	ExpectWs();
	ExpectString("POCZATKOWE>");

	TokSkipTill(TK_NL);

	ExpectNl();
	ExpectNl();

	/* Nag³ówek */
	ExpectString("HeatTransfer_1.0");
	ExpectNl();
	ExpectNl();

	l_wymiarow = ExpectInt();
	ExpectWs();

	l_wezlow = ExpectInt();
	ExpectWs();

	l_elem = ExpectInt();
	ExpectWs();

	l_obszarow = ExpectInt();
	ExpectWs();

	l_brzegow = ExpectInt();
	ExpectNl();

	ExpectNl();

	i = ExpectInt(); ExpectNl();

	(void)ExpectFp();

	if (verbose) {
		cout << "---------------------------" << endl;
		cout << "Wypisujê dane dot. pliku IC" << endl;
		cout << l_wymiarow << endl;
		cout << l_wezlow << endl;
		cout << l_elem << endl;
		cout << l_brzegow << endl;
		cout << l_obszarow << endl;
	}
}

void
IC::_IC(void)
{

	IC_Tokenize();
}

IC::IC(const char *fn) : TokList(fn)
{

	_IC();
}

IC::IC(const string &fn) : TokList(fn.c_str())
{

	_IC();
}
