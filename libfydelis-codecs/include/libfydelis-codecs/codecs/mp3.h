// ==================================================
// Decodificador MP3 — Implementação própria
// Baseado na especificação ISO/IEC 11172-3
// ==================================================
#ifndef LIBFYDELIS_CODECS_MP3_H
#define LIBFYDELIS_CODECS_MP3_H

#include "../decodificador.h"
#include <QVector>

namespace Fydelis {
namespace Codecs {

class DecodMP3 : public DecodificadorBase
{
public:
    QString nome() const override { return QStringLiteral("mp3"); }
    QString descricao() const override { return QStringLiteral("Áudio MPEG Layer III"); }

    bool inicializar(const QByteArray &config) override;
    bool decodificar(const QByteArray &entrada, QImage &, Amostras &saida) override;
    void reiniciar() override;

private:
    // Estrutura do quadro MP3
    struct QuadroCabecalho {
        int taxaBits;
        int amostrasPorSegundo;
        int canais;
        int camada;      // 3 = MP3
        bool valido;
    };

    bool lerCabecalho(const quint8 *dados, QuadroCabecalho &info);
    void descomprimirQuadro(const quint8 *dados, Amostras &saida);

    QuadroCabecalho _formato;
    QVector<float> _filtroEsq, _filtroDir; // Filtros de síntese
    int _ultimoTaxa = 0;
};

} // namespace Codecs
} // namespace Fydelis

#endif // LIBFYDELIS_CODECS_MP3_H