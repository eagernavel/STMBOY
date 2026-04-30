# STMBOY

STMBOY es un proyecto bare-metal para STM32 NUCLEO-F411RE que implementa una pequena consola grafica con pantalla TFT ILI9486 en bus paralelo de 8 bits, entrada por botones fisicos y una aplicacion final tipo Pong.

## Hardware principal

- Placa STM32 NUCLEO-F411RE.
- Pantalla TFT con controlador ILI9486 y bus paralelo 8080 de 8 bits.
- Tres pulsadores: `BTN_UP`, `BTN_DOWN`, `BTN_START`.
- Programador/debugger ST-Link integrado en la Nucleo.

## Estructura

```text
source/                         Aplicacion final Pong y soporte newlib
lib/common/                     Tipos compartidos y HAL abstracto de display
lib/graphics/                   Primitivas 2D, texto y sprites
lib/game_engine/                Actor y animacion por frames
lib/bsp/                        Fachada de inicializacion de placa
lib/platform/                   Seleccion y adaptacion de plataforma
lib/platform/nucleof411re/      Reloj, UART, SysTick, botones e ILI9486
lib/platform/component/ili9486/ Contrato generico del controlador LCD
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

La plataforma principal soportada es `stm32f411re`. El repositorio contiene infraestructura parcial para otras placas STM32F4, pero la aplicacion final y el pinout documentado corresponden a NUCLEO-F411RE.
