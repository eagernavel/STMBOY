# STMBOY - Documentacion tecnica del proyecto

> **Documento historico.** Este texto conserva decisiones de una version
> anterior y puede mencionar UART, `syscalls.c`, `sysmem.c` y pruebas de host
> que ya no forman parte del firmware final. La referencia actual es
> [TFM_READAPTADO.docx](../TFM_READAPTADO.docx).

> Documento base para memoria de Trabajo Final de Master.  
> Plataforma principal: STM32 NUCLEO-F411RE.  
> Pantalla: ILI9486 en bus paralelo 8080 de 8 bits.  
> Aplicacion final: juego tipo Pong con entrada por botones fisicos.

---

## 1. Objetivo del proyecto

STMBOY es un proyecto de sistema embebido bare-metal que implementa una pequena consola grafica sobre una placa STM32 NUCLEO-F411RE. El sistema integra una pantalla TFT ILI9486 mediante bus paralelo de 8 bits, tres pulsadores fisicos conectados a GPIO y una aplicacion interactiva tipo Pong.

El objetivo tecnico no es solo hacer funcionar una demo grafica, sino construir una base modular que separe:

- la aplicacion de juego;
- el motor grafico;
- la logica reutilizable de entidades/animacion;
- la adaptacion a hardware concreto;
- las dependencias CMSIS/LL de ST.

Esta separacion permite razonar el sistema por capas, facilita el mantenimiento y deja abierta la posibilidad de portar el proyecto a otra placa STM32 o a otro controlador de pantalla sin reescribir toda la aplicacion.

---

## 2. Vision global de arquitectura

La arquitectura se organiza de arriba hacia abajo:

```text
source/main.c
    |
    v
lib/bsp
    |
    v
lib/platform
    |
    +--> lib/platform/nucleof411re
    |       +--> reloj, UART, SysTick, botones, bus ILI9486
    |
    +--> lib/platform/component/ili9486
            +--> contrato generico del controlador LCD

lib/graphics
    +--> primitivas 2D, texto, sprites, HAL de display

lib/game_engine
    +--> actor y animacion por frames

third-party/stm32_driver
    +--> CMSIS, startup y drivers LL de ST
```

La aplicacion final en `source/main.c` une estas capas. El motor grafico no conoce registros STM32; solo conoce una interfaz abstracta `display_hal_t`. La plataforma conoce los pines fisicos y las secuencias electricas. El BSP ofrece una entrada simple de inicializacion (`bsp_init()`), ocultando la placa concreta a la aplicacion.

---

## 3. Decisiones de diseno principales

| Decision | Razonamiento | Contribucion al proyecto |
|---|---|---|
| C bare-metal sin RTOS | El alcance del proyecto es reducido y requiere control directo de GPIO/timings. | Reduce dependencias y facilita explicar el flujo completo de arranque. |
| STM32 LL en lugar de HAL completa | LL permite acceso mas directo y predecible a registros. | Mejora el control temporal del bus paralelo y reduce overhead. |
| Render inmediato sin framebuffer completo | Un framebuffer 320x480 RGB565 requiere 307200 bytes, demasiado para muchos STM32F411RE. | Permite dibujar en la pantalla usando poca RAM. |
| `display_hal_t` con punteros a funcion | Desacopla `graphics` del controlador fisico. | Facilita portabilidad y pruebas con backends alternativos. |
| Bus paralelo de 8 bits para ILI9486 | Aumenta ancho de banda frente a protocolos serie simples. | Permite refresco aceptable para primitivas y Pong. |
| Escritura GPIO con `BSRR` y LUT | Evita read-modify-write y calcula de antemano las mascaras de cada byte. | Mejora rendimiento y estabilidad temporal del bus. |
| Botones active-low con pull-up interno | Requiere menos componentes externos. | Simplifica cableado de breadboard y reduce costes. |
| Estado de botones con `held`, `pressed`, `released` | Distingue nivel actual y eventos de flanco. | Permite movimiento continuo y acciones puntuales con la misma API. |
| Render incremental en Pong | Solo borra/redibuja elementos que han cambiado. | Reduce trafico por bus y evita limpiar toda la pantalla cada frame. |

---

## 4. Flujo de ejecucion

El arranque de software sigue este orden:

