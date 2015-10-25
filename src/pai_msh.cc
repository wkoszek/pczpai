#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <iomanip>

#include <sysexits.h>
#include <string.h>

using namespace std;

#include "pai_types.h"
#include "pai_tok.h"
#include "pai_toklist.h"

#include "pai_msh.h"

extern int verbose;

void
MSH::MSH_Tokenize(void)
{
	string s;
	int i, j;
	int widx;
	double wx, wy;
	int lw = 0;
	int *bip;
	int o;

	if (verbose)
		TokListPrint();

	i = j = 0;

	/*
	 * <SIATKA>'\n'
	 */
	ExpectString("<SIATKA>");
	TokSkip1(TK_WS);
	ExpectNl();

	TokSkip2(TK_WS, TK_NL);

	/*
	 * [OPIS]'\n'
	 */
	ExpectString("[OPIS]");
	TokSkip2(TK_WS, TK_NL);
	/* Xx: Czytanie opisu do 'opis' */

	/*
	 * [LICZBA WYMIAROW]
	 */
	ExpectString("[LICZBA"); ExpectWs(); ExpectString("WYMIAROW]"); TokSkip1(TK_WS); ExpectNl();

	l_wymiarow = ExpectInt(); TokSkip1(TK_WS); ExpectNl();
	TokSkip2(TK_WS, TK_NL);


	ExpectString("[LICZBA"); ExpectWs(); ExpectString("WEZLOW]"); TokSkip1(TK_WS); ExpectNl();
	l_wezlow = ExpectInt(); TokSkip1(TK_WS); ExpectNl();
	TokSkip2(TK_WS, TK_NL);

	ExpectString("[LICZBA"); ExpectWs(); ExpectString("ELEMENTOW]"); TokSkip1(TK_WS); ExpectNl();
	l_elem = ExpectInt(); TokSkip1(TK_WS); ExpectNl();
	TokSkip2(TK_WS, TK_NL);

	ExpectString("[LICZBA"); ExpectWs(); ExpectString("BRZEGOW]"); TokSkip1(TK_WS); ExpectNl();
	l_brzegow = ExpectInt(); TokSkip1(TK_WS); ExpectNl();
	TokSkip2(TK_WS, TK_NL);

	ExpectString("[LICZBA"); ExpectWs(); ExpectString("OBSZAROW]"); TokSkip1(TK_WS); ExpectNl();
	l_obszarow = ExpectInt(); TokSkip1(TK_WS); ExpectNl();
	TokSkip2(TK_WS, TK_NL);

	ExpectString("[WSPOLRZEDNE"); ExpectWs(); ExpectString("WEZLOW]"); TokSkip1(TK_WS); ExpectNl();

	/* ------------------------------------------------------------ 
	 * Alokacja pamięci do przechowywania danych.
	 */
	wezly = new (nothrow) wezel[l_wezlow];
	assert(wezly != NULL && "nie moge zaalokowac pamieci na wezly");

	koneksje = new (nothrow) koneksja[l_elem];
	assert(koneksje != NULL && "nie mogę zaalokować pamięci na"
	    " koneksje");

	brzegi = new (nothrow) brzeg[l_brzegow];
	assert(koneksje != NULL && "nie mogę zaalokować pamięci na"
	    " brzegi");
	
	/*
	 * WK: Wychwycić błędy z dziwnymi indeksami:
	 *     - czy indeksy mają być po kolei?
	 *     - co zrobić, gdy podano 2 wiersze z tym samym indeksem:
	 *       - pewnie nadpisać stary wiersz
	 */
	for (j = 0; j < l_wezlow; j++) {
		widx = ExpectInt();
		ExpectWs();

		wx = ExpectFp();
		ExpectWs();

		wy = ExpectFp();
		ExpectNl();

		assert(widx >= 1 && widx <= l_wezlow);
		wezly[j].w_idx = widx;
		wezly[j].w_x = wx;
		wezly[j].w_y = wy;
	}
	ExpectNl();
	ExpectString("[KONEKSJE]");
	ExpectNl();

	for (j = 0; j < l_elem; j++) {
		koneksje[j].k_idx = ExpectInt();
		ExpectWs();
		ExpectString("2D31");
		ExpectWs();
		for (i = 0; i < 3; i++) {
			koneksje[j].k_wzl[i] = ExpectInt();
			ExpectWs();
		}
		/*
		 * Powinienem móc wymusić na ExpectInt() możliwość
		 * oczekiwania konkretnej wartości.
		 */
		i = ExpectInt();
		assert(i == 1 && "Zły plik z danymi");
		ExpectNl();
	}
	ExpectNl();
	ExpectString("[BRZEGI]");
	ExpectNl();
	

	for (j = 0; j < l_brzegow; j++) {
		brzegi[j].b_idx = ExpectInt();
		ExpectWs();
		brzegi[j].b_liczba_wezlow = lw = ExpectInt();
		ExpectWs();
		brzegi[j].b_numer_obszaru = o = ExpectInt();
		assert(o == 1);
		ExpectNl();

		brzegi[j].b_idxs = bip = new int[lw];
		assert(brzegi[j].b_idxs != NULL);

		for (i = 0; i < lw; i++) {
			bip[i] = ExpectInt();
			if (i != lw - 1)
				ExpectWs();
		}
		ExpectNl();
		ExpectNl();
	}
	ExpectNl();
	ExpectString("[OBSZARY]");
	ExpectNl();
	i = ExpectInt();
	assert(i == 1);
	ExpectWs();
	j = ExpectInt();
	ExpectNl();

	for (i = 0; i < j; i++) {
		ExpectInt();
		if (i != j - 1)
			ExpectWs();
	}
	ExpectNl();
	TokSkip2(TK_WS, TK_NL);
	ExpectEOI();
	
	if (verbose) {
		cout << "--------------------------" << endl;
		cout << "Wypisuję dane z pliku .msh" << endl;
		cout << l_wymiarow << endl;
		cout << l_wezlow << endl;
		cout << l_elem << endl;
		cout << l_brzegow << endl;
		cout << l_obszarow << endl;
	}
}

