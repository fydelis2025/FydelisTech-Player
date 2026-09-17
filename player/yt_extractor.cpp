#include "yt_extractor.h"
#include <QNetworkAccessManager>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static QNetworkAccessManager* nam() {
    static QNetworkAccessManager* m = nullptr;
    if (!m) m = new QNetworkAccessManager;
    return m;
}

YTExtractor::YTExtractor(QObject *parent) : QObject(parent) {}

YTExtractor::~YTExtractor() {
    if (m_reply) m_reply->deleteLater();
}

QString YTExtractor::extractVideoId(const QString &url) {
    QRegularExpression re(R"((?:youtube\.com/watch\?v=|youtu\.be/|youtube\.com/embed/)([A-Za-z0-9_-]{11}))");
    auto match = re.match(url);
    return match.hasMatch() ? match.captured(1) : QString();
}

void YTExtractor::extract(const QString &url) {
    QString id = extractVideoId(url);
    if (id.isEmpty()) {
        emit failed("ID do vídeo não encontrado na URL");
        return;
    }
    // Estrutura pronta — implementação de busca da página/API em desenvolvimento
    emit failed("Resolvedor YouTube: módulo de extração em desenvolvimento — use link direto para teste");
}

// ✅ Funções que estavam faltando — implementações vazias (serão preenchidas depois)
void YTExtractor::onVideoPage(QNetworkReply*) {
}

void YTExtractor::onManifest(QNetworkReply*) {
}