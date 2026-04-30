# Componente ILI9486

Este directorio define el contrato generico del controlador LCD ILI9486. Su objetivo es separar el protocolo del dispositivo de la implementacion fisica sobre una placa concreta.

## Archivos

| Archivo | Responsabilidad |
|---|---|
| `ili9486.h` | Interfaz publica del componente |
| `ili9486.c` | Registro de la interfaz fisica inyectada |
| `CMakeLists.txt` | Biblioteca estatica `COMPONENT_ILI9486` |

## Diseño

La estructura `ili9486_8bitParallelInterface` modela las senales de un bus paralelo 8080:

- `RES`;
- `CS`;
- `DCX`;
- `WRX`;
- `RDX`;
- `DB[7:0]` para lectura y escritura.

El componente no incluye tipos STM32 ni accede a registros. La plataforma concreta proporciona callbacks para manipular pines fisicos.

## Estado actual

`ili9486_init()` almacena la interfaz inyectada. La secuencia de inicializacion y las funciones de pixel stream todavia estan implementadas en `platform_nucleof411re_ili9486.c`.

Esta distribucion fue util durante el bring-up hardware porque permitio validar rapidamente pines, tiempos y comandos. Como mejora futura, conviene mover al componente generico:

- constantes de comandos ILI9486;
- `write_command`;
- `write_data`;
- `set_addr_window`;
- `begin_pixels`;
- `push_color`;
- `push_pixels_rgb565`.

La plataforma deberia quedarse solo con GPIO, `BSRR`, temporizacion y mapeo fisico de pines.
