// ==================================================
// FydelisTech-Player — Implementação Principal
// ==================================================
#include "player.h"
#include "ui_player.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QScreen>
#include <QDebug>

FydelisTechPlayer::FydelisTechPlayer(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Player)
{
    ui->setupUi(this);
    resize(850, 520);
    move((QApplication::primaryScreen()->geometry().width() - width())/2,
         (QApplication::primaryScreen()->geometry().height() - height())/2);

    loadStyle();
    setWindowTitle("FydelisTech-Player v1.0 — FydelisTech OS");
	setWindowIcon(QIcon(":/fydelis/resources/icons/player.png"));

    m_source = new MediaSource(this);
    m_decoder = new Decoder(this);
    m_audio = new AudioOutput(this);
    m_ytExtractor = new YTExtractor(this);
    m_timer = new QTimer(this);
    m_timer->setInterval(33);

    connectAll();
    
    // Inicializa o volume do áudio com o valor atual do slider da interface
    if (ui->sliderVolume) {
        m_audio->setVolume(static_cast<float>(ui->sliderVolume->value()) / 100.0f);
    }

    ui->statusbar->showMessage("Sistema pronto — Codecs: " +
                               Fydelis::Codecs::CodecRegistro::instancia().listarNomes().join(", "));
}

FydelisTechPlayer::~FydelisTechPlayer() { stop(); delete ui; }

void FydelisTechPlayer::loadStyle() {
    QFile f(":/fydelis/style.qss");
    if (f.open(QFile::ReadOnly | QFile::Text))
        setStyleSheet(QString::fromUtf8(f.readAll()));
}

void FydelisTechPlayer::connectAll() {
    connect(ui->btnLocal, &QPushButton::clicked, this, &FydelisTechPlayer::openLocalFile);
    connect(ui->btnUrl, &QPushButton::clicked, this, &FydelisTechPlayer::openUrl);
    connect(ui->btnPlay, &QPushButton::clicked, this, &FydelisTechPlayer::play);
    connect(ui->btnPause, &QPushButton::clicked, this, &FydelisTechPlayer::pause);
    connect(ui->btnStop, &QPushButton::clicked, this, &FydelisTechPlayer::stop);

    // ✅ Conexão do Slider de Volume da interface
    if (ui->sliderVolume) {
        connect(ui->sliderVolume, &QSlider::valueChanged, this, &FydelisTechPlayer::onVolumeChanged);
    }

    connect(m_source, &MediaSource::readyForData, this, &FydelisTechPlayer::onSourceReady);
    connect(m_source, &MediaSource::errorOccurred, this, &FydelisTechPlayer::onSourceError);

    connect(m_decoder, &Decoder::audioReady, this, &FydelisTechPlayer::onAudioSamples);
    connect(m_decoder, &Decoder::videoFrame, this, &FydelisTechPlayer::onVideoFrame);
    connect(m_decoder, &Decoder::decodeError, this, &FydelisTechPlayer::onSourceError);
    connect(m_decoder, &Decoder::endOfStream, this, &FydelisTechPlayer::onEndOfStream);

    connect(m_ytExtractor, &YTExtractor::resolved, this, &FydelisTechPlayer::onYtResolved);
    connect(m_ytExtractor, &YTExtractor::failed, this, &FydelisTechPlayer::onYtFailed);

    connect(m_timer, &QTimer::timeout, this, &FydelisTechPlayer::tick);
}

void FydelisTechPlayer::openLocalFile() {
    QString path = QFileDialog::getOpenFileName(
        this, "Abrir Arquivo", "",
        "Suportados (*.wav *.mp3 *.mjpeg);;Todos (*.*)"
    );
    if (path.isEmpty()) return;
    stop();
    if (!m_source->open(path)) return;
    if (!m_decoder->setSource(m_source) || !m_decoder->initializeCodec()) {
        onSourceError("Não foi possível carregar o codec");
        return;
    }
    ui->lblFile->setText(path.section('/', -1));
    m_timeMs = 0;
    updateUI();
}