1. `main()` llama a `bsp_init()`.
2. `bsp_init()` delega en `platform_init()`.
3. `platform_init()` configura reloj, UART, display ILI9486 y botones.
4. `main()` inicializa SysTick para obtener una base temporal en milisegundos.
5. Se crea el contexto grafico `stm32boy_t` con el backend ILI9486.
6. Se muestra la pantalla de titulo de Pong.
7. En el bucle principal:
   - se actualizan botones;
   - se lee `systick_millis()`;
   - se actualiza la maquina de estados del juego;
   - se redibuja solo lo necesario.

Este flujo deja claro que el hardware se estabiliza antes de que la aplicacion intente dibujar o leer entradas.

---

## 5. Revision archivo por archivo

### 5.1 Raiz del repositorio

#### `CMakeLists.txt`

Define el proyecto C/ASM, fija C11 y agrega tres bloques:

- `third-party/stm32_driver`: CMSIS, startup y drivers LL;
- `lib`: bibliotecas propias del proyecto;
- `source`: ejecutable final.

La decision de mantener un `CMakeLists.txt` raiz pequeno es correcta: el archivo actua como orquestador y deja las decisiones de cada modulo en su propio directorio.

#### `CMakePresets.json`

Define presets para `stm32f411re` y `stm32f412zg`. El preset activo y coherente con el proyecto actual es `stm32f411re`.

Inconsistencia detectada: existe un preset `stm32f412zg`, pero no hay implementacion de plataforma `lib/platform/nucleof412zg`. El sistema de build tiene la intencion de ser portable, pero esa segunda plataforma aun no esta completada. Para una entrega final conviene documentarlo como trabajo futuro o retirar temporalmente el preset para evitar confusion.

#### `Makefile`

Envuelve los comandos de CMake mas habituales:

- `make configure`;
- `make`;
- `make clean`;
- `make flash`;
- `make allf`.

La decision es pragmatica: conserva CMake como sistema real de build y ofrece una interfaz corta para uso diario. `flash` usa OpenOCD con ST-Link y programa `firmware.bin` en `0x08000000`, direccion base de Flash en STM32F4.

#### `README.md`

Actualmente es minimo. Para TFM deberia funcionar como puerta de entrada: objetivo, hardware requerido, comandos de build, estructura de carpetas y enlaces a documentacion. Este documento propone completarlo y mantener el detalle profundo en `docs/`.

---

### 5.2 `source/`

#### `source/main.c`

Es la aplicacion final. Implementa un Pong simple con:

- pantalla de titulo;
- modo de juego;
- pausa;
- retardo tras punto;
- pantalla de fin;
- jugador izquierdo controlado por botones;
- pala derecha controlada por CPU.

La estructura interna separa logica y render:

- `pong_t` contiene estado de juego;
- `render_prev_t` guarda el estado dibujado previamente;
- `prv_update()` actualiza fisica, entradas, puntuacion y modo;
- `prv_draw_full()` dibuja la escena completa;
- `prv_draw_incremental()` actualiza solo los elementos modificados;
- `prv_draw_overlay()` muestra titulo, pausa o fin de juego.

Decision relevante: el render incremental evita redibujar pantalla completa cada iteracion. En un display conectado por bus paralelo bit-banged, esta optimizacion es importante porque el coste dominante es el envio de pixeles.

Punto de simplificacion: `main.c` ya contiene bastante logica. Para una evolucion posterior, se podria extraer Pong a `lib/game_engine/pong.c/.h` o a `source/pong.c/.h`. No es obligatorio para el TFM si se explica que `main.c` actua como aplicacion de demostracion final, pero la separacion mejoraria legibilidad.

#### `source/syscalls.c`

Proporciona stubs de llamadas al sistema que newlib necesita en un entorno bare-metal: `_read`, `_write`, `_close`, `_fstat`, `_isatty`, etc.

La mayoria devuelven error o comportamiento minimo, lo cual es habitual en microcontroladores sin sistema operativo. `_write` es debil y queda sustituida por la implementacion UART de `platform_nucleof411re_serial.c`.

#### `source/sysmem.c`

Implementa `_sbrk()` para dar soporte a heap/newlib. Protege el crecimiento del heap para que no invada la region reservada a la pila.

Aunque el proyecto evita uso intensivo de memoria dinamica, mantener `_sbrk()` permite enlazar funciones de libc que puedan requerir heap. La decision es conservadora y compatible con el ecosistema STM32.

#### `source/assert.c`

Integra dos mecanismos de assert:

- `assert_failed()` para asserts de STM32 LL cuando `USE_FULL_ASSERT` esta activo;
- `__assert_func()` para `assert()` de libc.

