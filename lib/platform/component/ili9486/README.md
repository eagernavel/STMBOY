# Componente ILI9486

Este directorio define el contrato generico del controlador LCD ILI9486. Su objetivo es separar el protocolo del dispositivo de la implementacion fisica sobre una placa concreta.

## Archivos

| Archivo | Responsabilidad |
|---|---|
| `ili9486.h` | Interfaz publica del componente |
| `ili9486.c` | Protocolo, inicializacion y adaptacion a `display_hal_t` |
| `CMakeLists.txt` | Biblioteca estatica `COMPONENT_ILI9486` |

## Diseño

La estructura `ili9486_bus_t` define las operaciones fisicas que debe proporcionar cualquier plataforma:

- control de reset;
- escritura de comandos y datos;
- inicio y fin de un flujo de pixeles;
- transferencia optimizada de colores y buffers RGB565;
- retardo en milisegundos.

El componente no incluye tipos STM32 ni accede a registros. La plataforma concreta proporciona callbacks para el transporte fisico y el componente conserva el protocolo ILI9486.

## Estado actual

`ili9486_init()` valida e inyecta el bus, ejecuta el reset y la secuencia de configuracion RGB565, y publica una instancia de `display_hal_t` con contexto y dimensiones. Las ventanas de direccion y el comando de escritura de memoria tambien se resuelven en este componente.
