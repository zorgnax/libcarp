#ifndef __FOO_H__
#define __FOO_H__

#ifdef _WIN32
#   define FOO_EXP __declspec(dllexport)
#else
#   define FOO_EXP
#endif

FOO_EXP char *fooa ();
FOO_EXP char *foob ();
FOO_EXP char *fooc ();
FOO_EXP char *food ();
FOO_EXP char *fooe ();
FOO_EXP char *foof ();
FOO_EXP char *foog ();
FOO_EXP char *fooh ();

#endif

