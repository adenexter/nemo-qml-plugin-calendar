/*
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Aaron Kennedy <aaron.kennedy@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "calendarnotebookmodel.h"

#include "calendardb.h"
#include "calendareventcache.h"

NemoCalendarNotebookModel::NemoCalendarNotebookModel()
{
}

int NemoCalendarNotebookModel::rowCount(const QModelIndex &index) const
{
    if (index != QModelIndex())
        return 0;

    return NemoCalendarDb::storage()->notebooks().count();
}

QVariant NemoCalendarNotebookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= NemoCalendarDb::storage()->notebooks().count())
        return QVariant();

    mKCal::Notebook::Ptr notebook = NemoCalendarDb::storage()->notebooks().at(index.row());

    switch (role) {
    case NameRole:
        return notebook->name();
    case UidRole:
        return notebook->uid();
    case DescriptionRole:
        return notebook->description();
    case ColorRole:
        return NemoCalendarEventCache::instance()->notebookColor(notebook->uid());
    case DefaultRole:
        return notebook->isDefault();
    case ReadOnlyRole:
        return notebook->isReadOnly();
    case LocalCalendarRole:
        return (notebook->isMaster() && !notebook->isShared() && notebook->pluginName().isEmpty());
    default:
        return QVariant();
    }
}

bool NemoCalendarNotebookModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    if (!index.isValid()
     || index.row() >= NemoCalendarDb::storage()->notebooks().count()
     || (role != ColorRole && role != DefaultRole))
       return false; 

    mKCal::Notebook::Ptr notebook = NemoCalendarDb::storage()->notebooks().at(index.row());

    if (role == ColorRole) {
        NemoCalendarEventCache::instance()->setNotebookColor(notebook->uid(), data.toString());
        emit dataChanged(index, index, QVector<int>() << role);
    } else if (role == DefaultRole) {
        NemoCalendarDb::storage()->setDefaultNotebook(notebook);
        emit dataChanged(this->index(0, 0), this->index(NemoCalendarDb::storage()->notebooks().count() - 1, 0), QVector<int>() << role);
    }

    return true;
}

QHash<int, QByteArray> NemoCalendarNotebookModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[NameRole] = "name";
    roleNames[UidRole] = "uid";
    roleNames[DescriptionRole] = "description";
    roleNames[ColorRole] = "color";
    roleNames[DefaultRole] = "isDefault";
    roleNames[ReadOnlyRole] = "readOnly";
    roleNames[LocalCalendarRole] = "localCalendar";

    return roleNames;
}
