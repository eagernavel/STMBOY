# Resumen tecnico de STMBOY

Este documento resume el estado actual del proyecto. La documentacion extendida y razonada para memoria de TFM esta en [TFM_DOCUMENTACION_TECNICA.md](TFM_DOCUMENTACION_TECNICA.md).

## Objetivo

STMBOY implementa una aplicacion tipo Pong sobre una STM32 NUCLEO-F411RE, usando una pantalla ILI9486 conectada por bus paralelo de 8 bits y tres botones fisicos conectados a GPIO.

## Capas del sistema

| Capa | Directorio | Responsabilidad |
|---|---|---|
| Aplicacion | `source/main.c` | Maquina de estados de Pong, entrada, fisica y render incremental |
| BSP | `lib/bsp/` | Fachada de inicializacion de placa |
| Plataforma | `lib/platform/nucleof411re/` | Reloj, UART, SysTick, botones y bus fisico ILI9486 |
| Componente LCD | `lib/platform/component/ili9486/` | Contrato generico del controlador ILI9486 |
| Graficos | `lib/graphics/` | Primitivas 2D, texto y sprites sobre `display_hal_t` |
| Tipos comunes | `lib/common/` | Colores RGB565, sprites y HAL abstracto de display |
| Motor reutilizable | `lib/game_engine/` | Actor y animacion por frames |
| Dependencias ST | `third-party/stm32_driver/` | CMSIS, startup y drivers LL |

## Flujo de arranque

```text
main()
  -> bsp_init()
      -> platform_init()
          -> clock
          -> UART
          -> ILI9486
          -> botones
  -> systick_init(SystemCoreClock)
  -> stm32boy_init(...)
  -> bucle Pong
```

## Aplicacion Pong

`source/main.c` define:

- `pong_t`: estado del juego;
- `render_prev_t`: ultimo estado dibujado;
- `prv_update()`: entradas, fisica, puntuacion y modos;
- `prv_draw_full()`: redibujo completo;
- `prv_draw_incremental()`: redibujo parcial;
- `prv_draw_overlay()`: titulo, pausa y fin de partida.

Modos:

```text
MODE_TITLE -> MODE_RUNNING -> MODE_PAUSED
                         \-> MODE_POINT -> MODE_RUNNING
                         \-> MODE_GAME_OVER
```

La decision clave es el render incremental: se borra y redibuja solo bola, palas o marcador cuando cambian. Esto reduce el trafico por el bus paralelo y evita limpiar toda la pantalla a cada frame.

## Graficos

`lib/graphics/stm32boy.c` funciona sin framebuffer completo. Cada primitiva configura una ventana del display y envia pixeles inmediatamente mediante `display_hal_t`.

Funciones principales:

- `stm32boy_fill_rect()`;
- `stm32boy_draw_rect()`;
- `stm32boy_draw_line()`;
- `stm32boy_draw_bitmap_rgb565()`;
- `stm32boy_write_at()`.

## Display ILI9486

La plataforma NUCLEO-F411RE implementa el backend del display en `platform_nucleof411re_ili9486.c`.

Decisiones relevantes:

- bus paralelo 8080 de 8 bits;
- pines DB0..DB7 repartidos entre GPIOA/GPIOB/GPIOC;
- escritura atomica con `GPIOx->BSRR`;
- tabla `db_lut.h` para precalcular mascaras de escritura;
- API publica compatible con `display_hal_t`.

La frontera generica existe en `lib/platform/component/ili9486`, aunque parte del protocolo ILI9486 todavia vive en la plataforma. Es la principal mejora futura de arquitectura.

## Botones

Los botones son active-low con pull-up interno:

| Boton | GPIO | Funcion |
|---|---|---|
| `BTN_UP` | PC0 | Mover pala izquierda hacia arriba |
| `BTN_DOWN` | PC2 | Mover pala izquierda hacia abajo |
| `BTN_START` | PC3 | Iniciar, pausar o reiniciar |

El driver expone:

- `held`: pulsado ahora;
- `pressed`: flanco de pulsacion;
- `released`: flanco de liberacion.

## Build

Comandos principales:

```bash
cmake --preset stm32f411re
cmake --build --preset stm32f411re
make flash
```

## Riesgos y mejoras recomendadas

| Punto | Recomendacion |
|---|---|
| Preset `stm32f412zg` sin plataforma completa | Documentar como trabajo futuro o retirarlo |
| Protocolo ILI9486 dentro de plataforma | Migrar comandos al componente generico |
| `main.c` concentra Pong completo | Extraer `pong.c/.h` si se quiere mas modularidad |
| Bitmap sin clipping general | Anadir clipping o documentar precondicion |
| Codigo de self-test en driver ILI9486 | Separar o proteger con `#ifdef` |

