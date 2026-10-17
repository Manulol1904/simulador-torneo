#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

constexpr unsigned int kSemilla = 1;
constexpr int kAnioTorneo = 2006;
constexpr int kDiasEnero = 31;
constexpr int kDiasFebrero = 28;
constexpr int kAnchoFecha = 2;
constexpr int kMinutosReglamento = 90;
constexpr int kMinutosProrroga = 30;
constexpr int kTirosPorEquipo = 5;
constexpr int kPuntosVictoria = 3;
constexpr int kPuntosEmpate = 1;
constexpr int kCantidadGrupos = 8;
constexpr int kEquiposPorGrupo = 4;
constexpr int kIndicePrimero = 0;
constexpr int kIndiceSegundo = 1;

struct Equipo {
    std::string nombre;
    int goles_a_favor;
    int goles_en_contra;
    int puntos;
    double probabilidad_gol;
    std::string estadio;
    std::string fecha;
};

// Función para obtener una fecha en formato dd/mm/2006
std::string ObtenerFecha() {
    static int dia = 1;
    static int mes = 1;
    static int anno = kAnioTorneo;

    // Incrementar la fecha en un día
    dia++;

    // Verificar si se ha alcanzado el final de febrero
    if ((mes == 1 && dia > kDiasEnero) || (mes == 2 && dia > kDiasFebrero)) {
        mes++;
        dia = 1;
    }

    // Formatear la fecha
    std::ostringstream fechaStream;
    fechaStream << std::setw(kAnchoFecha) << std::setfill('0') << dia << '/';
    fechaStream << std::setw(kAnchoFecha) << std::setfill('0') << mes << '/';
    fechaStream << anno;

    return fechaStream.str();
}

