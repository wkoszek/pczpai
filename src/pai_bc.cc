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

#include "pai_bc.h"

extern int verbose;

void
BC::BC_Tokenize(void)
{
	int lb = 0;

	if (verbose)
		TokListPrint();

	/*
	 * <WARUNKI BRZEGOWE>
	 */
	ExpectString("<WARUNKI");
	ExpectWs();
	ExpectString("BRZEGOWE>");

	TokSkipTill(TK_NL);
#if 0
	TokSkip2(TK_STRING, TK_WS);
#endif
	ExpectNl();
	ExpectNl();

	/* Nagłówek */
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

	/* Powtórzona liczba brzegow */
	ExpectNl();
	lb = ExpectInt();
	ExpectNl();
	ExpectNl();

	assert(lb == l_brzegow && "zła powtórzona liczba brzegow");
	assert(lb >= 0 && "ujemna liczba brzegow");

	warunki = new (nothrow) warunek[l_brzegow];
	assert(warunki != NULL && "nie moge zaalokowac warunkow");

	int i;
	int _nr_brzegu;
	int _nr_obszaru;
	int _l_wezlow;
	int wb;

	if (verbose) {
		cerr << "-------------------------" << endl;
		cerr << "Wczytywanie brzegow z BC" << endl;
	}

	for (i = 0; i < l_brzegow; i++) {
		/* pierwsza linia warunku */
		_nr_brzegu = ExpectInt();
		ExpectWs();

		_nr_obszaru = ExpectInt();
		ExpectWs();

		_l_wezlow = ExpectInt();
		ExpectWs();

		wb = ExpectInt();
		ExpectNl();

		_nr_brzegu += 1;
		_nr_obszaru += 1;
		/* _l_wezlow jest niepomniejszona */

		/* 
		 * XXX: WK: Tutaj wstawiamy przypisanie wskaznika w strukturze
		 * warunek do wskaznika brzegu z MSH
		 */

		/* 
		 * Nie ma potrzeby wczytywania warunkow brzegowych--przechodzimy do
		 * kolejnej iterakcji
		 */
		if (wb == 0) {
			ExpectNl();
			continue;
		}

		int _l_wezlow2;
		int _id_war;
		int _jak_def;
		int _l_param;

		_l_wezlow2 = ExpectInt();
		ExpectWs();
		_id_war = ExpectInt();
		ExpectWs();
		_jak_def = ExpectInt();
		ExpectWs();
		_l_param = ExpectInt();
		ExpectNl();
		/*
		 * Sa parametry, czyli oczekujemy odstepu i kolejnych danych
		 * dot. parametrow definiujacych.
		 */
		ExpectNl();

		if (_id_war == 3)
			assert(_l_param == 2 && "zla liczba parametrow a");
		else if (_id_war == 2)
			assert(_l_param == 1 && "zla liczba parametrow b");
		else
			assert(_l_param == 1 || _l_param == 2 && "Musi byc 1 albo 2");

		struct param_def *pds = NULL;
		struct param_def *pd = NULL;
		int j;

		warunki[i].w_param = pds = new param_def[_l_param];
		assert(pds != NULL && "nie mam pamieci na warunki");

		for (j = 0; j < _l_param; j++) {
			pd = &pds[j];
			assert(pd != NULL && "pusty warunek");
			pd->p_id = ExpectInt();
			ExpectWs();
			pd->_p_jeden = ExpectInt();
			ExpectWs();
			pd->_p_zero = ExpectInt();
			ExpectNl();
			pd->p_val = ExpectFp();
			ExpectNl();
			if (j == _l_param - 1)
				ExpectNl();
		}
	}


	if (verbose) {
		cout << "---------------------------" << endl;
		cout << "Wypisuję dane dot. pliku BC" << endl;
		cout << l_wymiarow << endl;
		cout << l_wezlow << endl;
		cout << l_elem << endl;
		cout << l_brzegow << endl;
		cout << l_obszarow << endl;
	}
}

void
BC::_BC(void)
{

	BC_Tokenize();
}

BC::BC(const char *fn) : TokList(fn)
{

	_BC();
}

BC::BC(const string &fn) : TokList(fn.c_str())
{

	_BC();
}
