/*
 * Copyright (C) 2010 Holger Hans Peter Freyther
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef benchmark_h
#define benchmark_h

#include <QPair>
#include <QString>
#include <QTimer>

/**
 * Benchmark related functionality
 */

class Benchmark {
public:
    Benchmark();
    Benchmark(const QString& name);

    QString name() const;
    bool isEmpty() const;

    void addBenchmark(const Benchmark&);
    QList<Benchmark> benchmarks() const;

    void addResult(long long result);
    QList<long long> results() const;

private:
    bool m_empty;
    QString m_name;
    QList<long long> m_results;
    QList<Benchmark> m_benchmarks;
};

class BenchmarkController {
public:
    BenchmarkController(const QString& name, Benchmark *parent, int iterations = 11);
    ~BenchmarkController();

    void next();
    void timeNow();
    int iterations() const;

    int i;

private:
    long long timeElapsed() const;
    const int m_iterations;
    Benchmark m_benchmark;
    Benchmark* m_parent;
    bool m_timed;
};

// used to measure sub-sections of the code which is run multiple time
class SubSectionBenchmarkController {
public:
    SubSectionBenchmarkController(const QString& name, Benchmark *parent, int iterations = 11);
    ~SubSectionBenchmarkController();
    void next();
    void startSubMeasure();
    void stopSubMeasure();
    int iterations() const { return m_iterations; }

    int i;
private:
    const int m_iterations;
    Benchmark m_benchmark;
    Benchmark* m_parent;
};

struct SummaryResult {
    SummaryResult()
        : mean(0)
        , average(0)
        , size(0)
    {}

    long long mean;
    long long average;
    int size;
};

SummaryResult benchmarkOutput(const Benchmark&, const QString& indent = QString());

/*
 * working on the benchmark
 */
long long benchmarkMean(const Benchmark&);
long long benchmarkAverage(const Benchmark&);
long long benchmarkStdError(const Benchmark&);
double benchmarkStdDeviationUnbiased(const Benchmark&);
double benchmarkStdDeviationBiased(const Benchmark&);

/**
 * Starts an event loop that runs until the given signal is received.
 Optionally the event loop
 * can return earlier on a timeout.
 *
 * \return \p true if the requested signal was received
 *         \p false on timeout
 */
bool waitForSignal(QObject* obj, const char* signal, int timeout = 0);

/*
 * Use shadowing
 */
extern Benchmark* benchmark_parent;

/*
 * macros for benchmarking
 */
#define WEB_CREATE_GROUP(name) \
    static Benchmark* old_parent = ::benchmark_parent; \
    static Benchmark* benchmark_parent = new Benchmark(name); \
    ::old_parent->add(benchmark_parent);

#define WEB_BENCHMARK(name) \
    for (BenchmarkController web__controller(name, benchmark_parent); \
         web__controller.i < web__controller.iterations(); web__controller.next())

#define WEB_BENCHMARK_ITER(name, iter) \
    for (BenchmarkController web__controller(name, benchmark_parent, iter); \
         web__controller.i < web__controller.iterations(); web__controller.next())

#define TIME_NOW \
        web__controller.timeNow();

#define WEB_BENCHMARK_SUBSECTION(name) \
    for (SubSectionBenchmarkController web__controller(name, benchmark_parent); \
         web__controller.i < web__controller.iterations(); web__controller.next())

#endif
