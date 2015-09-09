#ifndef _PAI_TOKLIST_H_
#define _PAI_TOKLIST_H_

/* -------------------------------------------------------------
 * Szczególne przypadki wczytywania--pliki z danymi
 *
 * Ka¿dy typ pliku przyporz±dkowany jest do okre¶lonej klasy:
 *
 * .msh		class MSH
 * .bc		class BC
 *
 * Ka¿da z klas jest pochodn± klasy TokList. Tworz±c obiekt danej klasy
 * automatycznie tworzona jest lista tokenów z danego pliku. Przekazanie
 * nazwy pliku odbywa siê poprzez przekazanie jej jako argumentu
 * konstruktora. Nastêpnie, wo³ana jest metoda specyficzna dla ka¿dej z
 * klas--a co za tym idzie, poszczególnego pliku.
 *
 * WK: Rozwa¿yæ wykorzystanie klasy abstrakcyjnej TokList z metod±
 *     wirtualn± ReadData.
 */

/*
 * Lista z tokenami wej¶ciowymi.
 */
class TokList {
	public:
		list <Tok *> tklist;
		void _TokList(const char *fn);
		int _Tok2Line(const Tok *, string &s);
		string &TokErrStr(const Tok *, const string &);
		string	fname;
		int _GetLine(int ln, string &s);

		Tok				*curr_tok;
		list<Tok *>::const_iterator	 curr_tkit;



		TokList() {}
		TokList(const char *fn);
		TokList(const string &fn);
		void TokListPrint(void) const;
		
		string& Expect(const tok_type &tt,
		    const string &eval);
		
		string& ExpectString(const string &);
		int ExpectInt(void);
		double ExpectFp(void);
		int ExpectNl(void);
		string& ExpectWs(void);
		void ExpectSection(const string &s);
		void ExpectUndef(void);
		void ExpectEOI(void);
		void TokSkip1(const tok_type &t1);
		void TokSkip2(const tok_type &t1, const tok_type &t2);
		void TokSkipTill(const tok_type &t);

		/*
		 * Wypada³oby pomy¶leæ o czym¶ innym ni¿ TokGet.
		 *
		 * Rozwa¿yæ:
		 *
		 * 	TokGet() -- zwraca obecny token
		 * 	TokNext()-- jedynie przesuwa wska¼nik.
		 *
		 */

		Tok*
		TokGet(void)
		{
			Tok *t = NULL;

			t = curr_tok;
			
			if (curr_tkit == tklist.end())
				return (NULL);

			curr_tkit++;
			curr_tok = *curr_tkit;
			assert(curr_tok != NULL);
			return (t);
		}

		Tok*
		TokCurr(void)
		{
		
			return (curr_tok);
		}
};

#endif /* _PAI_TOKLIST_H_ */
