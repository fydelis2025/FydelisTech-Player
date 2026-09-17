#ifndef YT_EXTRACTOR_H
#define YT_EXTRACTOR_H

#include <QObject>
#include <QNetworkReply>

class YTExtractor : public QObject
{
    Q_OBJECT
public:
    explicit YTExtractor(QObject *parent = nullptr);
    ~YTExtractor();  // ✅ Destrutor

    void extract(const QString &url);

signals:
    void resolved(const QString &directUrl, const QString &title, const QString &format);
    void failed(const QString &reason);

private slots:
    void onVideoPage(QNetworkReply*);
    void onManifest(QNetworkReply*);

private:
    QString extractVideoId(const QString &url);
    QNetworkReply *m_reply = nullptr;
};

#endif