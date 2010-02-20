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

#include "qclevent.h"
#include "qclcommandqueue.h"
#include "qclcontext.h"
#include <QtCore/qdebug.h>
#include <QtCore/qtconcurrentrun.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLEvent
    \brief The QCLEvent class represents an OpenCL event object.
    \since 4.7
    \ingroup opencl

    QCLEvent objects are used to track the current status of an
    asynchronous command that was executed against a QCLContext.
    Events progress through a number of states:

    \table
    \row \o isQueued() \o The command has been enqueued on the
    command queue for the QCLContext but has not yet been submitted
    to the OpenCL device for execution.
    \row \o isSubmitted() \o The command has been submitted to
    the OpenCL device for execution, but has not started executing.
    \row \o isRunning() \o The command is running on the OpenCL device,
    but has not yet finished.
    \row \o isFinished() \o The command has finished execution and
    the results, if any, are now available for use in further commands.
    \endtable

    Host applications can wait for the event (and thus, the command
    that created it) to finish by calling waitForFinished():

    \code
    QCLBuffer buffer = ...;
    QCLEvent event = buffer.readAsync(offset, data, size);
    ...
    event.waitForFinished();
    \endcode

    Applications can also pass a QCLEventList of event objects to
    another command to tell it to start executing only once all events
    in the list have finished:

    \code
    QCLBuffer buffer = ...;
    QCLEvent event1 = buffer.readAsync(offset1, data1, size1);
    QCLEvent event2 = buffer.readAsync(offset2, data2, size2);

    QCLEventList after;
    after << event1 << event2;
    QCLEvent event3 = buffer.readAsync(offset3, data3, size3, after);
    ...
    event3.waitForFinished();
    \endcode

    Normally it isn't necessary to wait for previous requests to
    finish because the command queue's natural order will enforce
    the conditions.  If however QCLCommandQueue::isOutOfOrder() is set,
    it is possible for the second and third QCLBuffer::readAsync()
    commands above to start before the first command has finished.
    Event lists can be used to order commands when out-of-order
    command execution is in use.

    \sa QCLCommandQueue::isOutOfOrder(), QCLEventList
*/

/*!
    \fn QCLEvent::QCLEvent()

    Constructs a null OpenCL event object.
*/

/*!
    \fn QCLEvent::QCLEvent(cl_event id)

    Constructs an OpenCL event object from the native identifier \a id.
    This class takes over ownership of \a id and will release it in
    the destructor.
*/

/*!
    Constructs a copy of \a other.  The \c{clRetainEvent()} function
    will be called to update the reference count on id().
*/
QCLEvent::QCLEvent(const QCLEvent &other)
    : m_id(other.m_id)
{
    if (m_id)
        clRetainEvent(m_id);
}

/*!
    Releases this OpenCL event object by calling \c{clReleaseEvent()}.
*/
QCLEvent::~QCLEvent()
{
    if (m_id)
        clReleaseEvent(m_id);
}

/*!
    Assigns \a other to this OpenCL event object.  The current id() will
    be released with \c{clReleaseEvent()}, and the new id() will be
    retained with \c{clRetainEvent()}.
*/
QCLEvent &QCLEvent::operator=(const QCLEvent &other)
{
    if (m_id != other.m_id) {
        if (m_id)
            clReleaseEvent(m_id);
        m_id = other.m_id;
        if (m_id)
            clRetainEvent(m_id);
    }
    return *this;
}

/*!
    \fn bool QCLEvent::isNull() const

    Returns true if this OpenCL event object is null; false otherwise.
*/

/*!
    \fn cl_event QCLEvent::id() const

    Returns the native OpenCL identifier for this event.
*/

/*!
    \fn bool QCLEvent::isQueued() const

    Returns true if the command associated with this OpenCL event has been
    queued for execution on the host, but has not yet been submitted to
    the device yet.

    \sa isSubmitted(), isRunning(), isFinished()
*/

/*!
    \fn bool QCLEvent::isSubmitted() const

    Returns true if the command associated with this OpenCL event has been
    submitted for execution on the device yet, but is not yet running.

    \sa isQueued(), isRunning(), isFinished()
*/

/*!
    \fn bool QCLEvent::isRunning() const

    Returns true if the command associated with this OpenCL event is
    running on the device, but has not yet finished.

    \sa isQueued(), isSubmitted(), isFinished()
*/

/*!
    \fn bool QCLEvent::isFinished() const

    Returns true if the command associated with this OpenCL event
    has finished execution on the device.

    \sa isQueued(), isSubmitted(), isRunning()
*/

