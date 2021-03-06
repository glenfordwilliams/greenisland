/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QStandardPaths>
#include <QtQml/QQmlContext>

#include "compositor.h"
#include "gldebug.h"
#include "globalregistry.h"
#include "output.h"
#include "outputwindow.h"
#include "quicksurface.h"
#include "windowview.h"
#include "shellwindowview.h"

#include "protocols/fullscreen-shell/fullscreenshellclient.h"

namespace GreenIsland {

OutputWindow::OutputWindow(Compositor *compositor)
    : QQuickView()
    , m_compositor(compositor)
    , m_output(Q_NULLPTR)
{
    // Setup window
    setColor(Qt::black);
    winId();

    // Send frame callbacks after rendering
    connect(this, &QQuickView::afterRendering,
            this, &OutputWindow::sendCallbacks);

    // Show the window as soon as QML is loaded
    connect(this, &QQuickView::statusChanged,
            this, &OutputWindow::componentStatusChanged);

    connect(this, SIGNAL(sceneGraphInitialized()),
            this, SLOT(printInfo()),
            Qt::DirectConnection);

    // Retrieve full screen shell client object, this will be available
    // only when Green Island is nested into another compositor
    // that supports the fullscreen-shell interface
    FullScreenShellClient *fsh = GlobalRegistry::fullScreenShell();

    if (fsh) {
        // Disable decorations
        setFlags(flags() | Qt::BypassWindowManagerHint);

        // Present output window when using full screen shell
        connect(this, &QQuickView::visibleChanged, [=](bool value) {
            if (value)
                fsh->showOutput(m_output);
            else
                fsh->hideOutput(m_output);
        });
    }
}

Output *OutputWindow::output() const
{
    return m_output;
}

void OutputWindow::setOutput(Output *output)
{
    // Prevent assigning another output that doesn't know about this window
    if (m_output)
        return;

    // Save output reference
    m_output = output;

    // Make compositor instance available to QML
    rootContext()->setContextProperty("compositor", m_compositor);

    // Add a context property to reference this window
    rootContext()->setContextProperty("_greenisland_window", this);

    // Add a context property to reference the output
    rootContext()->setContextProperty("_greenisland_output", m_output);

    // Load QML and setup window
    setResizeMode(QQuickView::SizeRootObjectToView);
    if (Compositor::s_fixedPlugin.isEmpty()) {
        qFatal("No plugin specified, cannot continue!");
    } else {
        qDebug() << "Loading" << Compositor::s_fixedPlugin << "plugin";

        QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                              QString("greenisland/%1/Compositor.qml").arg(Compositor::s_fixedPlugin));

        // Load main file or bail out
        if (QFile(path).exists(path))
            setSource(path);
        else
            qFatal("Plugin \"%s\" is not valid, cannot continue!",
                   qPrintable(Compositor::s_fixedPlugin));
    }
}

void OutputWindow::keyPressEvent(QKeyEvent *event)
{
    m_compositor->setIdleInhibit(m_compositor->idleInhibit() + 1);

    QQuickView::keyPressEvent(event);
}

void OutputWindow::keyReleaseEvent(QKeyEvent *event)
{
    m_compositor->setIdleInhibit(m_compositor->idleInhibit() - 1);

    QQuickView::keyReleaseEvent(event);
}

void OutputWindow::mousePressEvent(QMouseEvent *event)
{
    m_compositor->setIdleInhibit(m_compositor->idleInhibit() + 1);

    QQuickView::mousePressEvent(event);
}

void OutputWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_compositor->setIdleInhibit(m_compositor->idleInhibit() - 1);

    QQuickView::mouseReleaseEvent(event);
}

void OutputWindow::mouseMoveEvent(QMouseEvent *event)
{
    m_compositor->setState(Compositor::Active);

    QQuickView::mouseMoveEvent(event);
}

void OutputWindow::wheelEvent(QWheelEvent *event)
{
    m_compositor->setState(Compositor::Active);

    QQuickView::wheelEvent(event);
}

void OutputWindow::printInfo()
{
    GreenIsland::printGraphicsInformation(this);
}

void OutputWindow::sendCallbacks()
{
    m_compositor->sendFrameCallbacks(m_compositor->surfaces());
}

void OutputWindow::componentStatusChanged(const QQuickView::Status &status)
{
    if (status == QQuickView::Ready)
        show();
}

}

#include "moc_outputwindow.cpp"
