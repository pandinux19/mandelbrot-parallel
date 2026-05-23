# mandelbrot-parallel
Optimización híbrida en C++ usando OpenMP (Thread &amp; SIMD/SPMD) para la generación de Mandelbrot 8K, convolución y análisis estadístico. Proyecto Final personal de Programación Paralela y Concurrente, CUCEI.

#**Optimización de Algoritmos Paralelos e Instrucción Vectorial (OpenMP & SPMD)**

Este repositorio contiene el proyecto final para la materia de Programación Paralela y Concurrente - D02 en el Centro Universitario de Ciencias Exactas e Ingenierías (CUCEI).

El objetivo del proyecto es evaluar, corregir y optimizar de forma manual una línea base de paralelismo generada por IA, aplicando técnicas avanzadas de balanceo de carga, sincronización eficiente a nivel de hilos y vectorización por hardware.

---

## Características del Proyecto

El programa ejecuta de manera integrada tres etapas computacionalmente pesadas sobre una resolución de ultra-alta definición 8K UHD (7680x4320):

1. **Tarea A (Generación de Fractal):** Cálculo adaptativo del Conjunto de Mandelbrot optimizado mediante planificación dinámica de hilos (`schedule(dynamic, 16)`).
2. **Tarea B (Procesamiento de Imágenes):** Filtro de convolución Gaussiana 2D implementado bajo una estructura SPMD (Single Program, Multiple Data) forzando la vectorización de bucles.
3. **Tarea C (Análisis Estadístico):** Construcción de un histograma de frecuencias de color optimizado mediante la cláusula `reduction` para eliminar la contención y el *False Sharing*.

---

## Requisitos del Sistema (Entorno de Prueba)
* **Sistema Operativo:** Ubuntu Linux (nativo)
* **Compilador:** `g++` (con soporte para C++17 y OpenMP 4.5+)

---

## Instrucciones de Compilación y Ejecución

Para validar la correcta vectorización por hardware (registros AVX/SIMD) y ejecutar el programa con políticas de afinidad de hilos en la caché L1/L2, ejecuta los siguientes comandos en la terminal de Ubuntu:

### 1. Compilación con reporte de vectorización:
```bash
g++ -O3 -march=native -std=c++17 -fopenmp -fopt-info-vec-optimized proyecto_final_concurrente.cpp -o proyecto_final
