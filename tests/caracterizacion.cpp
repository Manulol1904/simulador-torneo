#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define main simulador_main
#include "../main2.cpp"
#undef main

#include "rutas_prueba.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace {

class SilencioCout {
public:
    SilencioCout() : anterior_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~SilencioCout() { std::cout.rdbuf(anterior_); }
    std::string texto() const { return buffer_.str(); }

private:
    std::stringstream buffer_;
    std::streambuf* anterior_;
};

std::string CapturarSimulador() {
    namespace fs = std::filesystem;
    const fs::path runtime = RUNTIME_DIR;
    const fs::path destino = runtime / "desktop" / "proyecto c++";
    fs::create_directories(destino);
    fs::copy_file(EQUIPOS_SRC, destino / "equipos2.txt", fs::copy_options::overwrite_existing);

    SECURITY_ATTRIBUTES atributos{};
    atributos.nLength = sizeof(atributos);
    atributos.bInheritHandle = TRUE;
    HANDLE lectura = nullptr;
    HANDLE escritura = nullptr;
    if (!CreatePipe(&lectura, &escritura, &atributos, 0)) {
        throw std::runtime_error("No se pudo crear la tuberia de salida.");
    }
    SetHandleInformation(lectura, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW inicio{};
    inicio.cb = sizeof(inicio);
    inicio.dwFlags = STARTF_USESTDHANDLES;
    inicio.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    inicio.hStdOutput = escritura;
    inicio.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    const std::wstring ejecutable = fs::path(SIMULADOR_EXE).wstring();
    std::wstring comando = L"\"" + ejecutable + L"\"";
    std::vector<wchar_t> linea(comando.begin(), comando.end());
    linea.push_back(L'\0');
    const std::wstring directorio = runtime.wstring();

    PROCESS_INFORMATION proceso{};
    const BOOL creado = CreateProcessW(
        nullptr, linea.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
        nullptr, directorio.c_str(), &inicio, &proceso);
    CloseHandle(escritura);
    if (!creado) {
        CloseHandle(lectura);
        throw std::runtime_error("No se pudo ejecutar el simulador.");
    }

    std::string salida;
    char bloque[4096];
    DWORD leidos = 0;
    while (ReadFile(lectura, bloque, sizeof(bloque), &leidos, nullptr) && leidos > 0) {
        salida.append(bloque, bloque + leidos);
    }
    WaitForSingleObject(proceso.hProcess, INFINITE);
    DWORD codigo = 1;
    GetExitCodeProcess(proceso.hProcess, &codigo);
    CloseHandle(proceso.hProcess);
    CloseHandle(proceso.hThread);
    CloseHandle(lectura);
    if (codigo != 0) {
        throw std::runtime_error("El simulador termino con codigo " + std::to_string(codigo));
    }
    return salida;
}

std::string LeerArchivo(const std::string& ruta) {
    std::ifstream entrada(ruta, std::ios::binary);
    if (!entrada) {
        throw std::runtime_error("No se pudo abrir " + ruta);
    }
    return std::string(std::istreambuf_iterator<char>(entrada), std::istreambuf_iterator<char>());
}

}  // namespace

TEST(CalcularDiferenciaGoles, RestaFavorMenosContra) {
    const Equipo equipo{"Local", 5, 2, 0, 0.5, "", ""};
    EXPECT_EQ(3, CalcularDiferenciaGoles(equipo));
}

TEST(CalcularDiferenciaGoles, PermiteNegativaYCero) {
    const Equipo negativo{"A", 1, 4, 0, 0.5, "", ""};
    const Equipo cero{"B", 2, 2, 0, 0.5, "", ""};
    EXPECT_EQ(-3, CalcularDiferenciaGoles(negativo));
    EXPECT_EQ(0, CalcularDiferenciaGoles(cero));
}

TEST(Puntos, EmpateCuandoNoHayProbabilidadDeGol) {
    Equipo local{"Local", 0, 0, 0, 0.0, "", ""};
    Equipo visita{"Visita", 0, 0, 0, 0.0, "", ""};
    const SilencioCout silencio;
    SimularPartido(local, visita, false);
    EXPECT_EQ(0, local.goles_a_favor);
    EXPECT_EQ(0, visita.goles_a_favor);
    EXPECT_EQ(1, local.puntos);
    EXPECT_EQ(1, visita.puntos);
    static_cast<void>(silencio);
}

