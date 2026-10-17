#include "simulador.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::srand(kSemilla);

    const std::string archivo_equipos = "desktop/proyecto c++/equipos2.txt";
    std::vector<Equipo> equipos;

    std::ifstream archivo(archivo_equipos);
    if (archivo.is_open()) {
        std::string linea;
        while (std::getline(archivo, linea)) {
            std::istringstream flujo(linea);
            std::string nombre;
            double probabilidad_gol = 0.0;
            flujo >> nombre >> probabilidad_gol;
            equipos.push_back({nombre, 0, 0, 0, probabilidad_gol});
        }
        archivo.close();
    } else {
        std::cerr << "No se pudo abrir el archivo de equipos." << std::endl;
        return 1;
    }

    std::mt19937 generador_sorteo(kSemilla);
    std::shuffle(equipos.begin(), equipos.end(), generador_sorteo);

    std::mt19937 generador_estadios(kSemilla);
    std::vector<std::vector<Equipo>> grupos(kCantidadGrupos);

    for (int i = 0; i < kCantidadGrupos; i++) {
        for (int j = 0; j < kEquiposPorGrupo; j++) {
            grupos[i].push_back(equipos[i * kEquiposPorGrupo + j]);
        }
    }

    for (int i = 0; i < kCantidadGrupos; i++) {
        for (int j = 0; j < kEquiposPorGrupo; j++) {
            for (int k = j + 1; k < kEquiposPorGrupo; k++) {
                SimularPartido(grupos[i][j], grupos[i][k], false, generador_estadios);
            }
        }
    }

    for (int i = 0; i < kCantidadGrupos; i++) {
        std::sort(grupos[i].begin(), grupos[i].end(), CompararClasificacionGrupo);
    }

    for (int i = 0; i < kCantidadGrupos; i++) {
        std::cout << "Clasificados del Grupo " << (i + 1) << ":" << std::endl;
        for (int j = 0; j < kEquiposPorGrupo; j++) {
            const Equipo& equipo = grupos[i][j];
            const int diferencia_goles = CalcularDiferenciaGoles(equipo);
            std::cout << "Equipo: " << equipo.nombre << " - Puntos: " << equipo.puntos << " GA: " << equipo.goles_a_favor << " GC: " << equipo.goles_en_contra << " Dif. Goles: " << diferencia_goles;
            std::cout << std::endl;
        }
        std::cout << "------------------------" << std::endl;
    }

    std::vector<Equipo> equipos_clasificados;
    for (int i = 0; i < kCantidadGrupos; i++) {
        equipos_clasificados.push_back(grupos[i][kIndicePrimero]);
        equipos_clasificados.push_back(grupos[i][kIndiceSegundo]);
    }

    std::cout << "------ Fase de Eliminación Directa ------" << std::endl;

    SimularRondaEliminacionDirecta(equipos_clasificados, generador_estadios);
    std::vector<Equipo> ganadores_octavos = ObtenerGanadoresRonda(equipos_clasificados);

    SimularRondaEliminacionDirecta(ganadores_octavos, generador_estadios);
    std::vector<Equipo> ganadores_cuartos = ObtenerGanadoresRonda(ganadores_octavos);

    SimularRondaEliminacionDirecta(ganadores_cuartos, generador_estadios);
    std::vector<Equipo> ganadores_semifinales = ObtenerGanadoresRonda(ganadores_cuartos);

    std::cout << "------ Partido por el Tercer y Cuarto Puesto ------" << std::endl;
    SimularPartido(ganadores_cuartos[kIndiceSegundo], ganadores_cuartos[kIndicePrimero], true, generador_estadios);

    std::cout << "------ Final ------" << std::endl;
    SimularPartido(ganadores_semifinales[kIndicePrimero], ganadores_semifinales[kIndiceSegundo], true, generador_estadios);

    return 0;
}
