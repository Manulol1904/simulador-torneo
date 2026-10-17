#pragma once

#include "equipo.h"

#include <random>
#include <string>
#include <vector>

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

std::string ObtenerFecha();
std::string ObtenerEstadioAleatorio(std::mt19937& generador);
double AleatorioUnitario();
void SimularTiempo(int minutos, double probabilidad1, double probabilidad2, int& goles1, int& goles2);
void SimularTandaPenales(const Equipo& equipo1, const Equipo& equipo2, double probabilidad1, double probabilidad2, int& goles1, int& goles2);
void ActualizarEstadisticas(Equipo& equipo1, Equipo& equipo2, int goles1, int goles2);
void ImprimirResultado(const Equipo& equipo1, const Equipo& equipo2, int goles1, int goles2, const Partido& partido);
void SimularPartido(Equipo& equipo1, Equipo& equipo2, bool es_eliminacion_directa, std::mt19937& generador_estadios);
int CalcularDiferenciaGoles(const Equipo& equipo);
void SimularRondaEliminacionDirecta(std::vector<Equipo>& equipos, std::mt19937& generador_estadios);
std::vector<Equipo> ObtenerGanadoresRonda(const std::vector<Equipo>& equipos);
bool CompararClasificacionGrupo(const Equipo& equipo_a, const Equipo& equipo_b);
