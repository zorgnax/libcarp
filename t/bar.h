#ifndef __BAR_H__
#define __BAR_H__

#ifdef _WIN32
#   define BAR_EXP __declspec(dllexport)
#else
#   define BAR_EXP
#endif

BAR_EXP char *bara ();
BAR_EXP char *barb ();
BAR_EXP char *barc ();
BAR_EXP char *bard ();

#endif
