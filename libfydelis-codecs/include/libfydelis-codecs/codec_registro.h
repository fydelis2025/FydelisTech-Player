// ==================================================
// libfydelis-codecs — Registro e Fábrica de Codecs
// Permite adicionar codecs sem alterar o resto do sistema
// ==================================================
#ifndef LIBFYDELIS_CODECS_REGISTRO_H
#define LIBFYDELIS_CODECS_REGISTRO_H

#include "decodificador.h"
#include <QMap>
#include <QStringList>

namespace Fydelis {
namespace Codecs {

class CodecRegistro
{
public:
    static CodecRegistro& instancia();

    // Registra um novo codec
    void registrar(DecodificadorBase* instancia);

    // Cria decodificador pelo nome (ex: "wav")
    DecodificadorBase* criar(const QString &nome);

    // Lista todos os codecs disponíveis
    QStringList listarNomes() const;

private:
    CodecRegistro() = default;
    QMap<QString, DecodificadorBase*> _fabrica;
};

// Chamar uma vez para disponibilizar todos os codecs
void registrarTodosCodecs();

} // namespace Codecs
} // namespace Fydelis

#endif // LIBFYDELIS_CODECS_REGISTRO_H