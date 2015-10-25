#include <iostream>
#include <fstream>
#include <iterator>
#include <cassert>
#include <list>
#include <string.h>
#include <stdlib.h>

using namespace std;

#include "pai_types.h"
#include "pai_tok.h"
#include "pai_toklist.h"

extern int verbose;
extern int ctx_lines;

/*
 * Tworzy listę tokenów na podstawie zawartości pliku, którego nazwa
 * przekazana jest jako parametr.
 */
void
TokList::_TokList(const char *fn)
{
	ifstream fin;
	string e;
	string line;
	string ws, word;
	Tok *tk;
	string *sptr[2];
	string *sp;
	int i;
	int lineno;
	int pos;

	pos = -1;
	i = lineno = -1;
	sp = NULL;
	tk = NULL;

	fin.open(fn);
	if (fin.is_open() != 1) {
		e = "Nie mogę otworzyć pliku: ";
		e += fn;
		throw (e);
	}
	fname = fn;

	lineno = 1;
	while (getline(fin, line)) {
		/*
		 * Potrzebuję również rozróżnienia, kiedy i gdzie
		 * występuje znak nowej linii. Najłatwiejszym sposobem
		 * jest ponowne dodanie znaku '\n' na koniec 'line',
		 * gdyż został on usunięty w funkcji getline().
		 */
		line += '\n';
		pos = 0;

		for (string::const_iterator si = line.begin();
		    si != line.end(); ) {

			/*
			 * Zależy mi jedynie na podziale: tekst
			 * składający się z "białych znaków" bądź
			 * innych.
			 */
			word = "";
			ws = "";
			while (isspace(*si) && si != line.end()) {
				ws += *si;
				si++;
				pos++;
			}
			while (!isspace(*si) && si != line.end()) {
				word += *si;
				si++;
				pos++;
			}
	
			/*
			 * Dla obu ciągów stosuję te same kryteria: gdy
			 * długość danego ciągu jest większa od 0, to na
			 * podstawie ciągu tworzymy token. Stąd dokonuję
			 * tych samych testów na dwóch obiektach klasy
			 * string.
			 */
			sptr[0] = &ws;
			sptr[1] = &word;
			for (i = 0; i < 2; i++) {
				sp = sptr[i];
				if (sp->length() != 0) {
					tk = new Tok(*sp, lineno,
					    pos - sp->length());
					assert(tk != NULL && "can't allocate space token");
					/*
					 * Lista tokenów.
					 */
					tklist.push_back(tk);
				}
			}
		}
		lineno++;
	}
	tk = new Tok(TK_EOI);
	assert(tk != NULL);
	tklist.push_back(tk);

	curr_tkit = tklist.begin();
	curr_tok = *curr_tkit;
}

/*
 * Konstruktory i wypisywanie listy (debugging)
 */
TokList::TokList(const char *fn)
{

	_TokList(fn);
}

TokList::TokList(const string &fn)
{

	_TokList(fn.c_str());
}

void
TokList::TokListPrint(void) const
{

	for (list<Tok *>::const_iterator ti = tklist.begin(); ti !=
	    tklist.end(); ti++) {
		const Tok *tk = *ti;
		cout << "--" << endl;
		tk->TkPrint();
		cout << "========" << endl;
	}
}

int
TokList::_GetLine(int ln, string &s)
{
	list <Tok *>::const_iterator it;
	Tok *j;
	int ok = 0;

	if (ln <= 0)
		return (0);

	/* 
	 * Przejdź do interesującej nas linii.
	 */
	for (it = tklist.begin(); it != tklist.end(); it++) {
		j = *it;
		assert(j != NULL);
		if (j->TkLineNo() == ln) {
			ok = 1;
			break;
		}
	}
	if (ok == 0)
		return (0);
	assert(j != NULL);

	/*
	 * Odtwórz linię poprzez pobranie wszystkich wartości jej
	 * tokenów.
	 */
	s = "";
	while (j->TkLineNo() == ln) {
		/* WK: pamiętaj, że konstruktor robi tk_lineno = -1 */
		if (j->isNewLine())
			s += " ";
		else
			s += j->TkData();
		it++;
		j = *it;
	}

	if (s.length() > 0)
		return (1);
	return (0);
}

/*
 * Na podstawie tokeka odtwórz linię, z której pochodzi.
 */
int
TokList::_Tok2Line(const Tok *tk, string &s)
{

	assert(tk != NULL);

	return (_GetLine(tk->TkLineNo(), s));
}

/*
 * To doprawy beznadziejne, że muszę kodować tę konwersję ręcznie.
 * Powinienem móc zrobić:
 *
 * 	double pi = 3.14;
 * 	string a = string(pi);
 *
 * Strumienie wewnętrzne to jednak nie to..
 */
void
Int2String(int i, string *s)
{
	int d;
	string &ret = *s;

	cout << "XXXXXXXXXX" << i<< endl;

	ret = "";
	if (i == 0)
		ret = "0";
	while (i > 0) {
		d = i % 10;
		ret = ((char)('0' + d)) + ret;
		i /= 10;
	}
}

/*
 * Na podstawie tokena, stwórz informację o błędzie.
 */
