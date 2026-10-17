# Simulador de torneo

Simulador de un torneo de fútbol en C++17: fase de grupos (8 grupos de 4 equipos) y eliminatoria (octavos, cuartos, semifinales, tercer puesto y final). El caso se usó en la actividad de mantenimiento de software para diagnosticar deuda técnica, refactorizar sin cambiar la salida y dejar pruebas que lo demuestren.

Repositorio público: https://github.com/Manulol1904/simulador-torneo

La versión integrada está etiquetada como `v1.0-refactor`.

## Estructura

```
equipo.h          Equipo y Partido
simulador.h       Constantes y declaraciones
simulador.cpp     Simulación, fecha, estadios y clasificación
main.cpp          Carga del archivo, grupos y eliminatoria
equipos2.txt      Un equipo por línea: nombre y probabilidad de gol
tests/            Pruebas de caracterización con GoogleTest
evidencia/        Reportes de herramientas, cobertura y golden master
```

## Compilar

Hace falta g++ con C++17, CMake y Ninja. En este equipo están en MSYS2 UCRT64. Si la terminal no los encuentra:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++
cmake --build build
```

## Ejecutar

El programa sigue leyendo la ruta original `desktop/proyecto c++/equipos2.txt`, relativa al directorio de trabajo. No se cambió durante la refactorización. Para correrlo desde este repositorio:

```powershell
New-Item -ItemType Directory -Force -Path "runtime-data\desktop\proyecto c++" | Out-Null
Copy-Item equipos2.txt "runtime-data\desktop\proyecto c++\equipos2.txt"
Push-Location runtime-data
..\build\simulador.exe
Pop-Location
```

`runtime-data/` no se versiona. La semilla es fija (`kSemilla = 1`), así que dos ejecuciones imprimen lo mismo.

## Pruebas

GoogleTest 1.15.2 se descarga con CMake la primera vez que se configura el proyecto.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
cmake --build build
.\build\simulador_tests.exe --gtest_color=no
```

La suite cubre diferencia de goles, puntos, fecha, desempate de grupos, ganadores de ronda y la salida completa contra `evidencia/salida_original.txt`. Después de la refactorización esa comparación sigue vacía: `evidencia/diff_golden_master.txt`.

## Herramientas

| Uso | Herramienta |
|---|---|
| Análisis estático | cppcheck 2.21 y clang-tidy 23 |
| Métricas | lizard 1.24 |
| Análisis dinámico | gcov y gcovr 8.7 |
| Pruebas | GoogleTest 1.15.2 |
| Build | CMake 4.4, Ninja, g++ 16.2 |

AddressSanitizer y UBSan no enlazan en este MSYS2: faltan `libasan` y `libubsan`. Valgrind no existe en Windows. La evidencia de ese intento está en `evidencia/sanitizers_antes.txt` y `evidencia/sanitizers_despues.txt`. La herramienta dinámica usada es la cobertura.

## Evidencia

- Diagnóstico: `evidencia/diagnostico_smells.txt`, `cppcheck_antes.txt`, `lizard_antes.txt`, `clang_tidy_antes.txt`
- Después: `cppcheck_despues.txt`, `lizard_despues.txt`, `comparacion_antes_despues.txt`
- Cobertura: `cobertura_antes.html` y `cobertura_despues.html`
- Historial: `git_log_graph.txt`

Los bugs de comportamiento (fecha a partir de marzo, ganador por goles acumulados, tercer puesto, penales que pueden empatar, nombres con espacio) están documentados y no se corrigieron en la refactorización.

Informe técnico: [informe/informe.pdf](informe/informe.pdf) (3 páginas).
