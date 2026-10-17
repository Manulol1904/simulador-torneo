#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <ctime>

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
    static int anno = 2006;

    // Incrementar la fecha en un día
    dia++;

    // Verificar si se ha alcanzado el final de febrero
    if ((mes == 1 && dia > 31) || (mes == 2 && dia > 28)) {
        mes++;
        dia = 1;
    }

    // Formatear la fecha
    std::ostringstream fechaStream;
    fechaStream << std::setw(2) << std::setfill('0') << dia << '/';
    fechaStream << std::setw(2) << std::setfill('0') << mes << '/';
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

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, estadios.size() - 1);
    return estadios[dist(gen)];
}

// Función para simular un partido entre dos equipos y actualizar sus estadísticas
void SimularPartido(Equipo& equipo1, Equipo& equipo2, bool esEliminacionDirecta) {
    
    double probabilidadGolEquipo1 = equipo1.probabilidad_gol;
    double probabilidadGolEquipo2 = equipo2.probabilidad_gol;

    int golesEquipo1 = 0;
    int golesEquipo2 = 0;

    // Simulación del tiempo reglamentario (90 minutos)
    for (int minuto = 0; minuto < 90; minuto++) {
        double resultadoAleatorio = static_cast<double>(std::rand()) / RAND_MAX;

        // Determinar si ocurre un gol para el equipo 1 en este minuto
        if (resultadoAleatorio < probabilidadGolEquipo1 / 90) {
            golesEquipo1++;
        }

        resultadoAleatorio = static_cast<double>(std::rand()) / RAND_MAX;

        // Determinar si ocurre un gol para el equipo 2 en este minuto
        if (resultadoAleatorio < probabilidadGolEquipo2 / 90) {
            golesEquipo2++;
        }
    }

    // En caso de empate en tiempo reglamentario y es un partido de eliminación directa, se juega tiempo extra y/o tanda de penales
    if (esEliminacionDirecta && golesEquipo1 == golesEquipo2) {
        std::cout << "Empate en tiempo reglamentario. Se define en tiempo extra y/o tanda de penales." << std::endl;

        // Simulación de tiempo extra (30 minutos)
        for (int minuto = 0; minuto < 30; minuto++) {
            double resultadoAleatorio = static_cast<double>(std::rand()) / RAND_MAX;

            // Determinar si ocurre un gol para el equipo 1 en este minuto
            if (resultadoAleatorio < probabilidadGolEquipo1 / 30) {
                golesEquipo1++;
            }

            resultadoAleatorio = static_cast<double>(std::rand()) / RAND_MAX;

            // Determinar si ocurre un gol para el equipo 2 en este minuto
            if (resultadoAleatorio < probabilidadGolEquipo2 / 30) {
                golesEquipo2++;
            }
        }

        // En caso de empate después del tiempo extra, se define por tanda de penales
        if (golesEquipo1 == golesEquipo2) {
            std::cout << "Empate después del tiempo extra. Se define por tanda de penales." << std::endl;

            // Simulación de tanda de penales
            int penalesEquipo1 = 0;
            int penalesEquipo2 = 0;

            for (int i = 0; i < 5; i++) {
                double resultadoAleatorio = static_cast<double>(std::rand()) / RAND_MAX;
                if (resultadoAleatorio < probabilidadGolEquipo1) {
                    penalesEquipo1++;
                }
                resultadoAleatorio = static_cast<double>(std::rand()) / RAND_MAX;
                if (resultadoAleatorio < probabilidadGolEquipo2) {
                    penalesEquipo2++;
                }
            }

            // Actualiza el marcador con los penales
            golesEquipo1 += penalesEquipo1;
            golesEquipo2 += penalesEquipo2;

            std::cout << "Resultado de la tanda de penales: " << equipo1.nombre << " " << penalesEquipo1 << " - " << penalesEquipo2 << " " << equipo2.nombre << std::endl;
        }
    }

    // Actualiza las estadísticas de los equipos
    equipo1.goles_a_favor += golesEquipo1;
    equipo1.goles_en_contra += golesEquipo2;
    equipo2.goles_a_favor += golesEquipo2;
    equipo2.goles_en_contra += golesEquipo1;

    if (golesEquipo1 > golesEquipo2) {
        equipo1.puntos += 3;  // Equipo 1 gana
    } else if (golesEquipo1 < golesEquipo2) {
        equipo2.puntos += 3;  // Equipo 2 gana
    } else {
        equipo1.puntos += 1;  // Empate
        equipo2.puntos += 1;
    }

    // Actualiza la fecha y el estadio del partido
    std::string fecha = ObtenerFecha();
    equipo1.fecha = fecha;
    equipo2.fecha = fecha;
    equipo1.estadio = ObtenerEstadioAleatorio();
    equipo2.estadio = ObtenerEstadioAleatorio();

    // Muestra información completa del partido
    std::cout << "Fecha: " << fecha << " - Estadio: " << equipo1.estadio << std::endl;
    std::cout << equipo1.nombre << " " << golesEquipo1 << " - " << golesEquipo2 << " " << equipo2.nombre << std::endl;
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

int main() {

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Ruta al archivo de equipos (ajusta la ruta según la ubicación de tu archivo)
    std::string archivoEquipos = "desktop/proyecto c++/equipos2.txt";

    // Crear un vector de objetos Equipo
    std::vector<Equipo> equipos;

    // Cargar los nombres de los equipos desde un archivo
    std::ifstream inputFile(archivoEquipos);
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string nombre;
            double probabilidad_gol;
            iss >> nombre >> probabilidad_gol;
            equipos.push_back({nombre, 0, 0, 0, probabilidad_gol, "", ""});
        }
        inputFile.close();
    } else {
        std::cerr << "No se pudo abrir el archivo de equipos." << std::endl;
        return 1;
    }

    // Mezclar aleatoriamente el orden de los equipos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(equipos.begin(), equipos.end(), gen);

    // Dividir los equipos en 8 grupos de 4 equipos cada uno
    int equiposPorGrupo = 4;
    std::vector<std::vector<Equipo>> grupos(8);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < equiposPorGrupo; j++) {
            grupos[i].push_back(equipos[i * equiposPorGrupo + j]);
        }
    }

    // Simular enfrentamientos entre equipos en cada grupo
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < equiposPorGrupo; j++) {
            for (int k = j + 1; k < equiposPorGrupo; k++) {
                SimularPartido(grupos[i][j], grupos[i][k], false);
            }
        }
    }

    // Ordenar equipos en cada grupo por puntos (y diferencia de goles en caso de empate en puntos)
    for (int i = 0; i < 8; i++) {
        std::sort(grupos[i].begin(), grupos[i].end(), [](const Equipo& a, const Equipo& b) {
            if (a.puntos == b.puntos) {
                if (CalcularDiferenciaGoles(a) == CalcularDiferenciaGoles(b)) {
                    return a.goles_a_favor > b.goles_a_favor;  // Ordenar por goles a favor en caso de empate en puntos y diferencia de goles
                }
                return CalcularDiferenciaGoles(a) > CalcularDiferenciaGoles(b);
            }
            return a.puntos > b.puntos;
        });
    }

    // Imprimir los equipos clasificados (los 2 primeros de cada grupo) con toda la información
    for (int i = 0; i < 8; i++) {
        std::cout << "Clasificados del Grupo " << (i + 1) << ":" << std::endl;
        for (int j = 0; j < 4; j++) {
            Equipo& equipo = grupos[i][j];
            int diferenciaGoles = CalcularDiferenciaGoles(equipo);
            std::cout << "Equipo: " << equipo.nombre << " - Puntos: " << equipo.puntos << " GA: " << equipo.goles_a_favor << " GC: " << equipo.goles_en_contra << " Dif. Goles: " << diferenciaGoles;
            std::cout << std::endl;
        }
        std::cout << "------------------------" << std::endl;
    }

    // Clasificación a la siguiente fase
    std::vector<Equipo> equiposClasificados;

    for (int i = 0; i < 8; i++) {
        equiposClasificados.push_back(grupos[i][0]);
        equiposClasificados.push_back(grupos[i][1]);
    }

    // Fase de Eliminación Directa
    std::cout << "------ Fase de Eliminación Directa ------" << std::endl;

    // Octavos de Final
    SimularRondaEliminacionDirecta(equiposClasificados);
    std::vector<Equipo> ganadoresOctavos = ObtenerGanadoresRonda(equiposClasificados);

    // Cuartos de Final
    SimularRondaEliminacionDirecta(ganadoresOctavos);
    std::vector<Equipo> ganadoresCuartos = ObtenerGanadoresRonda(ganadoresOctavos);

    // Semifinales
    SimularRondaEliminacionDirecta(ganadoresCuartos);
    std::vector<Equipo> ganadoresSemifinales = ObtenerGanadoresRonda(ganadoresCuartos);

    // Tercer y Cuarto Puesto
    std::cout << "------ Partido por el Tercer y Cuarto Puesto ------" << std::endl;
    SimularPartido(ganadoresCuartos[1], ganadoresCuartos[0], true);

    // Final
    std::cout << "------ Final ------" << std::endl;
    SimularPartido(ganadoresSemifinales[0], ganadoresSemifinales[1], true);

    return 0;
}
