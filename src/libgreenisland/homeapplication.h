/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef HOMEAPPLICATION_H
#define HOMEAPPLICATION_H

#include <QtWidgets/QApplication>

#include <greenisland/greenisland_export.h>

namespace GreenIsland {

class Compositor;

class GREENISLAND_EXPORT HomeApplication : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(QString plugin READ plugin CONSTANT)
    Q_PROPERTY(QString socket READ socket CONSTANT)
public:
    HomeApplication(int &argc, char **argv);
    ~HomeApplication();

    Compositor *compositor() const;

    QString plugin() const;

    QString socket() const;
    void setSocket(const QString &socket);

    QString fakeScreenData() const;
    void setFakeScreenData(const QString &fileName);

    int idleTime() const;
    void setIdleTime(int time);

    bool run(const QString &plugin);

private:
    QString m_socket;
    QString m_fakeScreenFileName;
    int m_idleTime;
    Compositor *m_compositor;
};

} // namespace GreenIsland

#endif // HOMEAPPLICATION_H

