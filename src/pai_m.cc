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

#include "pai_m.h"

#if 0
#include "pai_bc.h"


<WLASNOSCI MATERIALOWE>   (wygenerowany plik - NIE EDYTOWAC)

HeatTransfer_1.0

2 9 8 1 4

101
3
101 0
6.999000e+003

102 0
4.001000e+003

103 0
1.000000e+002


#endif

extern int verbose;

void
M::M_Tokenize(void)
{
	int i;

	if (verbose)
		TokListPrint();

	/*
	 * <WARUNKI BRZEGOWE>
	 */
	ExpectString("<WARUNKI");
	ExpectWs();
	ExpectString("MATERIALOWE>");

	TokSkipTill(TK_NL);
#if 0
	TokSkip2(TK_STRING, TK_WS);
#endif

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

	i = ExpectInt(); ExpectNl();

	i = ExpectInt(); ExpectWs();

	i = ExpectInt(); ExpectNl();

	(void)ExpectFp();

	if (verbose) {
		cout << "---------------------------" << endl;
		cout << "Wypisujê dane dot. pliku BC" << endl;
		cout << l_wymiarow << endl;
		cout << l_wezlow << endl;
		cout << l_elem << endl;
		cout << l_brzegow << endl;
		cout << l_obszarow << endl;
	}
}

void
M::_M(void)
{

	M_Tokenize();
}

M::M(const char *fn) : TokList(fn)
{

	_M();
}

M::M(const string &fn) : TokList(fn.c_str())
{

	_M();
}
