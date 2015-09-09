#ifndef _PAI_TOK_H_
#define _PAI_TOK_H_

/*
 * Rodzaj tokenu wczytanego z wej¶cia.
 */
typedef enum {
	TK_UNDEFINED = -1,	/* ci±g niezdefiniowany */
	TK_WS = 0,	/* zwyk³y odstêp */
	TK_NL = 1,	/* nowa linia */
	TK_INT = 2,	/* warto¶æ ca³kowita */
	TK_FP = 3,	/* warto¶æ zmiennoprzecinkowa */
	TK_STRING = 4,	/* ci±g znaków */
	TK_EOI = 5
} tok_type;

const string& _TokType2Name(const tok_type tt);

/*
 * Pojedynczy token wej¶cia.
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
