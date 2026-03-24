# nucleo-ili9486

Documentación inicial del proyecto:

- Plataforma NUCLEO-F411RE: [`lib/platform/nucleof411re/README.md`](lib/platform/nucleof411re/README.md)
- Componente ILI9486: [`lib/platform/component/ili9486/README.md`](lib/platform/component/ili9486/RE

- # Documentación técnica: componente `ili9486`

## 1) Propósito del componente

La carpeta `lib/platform/component/ili9486` define el **componente genérico del controlador LCD ILI9486**. Su misión es separar el *qué* (protocolo/comandos del display) del *cómo* (GPIO y temporización concreta del MCU).

En esta versión del proyecto, el componente ya define un contrato sólido de interfaz, mientras que parte de la lógica transaccional de alto nivel aún reside en la plataforma `nucleof411re`.

---

## 2) Contrato público (`ili9486.h`)

El núcleo de diseño es la estructura `ili9486_8bitParallelInterface`, que modela el bus paralelo 8080 de 8 bits mediante callbacks:

- Control de pines: `RES`, `CS`, `DCX`, `WRX`, `RDX`.
- Escritura de bus: `db70_write(uint8_t data)`.
- Lectura de bus: `db70_read(void)`.

### Razonamiento de arquitectura

Este patrón desacopla totalmente el componente del microcontrolador:

- **Portabilidad:** no hay dependencias directas de registros STM32 dentro de la API pública.
- **Testabilidad:** se pueden inyectar funciones fake/mock para validar secuencias de comandos.
- **Evolución:** el mismo componente puede reutilizarse con otras familias MCU o incluso con capas de simulación.

---

## 3) Estado actual de implementación (`ili9486.c`)

Actualmente el componente implementa:

- almacenamiento estático de la interfaz inyectada (`s_interface`);
- inicialización por copia en `ili9486_init(...)`.

Esto convierte al componente en un **punto de anclaje de arquitectura**: la frontera entre dispositivo genérico y plataforma física ya está establecida, aunque todavía no concentra toda la lógica de comandos.

---

## 4) Integración con la plataforma `nucleof411re`

La plataforma NUCLEO-F411RE realiza:

1. Configuración de pines físicos del bus (`GPIOA/B/C`, señales de control y DB0..DB7).
2. Construcción de una instancia `ili9486_8bitParallelInterface` con callbacks concretos.
3. Llamada a `ili9486_init(interface)` para registrar el backend físico.

### ¿Por qué esta distribución?

- El conocimiento de *pinout* pertenece a plataforma.
- El conocimiento de protocolo ILI9486 debe converger hacia el componente.
- El acoplamiento queda en una única seam: la estructura de callbacks.

---

## 5) Dependencias de build

`CMakeLists.txt` publica `COMPONENT_ILI9486` como `INTERFACE library`:

- exporta include dirs de plataforma/componente;
- expone `ili9486.c` como parte de la composición final.

Decisión útil para embedded: mantener bajo costo de enlace y una integración limpia con targets de plataforma.

---

## 6) Hoja de ruta recomendada (profesionalización)

Para madurar el componente y completar la separación por capas:

1. **Mover protocolo común al componente**
   - comandos (`SWRESET`, `SLPOUT`, `COLMOD`, `MADCTL`, `RAMWR`, etc.);
   - helpers `write_cmd`, `write_data8`, `write_data16`.

2. **Consolidar primitivas de pixel stream**
   - `set_addr_window`, `begin_pixels`, `push_color`, `push_pixels` dentro del componente;
   - mantener en plataforma solo control eléctrico de líneas.

3. **Definir enums/constantes de comandos**
   - eliminar números mágicos;
   - hacer auto-documentable cada secuencia de init.

4. **Añadir validación defensiva de interfaz**
   - assert/check de callbacks nulos en `ili9486_init`;
   - fallback claro en debug para detectar configuración incompleta.

5. **Preparar test de host**
   - backend mock que registre transacciones;
   - verificar orden de pulsos `CS/DCX/WRX` y payload esperado.

---

## 7) Criterios de diseño para futuras contribuciones

- Mantener el componente libre de detalles de `GPIO_TypeDef` o headers específicos de vendor.
- Todo acceso a registros debe vivir en la carpeta de plataforma.
- Las APIs públicas del componente deben ser pequeñas, explícitas y estables.
- Documentar cualquier timing crítico del bus paralelo en comentarios junto a la rutina relevante.

Con estos criterios, `ili9486` puede convertirse en un driver reusable y mantenible para múltiples objetivos embebidos.
