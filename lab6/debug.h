#ifndef DEBUG_H__
#define DEBUG_H__

#ifdef DEBUG
#define printfd printf
#else
#define printfd //
#endif

#endif /* DEBUG_H__ */