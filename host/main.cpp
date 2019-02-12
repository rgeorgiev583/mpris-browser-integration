/*
    Copyright (C) 2017 by Kai Uwe Broulik <kde@privat.broulik.de>
    Copyright (C) 2017 by David Edmundson <davidedmundson@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDebug>

#include "connection.h"
#include "abstractbrowserplugin.h"

#include "mprisplugin.h"

void msgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);

    QJsonObject data;
    data[QStringLiteral("subsystem")] = QStringLiteral("debug");
    switch(type) {
        case QtDebugMsg:
        case QtInfoMsg:
            data[QStringLiteral("action")] = QStringLiteral("debug");
            break;
        default:
            data[QStringLiteral("action")] = QStringLiteral("warning");
    }
    data[QStringLiteral("payload")] = QJsonObject({{QStringLiteral("message"), msg}});

    Connection::self()->sendData(data);
}

int main(int argc, char *argv[])
{
    // otherwise when logging out, session manager will ask the host to quit
    // (it's a "regular X app" after all) and then the browser will complain
    qunsetenv("SESSION_MANAGER");

    QCoreApplication a(argc, argv);

    qInstallMessageHandler(msgHandler);

    // NOTE if you add a new plugin here, make sure to adjust the
    // "DEFAULT_EXTENSION_SETTINGS" in constants.js or else it won't
    // even bother loading your shiny new plugin!
    QList<AbstractBrowserPlugin*> m_plugins;
    m_plugins << new MPrisPlugin(&a);

    QString serviceName = QStringLiteral("com.github.rgeorgiev583.mpris.browser_integration");
    if (!QDBusConnection::sessionBus().registerService(serviceName)) {
        // now try appending PID in case multiple hosts are running
        serviceName.append(QLatin1String("-")).append(QString::number(QCoreApplication::applicationPid()));
        if (!QDBusConnection::sessionBus().registerService(serviceName)) {
            qWarning() << "Failed to register DBus service name" << serviceName;
        }
    }

    QObject::connect(Connection::self(), &Connection::dataReceived, [m_plugins](const QJsonObject &json) {
        const QString subsystem = json.value(QStringLiteral("subsystem")).toString();

        if (subsystem.isEmpty()) {
            //qDebug() << "No subsystem provided";
            return;
        }

        const QString event = json.value(QStringLiteral("event")).toString();
        if (event.isEmpty()) {
            //qDebug() << "No event provided";
            return;
        }

        foreach(AbstractBrowserPlugin *plugin, m_plugins) {
            if (!plugin->isLoaded()) {
                continue;
            }

            if (plugin->subsystem() == subsystem) {
                //design question, should we have a JSON of subsystem, event, payload, or have all data at the root level?
                plugin->handleData(event, json);
                return;
            }
        }

        qDebug() << "Don't know how to handle event" << event << "for subsystem" << subsystem;
    });

    return a.exec();
}
