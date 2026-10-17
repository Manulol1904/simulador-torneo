#pragma once

#include <string>

struct Equipo {
    std::string nombre;
    int goles_a_favor;
    int goles_en_contra;
    int puntos;
    double probabilidad_gol;
};

struct Partido {
    std::string fecha;
    std::string estadio;
};
