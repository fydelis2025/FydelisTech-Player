#include "media_source.h"
#include <QNetworkAccessManager>
#include <QRegularExpression>

static QNetworkAccessManager* getNAM() {
    static QNetworkAccessManager* nam = nullptr;
    if (!nam) nam = new QNetworkAccessManager;
    return nam;
}

MediaSource::MediaSource(QObject *parent) : QObject(parent) {}
MediaSource::~MediaSource() { close(); }

bool MediaSource::open(const QString &loc) {
    close();
    m_location = loc;

    // Detectar YouTube
    if (loc.contains(QRegularExpression(R"(youtube\.com|youtu\.be)"))) {
        m_type = Type::YouTube;
        emit readyForData();
        return true;
    }

    // Detectar URL de rede
    if (loc.startsWith("http://") || loc.startsWith("https://")) {
        m_type = Type::NetworkStream;
        QUrl url(loc);                           // ✅ Separado = sem confusão
        QNetworkRequest req(url);                 // ✅ Agora claro
        m_reply = getNAM()->get(req);             // ✅ Compila certinho
        connect(m_reply, &QNetworkReply::readyRead, this, &MediaSource::onNetworkData);
        connect(m_reply, &QNetworkReply::finished, this, &MediaSource::onNetworkFinished);
        connect(m_reply, &QNetworkReply::errorOccurred, this, &MediaSource::onNetworkError);
        return true;
    }

    // Arquivo local
    m_type = Type::LocalFile;
    m_file.setFileName(loc);
    if (!m_file.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Não foi possível abrir: " + loc);
        return false;
    }
    m_totalSize = m_file.size();
    m_position = 0;
    emit readyForData();
    return true;
}

void MediaSource::close() {
    if (m_reply) { m_reply->abort(); m_reply->deleteLater(); m_reply = nullptr; }
    m_file.close();
    m_buffer.clear();
    m_type = Type::None;
    m_location.clear();
    m_totalSize = m_position = 0;
}

QByteArray MediaSource::read(qint64 maxSize) {
    if (m_type == Type::LocalFile) {
        QByteArray data = m_file.read(maxSize);
        m_position += data.size();
        return data;
    }
    if (!m_buffer.isEmpty()) {
        qint64 take = qMin(maxSize, (qint64)m_buffer.size());
        QByteArray data = m_buffer.left(take);
        m_buffer = m_buffer.mid(take);
        m_position += take;
        return data;
    }
    return {};
}

bool MediaSource::seek(qint64 pos) {
    if (m_type == Type::LocalFile && m_file.seek(pos)) {
        m_position = pos;
        return true;
    }
    return false;
}

bool MediaSource::isReady() const {
    return (m_type == Type::LocalFile) || !m_buffer.isEmpty();
}

void MediaSource::onNetworkData() {
    if (!m_totalSize && m_reply->header(QNetworkRequest::ContentLengthHeader).isValid())
        m_totalSize = m_reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    m_buffer.append(m_reply->readAll());
    emit readyForData();
    emit progressUpdated(m_position + m_buffer.size(), m_totalSize);
}

void MediaSource::onNetworkFinished() {
    emit progressUpdated(m_position, m_totalSize);
}

void MediaSource::onNetworkError(QNetworkReply::NetworkError) {
    emit errorOccurred(m_reply->errorString());
}