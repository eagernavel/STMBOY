# Documentación técnica: plataforma `nucleof411re`

## 1. Objetivo del módulo

La carpeta `lib/platform/nucleof411re` implementa la **capa de adaptación a hardware (HAL/BSP local)** para ejecutar STMBOY sobre una placa **NUCLEO-F411RE** con una pantalla **ILI9486** en bus paralelo de 8 bits.

La decisión arquitectónica principal es separar el proyecto en capas:

- **Aplicación** (`source/main.c`): lógica de demostración y uso de API gráfica.
- **Plataforma** (`lib/platform/nucleof411re`): reloj, UART, GPIO y transporte físico al display.
- **Componente de dispositivo** (`lib/platform/component/ili9486`): contrato del periférico ILI9486 desacoplado del microcontrolador.

Esta separación permite portar el mismo componente gráfico a otro MCU cambiando solo la carpeta de plataforma.

---

## 2. Arquitectura por capas

## 2.1 Capa de aplicación

En `main.c` se inicializa la plataforma/display y se ejercitan primitivas gráficas (`líneas`, `rectángulos`, `triángulos`, `texto`) sobre un contexto `stm32boy_t`.

Responsabilidades:

- inicialización de alto nivel;
- orquestación de demo/render;
- loop principal.

No contiene detalles de reloj, multiplexado GPIO o protocolo de bajo nivel.

## 2.2 Capa de plataforma NUCLEO-F411RE

Incluye módulos especializados:

- `platform_nucleof411re_clock.c`: configuración de HSI + PLL, prescalers y `SystemCoreClock`.
- `platform_nucleof411re_serial.c`: inicialización de USART2 TX y redirección de `_write` para `printf`.
- `platform_nucleof411re_ili9486.c`: mapeo de pines y secuencia de señalización para el bus paralelo DB[7:0] del LCD.
- `stm32boy.c/.h`: primitivas de dibujo y texto sobre framebuffer lógico inmediato (sin framebuffer en RAM).

Razonamiento de diseño:

- **Cohesión alta por periferia**: cada archivo gestiona una responsabilidad de hardware clara.
- **Acoplamiento bajo con aplicación**: la app usa APIs estables (`stm32_*`, `ili9486_*`), no registros.
- **Portabilidad dirigida**: mover a otra placa implica reemplazar configuración de reloj/pines y funciones físicas de bus.

## 2.3 Capa componente ILI9486

`lib/platform/component/ili9486/ili9486.h` define `ili9486_8bitParallelInterface` como tabla de funciones para controlar señales (`CS`, `DCX`, `WR`, `RD`, `DB[7:0]`).

Justificación:

- abstrae el controlador LCD como un dispositivo con interfaz de callbacks;
- permite pruebas unitarias por sustitución de funciones (mocking del bus);
- evita hardcodear registros STM32 dentro del componente genérico.

> Nota: actualmente el componente base es mínimo (`ili9486_init`) y gran parte de las primitivas de LCD viven todavía en la capa `platform_nucleof411re_ili9486.c`.

---

## 3. Flujo de arranque (boot de software)

Flujo seguido en el código:

1. Configurar clock del MCU (HSI + PLL) para asegurar timings correctos de buses.
2. Inicializar UART para trazas tempranas.
3. Configurar GPIO de control y datos del ILI9486.
4. Ejecutar secuencia de init del display.
5. Inicializar contexto gráfico `stm32boy_t`.
6. Renderizar primitivas.

### ¿Por qué este orden?

- El cálculo de baudrate UART depende de `SystemCoreClock`.
- El display requiere líneas de control en estado conocido antes de comandos.
- Las primitivas gráficas asumen controlador LCD operativo.

---

## 4. Diseño del subsistema gráfico (`stm32boy`)

`stm32boy_t` mantiene estado de dibujo:

- dimensión lógica (`width`, `height`);
- cursor de texto;
- colores fg/bg;
- escala y modo transparente.

