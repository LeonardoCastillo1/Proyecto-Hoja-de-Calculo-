# Proyecto Hoja de Calculo (C++ + SFML)

Aplicacion tipo mini-hoja de calculo en C++ con interfaz SFML.
El proyecto incluye una implementacion de matriz dispersa en `Proyecto_v006.cpp` y una interfaz base en `main.cpp`.

## Nombre del proyecto

- Carpeta del proyecto: `Proyecto-Hoja-de-Calculo-`
- Target principal de CMake: `ProyectoHojaDeCalculo`
- Ejecutable generado en macOS/Linux: `ProyectoHojaDeCalculo`
- Ejecutable generado en Windows: `ProyectoHojaDeCalculo.exe`

## Requisitos en macOS

- macOS con Command Line Tools de Xcode
- Homebrew
- Paquetes Homebrew:
  - `cmake`
  - `ninja`
  - `sfml`

## Instalacion de dependencias en macOS

```bash
xcode-select --install
brew install cmake ninja sfml
```

## Como ejecutar en macOS

Desde Terminal:

```bash
cd "/Users/alvarodasso/Documents/courses/AED/Proyecto-Hoja-de-Calculo-"
rm -rf cmake-build-debug
cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug --target ProyectoHojaDeCalculo -j 8
./cmake-build-debug/ProyectoHojaDeCalculo
```

Si CMake no encuentra SFML en macOS, configura indicando el prefijo de Homebrew:

```bash
cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$(brew --prefix sfml)"
cmake --build cmake-build-debug --target ProyectoHojaDeCalculo -j 8
./cmake-build-debug/ProyectoHojaDeCalculo
```

## Requisitos en Windows

- Windows
- MSYS2 instalado en `C:\msys64`
- Paquetes MSYS2 (mingw64/ucrt64):
  - `mingw-w64-x86_64-gcc`
  - `mingw-w64-x86_64-cmake`
  - `mingw-w64-x86_64-ninja`
  - `mingw-w64-x86_64-sfml`

## Instalacion de dependencias en Windows

Desde **MSYS2 UCRT64/MINGW64 shell** o usando `bash.exe` de MSYS2:

```bash
pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-sfml
```

## Como ejecutar (terminal)

En shell MSYS2 MINGW64:

```bash
cd /c/ruta/al/proyecto
rm -rf cmake-build-debug
/mingw64/bin/cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_C_COMPILER=/mingw64/bin/gcc.exe -DCMAKE_CXX_COMPILER=/mingw64/bin/g++.exe -DCMAKE_BUILD_TYPE=Debug
/mingw64/bin/cmake --build cmake-build-debug --target ProyectoHojaDeCalculo -j 8
./cmake-build-debug/ProyectoHojaDeCalculo.exe
```

## Como ejecutar en CLion

1. Abrir la carpeta del proyecto.
2. En macOS, usar el toolchain de Apple Clang y Ninja.
3. En Windows, configurar Toolchain para usar:
   - C Compiler: `C:\msys64\mingw64\bin\gcc.exe`
   - C++ Compiler: `C:\msys64\mingw64\bin\g++.exe`
4. Usar un perfil CMake con generador Ninja.
5. Recargar CMake.
6. Ejecutar el target `ProyectoHojaDeCalculo`.

## Estructura principal

- `main.cpp`: interfaz SFML (grid basico editable)
- `Proyecto_v006.cpp`: logica de matriz dispersa
- `demo_v006.cpp`: ejecutable de prueba de estructura
- `CMakeLists.txt`: configuracion de compilacion

## Notas

- El target principal de interfaz es `ProyectoHojaDeCalculo`.
- Si hay errores de runtime en Windows, normalmente son por toolchain incorrecto o mezcla de DLLs fuera de `C:\msys64\mingw64`.
