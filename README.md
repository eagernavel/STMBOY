# STMBOY

STMBOY es un proyecto bare-metal para STM32 NUCLEO-F411RE que implementa una API grafica modular para una pantalla ILI9486 en bus paralelo de 8 bits. La aplicacion incluida es una demostracion interactiva de primitivas, texto, entrada y renderizado incremental.

## Hardware principal

- Placa STM32 NUCLEO-F411RE.
- Pantalla TFT con controlador ILI9486 y bus paralelo 8080 de 8 bits.
- Tres pulsadores: `BTN_UP`, `BTN_DOWN`, `BTN_START`.
- Programador/debugger ST-Link integrado en la Nucleo.

## Estructura

```text
source/                         Demostracion grafica
lib/common/                     Tipos compartidos y HAL abstracto de display
lib/graphics/                   Primitivas 2D, texto y sprites
lib/game_engine/                Prototipo experimental no incluido en el firmware
lib/bsp/                        Fachada de inicializacion de placa
lib/platform/                   Seleccion y adaptacion de plataforma
lib/platform/nucleof411re/      Reloj, SysTick, botones e ILI9486
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

- [Memoria TFM readaptada](TFM_READAPTADO.docx)
- [Guia de montaje del shield y botones](docs/BUTTON_WIRING_GUIDE.md)
- [Resumen tecnico de modulos](docs/TECHNICAL_OVERVIEW.md)
- [Documentacion tecnica historica](docs/TFM_DOCUMENTACION_TECNICA.md)
- [Revision arquitectonica previa](docs/revision_arquitectura_stm32boy.pdf)

## Estado del proyecto

La unica plataforma incluida en el flujo de compilacion es `stm32f411re`. El proyecto genera correctamente el firmware para la placa y la validacion funcional se realiza manualmente sobre el hardware.

El firmware no usa memoria dinamica, consola serie ni llamadas al sistema de
Newlib. Las aserciones de los controladores LL se mantienen como parada segura:
deshabilitan las interrupciones y detienen la ejecucion, pero no imprimen
mensajes. Se conserva el entorno C minimo de Newlib-nano para el arranque y
funciones basicas como `memset()`; no se trata de un firmware totalmente libre
de biblioteca C.

## Validacion manual

Despues de compilar y programar la placa, comprobar:

1. La pantalla muestra la interfaz completa al arrancar.
2. `BTN_UP` aumenta el nivel hasta un maximo de 100.
3. `BTN_DOWN` reduce el nivel hasta un minimo de 0.
4. `BTN_START` cambia el color principal de la interfaz.
5. El indicador de latido cambia periodicamente sin bloquear los botones.
6. Al mantener `BTN_UP` y `BTN_DOWN` simultaneamente, el nivel permanece estable.

Estas comprobaciones deben registrarse indicando fecha, version del firmware, resultado y cualquier incidencia observada.
