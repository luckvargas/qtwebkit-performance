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
#include "benchmark.h"

#include <qwebframe.h>
#include <qwebview.h>
#include <qpainter.h>

class tst_Painting : public QObject
{
    Q_OBJECT

public:
    ~tst_Painting();

public Q_SLOTS:
    void init();
    void cleanup();

private Q_SLOTS:
    void paint_data();
    void paint();

private:
    QWebView* m_view;
    QWebPage* m_page;
};

tst_Painting::~tst_Painting()
{
    benchmarkOutput(*benchmark_parent);
}

void tst_Painting::init()
{
    m_view = new QWebView;
    m_page = m_view->page();

    QSize viewportSize(1024, 768);
    m_view->setFixedSize(viewportSize);
    m_page->setViewportSize(viewportSize);
}

void tst_Painting::cleanup()
{
    delete m_view;
}

void tst_Painting::paint_data()
{
    add_test_urls();
}

void tst_Painting::paint()
{
    QFETCH(QUrl, url);

    m_view->load(url);
    ::waitForSignal(m_view, SIGNAL(loadFinished(bool)));

    /* force a layout */
    QWebFrame* mainFrame = m_page->mainFrame();
    mainFrame->toPlainText();

    QPixmap pixmap(m_page->viewportSize());
    WEB_BENCHMARK(url.toString()) {
        QPainter painter(&pixmap);
        mainFrame->render(&painter, QRect(QPoint(0, 0), m_page->viewportSize()));
        painter.end();

        // force badness... to have some reliable result on non raster..
        // this will make the result "unreal" in some ways but might be
        // better than just using the raster engine.
        QApplication::syncX();
    }
}

QTEST_MAIN(tst_Painting)
#include "tst_painting.moc"
