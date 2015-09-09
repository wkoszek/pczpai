#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <sysexits.h>

#include <unistd.h>

using namespace std;

#include "pai_types.h"
#include "pai_tok.h"
#include "pai_toklist.h"

#include "pai_msh.h"
#include "pai_bc.h"
#include "pai_m.h"
#include "pai_ic.h"

/*
 * B��dy:
 * - nie zachowa�em plik�w z b��dnymi danymi, dzi�ki kt�rym wy�apywa�em
 *   b��dy w razie procesu testowania.
 */

/*
 * Zmienne globalne:
 * 
 * verbose:	poziom logowania
 * ctx_lines:	ilo�� linii wy�wietlanych w konek�cie b��du
 *
 */
int verbose = 0;
int ctx_lines =  1;

/* -----------------------------------------------------------------
 * Spos�b obs�ugi plik�w wej�ciowych jest analogiczny do sytuacji
 * spotykanej w cz�ci typu front-end kompilatora:
 * 
 * (1) analiza leksykalna
 * (2) analiza semantyczna
 *
 * W trakcie analizy leksykalnej wczytuj�c plik wej�ciowy wyr�niamy
 * poszczeg�lne fragmenty tekstu, kt�re z punktu widzenia p�niejszej
 * analizy s� dla nas interesuj�ce. Fragmenty te, dalej zwane tokenami,
 * posiadaj� swoj� warto��, typ oraz przynale�� do konkretnej linii
 * tekstu, z kt�rej zosta�y stworzone. Ostatnia informacja jest podstaw�
 * do procesu odtworzenia linii pliku wej�ciowego z kt�rej dany token
 * pochodzi; funkcjonalno�� ta jest przydatna przy generacji komunikat�w
 * b��d�w w przypadku niepoprawnej struktury pliku wej�ciowego.
 *
 * Token (class Tok) po rozpoznaniu dodawany jest do listy token�w
 * (class TokList) w kolejno�ci pojawienia si� w pliku wej�ciowym. Owa
 * lista pr�cz token�w stworzonych w oparciu o dane wej�ciowe, zawiera
 * jeden, syntetyczny token sygnalizuj�cy koniec listy i s�u��cy do
 * zaznaczenia ko�ca danych wej�ciowych.
 *
 * Rozr�niamy tokeny nast�puj�cych typ�w:
 * 
 * - TK_UNDEFINED (wewn�trzny, stosowany w tej implementacji)
 * - TK_WS	- token zawieraj�cy jedynie bia�e znaki
 * - TK_NL	- token nowej linii
 * - TK_INT	- token z liczb� ca�kowit�
 * - TK_FP	- token z liczb� zmiennoprzecinkow�
 *
 * Token TK_INT jest syntetycznym tworem tworzonym na bazie tokenu o
 * rodzaju TK_FP, gdy ten posiada jedynie wy��cznie cz�� ca�kowit�
 * (wy��cznie zera po znaku separatora '.').
 *
 * Analiza semantyczna polega na sprawdzeniu, czy tokeny obecne na
 * li�cie pojawi�y si� w pliku wej�ciowym z odpowiednimi warto�ciami i
 * nale�yt� kolejno�ci�. Funkcje Expect*() sprawdzaj� obecno��
 * spodziewanego tokena. Zwracaj� jego warto�� w przypadku powodzenia.
 * Wywo�uj� wyj�tek w przypadku b��du, generuj�c przy tym stosowny
 * komunikat.
 */

/*
 * Konwertuje rodzaj tokenu do postaci mo�liwej do wypisania.
 */
const string&
_TokType2Name(const tok_type tt = TK_UNDEFINED)
{
	string j;
	string *r = NULL;

	switch (tt) {
	case TK_INT:
		j = "Integer";
		break;
	case TK_FP:
		j = "Float";
		break;
	case TK_STRING:
		j = "String";
		break;
	case TK_NL:
		j = "Newline";
		break;
	case TK_WS:
		j = "Whitespace";
		break;
	case TK_UNDEFINED:
		j = "Undefined";
		break;
	case TK_EOI:
		j = "End of input";
		break;
	}
	r = new string(j);
	assert(r != NULL);
	return (*r);
}

/*
 * Jak u�ywa� tego programu.
 */
static void
usage(const char *progname)
{

	cerr << "Uzycie: " << progname << " [-a <prefix>] [-m <mtx>] " << 
	    "[-o <outfile>] [-E[B|F]] [-v]" << endl;
	cerr << "        " << progname << "<fn>" << endl;
	exit(EX_USAGE);
}

/*
 * Program g��wny.
 */
