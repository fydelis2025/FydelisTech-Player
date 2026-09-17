// ==================================================
// libfydelis-codecs — Dados de Áudio Decodificado
// ==================================================
#ifndef LIBFYDELIS_CODECS_AMOSTRAS_H
#define LIBFYDELIS_CODECS_AMOSTRAS_H

#include <QVector>

namespace Fydelis {
namespace Codecs {

struct Amostras
{
    QVector<float> esquerdo;   // Canal esquerdo / mono
    QVector<float> direito;    // Canal direito (vazio se mono)
    int taxa;                   // Amostras por segundo (ex: 44100)
    int canais;                 // 1 = mono, 2 = estéreo

    void limpar() {
        esquerdo.clear();
        direito.clear();
        taxa = 0;
        canais = 0;
    }
};

} // namespace Codecs
} // namespace Fydelis

#endif // LIBFYDELIS_CODECS_AMOSTRAS_H