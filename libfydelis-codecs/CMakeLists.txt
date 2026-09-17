# ==================================================
# libfydelis-codecs — Biblioteca de Codecs
# © 2026 FydelisTech. Todos os direitos reservados.
# ==================================================
cmake_minimum_required(VERSION 3.16)
project(fydelis-codecs VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

set(RAIZ ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${RAIZ}/include)

set(SRC
    ${RAIZ}/src/decodificador.cpp
    ${RAIZ}/src/codec_registro.cpp
    ${RAIZ}/src/codecs/wav.cpp
    ${RAIZ}/src/codecs/mp3.cpp
    ${RAIZ}/src/codecs/mjpeg.cpp
)

set(PUBLIC_HEADERS
    ${RAIZ}/include/libfydelis-codecs/decodificador.h
    ${RAIZ}/include/libfydelis-codecs/amostras.h
    ${RAIZ}/include/libfydelis-codecs/codec_registro.h
    ${RAIZ}/include/libfydelis-codecs/codecs/wav.h
    ${RAIZ}/include/libfydelis-codecs/codecs/mp3.h
    ${RAIZ}/include/libfydelis-codecs/codecs/mjpeg.h
)

add_library(fydelis-codecs SHARED ${SRC} ${PUBLIC_HEADERS})

target_link_libraries(fydelis-codecs PUBLIC Qt6::Core Qt6::Gui)

set_target_properties(fydelis-codecs PROPERTIES
    OUTPUT_NAME fydelis-codecs
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

install(TARGETS fydelis-codecs
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/libfydelis-codecs
)