// Función para obtener el nombre de un estadio aleatorio
std::string ObtenerEstadioAleatorio() {
    std::vector<std::string> estadios = {
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

    static std::mt19937 gen(kSemilla);
    std::uniform_int_distribution<int> dist(0, estadios.size() - 1);
    return estadios[dist(gen)];
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

void ImprimirResultado(const Equipo& equipo1, const Equipo& equipo2, int goles1, int goles2, const std::string& fecha) {
    std::cout << "Fecha: " << fecha << " - Estadio: " << equipo1.estadio << std::endl;
    std::cout << equipo1.nombre << " " << goles1 << " - " << goles2 << " " << equipo2.nombre << std::endl;
}

void SimularPartido(Equipo& equipo1, Equipo& equipo2, bool esEliminacionDirecta) {
    const double probabilidad1 = equipo1.probabilidad_gol;
    const double probabilidad2 = equipo2.probabilidad_gol;
    int goles1 = 0;
    int goles2 = 0;

    SimularTiempo(kMinutosReglamento, probabilidad1, probabilidad2, goles1, goles2);

    if (esEliminacionDirecta && goles1 == goles2) {
        std::cout << "Empate en tiempo reglamentario. Se define en tiempo extra y/o tanda de penales." << std::endl;
        SimularTiempo(kMinutosProrroga, probabilidad1, probabilidad2, goles1, goles2);
        if (goles1 == goles2) {
            SimularTandaPenales(equipo1, equipo2, probabilidad1, probabilidad2, goles1, goles2);
        }
    }

    ActualizarEstadisticas(equipo1, equipo2, goles1, goles2);

    const std::string fecha = ObtenerFecha();
    equipo1.fecha = fecha;
    equipo2.fecha = fecha;
    equipo1.estadio = ObtenerEstadioAleatorio();
    equipo2.estadio = ObtenerEstadioAleatorio();
    ImprimirResultado(equipo1, equipo2, goles1, goles2, fecha);
}

// Función para calcular la diferencia de goles de un equipo
int CalcularDiferenciaGoles(const Equipo& equipo) {
    return equipo.goles_a_favor - equipo.goles_en_contra;
}

// Función para simular una ronda de eliminación directa
void SimularRondaEliminacionDirecta(std::vector<Equipo>& equipos) {
    for (size_t i = 0; i < equipos.size(); i += 2) {
        SimularPartido(equipos[i], equipos[i + 1], true);
    }
}

// Función para obtener los ganadores de una ronda de eliminación directa
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

int main() {

    std::srand(kSemilla);

    // Ruta al archivo de equipos (ajusta la ruta según la ubicación de tu archivo)
    const std::string archivo_equipos = "desktop/proyecto c++/equipos2.txt";

    // Crear un vector de objetos Equipo
    std::vector<Equipo> equipos;

    // Cargar los nombres de los equipos desde un archivo
    std::ifstream archivo(archivo_equipos);
    if (archivo.is_open()) {
        std::string linea;
        while (std::getline(archivo, linea)) {
            std::istringstream flujo(linea);
            std::string nombre;
            double probabilidad_gol = 0.0;
            flujo >> nombre >> probabilidad_gol;
            equipos.push_back({nombre, 0, 0, 0, probabilidad_gol, "", ""});
        }
        archivo.close();
    } else {
        std::cerr << "No se pudo abrir el archivo de equipos." << std::endl;
        return 1;
    }

    // Mezclar aleatoriamente el orden de los equipos
    std::mt19937 generador_sorteo(kSemilla);
    std::shuffle(equipos.begin(), equipos.end(), generador_sorteo);

    // Dividir los equipos en 8 grupos de 4 equipos cada uno
    std::vector<std::vector<Equipo>> grupos(kCantidadGrupos);

    for (int i = 0; i < kCantidadGrupos; i++) {
        for (int j = 0; j < kEquiposPorGrupo; j++) {
            grupos[i].push_back(equipos[i * kEquiposPorGrupo + j]);
        }
    }

    // Simular enfrentamientos entre equipos en cada grupo
    for (int i = 0; i < kCantidadGrupos; i++) {
        for (int j = 0; j < kEquiposPorGrupo; j++) {
            for (int k = j + 1; k < kEquiposPorGrupo; k++) {
                SimularPartido(grupos[i][j], grupos[i][k], false);
            }
        }
    }

    // Ordenar equipos en cada grupo por puntos (y diferencia de goles en caso de empate en puntos)
    for (int i = 0; i < kCantidadGrupos; i++) {
        std::sort(grupos[i].begin(), grupos[i].end(), CompararClasificacionGrupo);
    }

    // Imprimir los equipos clasificados (los 2 primeros de cada grupo) con toda la información
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

    // Clasificación a la siguiente fase
    std::vector<Equipo> equipos_clasificados;

    for (int i = 0; i < kCantidadGrupos; i++) {
        equipos_clasificados.push_back(grupos[i][kIndicePrimero]);
        equipos_clasificados.push_back(grupos[i][kIndiceSegundo]);
    }

    // Fase de Eliminación Directa
    std::cout << "------ Fase de Eliminación Directa ------" << std::endl;

    // Octavos de Final
    SimularRondaEliminacionDirecta(equipos_clasificados);
    std::vector<Equipo> ganadores_octavos = ObtenerGanadoresRonda(equipos_clasificados);

    // Cuartos de Final
    SimularRondaEliminacionDirecta(ganadores_octavos);
    std::vector<Equipo> ganadores_cuartos = ObtenerGanadoresRonda(ganadores_octavos);

    // Semifinales
    SimularRondaEliminacionDirecta(ganadores_cuartos);
    std::vector<Equipo> ganadores_semifinales = ObtenerGanadoresRonda(ganadores_cuartos);

    // Tercer y Cuarto Puesto
    std::cout << "------ Partido por el Tercer y Cuarto Puesto ------" << std::endl;
    SimularPartido(ganadores_cuartos[kIndiceSegundo], ganadores_cuartos[kIndicePrimero], true);

    // Final
    std::cout << "------ Final ------" << std::endl;
    SimularPartido(ganadores_semifinales[kIndicePrimero], ganadores_semifinales[kIndiceSegundo], true);

    return 0;
}
