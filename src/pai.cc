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
 * Błędy:
 * - nie zachowałem plików z błędnymi danymi, dzięki którym wyłapywałem
 *   błędy w razie procesu testowania.
 */

/*
 * Zmienne globalne:
 * 
 * verbose:	poziom logowania
 * ctx_lines:	ilość linii wyświetlanych w konekście błędu
 *
 */
int verbose = 0;
int ctx_lines =  1;

/* -----------------------------------------------------------------
 * Sposób obsługi plików wejściowych jest analogiczny do sytuacji
 * spotykanej w części typu front-end kompilatora:
 * 
 * (1) analiza leksykalna
 * (2) analiza semantyczna
 *
 * W trakcie analizy leksykalnej wczytując plik wejściowy wyróżniamy
 * poszczególne fragmenty tekstu, które z punktu widzenia późniejszej
 * analizy są dla nas interesujące. Fragmenty te, dalej zwane tokenami,
 * posiadają swoją wartość, typ oraz przynależą do konkretnej linii
 * tekstu, z której zostały stworzone. Ostatnia informacja jest podstawą
 * do procesu odtworzenia linii pliku wejściowego z której dany token
 * pochodzi; funkcjonalność ta jest przydatna przy generacji komunikatów
 * błędów w przypadku niepoprawnej struktury pliku wejściowego.
 *
 * Token (class Tok) po rozpoznaniu dodawany jest do listy tokenów
 * (class TokList) w kolejności pojawienia się w pliku wejściowym. Owa
 * lista prócz tokenów stworzonych w oparciu o dane wejściowe, zawiera
 * jeden, syntetyczny token sygnalizujący koniec listy i służący do
 * zaznaczenia końca danych wejściowych.
 *
 * Rozróżniamy tokeny następujących typów:
 * 
 * - TK_UNDEFINED (wewnętrzny, stosowany w tej implementacji)
 * - TK_WS	- token zawierający jedynie białe znaki
 * - TK_NL	- token nowej linii
 * - TK_INT	- token z liczbą całkowitą
 * - TK_FP	- token z liczbą zmiennoprzecinkową
 *
 * Token TK_INT jest syntetycznym tworem tworzonym na bazie tokenu o
 * rodzaju TK_FP, gdy ten posiada jedynie wyłącznie część całkowitą
 * (wyłącznie zera po znaku separatora '.').
 *
 * Analiza semantyczna polega na sprawdzeniu, czy tokeny obecne na
 * liście pojawiły się w pliku wejściowym z odpowiednimi wartościami i
 * należytą kolejnością. Funkcje Expect*() sprawdzają obecność
 * spodziewanego tokena. Zwracają jego wartość w przypadku powodzenia.
 * Wywołują wyjątek w przypadku błędu, generując przy tym stosowny
 * komunikat.
 */

/*
 * Konwertuje rodzaj tokenu do postaci możliwej do wypisania.
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
 * Jak używać tego programu.
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
 * Program główny.
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
	 * Przeglądanie flag i argumentów.
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
	 * Sprawdzam poprawność przekazanych argumentów.
	 */
	if (prefix.length() > 0 && ac > 0) {
		cerr << "Podałeś redundantne parametry: '";
		for (i = 0; i < ac - 1; i++)
			cout << av[i] << " ";
		cout << av[ac - 1] << "'" << endl;
		usage(argv[0]);
		exit(EX_USAGE);
	}
	if (mode != 'F' && mode != 'B') {
		cerr << "Zły tryb" << endl;
		usage(argv[0]);
	}

	/*
	 * Dbam o założenia projektu dot. flag przekazywanych do
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
	 * Właściwe przetwarzanie plików rozpoczyna się tutaj.
	 */
	try {
		/*
		 * Wczytywanie plików. Każdy obiekt osobnej klasy
		 * reprezentuje osobny plik.
		 */
		tkl = new MSH(fn_msh.c_str());
		bcf = new BC(fn_bc.c_str());
		m = new M(fn_m.c_str());
		ic = new IC(fn_ic.c_str());
	} catch (const string &e) {
		/*
		 * Wystąpił wyjątek podczas przetwarzania pliku.
		 * Prawdopodobnie to błąd dot. formatu wczytywanych
		 * plików.
		 */
		cerr << e << endl;
	} catch (...) {
		/* 
		 * Tutaj możemy zjawić się, gdy wystąpił inny typ błędu.
		 * Musimy być przygotowani również na jego obsłużenie.
		 */
		cerr << "Internal error" << endl;
	}

	/*
	 * Dla pewności sprawdzamy, czy każdy z obiektów coś
	 * reprezentuje.
	 */
	assert(tkl != NULL);
	assert(bcf != NULL);
	assert(m != NULL);
	assert(ic != NULL);

	/*
	 * Właściwe przetwarzanie rozpoczyna się tutaj.
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
