// ==================================================
// FydelisTech — Decoder (Corrigido para Alinhamento de Áudio)
// ==================================================
#include "decoder.h"
using namespace Fydelis::Codecs;

Decoder::Decoder(QObject *parent) : QObject(parent) {
    registrarTodosCodecs();
}

Decoder::~Decoder() { delete m_decoder; }

bool Decoder::setSource(MediaSource *src) {
    m_source = src;
    m_headerBuffer.clear();
    m_initialized = false;
    return true;
}

bool Decoder::initializeCodec(const QString &hint) {
    if (!m_source || m_source->type() == MediaSource::Type::None) {
        emit decodeError("Nenhuma fonte de mídia conectada");
        return false;
    }

    QString codecName = hint;
    if (codecName.isEmpty()) {
        QString path = m_source->location().toLower();
        if (path.contains(".")) codecName = path.section('.', -1);
    }

    delete m_decoder;
    m_decoder = CodecRegistro::instancia().criar(codecName);
    if (!m_decoder) {
        emit decodeError("Codec não suportado: " + codecName);
        return false;
    }

    // ✅ Lê exatamente 44 bytes (tamanho padrão do cabeçalho WAV) para não corromper o PCM
    m_headerBuffer.clear();
    int headerSize = (codecName == "wav") ? 44 : 128;
    
    while (m_headerBuffer.size() < headerSize && m_source->isReady()) {
        QByteArray chunk = m_source->read(headerSize - m_headerBuffer.size());
        if (chunk.isEmpty()) break;
        m_headerBuffer.append(chunk);
    }

    if (!m_decoder->inicializar(m_headerBuffer)) {
        emit decodeError("Falha ao inicializar: " + codecName);
        delete m_decoder;
        m_decoder = nullptr;
        return false;
    }

    m_initialized = true;
    return true;
}

void Decoder::decodeFrame() {
    if (!m_initialized || !m_decoder || !m_source) return;

    QByteArray data = m_source->read(4096);
    if (data.isEmpty()) {
        emit endOfStream();
        return;
    }

    Amostras samples;
    QImage frame;
    if (m_decoder->decodificar(data, frame, samples)) {
        if (!frame.isNull()) emit videoFrame(frame);
        if (!samples.esquerdo.isEmpty()) emit audioReady(samples);
    }
}

void Decoder::reset() {
    if (m_decoder) m_decoder->reiniciar();
    m_headerBuffer.clear();
    m_initialized = false;
}