void
MSH::_MSH(void)
{

	MSH_Tokenize();
}

MSH::MSH(const string &fn) : TokList(fn)
{

	_MSH();
}
	
MSH::MSH(const char *fn) : TokList(fn)
{

	_MSH();
}

/*
 * Zamiast w tablicy, węzły powinny być trzymane w wektorze, albo mapie.
 */
wezel *
MSH::Wezel(int idx)
{
	int i;

	for (i = 0; i < l_wezlow; i++)
		if (wezly[i].w_idx == idx)
			return (&wezly[i]);

	return (NULL);
}


void
MSH::Process(void)
{
	int i;
#if 0
	struct matrix *t;
#endif
	double x1, y1;
	double x2, y2;
	double x3, y3;
	double a;
	double one_div_4a;
	double c1, c2, c3;
	int i1, i2, i3;
	wezel *w1, *w2, *w3;
	double c21, c22, c23, c31, c32, c33;
	double ml[3][3];
	double mp[3][3];

	mg = NULL;
	gmpk = NULL;

	int r,c;

	memset(ml, 0, sizeof(ml));
	memset(mp, 0, sizeof(mp));

	i1 = i2 = i3 = 0;
	w1 = w2 = w3 = NULL;

#if 0
	cout << "L_elem: " << l_elem << endl;
	t = new matrix[l_elem];
	assert(t != NULL && "t==NULL");
#endif

	if (verbose) {
		cout << "----------------------" << endl;
		cout << " Wykonanie" << endl;


		for (i = 0; i < l_wezlow; i++) {
			cout << "-- " << endl;
			cout << "Index:\t" << wezly[i].w_idx << endl;
			cout << "    X:\t" << wezly[i].w_x << endl;
			cout << "    Y:\t" << wezly[i].w_y << endl;
		}
	}

	mg = new double* [l_wezlow];
	assert(mg != NULL);
	memset(mg, 0, sizeof(double *) * l_wezlow);
	for (i = 0; i < l_wezlow; i++) {
		mg[i] = new double[l_wezlow];
		assert(mg[i] != NULL);
		memset(mg[i], 0, sizeof(double) * l_wezlow);
	}
	
	gmpk = new double* [l_wezlow];
	assert(gmpk != NULL);
	memset(gmpk, 0, sizeof(double *) * l_wezlow);
	for (i = 0; i < l_wezlow; i++) {
		gmpk[i] = new double[l_wezlow];
		assert(gmpk[i] != NULL);
		memset(gmpk[i], 0, sizeof(double) * l_wezlow);
	}

	for (i = 0; i < l_elem; i++) {
		/* indeksy wezłów */
		i1 = koneksje[i].k_wzl[0];
		i2 = koneksje[i].k_wzl[1];
		i3 = koneksje[i].k_wzl[2];

		/* stuktury danych węzłów */
		w1 = Wezel(i1);
		assert(w1 != NULL && "nie moge znalezc 1 wezla");
		w2 = Wezel(i2);
		assert(w2 != NULL && "nie moge znalezc 2 wezla");
		w3 = Wezel(i3);
		assert(w3 != NULL && "nie moge znalezc 3 wezla");

		/* tutaj już indeksy macierzy globalnej */
		i1 -= 1;
		i2 -= 1;
		i3 -= 1;

		assert(i1 >= 0 && i1 < l_wezlow);
		assert(i2 >= 0 && i2 < l_wezlow);
		assert(i3 >= 0 && i3 < l_wezlow);

		/* potrzebne do wzoru */
		x1 = w1->w_x;
		y1 = w1->w_y;
		x2 = w2->w_x;
		y2 = w2->w_y;
		x3 = w3->w_x;
		y3 = w3->w_y;

		/* czynniki */
		c1 = x1 * (y2 - y3);
		c2 = x2 * (y3 - y1);
		c3 = x3 * (y1 - y2);

		/* pole */
		a = 0.5 * (c1 + c2 + c3);

		/* WK: macierz lokalna -- do uproszczenia */
		/* i1                   i2                      i3		*/
		ml[0][0]							= 
					ml[1][1]				=
								ml[2][2]	= a/3;

		mg[i1][i1] += ml[0][0];
		mg[i2][i2] += ml[1][1];
		mg[i3][i3] += ml[2][2];


		c21 = y2 - y3;
		c22 = y3 - y1;
		c23 = y1 - y2;
		c31 = x3 - x2;
		c32 = x1 - x3;
		c33 = x2 - x1;

		/*
		 * Metodą Kopiego-Pasta. WK: Też można zoptymalizować.
		 */
		mp[0][0] = (c21 * c21) + (c31 * c31);
		mp[0][1] = (c21 * c22) + (c31 * c32);
		mp[0][2] = (c21 * c23) + (c31 * c33);

		mp[1][0] = (c21 * c22) + (c31 * c32);
		mp[1][1] = (c22 * c22) + (c32 * c32);
		mp[1][2] = (c22 * c23) + (c32 * c33);

		mp[2][0] = (c21 * c23) + (c31 * c33);
		mp[2][1] = (c22 * c23) + (c32 * c33);
		mp[2][2] = (c23 * c23) + (c33 * c33);

		one_div_4a = (1 / (4 * a));
		for (r = 0; r < 3; r++)
			for (c = 0; c < 3; c++)
				mp[r][c] *= one_div_4a;


		/* --------------------------------- */
		int tidx[3];
		int tr, tc;

		/* translacja indeksów lokalnych na globalne */
		tidx[0] = i1;
		tidx[1] = i2;
		tidx[2] = i3;
	
		if (verbose)
			cout << "Wezel: " << i << endl;
		for (r = 0; r < 3; r++) {
			tr = tidx[r];
			for (c = 0; c < 3; c++) {
				tc = tidx[c];
				if (verbose) {
					cout << "Element [" << r << "," << c;
					cout << "\t wpiszę do G[" << tr << ",";
					cout << tc << "]" << endl;
				}
				gmpk[tr][tc] += mp[r][c];
			}
		}
	}

	cout.setf(ios::fixed, ios::floatfield);
	cout.precision(10);

	cout << "K:" << endl;
	for (r = 0; r < l_wezlow; r++) {
		for (c = 0; c < l_wezlow; c++) {
			cout << "\t" << gmpk[r][c];
		}
		cout << endl;
	}

	cout << "M:" << endl;
	/* tylko po przekątnej */
	for (r = 0; r < l_wezlow; r++)
		cout << "\t" << mg[r][r];
	cout << endl;
}

int
MSH::MTXWrite(const char *path)
{
	ofstream of;
	int r, c;

	of.open(path);
	if (of.is_open() != 1)
		return (-1);
	
	of << "K:" << endl;
	for (r = 0; r < l_wezlow; r++) {
		for (c = 0; c < l_wezlow; c++)
			of << "\t" << gmpk[r][c];
		of << endl;
	}

	of << "M:" << endl;
	/* tylko po przekątnej */
	for (r = 0; r < l_wezlow; r++)
		of << "\t" << mg[r][r];
	of << endl;
	of.close();
	return (0);
}
