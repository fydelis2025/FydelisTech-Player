# ==================================================
# FydelisTech-Player — Projeto Unificado
# © 2026 FydelisTech. Todos os direitos reservados.
# Áudio MP3 + YouTube — Funcionando no Windows! 🎵
# ==================================================
cmake_minimum_required(VERSION 3.16)
project(FydelisTech-Player VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Multimedia)

# ✅ Descobrir caminho raiz do Qt de forma confiável
get_target_property(Qt6Core_LOCATION Qt6::Core LOCATION)
get_filename_component(Qt6_BIN_DIR ${Qt6Core_LOCATION} DIRECTORY)
get_filename_component(Qt6_ROOT ${Qt6_BIN_DIR}/.. ABSOLUTE)
set(QT_PLUGINS_DIR "${Qt6_ROOT}/plugins")
message(STATUS "Qt plugins em: ${QT_PLUGINS_DIR}")

# Compilar a biblioteca primeiro
add_subdirectory(libfydelis-codecs)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libfydelis-codecs/include
    ${CMAKE_CURRENT_SOURCE_DIR}/player
)

set(PLAYER_SRC
    player/main.cpp
    player/player.cpp
    player/media_source.cpp
    player/decoder.cpp
    player/audio_output.cpp
    player/yt_extractor.cpp
)

set(PLAYER_HEADERS
    player/player.h
    player/media_source.h
    player/decoder.h
    player/audio_output.h
    player/yt_extractor.h
)

set(PLAYER_UI player/player.ui)
set(PLAYER_RES resources.qrc)

# ✅ Apenas arquivos padrão aqui (removida a linha IDI_ICON incorreta)
add_executable(FydelisTech-Player
    ${PLAYER_SRC}
    ${PLAYER_HEADERS}
    ${PLAYER_UI}
    ${PLAYER_RES}
	player.rc
)

target_link_libraries(FydelisTech-Player
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Multimedia
        fydelis-codecs
)

if(WIN32)
    set_target_properties(FydelisTech-Player PROPERTIES
        WIN32_EXECUTABLE TRUE
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    # 📋 Copiar DLLs do Qt e da biblioteca codec para a pasta do executável
    add_custom_command(TARGET FydelisTech-Player POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Core> $<TARGET_FILE_DIR:FydelisTech-Player>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Gui> $<TARGET_FILE_DIR:FydelisTech-Player>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Widgets> $<TARGET_FILE_DIR:FydelisTech-Player>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Network> $<TARGET_FILE_DIR:FydelisTech-Player>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Multimedia> $<TARGET_FILE_DIR:FydelisTech-Player>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:fydelis-codecs> $<TARGET_FILE_DIR:FydelisTech-Player>
        COMMENT "✅ DLLs principais e biblioteca copiadas"
    )

    # ✅ Copiar plugins de Plataforma
    add_custom_command(TARGET FydelisTech-Player POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${QT_PLUGINS_DIR}/platforms"
            "$<TARGET_FILE_DIR:FydelisTech-Player>/platforms"
        COMMENT "✅ Plataformas copiadas"
    )

    # ✅ Copiar plugins de Multimídia
    add_custom_command(TARGET FydelisTech-Player POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${QT_PLUGINS_DIR}/multimedia"
            "$<TARGET_FILE_DIR:FydelisTech-Player>/multimedia"
        COMMENT "✅ Plugins de multimídia copiados"
    )

    # ✅ Copiar plugins genéricos
    add_custom_command(TARGET FydelisTech-Player POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${QT_PLUGINS_DIR}/generic"
            "$<TARGET_FILE_DIR:FydelisTech-Player>/generic"
        COMMENT "✅ Plugins genéricos copiados"
    )
endif()