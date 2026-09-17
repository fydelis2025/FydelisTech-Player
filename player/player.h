#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QTimer>
#include "media_source.h"
#include "decoder.h"
#include "audio_output.h"
#include "yt_extractor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Player; }
QT_END_NAMESPACE

class FydelisTechPlayer : public QMainWindow
{
    Q_OBJECT
public:
    FydelisTechPlayer(QWidget *parent = nullptr);
    ~FydelisTechPlayer();

private slots:
    void openLocalFile();
    void openUrl();
    void play();
    void pause();
    void stop();
    void tick();
    void onSourceReady();
    void onSourceError(const QString &msg);
    void onAudioSamples(const Fydelis::Codecs::Amostras &samples);
    void onVideoFrame(const QImage &frame);
    void onEndOfStream();
    void onYtResolved(const QString &directUrl, const QString &title, const QString &format);
    void onYtFailed(const QString &reason);
	void onVolumeChanged(int value);

private:
    Ui::Player *ui;
    MediaSource *m_source;
    Decoder *m_decoder;
    AudioOutput *m_audio;
    YTExtractor *m_ytExtractor;
    QTimer *m_timer;
    bool m_playing = false;
    int m_timeMs = 0;

    void loadStyle();
    void connectAll();
    void updateUI();
    QString formatTime(int ms);
};

#endif