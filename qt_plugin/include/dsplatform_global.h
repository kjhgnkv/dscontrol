#pragma once

#include <QtGlobal>

#if defined(DSPLATFORM_LIBRARY)
    #    define DSPLATFORMSHARED_EXPORT Q_DECL_EXPORT
#else
    #    define DSPLATFORMSHARED_EXPORT Q_DECL_IMPORT
#endif
