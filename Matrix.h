#pragma once
#include <iostream>
#include <vector>
#include <cmath>
constexpr double M_PI = 3.14159265358979323846;

class Matrix {
private:
    float data[4][4];  // Arreglo bidimensional para la matriz 4x4

public:
    Matrix() {
        // Inicializa la matriz identidad
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                data[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    float* operator[](int index) {
        return data[index];  // Sobrecarga del operador para acceder a las filas
    }

    // Multiplicación de matrices
    Matrix operator*(const Matrix& other) {
        Matrix result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

    // Método para traslación
    void translate(float tx, float ty, float tz) {
        data[0][3] += tx;
        data[1][3] += ty;
        data[2][3] += tz;
    }

    // Método para rotaciones alrededor de X, Y y Z
    void rotationX(float angle) {
        float rad = angle * M_PI / 180.0f;
        Matrix rotation;
        rotation[1][1] = cos(rad);
        rotation[1][2] = -sin(rad);
        rotation[2][1] = sin(rad);
        rotation[2][2] = cos(rad);
        *this = *this * rotation;
    }

    void rotationY(float angle) {
        float rad = angle * M_PI / 180.0f;
        Matrix rotation;
        rotation[0][0] = cos(rad);
        rotation[0][2] = sin(rad);
        rotation[2][0] = -sin(rad);
        rotation[2][2] = cos(rad);
        *this = *this * rotation;
    }

    void rotationZ(float angle) {
        float rad = angle * M_PI / 180.0f;
        Matrix rotation;
        rotation[0][0] = cos(rad);
        rotation[0][1] = -sin(rad);
        rotation[1][0] = sin(rad);
        rotation[1][1] = cos(rad);
        *this = *this * rotation;
    }

    // Escalado
    void scale(float sx, float sy, float sz) {
        data[0][0] *= sx;
        data[1][1] *= sy;
        data[2][2] *= sz;
    }
};


