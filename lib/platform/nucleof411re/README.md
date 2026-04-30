# Plataforma NUCLEO-F411RE

Este directorio implementa la adaptacion a hardware para ejecutar STMBOY en una placa STM32 NUCLEO-F411RE con pantalla ILI9486 en bus paralelo de 8 bits.

## Responsabilidad

La plataforma configura y controla los perifericos concretos de la placa:

- reloj del sistema;
- UART de depuracion;
- SysTick de 1 ms;
- GPIO de botones;
- GPIO y bus paralelo del display ILI9486.

La aplicacion no accede directamente a registros STM32. Lo hace a traves de las APIs publicas de plataforma, BSP y motor grafico.

## Archivos

| Archivo | Responsabilidad |
|---|---|
| `platform_nucleof411re.c` | Orquestacion de inicializacion |
| `platform_nucleof411re_clock.*` | HSI, PLL, Flash latency y prescalers |
| `platform_nucleof411re_serial.*` | USART2 TX y redireccion de `_write()` |
| `systick.*` | Contador de milisegundos por interrupcion SysTick |
| `platform_nucleof411re_buttons.*` | Tres botones active-low con pull-up interno |
| `platform_nucleof411re_ili9486.*` | Bus paralelo 8080, inicializacion y pixel stream |
| `db_lut.h` | Tabla de mascaras BSRR para acelerar DB0..DB7 |
| `CMakeLists.txt` | Target estatico `PLATFORM_NUCLEOF411RE` |

## Flujo de inicializacion

```text
platform_init()
  -> prv_init_clock()
  -> prv_init_serial()
  -> platform_nucleof411re_ili9486_init()
  -> prv_init_buttons()
```

El orden es deliberado:

1. El reloj se configura primero porque UART, delays y bus dependen de `SystemCoreClock`.
2. UART se inicializa temprano para permitir trazas de depuracion.
3. El display se deja en estado conocido antes de que la aplicacion dibuje.
4. Los botones se configuran antes del bucle principal.

## Botones

Los botones usan pull-up interno y se conectan entre GPIO y GND:

| Boton | GPIO |
|---|---|
| `BTN_UP` | PC0 |
| `BTN_DOWN` | PC2 |
| `BTN_START` | PC3 |

El driver invierte la lectura para exponer `true` cuando el boton esta pulsado. La guia de cableado esta en `docs/BUTTON_WIRING_GUIDE.md`.

## Display ILI9486

El display se controla por bus paralelo de 8 bits. Los datos DB0..DB7 estan repartidos entre GPIOA, GPIOB y GPIOC, por lo que la escritura de un byte requiere actuar sobre tres puertos.

Para reducir coste por pixel:

- se usa `GPIOx->BSRR` para escrituras atomicas;
- `db_lut.h` precalcula las mascaras de set/reset para cada valor de 8 bits;
- `ili9486_push_color()` y `ili9486_push_pixels_rgb565()` envian datos en formato RGB565.

## Notas de arquitectura

Actualmente este modulo contiene tanto acceso fisico a pines como parte del protocolo ILI9486. Es aceptable como resultado funcional de bring-up hardware, pero la evolucion recomendada es mover comandos y secuencias genericas al componente `lib/platform/component/ili9486`.
