#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <chrono>

using namespace std;

// Configuración de la imagen (8K UHD) [cite: 7]
const int WIDTH = 7680;
const int HEIGHT = 4320;
const int MAX_ITER = 500;

struct Color {
    unsigned char r, g, b;
};

// Tarea A: Generar el conjunto de Mandelbrot [cite: 7]
void generateMandelbrot(vector<Color>& image) {
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

// Tarea B: Filtro de Convolución (Desenfoque Gaussiano 5x5) [cite: 8]
void applyBlur(const vector<Color>& input, vector<Color>& output) {
    float kernel[5][5] = {
        {1/273.f, 4/273.f,  7/273.f,  4/273.f,  1/273.f},
        {4/273.f, 16/273.f, 26/273.f, 16/273.f, 4/273.f},
        {7/273.f, 26/273.f, 41/273.f, 26/273.f, 7/273.f},
        {4/273.f, 16/273.f, 26/273.f, 16/273.f, 4/273.f},
        {1/273.f, 4/273.f,  7/273.f,  4/273.f,  1/273.f}
    };

    for (int y = 2; y < HEIGHT - 2; ++y) {
        for (int x = 2; x < WIDTH - 2; ++x) {
            float r = 0, g = 0, b = 0;
            for (int ky = -2; ky <= 2; ++ky) {
                for (int kx = -2; kx <= 2; ++kx) {
                    Color pixel = input[(y + ky) * WIDTH + (x + kx)];
                    r += pixel.r * kernel[ky + 2][kx + 2];
                    g += pixel.g * kernel[ky + 2][kx + 2];
                    b += pixel.b * kernel[ky + 2][kx + 2];
                }
            }
            output[y * WIDTH + x] = {(unsigned char)r, (unsigned char)g, (unsigned char)b};
        }
    }
}

int main() {
    vector<Color> image(WIDTH * HEIGHT);
    vector<Color> filteredImage(WIDTH * HEIGHT);

    auto start = chrono::high_resolution_clock::now();

    cout << "Generando Mandelbrot (Tarea A)..." << endl;
    generateMandelbrot(image);

    cout << "Aplicando Filtro (Tarea B)..." << endl;
    applyBlur(image, filteredImage);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;

    cout << "Tiempo total secuencial: " << diff.count() << " segundos" << endl;

    return 0;
}