Ambos imprimen por UART, deshabilitan interrupciones y entran en bucle infinito. Esto es adecuado en bare-metal: ante una precondicion rota, detener el sistema facilita depuracion y evita continuar en estado indefinido.

---

### 5.3 `lib/common/`

#### `lib/common/types.h`

Centraliza tipos puros compartidos:

- colores RGB565;
- `stm32boy_color_t`;
- `sprite_t`;
- `rect_t`.

La decision evita dependencias circulares entre `graphics` y `game_engine`. Un sprite puede ser usado por el motor de animacion y dibujado por el motor grafico sin que ambos modulos se conozcan directamente.

#### `lib/common/display_hal.h`

Define `display_hal_t`, la abstraccion minima que necesita `graphics` para escribir pixeles:

- configurar ventana;
- iniciar transaccion;
- finalizar transaccion;
- enviar color repetido;
- enviar buffer RGB565.

Esta es una de las decisiones arquitectonicas mas importantes del proyecto. Permite que `stm32boy.c` sea independiente de ILI9486 y de STM32.

---

### 5.4 `lib/bsp/`

#### `lib/bsp/bsp.h` y `lib/bsp/bsp.c`

Ofrecen una fachada de inicializacion de placa:

```c
void bsp_init(void) {
    platform_init();
}
```

Aunque hoy es un wrapper pequeno, es util porque fija una frontera conceptual: la aplicacion inicializa la "placa" mediante BSP, no mediante detalles de plataforma. Esto permite reemplazar `platform_init()` en el futuro sin cambiar la aplicacion.

#### `lib/bsp/CMakeLists.txt`

Crea la biblioteca `BSP` y la enlaza con `PLATFORM`. Esta decision hace que el ejecutable solo necesite enlazar `BSP`, `graphics` y `game_engine`, manteniendo encapsulada la seleccion de placa.

---

### 5.5 `lib/graphics/`

#### `lib/graphics/stm32boy.h`

Define el contexto `stm32boy_t` y la API publica de dibujo:

- rectangulos, lineas, triangulos y poligonos;
- pixeles y lineas rapidas;
- bitmaps/sprites;
- texto con fuente 5x7;
- medicion de texto.

La estructura `stm32boy_t` contiene dimensiones, estado de texto y puntero al `display_hal_t`. Es una forma idiomatica en C de emular un "objeto" de renderizado sin usar C++.

#### `lib/graphics/stm32boy.c`

Implementa render inmediato. No mantiene framebuffer completo en RAM: cada primitiva traduce coordenadas a una ventana de escritura y envia pixeles al display.

Decisiones tecnicas relevantes:

- `stm32boy_fill_rect()` hace clipping contra los limites de pantalla. Esto protege al driver de coordenadas fuera de rango.
- `stm32boy_draw_line()` usa Bresenham dividido en casos horizontal-major y vertical-major. Es eficiente porque evita coma flotante.
- `stm32boy_draw_rect()` reutiliza `fill_rect()` para cuatro bordes.
- `stm32boy_draw_char()` escala caracteres rellenando rectangulos, lo que reutiliza primitivas existentes.
- `stm32boy_draw_bitmap_rgb565()` envia buffers directos, util para sprites.

Puntos de simplificacion:

- `stm32boy_draw_bitmap_rgb565()` no aplica clipping. Si se llama con sprites parcialmente fuera de pantalla podria enviar una ventana invalida. Para Pong no ocurre, pero para una libreria general convendria anadir clipping o documentar la precondicion.
- La direccion vertical del texto en `stm32boy_write()` usa `cy -= advance_y` en saltos de linea. Si el sistema de coordenadas de pantalla crece hacia abajo, lo esperable seria `cy += advance_y`. Si actualmente no se usa texto multilinea, no rompe la demo, pero conviene revisarlo antes de presentar la libreria como general.
- Hay estilos de comentario mixtos en ingles/espanol. Para memoria TFM no es critico, pero para mantenimiento conviene homogeneizar.

#### `lib/graphics/CMakeLists.txt`

Compila `stm32boy.c` como biblioteca estatica `graphics`. Es correcto porque el modulo contiene codigo `.c`, no solo headers.

---

### 5.6 `lib/game_engine/`

#### `lib/game_engine/animation.h` y `animation.c`

Implementan una animacion de sprites por frames. El avance se basa en diferencias de tiempo:

```c
if ((now_ms - last_tick_ms) >= frame_duration_ms) { ... }
```

