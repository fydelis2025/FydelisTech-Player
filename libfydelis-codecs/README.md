# libfydelis-codecs

> Biblioteca de codecs de áudio e vídeo 100% implementada pela FydelisTech.
> Sem dependências externas — sem FFmpeg, sem libVLC, sem libavcodec.

## Status dos Codecs

| Codec | Tipo | Status |
|---|---|---|
| **WAV/PCM** | Áudio | ✅ Pronto |
| **MP3** | Áudio | 🚧 Em implementação |
| **MJPEG** | Vídeo | 🚧 Em implementação |

## Uso Básico

```cpp
#include <libfydelis-codecs/codec_registro.h>
#include <libfydelis-codecs/codecs/wav.h>

using namespace Fydelis::Codecs;

int main() {
    registrarTodosCodecs();

    auto *dec = CodecRegistro::instancia().criar("wav");
    if (!dec) return 1;

    // Inicializar com cabeçalho do arquivo
    dec->inicializar(cabecalhoWAV);

    // Decodificar dados
    Amostras audio;
    QImage video;
    dec->decodificar(dadosComprimidos, video, audio);

    delete dec;
    return 0;
}