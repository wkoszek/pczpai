#ifndef _PAI_TYPES_H_
#define _PAI_TYPES_H_

/* ---------------------------------------------------------------------
 * Wewnętrzne typy danych. Te dane są według mnie wybitnie strukturalne
 * i nie ma potrzeby marnowania dodatkowych linii kodu na konwersję ze
 * zwykłych struktur do klas.
 */
class wezel {
	public:
		int	w_idx;
		double	w_x;
		double	w_y;

		wezel()
		{
		
			w_idx = -1;
			w_x = -1;
			w_y = -1;
		}
};

struct koneksja {
	int	k_idx;
	int	k_wzl[3];
	int	__k_stala; /* zawsze rowna 1 */
};

struct brzeg {
	int	 b_idx; /* zmienny */
	int	 b_liczba_wezlow; /* liczba wezlow na brzegu */
	int	 b_numer_obszaru; /* zawsze "1" */
	int	*b_idxs; /* tablica wielkosci b_liczba_wezlow */
};


struct param_def {
	int	p_id;
	int	_p_jeden;
	int	_p_zero;
	double	p_val;
};

struct warunek {
#if 0
	/*
	 * To nie jest konieczne, kiedy do konstruktora klasy BC bede
	 * przekazywal wskaznik do objektu klasy MSH, z ktorej w trakcie
	 * wczytywania bede mogl pobrac informacje dot. poszczegolnego
	 * brzegu i zweryfikowac, czy jego struktura jest taka sama jak w
	 * pliku BC.
	 */
	struct brzeg	*w_b;
#endif
	int	w_l_wezlow;
	int	w_id_war;
	int	w_jak_def;
	int	w_l_param;
	struct param_def *w_param;
};

struct obszar {
	int	 o_numer; /* numer obszaru: zawsze rowny "1" */
	int	 o_l_brzegow; /* ile brzegow tworzy obszar
			      * powinno byc rowne l_brzegow z klasy MSH
			      */
	int	*o_bidx; /* wielkosci l_brzegow */
};

#endif /* _PAI_TYPES_H_ */
