/**
 * GYP does not work well with autoconf, unfortunately.
 *
 * This is generated manually to have settings that will work with the
 * platforms node can be built on (and, yeah, that kind of defeats the
 * whole purpose of the autotools system).
 */

/* HAVE_INLINE Define to 1 if the inline keyword can be used with the C compliler.
   For now, assume this is true everywhere until it breaks */
#ifndef HAVE_INLINE
#define HAVE_INLINE 1
#endif