string&
TokList::TokErrStr(const Tok *tk, const string &msg)
{
	string e;
	string l;
	//string **lines;
	string num;
	int bl, el, i, k, sz, j, f;
	char buf[512];
	pair <string, string> *lines;

	if (0)
		TokListPrint();

	e += "==== Error: ";
	e += "File: " + fname;
	Int2String(tk->TkLineNo(), &num);
	e += ", line " + num;
	Int2String(tk->TkPos(), &num);
	e += ", pos " + num;
	Int2String(tk->TkData().length(), &num);
	e += ", length " + num;
	e += ":\n\n";
	e += "[...]\n";

	sz = 2 * ctx_lines + 1; /* ile alokuje */
	lines = new (nothrow) pair<string, string>[sz];
	assert(lines != NULL);
	bl = tk->TkLineNo() - ctx_lines; /* od której */
	el = tk->TkLineNo() + ctx_lines; /* do której */
	f = 0; /* ile faktycznie się nadaje */
	for (i = bl; i <= el; i++) {
		if (!_GetLine(i, l))
			continue;
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%*.d", 5, i);
		lines[f].first = buf;
		lines[f].second = l + "\n";
		if (i == tk->TkLineNo())
			/* w tej linii jest błąd */
			k = f;
		f++;
	}
	for (j = 0; j < f; j++) {
		e += "\t" + lines[j].first + ": " + lines[j].second;
		if (j == k) {
			/* Sprawdzić, czy się nie rozjeżdża. */
			e += ">-------"; 
			i = lines[j].first.length();
			i += tk->TkPos();
			i += 1; /* dla ':' */
			for (; i >= 0; i--)
				e += '-';
			i = tk->TkData().length();
			while (i--)
				e += '^';
			e += '\n';
		}
	}
	e += "\n[...]\n";
	e += msg + "\n";
	e += "===================================\n";
	return (*(new string(e)));
}


/*
 * WK:
 *
 * Każda z Expect* powinna wyrzucać wyjątek.
 *
 * Expect() powinna mieć argument powodujący zaniechanie wyrzucania
 * wyjątku.
 */
string&
TokList::Expect(const tok_type &et, const string &eval = "")
{	
	string e = "<Error Undefined>";
	int m = 0;
	int error = 0;

	Tok *t = TokGet();
	string &tval = t->TkData();
	tok_type tt = t->TkType();

	if (eval != "")
		m = (tval != eval);

	if (et == TK_INT && tt == TK_FP) {
		/*
		 * Wartość zmiennoprzecinkowa może być uznana z wartość
		 * całkowitą tylko wtedy, gdy nie występuje część
		 * ułamkowa.
		 */
		if (tval.find('.') != string::npos)
			error = 1;
	} else
		error = (et != t->TkType());


	/* Niezgodność typu lub wartości? */
	if (error || m) {
		e = "Expected object of type ";
		e += _TokType2Name(et);
		if (m)
			e += " == '" + eval + "'";
		e += " but got object of type ";
		e += t->TkTypeName();
		e += " == ";
		if (tt == TK_NL)
			e += "'\\n'";
		else
			e += "'" + t->TkData() + "'";
		throw (TokErrStr(t, e));
	}
	return (tval);
}

void
TokList::ExpectUndef(void)
{

	Expect(TK_UNDEFINED);
}

string&
TokList::ExpectString(const string &str)
{
	unsigned i;
	string tmp = "";

	/* 
	 * Zgodnie z wymaganiami, nie potrzebuję rozróżniać, czy
	 * ciąg będzie składał się z wielkich czy małych liter.
	 */
	for (i = 0; i < tmp.length(); i++)
		tmp[i] = tolower(tmp[i]);

	return (Expect(TK_STRING, tmp));
}

int
TokList::ExpectInt(void)
{
	string &j = Expect(TK_INT);

	return (atoi(j.c_str()));
}

double
TokList::ExpectFp(void)
{
	string &j = Expect(TK_FP);

	return (atof(j.c_str()));
}

int
TokList::ExpectNl(void)
{

	Expect(TK_NL);
	return (1);
}

string&
TokList::ExpectWs(void)
{

	return (Expect(TK_WS));
}

void
TokList::ExpectEOI(void)
{

	Expect(TK_EOI);
}

void
TokList::ExpectSection(const string &s)
{
	/*
	 * Żeby to zrobić dobrze to resztę trzeba będzie trochę
	 * przerobić na GetTok2(), GetTok3() i sklejanie tokenów.
	 */
	
#if 0
	string name = "[" + s + "]";
	Tok *b, *ws, *e;

	b = ws = e = NULL;

	b =	*(curr_tkit + 0);
	ws =	*(curr_tkit + 1);
	e =	*(curr_tkit + 2);


	b = ExpectString();
	ws = ExpectWs();
	e = ExpectString();

	b += ws + e;
	if (b != name)
#endif
}

/*
 * Pomiń wszystkie tokeny o typie t0 i t1, jeżeli jest wyspecyfikowane.
 */
void
TokList::TokSkip2(const tok_type &t0, const tok_type &t1 = TK_UNDEFINED)
{
	Tok *tk;
	int k = 0;
	
	tk = NULL;

	do {
		k = 0;
		tk = TokCurr();
		if (tk->TkType() == t0) {
			if (verbose) {
				cout << "--" << endl;
				cout << "Removing 1:";
				tk->TkPrint();
				cout << "--" << endl;
			}
			k = 1;
		}
		if (t1 != TK_UNDEFINED && tk->TkType() == t1) {
			if (verbose) {
				cout << "--" << endl;
				cout << "Removing 2:";
				tk->TkPrint();
				cout << "--" << endl;
			}
			k = 2;
		}
		if (k == 0)
			break;
		/* Ustaw wewnętrzny wskaźnik na kolejny token */
		(void)TokGet();
	} while (1);
}


void
TokList::TokSkipTill(const tok_type &t)
{
	Tok *tk;

	tk = NULL;

	tk = TokCurr();
	do {
		if (tk->TkType() == t)
			break;
		(void)TokGet();
		tk = TokCurr();
	} while (tk != NULL);
}

void
TokList::TokSkip1(const tok_type &t0)
{

	TokSkip2(t0);
}
