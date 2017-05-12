/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenCL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCLGLOBAL_H
#define QCLGLOBAL_H

#include <QtCore/qglobal.h>
#include <Qt/qglobal.h>
#include <QtConcurrent/qtconcurrentexception.h>
#include <QtConcurrent/qtconcurrentcompilertest.h>
#include <Qt/qtconcurrentcompilertest.h>

/*
   These functions make it possible to use standard C++ functions with
   a similar name from Qt header files (especially template classes).
*/
/// HACK H-A-C-K
Q_CORE_EXPORT void *qMalloc(size_t size);
Q_CORE_EXPORT void qFree(void *ptr);
Q_CORE_EXPORT void *qRealloc(void *ptr, size_t size);
Q_CORE_EXPORT void *qMallocAligned(size_t size, size_t alignment);
Q_CORE_EXPORT void *qReallocAligned(void *ptr, size_t size, size_t oldsize, size_t alignment);
Q_CORE_EXPORT void qFreeAligned(void *ptr);
Q_CORE_EXPORT void *qMemCopy(void *dest, const void *src, size_t n);
Q_CORE_EXPORT void *qMemSet(void *dest, int c, size_t n);


#define QT_OPENCL_1_1
#define QT_TYPENAME typename

// XXX: Move to qglobal.h eventually.
//QT_LICENSED_MODULE(CL)
#if defined(Q_OS_WIN) && defined(QT_MAKEDLL)
#   if defined(QT_BUILD_CL_LIB)
#       define Q_CL_EXPORT Q_DECL_EXPORT
#   else
#       define Q_CL_EXPORT Q_DECL_IMPORT
#   endif
#elif defined(Q_OS_WIN) && defined(QT_DLL)
#   define Q_CL_EXPORT Q_DECL_IMPORT
#endif
#if !defined(Q_CL_EXPORT)
#   if defined(QT_SHARED)
#       define Q_CL_EXPORT Q_DECL_EXPORT
#   else
#       define Q_CL_EXPORT
#   endif
#endif

//QT_LICENSED_MODULE(CLGL)
#if defined(Q_OS_WIN) && defined(QT_MAKEDLL)
#   if defined(QT_BUILD_CLGL_LIB)
#       define Q_CLGL_EXPORT Q_DECL_EXPORT
#   else
#       define Q_CLGL_EXPORT Q_DECL_IMPORT
#   endif
#elif defined(Q_OS_WIN) && defined(QT_DLL)
#   define Q_CLGL_EXPORT Q_DECL_IMPORT
#endif
#if !defined(Q_CLGL_EXPORT)
#   if defined(QT_SHARED)
#       define Q_CLGL_EXPORT Q_DECL_EXPORT
#   else
#       define Q_CLGL_EXPORT
#   endif
#endif

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl_platform.h>
#include <OpenCL/cl.h>
#else
#include <CL/cl_platform.h>
#include <CL/cl.h>
#endif

#endif
