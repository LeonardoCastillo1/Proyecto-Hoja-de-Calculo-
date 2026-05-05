# Proyecto Hoja de Calculo (C++ + SFML)

Aplicacion tipo mini-hoja de calculo en C++ con interfaz SFML.
El proyecto incluye una implementacion de matriz dispersa en `Proyecto_v006.cpp` y una interfaz base en `main.cpp`.

## Requisitos

- Windows
- MSYS2 instalado en `C:\msys64`
- Paquetes MSYS2 (mingw64):
  - `mingw-w64-x86_64-gcc`
  - `mingw-w64-x86_64-cmake`
  - `mingw-w64-x86_64-ninja`
  - `mingw-w64-x86_64-sfml`

## Instalacion de dependencias (una sola vez)

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
/mingw64/bin/cmake --build cmake-build-debug --target MiProyecto -j 8
./cmake-build-debug/MiProyecto.exe
```

## Como ejecutar en CLion

1. Abrir la carpeta del proyecto.
2. Configurar Toolchain para usar:
   - C Compiler: `C:\msys64\mingw64\bin\gcc.exe`
   - C++ Compiler: `C:\msys64\mingw64\bin\g++.exe`
3. Usar un perfil CMake con generador Ninja.
4. Recargar CMake.
5. Ejecutar el target `MiProyecto`.

## Estructura principal

- `main.cpp`: interfaz SFML (grid basico editable)
- `Proyecto_v006.cpp`: logica de matriz dispersa
- `demo_v006.cpp`: ejecutable de prueba de estructura
- `CMakeLists.txt`: configuracion de compilacion

## Notas

- El target principal de interfaz es `MiProyecto`.
- Si hay errores de runtime en Windows, normalmente son por toolchain incorrecto o mezcla de DLLs fuera de `C:\msys64\mingw64`.
