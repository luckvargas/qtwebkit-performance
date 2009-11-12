/*
 * Copyright (C) 2009 Holger Hans Peter Freyther
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QtTest/QtTest>

#include "common_init.h"

#include <qwebframe.h>
#include <qwebview.h>
#include <qpainter.h>

/**
 * Starts an event loop that runs until the given signal is received.
 Optionally the event loop
 * can return earlier on a timeout.
 *
 * \return \p true if the requested signal was received
 *         \p false on timeout
 */
static bool waitForSignal(QObject* obj, const char* signal, int timeout = 0)
{
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit()));
    QTimer timer;
    QSignalSpy timeoutSpy(&timer, SIGNAL(timeout()));
    if (timeout > 0) {
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.setSingleShot(true);
        timer.start(timeout);
    }
    loop.exec();
    return timeoutSpy.isEmpty();
}

class tst_Loading : public QObject
{
    Q_OBJECT

public:

public Q_SLOTS:
    void init();
    void cleanup();

private Q_SLOTS:
    void load();

private:
    QWebView* m_view;
    QWebPage* m_page;
};

void tst_Loading::init()
{
    QWebSettings::globalSettings()->setMaximumPagesInCache(0);
    QWebSettings::globalSettings()->setObjectCacheCapacities(0, 0, 0);

    m_view = new QWebView;
    m_page = m_view->page();

    QSize viewportSize(1024, 768);
    m_view->setFixedSize(viewportSize);
    m_page->setViewportSize(viewportSize);

    // this makes us different to the loading test...
    m_view->show();
}

void tst_Loading::cleanup()
{
    delete m_view;
}

void tst_Loading::load()
{
    const QList<QUrl> urls = test_urls();

    QBENCHMARK {
        foreach(const QUrl& url, urls) {
            m_view->load(url);
            // really wait for loading..
            ::waitForSignal(m_view, SIGNAL(loadFinished(bool)));
        }

        m_view->load(QUrl("about:blank"));
        QWebSettings::clearMemoryCaches();
    }
}

QTEST_MAIN(tst_Loading)
#include "tst_cycler.moc"
