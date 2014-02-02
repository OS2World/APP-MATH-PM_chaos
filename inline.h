/* Define the constant INLINE.  If we're using IBM's C Set++, it will expand 
   to a compiler directive that makes the function inline; otherwise, we'll
   just declare it static.

   There is no portable way to inline a function in C, but many compilers
   provide some way of doing this.  You may want to edit this file to tell
   it about whichever comiler you're using. 
*/

#ifdef INLINE
#undef INLINE
#endif

#ifdef __IBMC__
#define INLINE _Inline
#else
#define INLINE static
#endif
