// ==================================================
// FydelisTech — Saída de Áudio (Blindada contra ruído)
// ==================================================
#include "audio_output.h"
#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QtGlobal>

AudioOutput::AudioOutput(QObject *parent) : QObject(parent) {}

AudioOutput::~AudioOutput() { stop(); }

bool AudioOutput::start(int sampleRate, int channels) {
    stop();

    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate > 0 ? sampleRate : 44100);
    fmt.setChannelCount(channels > 0 ? channels : 2);
    fmt.setSampleFormat(QAudioFormat::Int16);

    QAudioDevice saida = QMediaDevices::defaultAudioOutput();
    if (!saida.isFormatSupported(fmt)) {
        fmt = saida.preferredFormat();
    }

    m_audio = new QAudioSink(saida, fmt, this);
    m_audio->setVolume(m_volume);
    
    m_device = new AudioBufferDevice(this);
    m_device->open(QIODevice::ReadOnly);

    m_audio->start(m_device);
    return m_audio->state() != QAudio::StoppedState;
}

void AudioOutput::stop() {
    if (m_audio) {
        m_audio->stop();
        delete m_audio;
        m_audio = nullptr;
    }
    m_device = nullptr;
}

void AudioOutput::feed(const Fydelis::Codecs::Amostras &samples) {
    if (!m_audio || m_audio->state() == QAudio::StoppedState || !m_device) return;

    QAudioFormat fmt = m_audio->format();
    int canais = fmt.channelCount();

    int totalAmostras = samples.esquerdo.size();
    if (totalAmostras == 0) return;

    QByteArray out;
    out.reserve(totalAmostras * 2 * canais);

    for (int i = 0; i < totalAmostras; ++i) {
        // Limita o ganho com qBound para evitar clipping e estouro de buffer (chiado)
        float l = qBound(-1.0f, samples.esquerdo[i] * m_volume, 1.0f);
        qint16 sl = static_cast<qint16>(l * 32767.0f);
        out.append(reinterpret_cast<const char*>(&sl), 2);

        if (canais == 2) {
            float r = (i < samples.direito.size()) ? samples.direito[i] : samples.esquerdo[i];
            r = qBound(-1.0f, r * m_volume, 1.0f);
            qint16 sr = static_cast<qint16>(r * 32767.0f);
            out.append(reinterpret_cast<const char*>(&sr), 2);
        }
    }

    if (!out.isEmpty()) {
        m_device->appendData(out);
    }
}

bool AudioOutput::isActive() const {
    if (!m_audio) return false;
    return (m_audio->state() == QAudio::ActiveState || m_audio->state() == QAudio::IdleState);
}

void AudioOutput::setVolume(float volume) {
    m_volume = qBound(0.0f, volume, 1.0f);
    if (m_audio) {
        m_audio->setVolume(m_volume);
    }
}