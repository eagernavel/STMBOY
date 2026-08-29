# Resumen tecnico de STMBOY

## Objetivo

STMBOY implementa una API grafica bare-metal para una pantalla ILI9486 conectada a una STM32 NUCLEO-F411RE. La aplicacion de validacion muestra primitivas, texto, colores, entrada por botones y actualizacion incremental sin utilizar un framebuffer completo.

## Capas del sistema

| Capa | Directorio | Responsabilidad |
|---|---|---|
| Aplicacion | `source/main.c` | Inicializacion y coordinacion del bucle principal |
| Modelo de demostracion | `source/demo/demo_model.*` | Estado, entrada y temporizacion sin dependencias graficas |
| Vista de demostracion | `source/demo/demo_view.*` | Traduccion del estado a primitivas STMBOY |
| BSP | `lib/bsp/` | Fachada neutral de display, entrada y tiempo |
| Graficos | `lib/graphics/` | Primitivas 2D, texto y sprites sobre `display_hal_t` |
| Componente ILI9486 | `lib/platform/component/ili9486/` | Protocolo, inicializacion y ventanas del controlador |
| Plataforma | `lib/platform/nucleof411re/` | Reloj, SysTick, GPIO y bus paralelo fisico |
| Tipos comunes | `lib/common/` | Display con contexto, entrada y tipos RGB565 compartidos |
| Dependencias ST | `third-party/stm32_driver/` | CMSIS, startup y drivers LL |

## Flujo de dependencias

```text
main
  -> demo_model
  -> demo_view -> graphics -> display_hal_t
  -> BSP -> platform -> ILI9486 -> bus fisico -> STM32 LL
```

`main.c`, el modelo, la vista y el motor grafico no incluyen cabeceras STM32 ni nombres de la NUCLEO-F411RE. La seleccion de la plataforma queda resuelta por CMake y por la fachada del BSP.

## Demostracion grafica

La pantalla de validacion contiene texto, contornos, rectangulos rellenos, lineas, un triangulo y muestras de color. Los controles son deliberadamente simples:

- `UP` y `DOWN` desplazan un indicador vertical;
- `START` cambia la paleta;
- un testigo parpadea cada 500 ms para comprobar la base temporal.

Los cambios de nivel y del testigo se actualizan por regiones. El redibujado completo solo se realiza al arrancar o al cambiar la paleta.

## Display y driver ILI9486

`display_hal_t` contiene dimensiones, contexto y callbacks. Esta forma permite tener instancias independientes y utilizar el mismo motor grafico con el display real o con un backend simulado.

El componente ILI9486 conserva:

- reset y secuencia de inicializacion;
- seleccion de RGB565;
- configuracion explicita de orientacion mediante `mirror_x`, `mirror_y` y `swap_xy`;
- comandos de ventana `0x2A` y `0x2B`;
- inicio de escritura de memoria `0x2C`;
- adaptacion final a `display_hal_t`.

La plataforma F411 conserva solo pines, GPIO, pulsos `WRX`, retardos y transferencia fisica de bytes. La LUT de `db_lut.h` sigue reduciendo cada escritura de bus a tres accesos `BSRR`.

El shield ILI9486 se monta directamente sobre los conectores Arduino de la
Nucleo; los botones utilizan `PC0`, `PC2` y `PC3` en el conector Morpho CN7.
La asignacion completa, el orden de montaje y las precauciones de alimentacion
se encuentran en `docs/BUTTON_WIRING_GUIDE.md`.

## Entorno de ejecucion simplificado

El firmware no necesita consola serie, asignacion dinamica ni llamadas al
sistema. Se han retirado `syscalls.c`, `sysmem.c`, el modulo de USART2 y la
reserva minima de heap. Las aserciones LL siguen activas: deshabilitan las
interrupciones y detienen la ejecucion para permitir su inspeccion con el
depurador, sin llamar a `printf()`.

Esta simplificacion no elimina toda la biblioteca C. El startup conserva
`__libc_init_array()`, los botones utilizan `memset()` y la toolchain enlaza la
variante compacta seleccionada mediante `nano.specs`.

## Compilacion y validacion

Firmware STM32:

```bash
cmake --preset stm32f411re
cmake --build --preset stm32f411re --clean-first
```

La validacion automatica se limita a comprobar que el firmware se configura,
compila y enlaza de forma limpia. Para revisar el tamano del resultado:

```bash
arm-none-eabi-size build/stm32f411re/source/firmware.elf
```

No se mantiene una suite de pruebas de host. La validacion funcional se realiza
en la NUCLEO-F411RE comprobando la pantalla, los tres botones, la temporizacion y
el redibujado parcial.

## Limites actuales

- La unica plataforma de hardware implementada y validada es NUCLEO-F411RE.
- La compilacion cruzada y la inspeccion del ELF no sustituyen una comprobacion final sobre pantalla y botones fisicos.
- El diagnostico de aserciones requiere depurador porque ya no existe salida UART.
- `game_engine` se conserva como biblioteca independiente, pero no forma parte de la demostracion actual.
