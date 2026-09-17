#ifndef DECODER_H
#define DECODER_H

#include <QObject>
#include "libfydelis-codecs/codec_registro.h"
#include "libfydelis-codecs/decodificador.h"
#include "libfydelis-codecs/amostras.h"
#include "media_source.h"

class Decoder : public QObject
{
    Q_OBJECT
public:
    explicit Decoder(QObject *parent = nullptr);
    ~Decoder();

    bool setSource(MediaSource *src);
    bool initializeCodec(const QString &hint = QString());
    void decodeFrame();
    void reset();
    QString currentCodec() const { return m_decoder ? m_decoder->nome() : "none"; }

signals:
    void audioReady(const Fydelis::Codecs::Amostras &);
    void videoFrame(const QImage &);
    void endOfStream();
    void decodeError(const QString &msg);

private:
    MediaSource *m_source = nullptr;
    Fydelis::Codecs::DecodificadorBase *m_decoder = nullptr;
    QByteArray m_headerBuffer;
    bool m_initialized = false;
};

#endif