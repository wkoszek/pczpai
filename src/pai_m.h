#ifndef _PAI_M_H_
#define _PAI_M_H_

#include "pai_types.h"
#include "pai_tok.h"
#include "pai_toklist.h"

class M : public TokList {
	public:
		int l_wymiarow;
		int l_wezlow;
		int l_elem;
		int l_obszarow;
		int l_brzegow;
		
#if 0
		struct warunek	*warunki;
#endif
	
		M() {}
		~M() {}

		void M_Tokenize(void);
		void _M(void);
		M(const string &fn);
		M(const char *fn);
};

#endif
