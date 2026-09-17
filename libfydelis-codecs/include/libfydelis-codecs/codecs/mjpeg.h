// ==================================================
// Decodificador MJPEG — Quadros JPEG sequenciais
// 100% código próprio, sem libjpeg
// ==================================================
#ifndef LIBFYDELIS_CODECS_MJPEG_H
#define LIBFYDELIS_CODECS_MJPEG_H

#include "../decodificador.h"
#include <QVector>

namespace Fydelis {
namespace Codecs {

class DecodMJPEG : public DecodificadorBase
{
public:
    QString nome() const override { return QStringLiteral("mjpeg"); }
    QString descricao() const override { return QStringLiteral("Vídeo Motion JPEG"); }

    bool inicializar(const QByteArray &config) override;
    bool decodificar(const QByteArray &entrada, QImage &quadro, Amostras &) override;
    void reiniciar() override;

private:
    // Estrutura do JPEG
    struct Componente {
        int largura, altura;
        int fatorH, fatorV;
        int tabelaQuant;
        int tabelaHuffDC;
        int tabelaHuffAC;
        QVector<float> amostras; // 8x8 blocos convertidos
    };

    bool lerMarcador(const quint8 *&ptr, int &tamanho);
    bool decodificarJPEG(const QByteArray &dados, QImage &saida);
    bool reconstruirBloco(Componente &comp, int x, int y);

    int _largura = 0;
    int _altura = 0;
    QVector<Componente> _componentes;
    bool _inicializado = false;
};

} // namespace Codecs
} // namespace Fydelis

#endif // LIBFYDELIS_CODECS_MJPEG_H