Esta forma es robusta ante overflow de `uint32_t`, siempre que se trabaje con diferencias sin signo, una practica comun en sistemas embebidos.

Aunque la aplicacion final de Pong no usa sprites animados, este modulo demuestra una arquitectura preparada para juegos mas ricos. Para TFM puede explicarse como componente reutilizable desarrollado durante el proyecto.

#### `lib/game_engine/actor.h` y `actor.c`

Definen una entidad con posicion, velocidad, dimensiones y animacion embebida. `prev_x` y `prev_y` permiten borrar la posicion anterior sin limpiar toda la pantalla.

Igual que `animation`, este modulo no es central en el Pong final, pero documenta una etapa evolutiva del proyecto: de primitivas graficas a entidades moviles.

Punto de simplificacion: si el TFM se centra solo en Pong, se podria etiquetar `game_engine` como modulo experimental/reutilizable. Si se quiere una memoria mas cohesionada, se puede extraer la logica de Pong a este modulo y dejar `main.c` mas pequeno.

#### `lib/game_engine/CMakeLists.txt`

Construye `game_engine` como biblioteca estatica y exporta includes desde `lib/`. Es coherente con el diseno modular.

---

### 5.7 `lib/platform/`

#### `lib/platform/platform.h`

Define la funcion generica:

```c
void platform_init(void);
```

Esta pequena API permite que BSP y aplicacion no dependan del nombre de la placa concreta.

#### `lib/platform/CMakeLists.txt`

Selecciona la plataforma concreta en funcion de `PLATFORM_TARGET`. Para `PLATFORM_NUCLEOF411RE` enlaza `PLATFORM_NUCLEOF411RE`.

Inconsistencia: contempla `PLATFORM_NUCLEOF412ZG`, pero no existe el target correspondiente. Esto debe tratarse como extension futura o eliminarse hasta que haya carpeta de plataforma.

---

### 5.8 `lib/platform/component/ili9486/`

#### `ili9486.h`

Define `ili9486_8bitParallelInterface`, una tabla de callbacks para controlar:

- `RES`;
- `CS`;
- `DCX`;
- `WRX`;
- `RDX`;
- escritura de `DB[7:0]`;
- lectura de `DB[7:0]`.

La decision separa el contrato del dispositivo de la implementacion fisica. El componente generico no incluye `GPIO_TypeDef` ni headers STM32.

#### `ili9486.c`

Actualmente almacena la interfaz inyectada mediante `ili9486_init()`. La implementacion es minima, pero importante como punto de anclaje arquitectonico.

Punto de mejora: la secuencia de comandos ILI9486 todavia vive en `platform_nucleof411re_ili9486.c`. Para una arquitectura mas limpia, comandos como `0x2A`, `0x2B`, `0x2C`, `0x36`, `0x3A` deberian migrar gradualmente al componente generico, dejando en plataforma solo GPIO y temporizacion.

#### `CMakeLists.txt`

Crea `COMPONENT_ILI9486` como biblioteca estatica. Esto es coherente con que `ili9486.c` contiene codigo compilable.

---

### 5.9 `lib/platform/nucleof411re/`

#### `platform_nucleof411re.c`

Orquesta la inicializacion de la placa:

1. reloj;
2. UART;
3. display;
4. botones.

Usa estructuras de configuracion para reloj, UART y botones. Esta decision reduce valores magicos dispersos y concentra el pinout en un lugar claro.

#### `platform_nucleof411re_clock.h` y `.c`

Configuran HSI, PLL, latencia Flash, prescalers AHB/APB y actualizan `SystemCoreClock`. Tambien llaman a `LL_Init1msTick(SystemCoreClock)` para que `LL_mDelay()` funcione de forma fiable.

La configuracion lleva el nucleo a 100 MHz usando HSI y PLL. El uso de una estructura `platform_nucleof411re_clock_Config` permite explicar claramente cada parametro de reloj en la memoria.

#### `platform_nucleof411re_serial.h` y `.c`

Inicializan USART2 TX en PA2 a 115200 baudios y redirigen `_write()` para que `printf()` envie texto por UART.

Decision: transmision bloqueante. Es sencilla y adecuada para depuracion, aunque puede afectar rendimiento si se imprime mucho durante el juego. En el estado actual, se usa como herramienta de diagnostico, no como canal de datos critico.

#### `systick.h` y `.c`

Configuran SysTick para una interrupcion cada 1 ms y exponen:

- `systick_millis()`;
- `systick_delay_ms()`.

