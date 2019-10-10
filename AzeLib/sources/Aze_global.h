#ifndef AZE_GLOBAL_H
#define AZE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AZE_LIBRARY)
#  define AZE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define AZE_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // AZE_GLOBAL_H