TEST(Puntos, VictoriaDelPrimeroConSemillaFija) {
    std::srand(1);
    Equipo favorito{"Favorito", 0, 0, 0, 90.0, "", ""};
    Equipo debil{"Debil", 0, 0, 0, 0.0, "", ""};
    const SilencioCout silencio;
    SimularPartido(favorito, debil, false);
    EXPECT_GT(favorito.goles_a_favor, debil.goles_a_favor);
    EXPECT_EQ(3, favorito.puntos);
    EXPECT_EQ(0, debil.puntos);
    EXPECT_EQ(favorito.goles_a_favor, debil.goles_en_contra);
    EXPECT_EQ(debil.goles_a_favor, favorito.goles_en_contra);
    static_cast<void>(silencio);
}

TEST(Puntos, DerrotaDelPrimeroConSemillaFija) {
    std::srand(1);
    Equipo debil{"Debil", 0, 0, 0, 0.0, "", ""};
    Equipo favorito{"Favorito", 0, 0, 0, 90.0, "", ""};
    const SilencioCout silencio;
    SimularPartido(debil, favorito, false);
    EXPECT_LT(debil.goles_a_favor, favorito.goles_a_favor);
    EXPECT_EQ(0, debil.puntos);
    EXPECT_EQ(3, favorito.puntos);
    static_cast<void>(silencio);
}

TEST(Penales, EmpateEnCeroSigueEmpatado) {
    Equipo local{"Local", 0, 0, 0, 0.0, "", ""};
    Equipo visita{"Visita", 0, 0, 0, 0.0, "", ""};
    SilencioCout silencio;
    SimularPartido(local, visita, true);
    EXPECT_EQ(0, local.goles_a_favor);
    EXPECT_EQ(0, visita.goles_a_favor);
    EXPECT_EQ(1, local.puntos);
    EXPECT_EQ(1, visita.puntos);
    EXPECT_NE(std::string::npos, silencio.texto().find("tanda de penales"));
}

TEST(ObtenerFecha, ConservaAnio2006YAvanza) {
    const std::string fecha = ObtenerFecha();
    const std::string siguiente = ObtenerFecha();
    const std::regex formato(R"(\d{2}/\d{2}/2006)");
    EXPECT_TRUE(std::regex_match(fecha, formato));
    EXPECT_TRUE(std::regex_match(siguiente, formato));
    EXPECT_NE(fecha, siguiente);
}

TEST(ClasificacionGrupo, OrdenaPorPuntosDiferenciaYGoles) {
    const Equipo conMasPuntos{"Puntos", 1, 1, 6, 0.0, "", ""};
    const Equipo mejorDiferencia{"Dif", 5, 1, 3, 0.0, "", ""};
    const Equipo masGoles{"Goles", 4, 2, 3, 0.0, "", ""};
    const Equipo menosGoles{"Menos", 2, 0, 3, 0.0, "", ""};

    EXPECT_TRUE(CompararClasificacionGrupo(conMasPuntos, mejorDiferencia));
    EXPECT_TRUE(CompararClasificacionGrupo(mejorDiferencia, masGoles));
    EXPECT_TRUE(CompararClasificacionGrupo(masGoles, menosGoles));

    std::vector<Equipo> grupo{menosGoles, masGoles, mejorDiferencia, conMasPuntos};
    std::sort(grupo.begin(), grupo.end(), CompararClasificacionGrupo);
    ASSERT_EQ(4u, grupo.size());
    EXPECT_EQ("Puntos", grupo[0].nombre);
    EXPECT_EQ("Dif", grupo[1].nombre);
    EXPECT_EQ("Goles", grupo[2].nombre);
    EXPECT_EQ("Menos", grupo[3].nombre);
}

TEST(GanadoresRonda, ComparaGolesAcumuladosYEnEmpateEligeAlSegundo) {
    const Equipo alto{"Alto", 4, 0, 0, 0.0, "", ""};
    const Equipo bajo{"Bajo", 1, 0, 0, 0.0, "", ""};
    const Equipo empate1{"E1", 2, 0, 0, 0.0, "", ""};
    const Equipo empate2{"E2", 2, 9, 0, 0.0, "", ""};
    const std::vector<Equipo> ronda{alto, bajo, empate1, empate2};
    const std::vector<Equipo> ganadores = ObtenerGanadoresRonda(ronda);
    ASSERT_EQ(2u, ganadores.size());
    EXPECT_EQ("Alto", ganadores[0].nombre);
    EXPECT_EQ("E2", ganadores[1].nombre);
}

TEST(GoldenMaster, RepiteLaSalidaOriginal) {
    const std::string primera = CapturarSimulador();
    const std::string segunda = CapturarSimulador();
    EXPECT_EQ(primera, segunda);
    EXPECT_EQ(LeerArchivo(GOLDEN_PATH), primera);
}