### Decisiones técnicas

- **Render inmediato**: los píxeles se escriben directamente al LCD (no framebuffer completo).
  - Ventaja: menor uso de RAM en STM32F411.
  - Coste: más tráfico de bus por operación pequeña.
- **Clipping básico en primitivas** (`fillRect`) para robustez y protección de límites.
- **Bresenham en líneas** (`stm32_line_h` / `stm32_line_v`) para trazado entero eficiente sin flotantes.

---

## 5. Interfaz física ILI9486 en NUCLEO-F411RE

El archivo `platform_nucleof411re_ili9486.c` define:

- pines de control (`RES`, `CS`, `DCX`, `WR`, `RD`);
- pines de datos discontiguos en GPIOA/B/C para DB0..DB7;
- helpers inline que escriben en `BSRR` (set/reset atómico por bitmask).

### Justificación de implementación con `BSRR`

- operación atómica por puerto;
- evita read-modify-write sobre `ODR`;
- minimiza jitter temporal en señales críticas del bus paralelo.

También hay funciones para alternar DB[7:0] entre entrada/salida, habilitando lectura/escritura del controlador cuando haga falta.

---

## 6. Reloj y serie

## 6.1 Clock

`platform_nucleof411re_clock_init`:

- habilita HSI;
- configura regulador y wait states de FLASH;
- configura y habilita PLL;
- ajusta prescalers AHB/APB;
- conmuta `SYSCLK` a PLL;
- actualiza `SystemCoreClock`.

### Motivo

Esta secuencia protege estabilidad eléctrica/temporal antes de elevar frecuencia de CPU y buses.

## 6.2 Serial

`platform_nucleof411re_serial_init` configura USART2 en TX-only (8N1) y `_write` envía byte a byte con espera bloqueante (`TC`/`TXE`).

### Motivo

- simplicidad para bring-up y depuración temprana;
- sin dependencias de RTOS/IRQ/DMA;
- coste aceptable en fase inicial de proyecto.

---

## 7. Build e integración

`lib/platform/nucleof411re/CMakeLists.txt` publica un target de interfaz que agrega:

- fuentes de plataforma;
- dependencia de `COMPONENT_ILI9486`;
- dependencia de drivers STM32 LL (`STM32_LL_DRIVER`).

Ventaja: composición modular desde CMake sin exponer detalles internos al ejecutable final.

---

## 8. Riesgos técnicos actuales y mejoras sugeridas

1. **Duplicidad conceptual de driver ILI9486**
   - hoy hay API en `component/ili9486` y lógica extensa en `platform_nucleof411re_ili9486.c`.
   - mejora: mover protocolo común al componente y dejar en plataforma solo callbacks físicos.

2. **Inconsistencias de resolución**
   - en algunos puntos aparece ancho `320`, y en `platform_nucleof411re_ili9486.c` hay macro `ILI9486_WIDTH 380`.
   - mejora: centralizar resolución activa en una sola constante pública.

3. **API naming legacy**
   - coexistencia de `stm32_Line` y estilos `snake_case`/`camelCase`.
   - mejora: definir convención única y plan de deprecación.

4. **TX bloqueante**
   - para cargas altas de log, `_write` puede impactar latencia de render.
   - mejora futura: UART por DMA o ring buffer con interrupciones.

---

## 9. Guía de evolución de arquitectura

Para mantener escalabilidad del proyecto embedded:

- conservar la frontera **componente genérico ↔ plataforma MCU**;
- aislar mapeos de pines por placa en un único módulo;
- preferir estructuras de configuración explícitas (como en `clock` y `serial`);
- introducir pruebas por host para primitivas geométricas puras (líneas, clipping, polígonos);
- documentar timings mínimos del bus LCD cuando se optimice throughput.

Con estas reglas, STMBOY puede crecer hacia más placas STM32 o incluso otros vendors sin rehacer la capa de aplicación.
