//-----------------------------------------------------------
// Utilitaires divers 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Eric Aubourg, 1995-1998 (heritage PEIDA - EROS )
//-----------------------------------------------------------

#ifndef UTILS_H_SEEN
#define UTILS_H_SEEN 

#include "machdefs.h"
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>


// MemCpy marche meme en cas de recouvrement.
// #define memcpy MemCpy

void* MemCpy(void* dest, const void* source, size_t n);

char const* PeidaWorkPath();
void buildPath(char* dst, const char* dir, const char* file);
void buildPath(std::string& dst, const std::string& dir, const std::string& file);
void changeSuffix(char* file, const char* suffix);
void changeSuffix(std::string& file, const std::string& suffix);
std::string itos(int);
std::string ftos(double);

inline void GetIntEnv(char const* s, int& v)
{
  char* pdeb = getenv(s);
  if (pdeb) v = atoi(pdeb);
}

/* if defined(__DECCXX) || defined(__xlC) */
#if defined(__xlC) || defined(__DECCXX)
#define STRUCTCOMP(s)                        \
  bool operator==(s const& b) const          \
{ return memcmp(this, &b, sizeof(s)) == 0; } \
  bool operator<(s const& b) const           \
{ return memcmp(this, &b, sizeof(s)) < 0; } 
  
#define STRUCTCOMPF(s,f)                        \
  bool operator==(s const& b) const          \
{ return f == b.f; }                          \
  bool operator<(s const& b) const           \
{ return f < b.f; }
#else
#define STRUCTCOMP(s) 
#define STRUCTCOMPF(s,f)
#endif

#endif // UTILS_EA