Esto evita delays bloqueantes para la logica principal. Pong usa `systick_millis()` para ejecutar la fisica cada `FRAME_MS`.

#### `platform_nucleof411re_buttons.h` y `.c`

Implementan tres botones:

- `BTN_UP`;
- `BTN_DOWN`;
- `BTN_START`.

Los pines son:

| Boton | GPIO | Pin fisico STM32 |
|---|---|---|
| `BTN_UP` | GPIOC | PC0 |
| `BTN_DOWN` | GPIOC | PC2 |
| `BTN_START` | GPIOC | PC3 |

El driver configura entradas con pull-up interno y lectura active-low. El estado resultante se expone como:

- `held`: nivel actual;
- `pressed`: flanco de pulsacion;
- `released`: flanco de liberacion.

Esta API es apropiada para videojuegos porque separa controles continuos de acciones discretas.

#### `platform_nucleof411re_ili9486.h` y `.c`

Es el modulo mas cercano al hardware del display. Contiene:

- definicion de pines de control y datos;
- configuracion de GPIOA/GPIOB/GPIOC;
- escritura del bus DB0..DB7;
- lectura del bus;
- pulsos `WR`;
- secuencia de inicializacion ILI9486;
- funciones publicas usadas por `display_hal_t`.

La decision de usar `GPIOx->BSRR` es clave. `BSRR` permite poner y limpiar bits de forma atomica, sin leer/modificar/escribir `ODR`. Esto reduce jitter y evita efectos colaterales al compartir un puerto con otros pines.

La LUT `g_dbLut[256]` precalcula las mascaras BSRR para cada byte posible. Como los ocho bits DB0..DB7 estan repartidos entre GPIOA, GPIOB y GPIOC, calcular las mascaras en tiempo real seria costoso. La tabla desplaza ese coste a memoria Flash y acelera cada escritura de pixel.

Puntos de simplificacion:

- Hay codigo comentado historico y funciones de self-test dentro del mismo archivo. Para una entrega final se puede conservar si se documenta como soporte de bring-up, pero una version mas limpia separaria `selftest` en otro archivo o lo protegeria con `#ifdef`.
- Los comandos ILI9486 se expresan como literales (`0x2A`, `0x2B`, `0x2C`, etc.). Conviene crear `enum` o `#define` con nombres semanticos.
- El componente generico `ili9486` todavia no absorbe el protocolo. Es el principal trabajo futuro de arquitectura.

#### `db_lut.h`

Tabla generada con 256 entradas para acelerar escritura de DB0..DB7. Cada entrada contiene tres valores BSRR: uno por GPIOA, GPIOB y GPIOC.

Esta tabla es una optimizacion razonable en un proyecto grafico embebido. Aumenta el consumo de Flash, pero reduce instrucciones por byte enviado al display.

#### `README.md`

Describe la plataforma NUCLEO-F411RE. Debe mantenerse alineado con este documento principal, especialmente en resolucion, pinout y estado actual de Pong.

#### `CMakeLists.txt`

Construye `PLATFORM_NUCLEOF411RE` como biblioteca estatica y la enlaza con `COMPONENT_ILI9486` y `STM32_LL_DRIVER`. Esta decision es correcta: la plataforma contiene codigo compilable y depende directamente de drivers STM32.

---

### 5.10 `cmake/toolchain/`

#### `cm4-fpu.cmake`

Define compilador `arm-none-eabi-*`, flags de Cortex-M4, FPU hard-float, eliminacion de secciones no usadas y sufijo `.elf`.

La decision de centralizar flags comunes evita duplicacion entre microcontroladores STM32F4.

#### `stm32f411.cmake`

Selecciona el linker script `STM32F411RETX_FLASH.ld` y `nano.specs`, y luego incluye `cm4-fpu.cmake`.

#### `stm32f412.cmake`

Equivalente para STM32F412ZG. Como no existe plataforma F412 completa, debe considerarse soporte parcial.

---

### 5.11 `ld/`

Los linker scripts describen mapa de memoria Flash/RAM y simbolos usados por startup/newlib (`_estack`, `_end`, `_Min_Stack_Size`, etc.).

El script realmente usado por el preset principal es `STM32F411RETX_FLASH.ld`. Los otros scripts indican intencion de soportar mas placas, pero deben documentarse como infraestructura no finalizada.

---

### 5.12 `third-party/stm32_driver/`

Contiene CMSIS, startup assembly, `system_stm32f4xx.c` y drivers LL necesarios. No es codigo propio, pero es parte critica del build.

