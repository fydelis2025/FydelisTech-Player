// ==================================================
// FydelisTech — libfydelis-codecs
// © 2026 FydelisTech. Todos os direitos reservados.
// ==================================================

#include "libfydelis-codecs/codec_registro.h"
#include "libfydelis-codecs/codecs/wav.h"
#include "libfydelis-codecs/codecs/mp3.h"
#include "libfydelis-codecs/codecs/mjpeg.h"

namespace Fydelis {
namespace Codecs {

CodecRegistro& CodecRegistro::instancia() {
    static CodecRegistro inst;
    return inst;
}

void CodecRegistro::registrar(DecodificadorBase* inst) {
    _fabrica[inst->nome()] = inst;
}

DecodificadorBase* CodecRegistro::criar(const QString &nome) {
    if (_fabrica.contains(nome))
        return _fabrica[nome];
    return nullptr;
}

QStringList CodecRegistro::listarNomes() const {
    return _fabrica.keys();
}

void registrarTodosCodecs() {
    auto &reg = CodecRegistro::instancia();
    reg.registrar(new DecodWAV);
    reg.registrar(new DecodMP3);
    reg.registrar(new DecodMJPEG);
}

} // namespace Codecs
} // namespace Fydelis