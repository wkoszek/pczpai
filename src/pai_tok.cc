#include <iostream>
#include <fstream>
#include <list>
#include <iterator>
#include <string>

using namespace std;

#include "pai_tok.h"

Tok::Tok(const tok_type tt) :
	tk_type(tt), tk_lineno(-1), tk_data("<INTERNAL>"), tk_pos(-1)
{

}


Tok::Tok(const string &tk, int lineno, int tpos)
{
	int ws;
	int dig;
	int dot;
	int alpha;
	int tklen;

	ws = 0;
	dig = 0;
	dot = 0;
	alpha = 0;
	tk_lineno = lineno;
	tk_data = tk;
	tk_type = TK_UNDEFINED;
	tk_pos = tpos;

	do {
		tklen = tk.length();
		/*
		 * Przeglądam znaki należące do tego
		 * tokena i klasyfikuję je do
		 * poszczególnych kategorii.
		 */

		/* może liczba ujemna? */
		string::const_iterator si = tk.begin();
		if (*si == '-')
			dig++;

		while (si != tk.end()) {

			if (isspace(*si))
				ws++;
			/*
			 * W przypadku liczb
			 * zmiennoprzecinkowych w
			 * przekazanym napisie prócz
			 * cyfr, pojawić się może
			 * separator liczbowy.
			 */
			if (isdigit(*si) || *si == '.' || *si == 'e' || *si=='+') {
				dig++;
				if (*si == '.')
					dot++;
			}
			if (isalpha(*si))
				alpha++;
			si++;
		}

		if ((dig == tklen && isdigit(tk_data[0])) ||
		    tk_data[0] == '-') {
			/* 
			 * Pierwszy znak to musi być liczba;
			 * dalej może być kropka.
			 */
			tk_type = TK_FP;

#if 0
			if (dot != 0)
				tk_type = TK_FP;
#endif
		} else if (tklen == ws) {
			string::size_type rp, rnp;
			
			tk_type = TK_WS;

			/*
			 * Każde wystąpienie ciągu
			 * białych znaków zakończonego
			 * białym znakiem '\n'
			 * propagujemy do tyku TK_NL,
			 * oznaczającego nową linię.
			 */
			rp = tk_data.find("\n");
			rnp = tk_data.find("\r\n");

			if (rp != string::npos ||
			    rnp != string::npos) {
				tk_data = '\n';
				tk_type = TK_NL;
			}
		} else {
			unsigned i;

			for (i = 0; i < tk_data.length(); i++) 
				tk_data[i] = tolower(tk_data[i]);
			tk_type = TK_STRING;
		}
	} while (0);
}

/*
 * Metody dostępowe i kilka pomocniczych rzeczy.
 */
const tok_type&
Tok::TkType(void) const
{

	return (tk_type);
}

tok_type&
Tok::TkType(void)
{

	return (tk_type);
}

const string&
Tok::TkData(void) const
{

	return (tk_data);
}

string&
Tok::TkData(void)
{

	return (tk_data);
}

const int&
Tok::TkLineNo(void) const
{

	return (tk_lineno);
}

const int&
Tok::TkPos(void) const
{

	return (tk_pos);
}


const string&
Tok::TkTypeName(void) const
{

	return (_TokType2Name(tk_type));
}

void
Tok::TkPrint(void) const
{

	cout << "Token:\t";
	cout << TkTypeName() << endl;
	cout << "Data:\t'";
	/*
	 * Tutaj powinienem zastępować każde wystąpienie
	 * '\n' na '\n' (literalnie), żeby ładnie
	 * zobaczyć tekst. Na razie niech będzie jak
	 * jest..
	 */
	if (tk_data == "\n" || tk_data == "\r\n")
		cout << "\\n";
	else
		cout << tk_data;
	cout << "'" << endl;
	cout << "Length:\t" << tk_data.length() << endl;
	cout << "Line:\t" << tk_lineno << endl;
}

int
Tok::isString(void) const
{

	return (tk_type == TK_STRING);
}

int
Tok::isNewLine(void) const
{

	return (tk_type == TK_NL);
}

int
Tok::isFp(void) const
{

	return (tk_type == TK_FP);
}

int
Tok::isInt(void) const
{

	return (tk_type == TK_INT);
}

int
Tok::isWhiteSpace(void) const
{

	return (tk_type == TK_WS);
}