El `CMakeLists.txt` crea `STM32_LL_DRIVER` como biblioteca de interfaz que exporta includes, fuentes LL necesarias y defines:

- `STM32F411xE` o `STM32F412Zx`;
- `USE_FULL_ASSERT`;
- `USE_FULL_LL_DRIVER`.

---

### 5.13 `docs/`

#### `docs/BUTTON_WIRING_GUIDE.md`

Guia de conexion de botones en breadboard. Es importante porque une firmware y montaje fisico. Debe mantenerse con el pinout real de NUCLEO-F411RE: GND en CN7 8/19/20/22 y botones en PC0/PC2/PC3.

#### `docs/TECHNICAL_OVERVIEW.md`

Documento tecnico previo sobre modulos incorporados. Parte del contenido sigue siendo util, pero estaba centrado en una etapa anterior con actores/sprites. Este documento TFM actualiza la vision hacia la aplicacion Pong final.

#### `docs/revision_arquitectura_stm32boy.html` y `.pdf`

Material de revision arquitectonica. Puede citarse como evidencia del proceso iterativo de refactor y mejora.

---

## 6. Inconsistencias y recomendaciones

| Prioridad | Punto | Impacto | Recomendacion |
|---|---|---|---|
| Alta | Preset `stm32f412zg` sin plataforma correspondiente | Puede confundir al evaluador o fallar al configurar. | Documentarlo como trabajo futuro o retirar preset. |
| Alta | Protocolo ILI9486 repartido entre componente y plataforma | Mezcla responsabilidad generica con pinout concreto. | Migrar comandos y API de pixel stream al componente `ili9486`. |
| Media | `main.c` concentra toda la aplicacion Pong | Legibilidad limitada a medida que crezca. | Extraer `pong.c/.h` si se quiere mostrar arquitectura de aplicacion mas limpia. |
| Media | `stm32boy_draw_bitmap_rgb565()` sin clipping | Riesgo si se dibujan sprites fuera de pantalla. | Anadir clipping o documentar precondicion. |
| Media | Texto multilinea usa decremento de Y | Puede ser contraintuitivo en coordenadas de pantalla. | Revisar y alinear con sistema de coordenadas elegido. |
| Baja | Codigo de test/commentado dentro de ILI9486 | Ruido en lectura academica. | Mover a `selftest` o proteger con `#ifdef`. |
| Baja | Mezcla de comentarios en ingles y espanol | Presentacion menos uniforme. | Homogeneizar comentarios finales. |

---

## 7. Como contribuye cada capa al resultado final

- `source/main.c` demuestra el sistema con una aplicacion interactiva real.
- `lib/graphics` permite construir interfaz visual sin depender del hardware.
- `lib/platform/nucleof411re` convierte operaciones abstractas en senales fisicas.
- `lib/common` evita acoplamiento innecesario entre motores.
- `lib/bsp` oculta detalles de placa al punto de entrada.
- `third-party/stm32_driver` aporta la base CMSIS/LL y startup.
- `cmake/`, `CMakePresets.json` y `Makefile` hacen reproducible la compilacion.
- `docs/` conecta software, arquitectura y montaje fisico.

La aportacion global es una cadena completa desde pulsador fisico hasta respuesta grafica en pantalla, pasando por inicializacion de microcontrolador, lectura GPIO, logica de juego y escritura directa al controlador LCD.

---

## 8. Conclusiones tecnicas

El proyecto alcanza un resultado funcional y defendible para un TFM porque integra hardware real, firmware bare-metal, render grafico y una aplicacion interactiva. La arquitectura por capas muestra criterio de ingenieria: se evita acoplar la logica de dibujo a registros STM32 y se encapsula el pinout en plataforma.

La principal deuda tecnica no esta en la funcionalidad, sino en la limpieza de fronteras: el driver ILI9486 de plataforma todavia contiene protocolo que conceptualmente pertenece al componente generico. Esta deuda es comprensible en un proyecto incremental de bring-up hardware, pero debe explicarse como decision temporal y linea de mejora.

Para una memoria final, la narrativa recomendable es:

1. empezar por el problema de construir una consola grafica bare-metal;
2. justificar restricciones de memoria y tiempo;
3. presentar la arquitectura modular;
4. explicar el bus ILI9486 y la optimizacion con LUT/BSRR;
5. explicar entrada active-low y estado por flancos;
6. cerrar con Pong como validacion de integracion completa.