int
main(int argc, char **argv)
{
	string prefix;
	string fn_mtx;
	string fn_o;
	string fn_msh;
	string fn_bc;
	string fn_ic;
	string fn_m;
	string fn_cp;
	string arg_o, arg_m;

	int mode = 'F';
	int o = -1;
	int arg_v = 0;
	char **av;
	int ac = 0;
	int i;

	/*
	 * Przegl�danie flag i argument�w.
	 */
	while ((o = getopt(argc, argv, "a:c:E:m:o:v")) != -1) {
		switch (o) {
			case 'a':
				prefix = optarg;
				break;
			case 'c':
				ctx_lines = atoi(optarg);
				break;
			case 'E':
				mode = optarg[0];
				break;
			case 'm':
				arg_m = optarg;
				break;
			case 'o':
				arg_o = optarg;
				break;
			case 'v':
				arg_v++;
				break;
			default:
				cerr << "Default" << endl;
				usage(argv[0]);
		}
	}

	verbose = arg_v;

	ac = argc - optind;
	av = argv + optind;

	/*
	 * Sprawdzam poprawno�� przekazanych argument�w.
	 */
	if (prefix.length() > 0 && ac > 0) {
		cerr << "Poda�e� redundantne parametry: '";
		for (i = 0; i < ac - 1; i++)
			cout << av[i] << " ";
		cout << av[ac - 1] << "'" << endl;
		usage(argv[0]);
		exit(EX_USAGE);
	}
	if (mode != 'F' && mode != 'B') {
		cerr << "Z�y tryb" << endl;
		usage(argv[0]);
	}

	/*
	 * Dbam o za�o�enia projektu dot. flag przekazywanych do
	 * programu.
	 */
	if (prefix.length() > 0) {
		fn_msh = prefix + ".msh";
		fn_bc = prefix + ".bc";
		fn_ic = prefix + ".ic";
		fn_m = prefix + ".m";
		fn_cp = prefix + ".cp";
	} else {
		if (argc < 1 + 5)
			usage(argv[0]);

		fn_msh = av[0];
		fn_bc = av[1];
		fn_ic = av[2];
		fn_m = av[3];
		fn_cp = av[4];
	}

	fn_o = fn_msh + ".<nr kroku?>.rlt";
	fn_mtx = fn_msh + ".mtx";

	if (arg_o.length() > 0)
		fn_o = arg_o + ".<nr kroku?>.rtl>";
	if (arg_m.length() > 0)
		fn_mtx = arg_m + ".mtx";

	if (arg_v) {
		cout << "Plik .msh:\t" << fn_msh << endl;
		cout << "Plik .bc:\t" << fn_bc << endl;
		cout << "Plik .ic:\t" << fn_ic << endl;
		cout << "Plik .m:\t" << fn_m << endl;
		cout << "Plik .cp:\t" << fn_cp << endl;
		cout << "Plik .rlt:\t" << fn_o << endl;
		cout << "Plik .mtx:\t" << fn_mtx << endl;
		cout << "Mode:\t\t" << (char) mode << endl;
		cout << "Verbose:\t" << arg_v << endl;
	}

	MSH *tkl = NULL;
	BC *bcf = NULL;
	M *m = NULL;
	IC *ic = NULL;

	/*
	 * W�a�ciwe przetwarzanie plik�w rozpoczyna si� tutaj.
	 */
	try {
		/*
		 * Wczytywanie plik�w. Ka�dy obiekt osobnej klasy
		 * reprezentuje osobny plik.
		 */
		tkl = new MSH(fn_msh.c_str());
		bcf = new BC(fn_bc.c_str());
		m = new M(fn_m.c_str());
		ic = new IC(fn_ic.c_str());
	} catch (const string &e) {
		/*
		 * Wyst�pi� wyj�tek podczas przetwarzania pliku.
		 * Prawdopodobnie to b��d dot. formatu wczytywanych
		 * plik�w.
		 */
		cerr << e << endl;
	} catch (...) {
		/* 
		 * Tutaj mo�emy zjawi� si�, gdy wyst�pi� inny typ b��du.
		 * Musimy by� przygotowani r�wnie� na jego obs�u�enie.
		 */
		cerr << "Internal error" << endl;
	}

	/*
	 * Dla pewno�ci sprawdzamy, czy ka�dy z obiekt�w co�
	 * reprezentuje.
	 */
	assert(tkl != NULL);
	assert(bcf != NULL);
	assert(m != NULL);
	assert(ic != NULL);

	/*
	 * W�a�ciwe przetwarzanie rozpoczyna si� tutaj.
	 */
	if (mode == 'F') {
		tkl->Process();
		try {
			tkl->MTXWrite(fn_mtx.c_str());
		} catch (const string &e) {
			cerr << "Problem z zapisem do pliku: " << e << endl;
		}
	}

	return (0);
}
