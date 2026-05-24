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

// Implementación con CRITICAL
void computeHistogramCritical(const vector<Color>& image, vector<int>& histo) {
    fill(histo.begin(), histo.end(), 0);

    #pragma omp parallel for
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        int color_val = image[i].r;
        
        #pragma omp critical
        {
            histo[color_val]++;
        }
    }
}

// Implementación corregida con REDUCTION usando puntero
void computeHistogramReduction(const vector<Color>& image, vector<int>& histo) {
    fill(histo.begin(), histo.end(), 0);

    int* histo_ptr = histo.data();

    #pragma omp parallel for reduction(+:histo_ptr[:256])
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        int color_val = image[i].r;
        histo_ptr[color_val]++;
    }
}

int main() {
    vector<Color> image(WIDTH * HEIGHT);
    vector<int> histogram(256, 0);

    cout << "Generando Imagen Base..." << endl;
    generateMandelbrot(image);

    cout << "\n--- Ejecutando Pruebas de Histograma ---" << endl;

    // Reducción primero para no esperar tanto 
    auto start2 = chrono::high_resolution_clock::now();
    computeHistogramReduction(image, histogram);
    auto end2 = chrono::high_resolution_clock::now();
    chrono::duration<double> diff2 = end2 - start2;
    cout << "Tiempo Histograma con REDUCTION: " << diff2.count() << " segundos." << endl;

    auto start1 = chrono::high_resolution_clock::now();
    computeHistogramCritical(image, histogram);
    auto end1 = chrono::high_resolution_clock::now();
    chrono::duration<double> diff1 = end1 - start1;
    cout << "Tiempo Histograma con CRITICAL : " << diff1.count() << " segundos." << endl;

    return 0;
}