/*!
    \internal
*/
int QCLEvent::status() const
{
    if (!m_id)
        return CL_INVALID_EVENT;
    cl_int st, error;
    error = clGetEventInfo(m_id, CL_EVENT_COMMAND_EXECUTION_STATUS,
                           sizeof(st), &st, 0);
    if (error != CL_SUCCESS)
        return error;
    else
        return st;
}

/*!
    Waits for this event to be signalled as finished.  The calling thread
    is blocked until the event is signalled.  This function returns
    immediately if the event is null.

    \sa isFinished(), QCLEventList::waitForFinished()
*/
void QCLEvent::waitForFinished()
{
    if (m_id) {
        cl_int error = clWaitForEvents(1, &m_id);
        if (error != CL_SUCCESS) {
            qWarning() << "QCLEvent::waitForFinished:"
                       << QCLContext::errorName(error);
        }
    }
}

/*!
    \fn bool QCLEvent::operator==(const QCLEvent &other) const

    Returns true if this OpenCL event is the same as \a other;
    false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QCLEvent::operator!=(const QCLEvent &other) const

    Returns true if this OpenCL event is not the same as \a other;
    false otherwise.

    \sa operator==()
*/

static void qt_cl_future_wait(cl_event event)
{
    clWaitForEvents(1, &event);
    clReleaseEvent(event);
}

/*!
    Returns a QFuture object that can be used to track when this
    OpenCL event finishes.

    This function creates a thread on the host CPU to monitor the
    event in the background.  If the caller wants to block in the
    foreground thread, then waitForFinished() is recommended instead
    of using toFuture(), because waitForFinished() does not need to
    create an extra thread on the host CPU.

    If however the caller wants to receive notification of the event
    finishing via a signal, then toFuture() can be used with
    QFutureWatcher to receive the signal:

    \code
    QCLEvent event = ...;
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(event.toFuture());
    connect(watcher, SIGNAL(finished()), this, SLOT(eventFinished()));
    \endcode

    QCLEvent has an implicit conversion operator to QFuture<void>,
    which allows the QFutureWatcher::setFuture() call to be shortened
    as follows:

    \code
    watcher->setFuture(event);
    \endcode

    \sa operator QFuture<void>()
*/
QFuture<void> QCLEvent::toFuture() const
{
    if (m_id) {
        clRetainEvent(m_id);
        return QtConcurrent::run(qt_cl_future_wait, m_id);
    } else {
        return QFuture<void>();
    }
}

/*!
    Equivalent to calling toFuture().

    This conversion operator is intended to help with interfacing
    OpenCL to code that uses QtConcurrent.
*/
QCLEvent::operator QFuture<void>() const
{
    return toFuture();
}

/*!
    \class QCLEventList
    \brief The QCLEventList class represents a list of QCLEvent objects.
    \since 4.7
    \ingroup opencl

    \sa QCLEvent
*/

/*!
    \fn QCLEventList::QCLEventList()

    Constructs an empty list of OpenCL events.
*/

/*!
    Constructs a list of OpenCL events that contains \a event.
    If \a event is null, this constructor will construct an
    empty list.

    \sa append()
*/
QCLEventList::QCLEventList(const QCLEvent &event)
{
    cl_event id = event.id();
    if (id) {
        clRetainEvent(id);
        m_events.append(id);
    }
}

/*!
    Constructs a copy of \a other.

    \sa operator=()
*/
QCLEventList::QCLEventList(const QCLEventList &other)
    : m_events(other.m_events)
{
    for (int index = 0; index < m_events.size(); ++index)
        clRetainEvent(m_events[index]);
}

/*!
    Destroys this list of OpenCL events.
*/
QCLEventList::~QCLEventList()
{
    for (int index = 0; index < m_events.size(); ++index)
        clReleaseEvent(m_events[index]);
}

/*!
    Assigns the contents of \a other to this object.
*/
QCLEventList &QCLEventList::operator=(const QCLEventList &other)
{
    if (this != &other) {
        for (int index = 0; index < m_events.size(); ++index)
            clReleaseEvent(m_events[index]);
        m_events = other.m_events;
        for (int index = 0; index < m_events.size(); ++index)
            clRetainEvent(m_events[index]);
    }
    return *this;
}

/*!
    \fn bool QCLEventList::isEmpty() const

    Returns true if this is an empty list; false otherwise.

    \sa size()
*/

/*!
    \fn int QCLEventList::size() const

    Returns the size of this event list.

    \sa isEmpty(), at()
*/

