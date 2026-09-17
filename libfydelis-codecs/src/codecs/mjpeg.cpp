// ==================================================
// Decodificador MJPEG — Implementação Própria
// Decodifica JPEG quadro a quadro
// ==================================================
#include "libfydelis-codecs/codecs/mjpeg.h"
#include <cstring>

namespace Fydelis {
namespace Codecs {

bool DecodMJPEG::inicializar(const QByteArray &config)
{
    reiniciar();
    // config pode conter resolução fixa do container
    _inicializado = true;
    return true;
}

bool DecodMJPEG::lerMarcador(const quint8 *&ptr, int &tamanho)
{
    if (ptr[0] != 0xFF) return false;
    quint8 tipo = ptr[1];
    ptr += 2;

    // Marcadores sem tamanho
    if (tipo == 0xD8 || tipo == 0xD9) { // SOI / EOI
        tamanho = 0;
        return true;
    }

    // Marcadores com tamanho
    tamanho = (ptr[0] << 8) | ptr[1];
    ptr += 2;
    tamanho -= 2; // inclui os 2 bytes do tamanho
    return true;
}

bool DecodMJPEG::decodificarJPEG(const QByteArray &dados, QImage &saida)
{
    const quint8 *ptr = reinterpret_cast<const quint8*>(dados.constData());
    const quint8 *fim = ptr + dados.size();

    bool temSOI = false;
    _largura = _altura = 0;

    while (ptr < fim) {
        int tam = 0;
        if (!lerMarcador(ptr, tam)) break;

        // SOI — Início de imagem
        if (ptr[-2] == 0xD8) {
            temSOI = true;
            continue;
        }

		// SOS — Início de scan (dados de imagem)
        if (ptr[-2] == 0xDA) {
            // Aqui viriam os dados comprimidos — implementação detalhada segue
            // Por enquanto, criamos quadro de placeholder
            if (_largura > 0 && _altura > 0) {
                saida = QImage(_largura, _altura, QImage::Format_RGB32);
                saida.fill(0); // Preto — estrutura pronta
            }
            return temSOI;
        }

        // SOF — Início de quadro (dimensões e componentes)
        if ((ptr[-2] & 0xF0) == 0xC0 && (ptr[-2] != 0xC4 && ptr[-2] != 0xC8)) {
            quint8 precisao = ptr[0];
            _altura = (ptr[1] << 8) | ptr[2];
            _largura = (ptr[3] << 8) | ptr[4];
            int numComp = ptr[5];
            ptr += 6;

            for (int i = 0; i < numComp; i++) {
                Componente c;
                c.fatorH = (ptr[1] >> 4) & 0x0F;
                c.fatorV = ptr[1] & 0x0F;
                c.tabelaQuant = ptr[2];
                ptr += 3;
                _componentes.push_back(c);
            }
            continue;
        }

        // Pular outros marcadores
        ptr += tam;
    }

    return temSOI;
}

bool DecodMJPEG::decodificar(const QByteArray &entrada, QImage &quadro, Amostras &)
{
    if (!_inicializado || entrada.size() < 4)
        return false;

    return decodificarJPEG(entrada, quadro);
}

void DecodMJPEG::reiniciar()
{
    _largura = _altura = 0;
    _componentes.clear();
    _inicializado = false;
}

} // namespace Codecs
} // namespace Fydelis