# Paint OpenGL (POO)

Este proyecto es una mini-aplicación de dibujo interactivo (estilo Paint) desarrollada en **C++** utilizando la biblioteca **OpenGL** (a través de **GLUT**). Su objetivo principal es demostrar la implementación manual de algoritmos fundamentales de la Computación Gráfica y la rasterización mediante primitivas, así como la gestión de transformaciones espaciales interactivas sobre las figuras resultantes.

## Características Principales

*   **Renderizado por Primitivas (Píxel a Píxel):** Las figuras no se dibujan usando las funciones poligonales de alto nivel de OpenGL, sino que se renderizan calculando e iluminando píxeles individuales (`GL_POINTS`) gracias a algoritmos matemáticos clásicos.
*   **Barra de Herramientas Interactiva (Navbar):** Interfaz gráfica clicable en la parte superior para intercambiar rápidamente entre las diferentes herramientas (F1 - F6).
*   **Manipulación de Figuras (Hitbox):** 
    *   **Selección (F1):** Permite aislar una figura dibujada en el lienzo dándole clic. Aparecerá un marco punteado (Bounding Box) alrededor de ella.
    *   **Traslación:** Manteniendo el clic presionado dentro de la figura seleccionada, se puede arrastrar por el lienzo.
    *   **Escalamiento:** Posee cuadrados (anclajes) en las cuatro esquinas que permiten encoger o agrandar dinámicamente la forma seleccionada.
    *   **Rotación:** Cuenta con un pivote en la parte superior del marco; arrastrando de él se girará la figura dinámicamente usando las matrices de modelo-vista (`GL_MODELVIEW`) de OpenGL.
*   **Navegación del Lienzo (Paneo - F2):** Posibilidad de arrastrar la cámara por un lienzo de trabajo infinito.
*   **Preview Dinámico:** Al construir una línea, rectángulo o elipse, se mostrará una previsualización de la forma adaptándose al ratón en tiempo real (antes de soltar el clic).

## Algoritmos de Rasterización Implementados

El proyecto hace uso intensivo de los siguientes algoritmos de rasterización:
1.  **Algoritmo de Línea de Bresenham:** Utilizado intensamente para dibujar líneas diagonales, rectángulos perfectos, y conectar los vértices arbitrarios de los polígonos irregulares en tiempo real.
2.  **Algoritmo del Punto Medio para Elipses:** Usado para trazar curvas cónicas controlando diferencialmente el crecimiento de los ejes $X$ y $Y$. (Previamente probado también para la circunferencia).

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
