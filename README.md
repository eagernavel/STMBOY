# STMBOY

STMBOY es un proyecto bare-metal para STM32 NUCLEO-F411RE que implementa una API grafica modular para una pantalla ILI9486 en bus paralelo de 8 bits. La aplicacion incluida es una demostracion interactiva de primitivas, texto, entrada y renderizado incremental.

## Hardware principal

- Placa STM32 NUCLEO-F411RE.
- Pantalla TFT con controlador ILI9486 y bus paralelo 8080 de 8 bits.
- Tres pulsadores: `BTN_UP`, `BTN_DOWN`, `BTN_START`.
- Programador/debugger ST-Link integrado en la Nucleo.

## Estructura

```text
source/                         Demostracion grafica y soporte newlib
lib/common/                     Tipos compartidos y HAL abstracto de display
lib/graphics/                   Primitivas 2D, texto y sprites
lib/game_engine/                Actor y animacion por frames
lib/bsp/                        Fachada de inicializacion de placa
lib/platform/                   Seleccion y adaptacion de plataforma
lib/platform/nucleof411re/      Reloj, UART, SysTick, botones e ILI9486
lib/platform/component/ili9486/ Driver generico del controlador LCD
third-party/stm32_driver/       CMSIS, startup y drivers LL de ST
cmake/toolchain/                Toolchains arm-none-eabi
ld/                             Linker scripts STM32
docs/                           Documentacion tecnica y cableado
```

## Build

Configurar:

```bash
cmake --preset stm32f411re
```

Compilar:

```bash
cmake --build --preset stm32f411re
```

Alternativa con Makefile:

```bash
make configure
make
```

Flashear con OpenOCD:

```bash
make flash
```

## Documentacion

- [Documentacion tecnica TFM](docs/TFM_DOCUMENTACION_TECNICA.md)
- [Guia de cableado de botones](docs/BUTTON_WIRING_GUIDE.md)
- [Resumen tecnico de modulos](docs/TECHNICAL_OVERVIEW.md)
- [Revision arquitectonica previa](docs/revision_arquitectura_stm32boy.pdf)

## Estado del proyecto

La plataforma soportada y validada es `stm32f411re`. Las capas de graficos, modelo de demostracion y driver ILI9486 disponen de pruebas nativas independientes del STM32.
