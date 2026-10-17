#include "simulador.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

std::string ObtenerFecha() {
    static int dia = 1;
    static int mes = 1;
    static int anno = kAnioTorneo;

    dia++;

    if ((mes == 1 && dia > kDiasEnero) || (mes == 2 && dia > kDiasFebrero)) {
        mes++;
        dia = 1;
    }

    std::ostringstream fecha_stream;
    fecha_stream << std::setw(kAnchoFecha) << std::setfill('0') << dia << '/';
    fecha_stream << std::setw(kAnchoFecha) << std::setfill('0') << mes << '/';
    fecha_stream << anno;
    return fecha_stream.str();
}

std::string ObtenerEstadioAleatorio(std::mt19937& generador) {
    const std::vector<std::string> estadios = {
        "Allianz Arena",
        "Olympiastadion",
        "Signal Iduna Park",
        "Fritz-Walter-Stadion",
        "Zentralstadion",
        "Arena AufSchalke",
        "Gottlieb-Daimler-Stadion",
        "EasyCredit-Stadion",
        "Arena Leipzig",
        "Veltins-Arena"
    };

    std::uniform_int_distribution<int> dist(0, static_cast<int>(estadios.size()) - 1);
    return estadios[dist(generador)];
}

double AleatorioUnitario() {
    return static_cast<double>(std::rand()) / RAND_MAX;
}

void SimularTiempo(int minutos, double probabilidad1, double probabilidad2, int& goles1, int& goles2) {
    for (int minuto = 0; minuto < minutos; minuto++) {
        if (AleatorioUnitario() < probabilidad1 / minutos) {
            goles1++;
        }
        if (AleatorioUnitario() < probabilidad2 / minutos) {
            goles2++;
        }
    }
}

void SimularTandaPenales(const Equipo& equipo1, const Equipo& equipo2, double probabilidad1, double probabilidad2, int& goles1, int& goles2) {
    std::cout << "Empate después del tiempo extra. Se define por tanda de penales." << std::endl;

    int penales1 = 0;
    int penales2 = 0;
    for (int tiro = 0; tiro < kTirosPorEquipo; tiro++) {
        if (AleatorioUnitario() < probabilidad1) {
            penales1++;
        }
        if (AleatorioUnitario() < probabilidad2) {
            penales2++;
        }
    }

    goles1 += penales1;
    goles2 += penales2;
    std::cout << "Resultado de la tanda de penales: " << equipo1.nombre << " " << penales1 << " - " << penales2 << " " << equipo2.nombre << std::endl;
}

void ActualizarEstadisticas(Equipo& equipo1, Equipo& equipo2, int goles1, int goles2) {
    equipo1.goles_a_favor += goles1;
    equipo1.goles_en_contra += goles2;
    equipo2.goles_a_favor += goles2;
    equipo2.goles_en_contra += goles1;

    if (goles1 > goles2) {
        equipo1.puntos += kPuntosVictoria;
    } else if (goles1 < goles2) {
        equipo2.puntos += kPuntosVictoria;
    } else {
        equipo1.puntos += kPuntosEmpate;
        equipo2.puntos += kPuntosEmpate;
    }
}

void ImprimirResultado(const Equipo& equipo1, const Equipo& equipo2, int goles1, int goles2, const Partido& partido) {
    std::cout << "Fecha: " << partido.fecha << " - Estadio: " << partido.estadio << std::endl;
    std::cout << equipo1.nombre << " " << goles1 << " - " << goles2 << " " << equipo2.nombre << std::endl;
}

void SimularPartido(Equipo& equipo1, Equipo& equipo2, bool es_eliminacion_directa, std::mt19937& generador_estadios) {
    const double probabilidad1 = equipo1.probabilidad_gol;
    const double probabilidad2 = equipo2.probabilidad_gol;
    int goles1 = 0;
    int goles2 = 0;

    SimularTiempo(kMinutosReglamento, probabilidad1, probabilidad2, goles1, goles2);

    if (es_eliminacion_directa && goles1 == goles2) {
        std::cout << "Empate en tiempo reglamentario. Se define en tiempo extra y/o tanda de penales." << std::endl;
        SimularTiempo(kMinutosProrroga, probabilidad1, probabilidad2, goles1, goles2);
        if (goles1 == goles2) {
            SimularTandaPenales(equipo1, equipo2, probabilidad1, probabilidad2, goles1, goles2);
        }
    }

    ActualizarEstadisticas(equipo1, equipo2, goles1, goles2);

    Partido partido;
    partido.fecha = ObtenerFecha();
    partido.estadio = ObtenerEstadioAleatorio(generador_estadios);
    // El codigo original pedia un segundo estadio y no lo imprimia.
    // La llamada se conserva para no desplazar los estadios siguientes.
    ObtenerEstadioAleatorio(generador_estadios);
    ImprimirResultado(equipo1, equipo2, goles1, goles2, partido);
}

int CalcularDiferenciaGoles(const Equipo& equipo) {
    return equipo.goles_a_favor - equipo.goles_en_contra;
}

void SimularRondaEliminacionDirecta(std::vector<Equipo>& equipos, std::mt19937& generador_estadios) {
    for (size_t i = 0; i < equipos.size(); i += 2) {
        SimularPartido(equipos[i], equipos[i + 1], true, generador_estadios);
    }
}

std::vector<Equipo> ObtenerGanadoresRonda(const std::vector<Equipo>& equipos) {
    std::vector<Equipo> ganadores;
    for (size_t i = 0; i < equipos.size(); i += 2) {
        if (equipos[i].goles_a_favor > equipos[i + 1].goles_a_favor) {
            ganadores.push_back(equipos[i]);
        } else {
            ganadores.push_back(equipos[i + 1]);
        }
    }
    return ganadores;
}

bool CompararClasificacionGrupo(const Equipo& equipo_a, const Equipo& equipo_b) {
    if (equipo_a.puntos == equipo_b.puntos) {
        if (CalcularDiferenciaGoles(equipo_a) == CalcularDiferenciaGoles(equipo_b)) {
            return equipo_a.goles_a_favor > equipo_b.goles_a_favor;
        }
        return CalcularDiferenciaGoles(equipo_a) > CalcularDiferenciaGoles(equipo_b);
    }
    return equipo_a.puntos > equipo_b.puntos;
}
