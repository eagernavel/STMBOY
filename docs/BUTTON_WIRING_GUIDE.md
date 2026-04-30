# Guía de Conexión: Botones de 4 Pines en Breadboard con STM32F411RE (Nucleo-F411RE)

---

## 1. Material necesario

- Placa **NUCLEO-F411RE**
- 3 × pulsador de 4 pines (tacto, 6×6 mm o similar)
- 1 × breadboard (protoboard)
- Cables Dupont macho-macho o macho-hembra, según necesidad
- (Opcional) 3 × resistencia de 10 kΩ pull-down externas — **no necesarias** porque el firmware usa el pull-up interno del STM32

---

## 2. Anatomía del pulsador de 4 pines

Los pulsadores de tactil de 4 pines tienen dos pares de pines internamente cortocircuitados. Vistos desde arriba:

```
    A ●───── B ●
      │         │
    (↕↑ pulsador interno ↕)
      │         │
    C ●───── D ●
```

- **A y B** están siempre conectados entre sí (mismo nodo).
- **C y D** están siempre conectados entre sí (mismo nodo).
- Al pulsar el botón, el nodo A-B se conecta con el nodo C-D.

### Cómo colocarlo en la breadboard

El pulsador se inserta a **caballo entre el canal central** de la breadboard, con cada par de pines en columnas distintas:

```
  ────────────────────────────────────────
  col:  ...  j   i   h   g   f [GAP] e   d   c   b   a  ...
                  ●───●                 ●───●
                  A   B                 C   D
                  │                     │
             (a este nodo          (a este nodo
              se conecta           se conecta GND)
               el GPIO)
```

> **Regla práctica**: usa solo dos pines diagonalmente opuestos, por ejemplo **A** y **D** (o **B** y **C**). El resultado es el mismo.

---

## 3. Esquema de conexión (active-low con pull-up interno)

El firmware configura el GPIO como **entrada con pull-up interno**. Por eso, el esquema es:

```
STM32 GPIO pin ──────── pulsador ──────── GND
    (pull-up interno a VDD 3.3 V)
```

- **Reposo** (botón sin pulsar): el pin flota a 3.3 V → nivel lógico `1`.
- **Pulsado**: el pin se lleva a GND → nivel lógico `0`.
- El driver en firmware **invierte** el nivel: `!LL_GPIO_IsInputPinSet(...)` → `true` cuando pulsado.

No se necesitan resistencias externas porque el pull-up interno (~40 kΩ) es suficiente para este uso.

---

## 4. Asignación de pines según el firmware

El archivo `platform_nucleof411re.c` define la siguiente configuración:

| Botón | GPIO | Pin STM32 | Función en el juego |
|-------|------|-----------|---------------------|
| `BTN_UP` | GPIOC | **PC0** | Mover la pala izquierda hacia arriba |
| `BTN_DOWN` | GPIOC | **PC2** | Mover la pala izquierda hacia abajo |
| `BTN_START` | GPIOC | **PC3** | Iniciar, pausar o reiniciar la partida |

---

## 5. Localización de los pines en la Nucleo-F411RE

La placa expone los GPIOs a través de dos conectores Morpho (CN7 y CN10) y los conectores Arduino (CN5, CN6, CN8, CN9).

### Conector Morpho CN7 (izquierdo — 2 filas × 19 pines)

```
         CN7  (ST Morpho, NUCLEO-F411RE)
         ┌────┬────┐
  pin 1  │ PC10│ PC11│ pin 2
  pin 3  │ PC12│ PD2 │ pin 4
  pin 5  │ VDD │ E5V │ pin 6
  pin 7  │BOOT0│ GND │ pin 8    ← GND recomendado
  pin 9  │  -  │  -  │ pin 10   ← NO es GND
  pin 11 │  -  │IOREF│ pin 12
  pin 13 │ PA13│RESET│ pin 14
  pin 15 │ PA14│+3V3 │ pin 16
  pin 17 │ PA15│ +5V │ pin 18
  pin 19 │ GND │ GND │ pin 20   ← GND
  pin 21 │ PB7 │ GND │ pin 22   ← GND
  pin 23 │ PC13│ VIN │ pin 24
  pin 25 │ PC14│  -  │ pin 26
  pin 27 │ PC15│ PA0 │ pin 28
  pin 29 │ PH0 │ PA1 │ pin 30
  pin 31 │ PH1 │ PA4 │ pin 32
  pin 33 │VBAT │ PB0 │ pin 34
  pin 35 │ PC2 │PC1* │ pin 36   ← BTN_DOWN  (PC2)
  pin 37 │ PC3 │PC0* │ pin 38   ← BTN_START (PC3), BTN_UP (PC0)
         └────┴────┘
```

`*` En UM1724, CN7 pin 36 y pin 38 dependen de solder bridges:
pin 36 puede ser PC1 o PB9; pin 38 puede ser PC0 o PB8. En la NUCLEO-F411RE
típica, pin 38 expone PC0.

> **Nota**: Consulta el **User Manual UM1724** de ST para la asignación exacta de tu revisión. Los pines clave son:

| Señal | Conector Morpho | Pin número CN7 |
|-------|----------------|----------------|
| **PC0** | CN7 | pin 38 (columna derecha) |
| **PC2** | CN7 | pin 35 (columna izquierda) |
| **PC3** | CN7 | pin 37 (columna izquierda) |
| **GND** | CN7 | pin 8, 19, 20, 22 o cualquier GND |

### Conector Arduino (alternativa para PC0)

En el conector Arduino **CN6** (analógico), el pin **A5** corresponde a **PC0**, lo que puede ser más accesible según tu breadboard:

```
CN6 (Arduino analógico)
┌──────┬─────┐
│  A0  │ PA0 │
│  A1  │ PA1 │
│  A2  │ PA4 │
│  A3  │ PB0 │
│  A4  │ PC1 │
│  A5  │ PC0 │  ← BTN_UP
└──────┴─────┘
```

---

## 6. Diagrama de conexión en la breadboard

```
NUCLEO-F411RE
─────────────────────────────────────────────────────
  CN7 pin 38 (PC0) ──┐
  CN7 pin 35 (PC2) ──┼── cable Dupont ──► BREADBOARD
  CN7 pin 37 (PC3) ──┘
  CN7 pin 8/19/20/22 (GND) ────────────► GND rail (-)

BREADBOARD
────────────────────────────────────────────────────
  GND rail (−) ─────────────────────────────────────────────────────────┐
                                                                         │
  ┌──────────────────────────────────┐                                   │
  │  BTN_UP (PC0)                    │                                   │
  │  ┌──────────────┐                │                                   │
  │  │  PULSADOR 1  │                │                                   │
  PC0 ►── pin A ──[PULSADOR]── pin D ──► fila GND ──────────────────────┤
  │  └──────────────┘                │                                   │
  │                                  │                                   │
  │  BTN_DOWN (PC2)                  │                                   │
  │  ┌──────────────┐                │                                   │
  PC2 ►── pin A ──[PULSADOR]── pin D ──► fila GND ──────────────────────┤
  │  └──────────────┘                │                                   │
  │                                  │                                   │
  │  BTN_START (PC3)                 │                                   │
  │  ┌──────────────┐                │                                   │
  PC3 ►── pin A ──[PULSADOR]── pin D ──► fila GND ──────────────────────┘
  └──────────────────────────────────┘
```

---

## 7. Paso a paso

### Paso 1 — Coloca los pulsadores en la breadboard

Inserta cada pulsador de 4 pines a caballo del canal central de la protoboard. Cada uno ocupará 4 columnas (2 a cada lado del canal).

```
     canal central
         ↓
  [ ][ ][ ][ ] | [ ][ ][ ][ ]
        A  B   |  C  D
```

Recomendación: deja 2-3 filas de separación entre cada pulsador para facilitar el cableado.

### Paso 2 — Conecta GND

Lleva un cable desde cualquier pin **GND** de la Nucleo (por ejemplo **CN7 pin 8, 19, 20 o 22**) hasta el **rail negativo (−)** de la breadboard.

Luego conecta el **pin D** (o C) de cada pulsador al rail GND de la breadboard. Puedes usar el mismo rail para los tres pulsadores.

### Paso 3 — Conecta los GPIOs

| Cable | Desde (Nucleo) | Hasta (Breadboard) |
|-------|----------------|--------------------|
| Azul | CN7 pin 38 → PC0 | Pin A del pulsador BTN_UP |
| Verde | CN7 pin 35 → PC2 | Pin A del pulsador BTN_DOWN |
| Amarillo | CN7 pin 37 → PC3 | Pin A del pulsador BTN_START |

> Si usas el conector Arduino, PC0 también está disponible en **CN6 pin A5**.

### Paso 4 — Verifica antes de energizar

Comprueba con un multímetro en modo continuidad:
- Pulsador **sin pulsar**: sin continuidad entre GPIO pin y GND.
- Pulsador **pulsado**: continuidad entre GPIO pin y GND.

### Paso 5 — Flashing y prueba

Compila y flashea el firmware con tu configuración de CMake:

```bash
# Ejemplo con CMakePresets
cmake --preset stm32f411re
cmake --build --preset stm32f411re
# O con Makefile:
make stm32f411re
```

Al arrancar el firmware:
- **BTN_UP** (PC0): la pala izquierda se mueve hacia arriba mientras se mantiene pulsado.
- **BTN_DOWN** (PC2): la pala izquierda se mueve hacia abajo mientras se mantiene pulsado.
- **BTN_START** (PC3): inicia la partida desde el titulo, pausa/reanuda durante el juego y reinicia tras game over.

---

## 8. Esquema eléctrico simplificado

```
3.3 V (interno)
    │
  [~40 kΩ pull-up interno GPIO]
    │
PC0 ──────────────────── A ┐
                            │ PULSADOR BTN_UP
                          D ┘────────── GND
```

---

## 9. Resolución de problemas comunes

| Síntoma | Causa probable | Solución |
|---------|---------------|----------|
| Botón no responde | Pin GPIO incorrecto | Verifica la columna del Morpho con el User Manual UM1724 |
| Botón siempre activo | Cortocircuito en breadboard | Revisa que GND y el GPIO no comparten la misma fila |
| Doble pulsación (rebote) | Sin debounce en firmware | El driver lee flancos por frame; si el problema persiste, añade un condensador de 100 nF entre pin y GND |
| Botón activo sin pulsar | Pulsador orientado 90° | Rota el pulsador 90°; los pares A-B / C-D cambian de posición |
| La pala no se mueve | Pin GPIO incorrecto, boton mal orientado o GND incorrecto | Prueba tocando PC0/PC2 directamente contra GND real |

---

## 10. Referencia rápida de pines

```
NUCLEO-F411RE — Pines usados por STMBOY buttons
═══════════════════════════════════════════════
  PC0  →  CN7 pin 38  ─►  BTN_UP    (pala arriba)
  PC2  →  CN7 pin 35  ─►  BTN_DOWN  (pala abajo)
  PC3  →  CN7 pin 37  ─►  BTN_START (inicio/pausa/reinicio)
  GND  →  CN7 pin 8/19/20/22 ─►  Rail GND breadboard
═══════════════════════════════════════════════
  Lógica: active-low | pull-up interno activado
```