void FydelisTechPlayer::openUrl() {
    QString url = QInputDialog::getText(this, "Abrir Endereço", "URL / YouTube:");
    if (url.isEmpty()) return;
    stop();

    if (url.contains("youtube.com") || url.contains("youtu.be")) {
        ui->lblFile->setText("Resolvendo YouTube...");
        m_ytExtractor->extract(url);
        return;
    }

    if (!m_source->open(url)) return;
    if (!m_decoder->setSource(m_source) || !m_decoder->initializeCodec()) {
        onSourceError("Codec não detectado");
        return;
    }
    ui->lblFile->setText(url);
    m_timeMs = 0;
    updateUI();
}

void FydelisTechPlayer::onYtResolved(const QString &directUrl, const QString &title, const QString &format) {
    if (!m_source->open(directUrl)) return;
    if (!m_decoder->setSource(m_source) || !m_decoder->initializeCodec()) {
        onSourceError("Codec do fluxo não suportado");
        return;
    }
    ui->lblFile->setText(title + " [" + format + "]");
    m_timeMs = 0;
    updateUI();
    play();
}

void FydelisTechPlayer::onYtFailed(const QString &reason) {
    QMessageBox::warning(this, "YouTube", reason);
    ui->lblFile->setText("Falha na resolução");
}

void FydelisTechPlayer::play() {
    if (m_source->type() == MediaSource::Type::None) return;
    m_playing = true;
    m_timer->start();
    
    // Garante que o volume atual está sincronizado antes de iniciar o fluxo
    if (ui->sliderVolume && m_audio) {
        m_audio->setVolume(static_cast<float>(ui->sliderVolume->value()) / 100.0f);
    }
    
    m_audio->start(44100, 2);
    updateUI();
}

void FydelisTechPlayer::pause() {
    m_playing = false;
    m_timer->stop();
    m_audio->stop();
    updateUI();
}

void FydelisTechPlayer::stop() {
    m_playing = false;
    m_timer->stop();
    m_audio->stop();
    m_source->close();
    m_decoder->reset();
    m_timeMs = 0;
    ui->lblFile->setText("Nenhum arquivo carregado");
    ui->lblVideo->clear();
    ui->lblTime->setText("00:00");
    updateUI();
}

void FydelisTechPlayer::tick() {
    if (!m_playing) return;
    m_decoder->decodeFrame();
    m_timeMs += 33;
    ui->lblTime->setText(formatTime(m_timeMs));
}

void FydelisTechPlayer::onSourceReady() {
    ui->statusbar->showMessage("Recebendo dados...");
}

void FydelisTechPlayer::onSourceError(const QString &msg) {
    QMessageBox::warning(this, "Erro", msg);
    pause();
}

void FydelisTechPlayer::onAudioSamples(const Fydelis::Codecs::Amostras &samples) {
    m_audio->feed(samples);
}

void FydelisTechPlayer::onVideoFrame(const QImage &frame) {
    qDebug() << "[Video] Frame recebido! Tamanho:" << frame.size() << "IsNull:" << frame.isNull();
    if (frame.isNull()) return;
    ui->lblVideo->setPixmap(QPixmap::fromImage(frame).scaled(
        ui->lblVideo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// ✅ Slot para controlar o volume em tempo real através do slider
void FydelisTechPlayer::onVolumeChanged(int value) {
    float vol = static_cast<float>(value) / 100.0f;
    if (m_audio) {
        m_audio->setVolume(vol);
    }
}

void FydelisTechPlayer::onEndOfStream() {
    pause();
    ui->statusbar->showMessage("Fim da reprodução");
}

void FydelisTechPlayer::updateUI() {
    ui->btnPlay->setEnabled(!m_playing && m_source->isReady());
    ui->btnPause->setEnabled(m_playing);
    ui->btnStop->setEnabled(m_source->type() != MediaSource::Type::None);
}

QString FydelisTechPlayer::formatTime(int ms) {
    int s = ms / 1000, m = s / 60; s %= 60;
    return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}