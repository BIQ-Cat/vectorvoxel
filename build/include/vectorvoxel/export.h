
#ifndef VECTORVOXEL_EXPORT_H
#define VECTORVOXEL_EXPORT_H

#ifdef VECTORVOXEL_STATIC_DEFINE
#  define VECTORVOXEL_EXPORT
#  define VECTORVOXEL_NO_EXPORT
#else
#  ifndef VECTORVOXEL_EXPORT
#    ifdef VectorVoxel_EXPORTS
        /* We are building this library */
#      define VECTORVOXEL_EXPORT 
#    else
        /* We are using this library */
#      define VECTORVOXEL_EXPORT 
#    endif
#  endif

#  ifndef VECTORVOXEL_NO_EXPORT
#    define VECTORVOXEL_NO_EXPORT 
#  endif
#endif

#ifndef VECTORVOXEL_DEPRECATED
#  define VECTORVOXEL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef VECTORVOXEL_DEPRECATED_EXPORT
#  define VECTORVOXEL_DEPRECATED_EXPORT VECTORVOXEL_EXPORT VECTORVOXEL_DEPRECATED
#endif

#ifndef VECTORVOXEL_DEPRECATED_NO_EXPORT
#  define VECTORVOXEL_DEPRECATED_NO_EXPORT VECTORVOXEL_NO_EXPORT VECTORVOXEL_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef VECTORVOXEL_NO_DEPRECATED
#    define VECTORVOXEL_NO_DEPRECATED
#  endif
#endif

#endif /* VECTORVOXEL_EXPORT_H */
