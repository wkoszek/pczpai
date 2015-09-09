#ifndef _PAIC_IC_H_
#define _PAIC_IC_H_

#include "pai_types.h"
#include "pai_tok.h"
#include "pai_toklist.h"

class IC : public TokList {
	public:
		int l_wymiarow;
		int l_wezlow;
		int l_elem;
		int l_obszarow;
		int l_brzegow;
		
#if 0
		struct warunek	*warunki;
#endif
	
		IC() {}
		~IC() {}

		void IC_Tokenize(void);
		void _IC(void);
		IC(const string &fn);
		IC(const char *fn);
};

#endif