/*!
    Appends \a event to this list of OpenCL events if it is not null.
    Does nothing if \a event is null.

    \sa remove()
*/
void QCLEventList::append(const QCLEvent &event)
{
    cl_event id = event.id();
    if (id) {
        clRetainEvent(id);
        m_events.append(id);
    }
}

/*!
    \overload

    Appends the contents of \a other to this event list.
*/
void QCLEventList::append(const QCLEventList &other)
{
    for (int index = 0; index < other.m_events.size(); ++index) {
        cl_event id = other.m_events[index];
        clRetainEvent(id);
        m_events.append(id);
    }
}

/*!
    Removes \a event from this event list.

    \sa append(), contains()
*/
void QCLEventList::remove(const QCLEvent &event)
{
    QVector<cl_event>::Iterator it = m_events.begin();
    while (it != m_events.end()) {
        if (*it == event.id()) {
            clReleaseEvent(*it);
            it = m_events.erase(it);
        } else {
            ++it;
        }
    }
}

/*!
    Returns the event at \a index in this event list, or a null
    QCLEvent if \a index is out of range.

    \sa size(), contains()
*/
QCLEvent QCLEventList::at(int index) const
{
    if (index >= 0 && index < m_events.size()) {
        cl_event id = m_events[index];
        clRetainEvent(id);
        return QCLEvent(id);
    } else {
        return QCLEvent();
    }
}

/*!
    \fn bool QCLEventList::contains(const QCLEvent &event) const

    Returns true if this event list contains \a event;
    false otherwise.

    \sa at(), remove()
*/

/*!
    \fn const cl_event *QCLEventList::eventData() const

    Returns a const pointer to the raw OpenCL event data in this
    event list; null if the list is empty.

    This function is intended for use with native OpenCL library
    functions that take an array of \c{cl_event} objects as
    an argument.

    \sa size()
*/

/*!
    \fn QCLEventList &QCLEventList::operator+=(const QCLEvent &event)

    Same as append(\event).
*/

/*!
    \fn QCLEventList &QCLEventList::operator+=(const QCLEventList &other)

    Same as append(\a other).
*/

/*!
    \fn QCLEventList &QCLEventList::operator<<(const QCLEvent &event)

    Same as append(\event).
*/

/*!
    \fn QCLEventList &QCLEventList::operator<<(const QCLEventList &other)

    Same as append(\a other).
*/

/*!
    Waits for all of the events in this list to be signalled as
    finished.  The calling thread is blocked until all of the
    events are signalled.  If the list is empty, then this function
    returns immediately.

    \sa QCLEvent::waitForFinished()
*/
void QCLEventList::waitForFinished()
{
    if (m_events.isEmpty())
        return;
    cl_int error = clWaitForEvents(size(), eventData());
    if (error != CL_SUCCESS) {
        qWarning() << "QCLEventList::waitForFinished:"
                   << QCLContext::errorName(error);
    }
}

static void qt_cl_future_list_wait(QVector<cl_event> events)
{
    clWaitForEvents(events.size(), events.constData());
    for (int index = 0; index < events.size(); ++index)
        clReleaseEvent(events[index]);
}

/*!
    Returns a QFuture object that can be used to track when all
    of the events on this list finish.

    This function creates a thread on the host CPU to monitor the
    events in the background.  If the caller wants to block in the
    foreground thread, then waitForFinished() is recommended instead
    of using toFuture(), because waitForFinished() does not need to
    create an extra thread on the host CPU.

    If however the caller wants to receive notification of the events
    finishing via a signal, then toFuture() can be used with
    QFutureWatcher to receive the signal:

    \code
    QCLEventList list;
    list += event1;
    list += event2;
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(list.toFuture());
    connect(watcher, SIGNAL(finished()), this, SLOT(eventsFinished()));
    \endcode

    QCLEventList has an implicit conversion operator to QFuture<void>,
    which allows the QFutureWatcher::setFuture() call to be shortened
    as follows:

    \code
    watcher->setFuture(list);
    \endcode

    \sa operator QFuture<void>()
*/
QFuture<void> QCLEventList::toFuture() const
{
    if (!m_events.isEmpty()) {
        QVector<cl_event> events(m_events);
        events.detach();
        for (int index = 0; index < events.size(); ++index)
            clRetainEvent(events[index]);
        return QtConcurrent::run(qt_cl_future_list_wait, events);
    } else {
        return QFuture<void>();
    }
}

/*!
    Equivalent to calling toFuture().

    This conversion operator is intended to help with interfacing
    OpenCL to code that uses QtConcurrent.
*/
QCLEventList::operator QFuture<void>() const
{
    return toFuture();
}

QT_END_NAMESPACE
