// ==================================================
// Decodificador WAV/PCM — 100% nosso código
// Sem compressão → referência e base de testes
// ==================================================
#ifndef LIBFYDELIS_CODECS_WAV_H
#define LIBFYDELIS_CODECS_WAV_H

#include "../decodificador.h"

namespace Fydelis {
namespace Codecs {

class DecodWAV : public DecodificadorBase
{
public:
    QString nome() const override { return QStringLiteral("wav"); }
    QString descricao() const override { return QStringLiteral("Áudio WAV / PCM"); }

    bool inicializar(const QByteArray &cabecalho) override;
    bool decodificar(const QByteArray &dados, QImage &, Amostras &saida) override;
    void reiniciar() override;

private:
    int _taxa = 0;
    int _canais = 0;
    int _bitsPorAmostra = 0;
};

} // namespace Codecs
} // namespace Fydelis

#endif // LIBFYDELIS_CODECS_WAV_H