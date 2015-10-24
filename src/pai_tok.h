#ifndef _PAI_TOK_H_
#define _PAI_TOK_H_

/*
 * Rodzaj tokenu wczytanego z wejścia.
 */
typedef enum {
	TK_UNDEFINED = -1,	/* ciąg niezdefiniowany */
	TK_WS = 0,	/* zwykły odstęp */
	TK_NL = 1,	/* nowa linia */
	TK_INT = 2,	/* wartość całkowita */
	TK_FP = 3,	/* wartość zmiennoprzecinkowa */
	TK_STRING = 4,	/* ciąg znaków */
	TK_EOI = 5
} tok_type;

const string& _TokType2Name(const tok_type tt);

/*
 * Pojedynczy token wejścia.
 */
class Tok {
	private:
		tok_type	tk_type;
		int		tk_lineno;
		string		tk_data;
		int		tk_pos;

	public:
		Tok(const tok_type tt);
		Tok(const string &tk, int lineno, int tpos);
		const tok_type& TkType(void) const;
		const string& TkData(void) const;
		tok_type& TkType(void);
		string& TkData(void);
		const int& TkLineNo(void) const;
		const int& TkPos(void) const;
		const string& TkTypeName(void) const;
		void TkPrint(void) const;
		int isString(void) const;
		int isNewLine(void) const;
		int isFp(void) const;
		int isInt(void) const;
		int isWhiteSpace(void) const;
};

#endif /* _PAI_TOK_H_ */
