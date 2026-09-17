// ==================================================
// FydelisTech — Fonte de Mídia
// © 2026 FydelisTech. Todos os direitos reservados.
// ==================================================
#ifndef MEDIA_SOURCE_H
#define MEDIA_SOURCE_H

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QNetworkReply>

class MediaSource : public QObject
{
    Q_OBJECT
public:
    explicit MediaSource(QObject *parent = nullptr);
    ~MediaSource();

    enum class Type { None, LocalFile, NetworkStream, YouTube };
    bool open(const QString &location);
    void close();
    QByteArray read(qint64 maxSize = 8192);
    bool seek(qint64 position);
    qint64 size() const { return m_totalSize; }
    qint64 position() const { return m_position; }
    Type type() const { return m_type; }
    QString location() const { return m_location; }
    bool isReady() const;

signals:
    void readyForData();
    void errorOccurred(const QString &message);
    void progressUpdated(qint64 received, qint64 total);
    void streamResolved(const QString &directUrl, const QString &format);

private slots:
    void onNetworkData();
    void onNetworkFinished();
    void onNetworkError(QNetworkReply::NetworkError);

private:
    Type m_type = Type::None;
    QString m_location;
    QFile m_file;
    QNetworkReply *m_reply = nullptr;
    QByteArray m_buffer;
    qint64 m_totalSize = 0;
    qint64 m_position = 0;
};

#endif