// ==================================================
// Decodificador WAV/PCM — FydelisTech OS
// ==================================================
#include "libfydelis-codecs/codecs/wav.h"
#include <cstring>

namespace Fydelis {
namespace Codecs {

#pragma pack(push, 1)
struct CabecalhoWAV {
    char     riff[4];
    quint32  tamArquivo;
    char     wave[4];
    char     fmt_[4];
    quint32  tamFmt;
    quint16  tipo;
    quint16  canais;
    quint32  taxa;
    quint32  bytesPorSec;
    quint16  alinhamento;
    quint16  bitsPorAmostra;
};
#pragma pack(pop)

bool DecodWAV::inicializar(const QByteArray &dados)
{
    if (dados.size() < (int)sizeof(CabecalhoWAV))
        return false;

    const CabecalhoWAV *h = reinterpret_cast<const CabecalhoWAV*>(dados.constData());

    // Valida assinaturas essenciais do formato PCM
    if (std::memcmp(h->riff, "RIFF", 4) != 0 ||
        std::memcmp(h->wave, "WAVE", 4) != 0 ||
        std::memcmp(h->fmt_, "fmt ", 4) != 0 ||
        h->tipo != 1) // 1 = PCM sem compressão
        return false;

    _canais = h->canais;
    _taxa = h->taxa;
    _bitsPorAmostra = h->bitsPorAmostra;
    return true;
}

bool DecodWAV::decodificar(const QByteArray &dados, QImage &, Amostras &saida)
{
    saida.limpar();
    saida.taxa = _taxa > 0 ? _taxa : 44100;
    saida.canais = _canais > 0 ? _canais : 2;

    const int bytesPorAmostra = _bitsPorAmostra > 0 ? (_bitsPorAmostra / 8) : 2;
    const int amostras = dados.size() / bytesPorAmostra / _canais;

    if (amostras <= 0) return false;

    saida.esquerdo.resize(amostras);
    const float fator = 1.0f / 32768.0f;

    if (_canais == 1) {
        const qint16 *p = reinterpret_cast<const qint16*>(dados.constData());
        for (int i = 0; i < amostras; i++)
            saida.esquerdo[i] = static_cast<float>(p[i]) * fator;
    } else {
        saida.direito.resize(amostras);
        const qint16 *p = reinterpret_cast<const qint16*>(dados.constData());
        for (int i = 0; i < amostras; i++) {
            saida.esquerdo[i] = static_cast<float>(p[i * 2 + 0]) * fator;
            saida.direito[i]  = static_cast<float>(p[i * 2 + 1]) * fator;
        }
    }
    return true;
}

void DecodWAV::reiniciar() {}

} // namespace Codecs
} // namespace Fydelis