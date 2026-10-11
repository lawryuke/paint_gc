# Paint OpenGL (POO)

Este proyecto es una mini-aplicación de dibujo interactivo (estilo Paint) desarrollada en **C++** utilizando la biblioteca **OpenGL** (a través de **GLUT**). Su objetivo principal es demostrar la implementación manual de algoritmos fundamentales de la Computación Gráfica y la rasterización mediante primitivas, así como la gestión de transformaciones espaciales interactivas sobre las figuras resultantes.

## Características Principales

*   **Renderizado por Primitivas (Píxel a Píxel):** Las figuras no se dibujan usando las funciones poligonales de alto nivel de OpenGL, sino que se renderizan calculando e iluminando píxeles individuales (`GL_POINTS`) gracias a algoritmos matemáticos clásicos.
*   **Barra de Herramientas Interactiva (Navbar):** Interfaz gráfica clicable en la parte superior para intercambiar rápidamente entre las diferentes herramientas (1 - 6): selección, paneo, línea, rectángulo, elipse y polígono.
*   **Manipulación de Figuras (Hitbox):** 
    *   **Selección (1):** Permite aislar una figura dibujada en el lienzo dándole clic. Aparecerá un marco punteado (Bounding Box) alrededor de ella.
    *   **Borrado (Delete o Backspace):** Elimina la figura seleccionada. Si no hay una selección, no hace nada.
    *   **Relleno (R / G / B):** Selecciona un polígono, rectángulo o elipse con 1 y pulsa R para rojo, G para verde o B para azul. Cada figura conserva su color al moverla, escalarla o rotarla.
    *   **Traslación:** Manteniendo el clic presionado dentro de la figura seleccionada, se puede arrastrar por el lienzo.
    *   **Escalamiento:** Posee cuadrados (anclajes) en las cuatro esquinas que permiten encoger o agrandar dinámicamente la forma seleccionada.
    *   **Rotación:** Cuenta con un pivote en la parte superior del marco; arrastrando de él se girará la figura dinámicamente usando las matrices de modelo-vista (`GL_MODELVIEW`) de OpenGL.
*   **Navegación del Lienzo (Paneo - 2):** Posibilidad de arrastrar la cámara por un lienzo de trabajo infinito.
*   **Preview Dinámico:** Al construir una línea, rectángulo o elipse, se mostrará una previsualización de la forma adaptándose al ratón en tiempo real (antes de soltar el clic).

## Algoritmos de Rasterización Implementados

El proyecto hace uso intensivo de los siguientes algoritmos de rasterización:
1.  **Algoritmo de Línea de Bresenham:** Utilizado intensamente para dibujar líneas diagonales, rectángulos perfectos, y conectar los vértices arbitrarios de los polígonos irregulares en tiempo real.
2.  **Algoritmo del Punto Medio para Elipses:** Usado para trazar curvas cónicas controlando diferencialmente el crecimiento de los ejes $X$ y $Y$. (Previamente probado también para la circunferencia).
3.  **Relleno Scan-Line con ET/EAT:** Construye una tabla de aristas, mantiene las aristas activas por fila y pinta entre pares de intersecciones usando la regla par-impar. Omite aristas horizontales y excluye los extremos superiores para evitar contar dos veces los vértices compartidos. Los rectángulos aportan cuatro vértices y las elipses un contorno aproximado mediante segmentos. El relleno se rasteriza después de transformar ese contorno para evitar huecos al escalar.

## Transformaciones Geométricas 2D

El módulo `Transform2D.h` / `Transform2D.cpp` implementa desde cero la teoría matricial en coordenadas homogéneas ($P = [x, y, 1]^T$) descrita en la guía `TransformacionesGeometricas_Algoritmos.txt`:

1.  **Traslación $T(dx, dy)$:** Matriz $3 \times 3$ con $dx, dy$ en la última columna. Desplaza figuras en los ejes $X$ e $Y$.
2.  **Escalamiento $S(sx, sy)$:** Matriz diagonal con factores $sx$ y $sy$. Soporta escalamiento uniforme y no uniforme (diferencial).
3.  **Rotación $R(\theta)$:** Matriz de rotación con ángulo convertido a radianes ($\theta \cdot \pi / 180$).
4.  **Composición de Transformaciones (Pivoteo sobre el centro):**
    *   Cálculo del centroide $C = (cx, cy)$ promediando vértices.
    *   Encadenamiento matricial: $M = T(cx + tx, cy + ty) \cdot R(\theta) \cdot S(sx, sy) \cdot T(-cx, -cy)$.
    *   Envío a OpenGL mediante matriz $4 \times 4$ en orden column-major (`glMultMatrixf`).
5.  **Inversa y Selección:** Cálculo de $M^{-1} = T(C) \cdot S(1/sx, 1/sy) \cdot R(-\theta) \cdot T(-F)$ para mapear el cursor a espacio local y permitir selección y arrastre preciso con figuras rotadas.

## Arquitectura Orientada a Objetos (POO)

El código monolítico original se refactorizó para seguir principios limpios orientados a objetos, logrando separar la lógica visual y de estado de la lógica puramente matemática:

*   **`Shape.h` / `Shape.cpp`:** Define una clase base abstracta `Shape` de la que heredan las primitivas (`LineShape`, `RectShape`, `EllipseShape`, `PolyShape`). Aquí residen las matemáticas, la detección de límites (Bounding Box inversa), los algoritmos de Punto Medio y la renderización visual del Hitbox con estado.
*   **`App.h` / `App.cpp`:** Actúa como el orquestador global (Singleton local). Maneja la matriz dinámica de figuras usando polimorfismo (`std::unique_ptr<Shape>`), controla la máquina de estados del ratón (`DragMode`), y gestiona los callbacks nativos de GLUT para eventos.
*   **`main.cpp`:** Punto de entrada aislado que inicia la aplicación.

## Compilación y Ejecución

El repositorio incluye un archivo `Makefile` para facilitar su construcción en entornos Linux/Unix que posean los paquetes de desarrollo de FreeGLUT.

1.  Asegúrate de estar en el directorio `paint/`.
2.  Ejecuta el compilador a través de `make`:
    ```bash
    make
    ```
3.  Ejecuta el binario resultante:
    ```bash
    ./paint
    ```
