#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <QObject>
#include <QAudioSink>
#include <QIODevice>
#include <QByteArray>
#include <QMutex>
#include "libfydelis-codecs/amostras.h"

class AudioBufferDevice : public QIODevice {
    Q_OBJECT
public:
    AudioBufferDevice(QObject *parent = nullptr) : QIODevice(parent) {}
    
    void appendData(const QByteArray &data) {
        QMutexLocker locker(&m_mutex);
        m_buffer.append(data);
    }

    qint64 readData(char *data, qint64 maxlen) override {
        QMutexLocker locker(&m_mutex);
        if (m_buffer.isEmpty()) return 0;
        qint64 chunk = qMin(maxlen, (qint64)m_buffer.size());
        memcpy(data, m_buffer.constData(), chunk);
        m_buffer.remove(0, chunk);
        return chunk;
    }

    qint64 writeData(const char *, qint64) override { return 0; }
    qint64 bytesAvailable() const override {
        QMutexLocker locker(&m_mutex);
        return m_buffer.size() + QIODevice::bytesAvailable();
    }

    void clear() {
        QMutexLocker locker(&m_mutex);
        m_buffer.clear();
    }

private:
    mutable QMutex m_mutex;
    QByteArray m_buffer;
};

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject *parent = nullptr);
    ~AudioOutput();

    bool start(int sampleRate, int channels);
    void stop();
    void feed(const Fydelis::Codecs::Amostras &samples);
    bool isActive() const;
    
    void setVolume(float volume); // 0.0 a 1.0
    float volume() const { return m_volume; }

private:
    QAudioSink *m_audio = nullptr;
    AudioBufferDevice *m_device = nullptr;
    float m_volume = 1.0f;
};

#endif