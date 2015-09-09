#ifndef _PAI_BC_H_
#define _PAI_BC_H_

#include "pai_types.h"
#include "pai_tok.h"
#include "pai_toklist.h"

class BC : public TokList {
	public:
		int l_wymiarow;
		int l_wezlow;
		int l_elem;
		int l_obszarow;
		int l_brzegow;
		
		struct warunek	*warunki;
	



		BC() {}
		~BC() {}

		void BC_Tokenize(void);
		void _BC(void);
		BC(const string &fn);
		BC(const char *fn);
};

#endif
