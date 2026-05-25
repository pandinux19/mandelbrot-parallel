#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <chrono>
#include <omp.h>

using namespace std;

const int WIDTH = 7680;
const int HEIGHT = 4320;
const int MAX_ITER = 500;

struct Color {
    unsigned char r, g, b;
};

// Tarea A: Generación de Mandelbrot con Planificador Óptimo
void generateMandelbrot(vector<Color>& image) {
    #pragma omp parallel for schedule(dynamic, 16)
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            double pr = 1.5 * (x - WIDTH / 2.0) / (0.5 * WIDTH);
            double pi = (y - HEIGHT / 2.0) / (0.5 * HEIGHT);
            double new_re = 0, new_im = 0, old_re = 0, old_im = 0;
            int i;
            for (i = 0; i < MAX_ITER; ++i) {
                old_re = new_re;
                old_im = new_im;
                new_re = old_re * old_re - old_im * old_im + pr;
                new_im = 2 * old_re * old_im + pi;
                if ((new_re * new_re + new_im * new_im) > 4) break;
            }
            int idx = y * WIDTH + x;
            unsigned char val = (unsigned char)(i % 256);
            image[idx] = {val, (unsigned char)(val * 2 % 256), (unsigned char)(val * 5 % 256)};
        }
    }
}

// Tarea B: Filtro de Convolución con Enfoque SPMD y Vectorización Forzada
void applyBlurSPMD(const vector<Color>& input, vector<Color>& output) {
    float kernel[5][5] = {
        {1/273.f, 4/273.f,  7/273.f,  4/273.f,  1/273.f},
        {4/273.f, 16/273.f, 26/273.f, 16/273.f, 4/273.f},
        {7/273.f, 26/273.f, 41/273.f, 26/273.f, 7/273.f},
        {4/273.f, 16/273.f, 26/273.f, 16/273.f, 4/273.f},
        {1/273.f, 4/273.f,  7/273.f,  4/273.f,  1/273.f}
    };

    // Paralelismo a nivel de hilos en las filas
    #pragma omp parallel for
    for (int y = 2; y < HEIGHT - 2; ++y) {
        // Forzamos la vectorización SIMD/SPMD en el bucle interno de los píxeles
        #pragma omp simd
        for (int x = 2; x < WIDTH - 2; ++x) {
            float r = 0, g = 0, b = 0;
            
            // Bucles del kernel desenrollados implícitamente por el compilador
            for (int ky = -2; ky <= 2; ++ky) {
                for (int kx = -2; kx <= 2; ++kx) {
                    Color pixel = input[(y + ky) * WIDTH + (x + kx)];
                    float k_val = kernel[ky + 2][kx + 2];
                    r += pixel.r * k_val;
                    g += pixel.g * k_val;
                    b += pixel.b * k_val;
                }
            }
            output[y * WIDTH + x] = {(unsigned char)r, (unsigned char)g, (unsigned char)b};
        }
    }
}

// Tarea C: Histograma Optimizado con Reduction
void computeHistogramReduction(const vector<Color>& image, vector<int>& histo) {
    fill(histo.begin(), histo.end(), 0);
    int* histo_ptr = histo.data();

    #pragma omp parallel for reduction(+:histo_ptr[:256])
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        histo_ptr[image[i].r]++;
    }
}

int main() {
    vector<Color> image(WIDTH * HEIGHT);
    vector<Color> filteredImage(WIDTH * HEIGHT);
    vector<int> histogram(256, 0);

    auto start = chrono::high_resolution_clock::now();

    cout << "1. Generando Mandelbrot (Dynamic, 16)..." << endl;
    generateMandelbrot(image);

    cout << "2. Aplicando Convolucion Gaussiana (SPMD Vectorizado)..." << endl;
    applyBlurSPMD(image, filteredImage);

    cout << "3. Calculando Histograma de Colores (Reduction)..." << endl;
    computeHistogramReduction(filteredImage, histogram);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;

    cout << "\n>>> PROYECTO FINAL COMPLETADO CORRECCIONES MANUALES <<<" << endl;
    cout << "Tiempo total de ejecucion optimizado: " << diff.count() << " segundos" << endl;

    return 0;
}
