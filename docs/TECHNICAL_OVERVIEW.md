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
| Plataforma | `lib/platform/nucleof411re/` | Reloj, UART, SysTick, GPIO y bus paralelo fisico |
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

## Compilacion y pruebas

Firmware STM32:

```bash
cmake --preset stm32f411re
cmake --build --preset stm32f411re
```

Pruebas nativas:

```bash
cmake --preset host-tests
cmake --build --preset host-tests
ctest --test-dir build/host-tests --output-on-failure
```

Las pruebas cubren clipping y contexto del motor grafico, transiciones del modelo, secuencia de comandos del ILI9486 y renderizado completo sobre un framebuffer de ordenador.

## Limites actuales

- La unica plataforma de hardware implementada y validada es NUCLEO-F411RE.
- La compilacion y las pruebas nativas no sustituyen una comprobacion final sobre pantalla y botones fisicos.
- `game_engine` se conserva como biblioteca independiente, pero no forma parte de la demostracion actual.
