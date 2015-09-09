#ifndef _PAI_MSH_H_
#define _PAI_MSH_H_

class MSH : public TokList {
	public:
		string opis;
		int l_wymiarow;
		int l_wezlow;
		int l_elem;
		int l_brzegow;
		int l_obszarow;
	
		double **mg;
		double **gmpk;

		struct wezel *wezly; /* wielkosci l_wezlow */
		struct koneksja *koneksje; /* liczba brzegow */

		struct brzeg *brzegi;	/* 
					 * zalezne od liczby brzegow 
					 * l_brzegow
					 */
	
		struct obszar *obszary; /*
					 * l_obszarow
					 */
		void MSH_Tokenize(void);
		void _MSH(void);
		MSH() {}
		~MSH() {}
		MSH(const string &fn);
		MSH(const char *fn);

		/* Zwraca wêze³ na podstawie jego indeksu */
		struct wezel *
		Wezel(int idx);

		/* 
		 * Tymczasowa funkcja zdolna do wykonywania obliczeñ na
		 * wcze¶niej wszytanych danych. To prawdopodobnie bêdzie
		 * zale¿eæ od rodzaju schematu obliczeñ.
		 */
		void
		Process(void);

		/*
		 * Zapis wyniku do pliku.
		 */
		int
		MTXWrite(const char *fname);
};

#endif /* _PAI_MSH_H_ */
