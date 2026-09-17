// ==================================================
// libfydelis-codecs — Interface Base do Decodificador
// Versão: 1.0.0 | 100% código próprio
// ==================================================
#ifndef LIBFYDELIS_CODECS_DECODIFICADOR_H
#define LIBFYDELIS_CODECS_DECODIFICADOR_H

#include "amostras.h"
#include <QByteArray>
#include <QImage>
#include <QString>

namespace Fydelis {
namespace Codecs {

class DecodificadorBase
{
public:
    virtual ~DecodificadorBase();

    // Nome único do codec (ex: "wav", "mp3", "h264")
    virtual QString nome() const = 0;

    // Descrição amigável para interface
    virtual QString descricao() const = 0;

    // Chamado antes de começar → configura o decodificador
    virtual bool inicializar(const QByteArray &config) = 0;

    // Dados comprimidos → sai quadro de vídeo e/ou amostras de áudio
    virtual bool decodificar(const QByteArray &entrada,
                              QImage &quadroVideo,
                              Amostras &saidaAudio) = 0;

    // Volta ao estado inicial (busca, retorno)
    virtual void reiniciar() = 0;
};

} // namespace Codecs
} // namespace Fydelis

#endif // LIBFYDELIS_CODECS_DECODIFICADOR_H