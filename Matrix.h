#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include "Vertex.h"
constexpr double M_PI = 3.14159265358979323846;

class Matrix {
private:
    float mat[4][4];

public:
    Matrix() {
        //Initialize identity matrix
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                mat[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    //Multiplication with another matrix
    Matrix operator*(const Matrix& other) {
        Matrix result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.mat[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.mat[i][j] += mat[i][k] * other.mat[k][j];
                }
            }
        }
        return result;
    }

    // Método para traslación
    static Matrix translationMatrix(float tx, float ty, float tz) {
        Matrix mat;
        mat.mat[0][3] = tx;
        mat.mat[1][3] = ty;
        mat.mat[2][3] = tz;
        return mat;
    }

    // Método para rotaciones alrededor de X, Y y Z
    static Matrix rotationX(float angle) {
        Matrix mat;
        float rad = angle * (M_PI / 180.0f); // Convert degrees to radians
        mat.mat[1][1] = cos(rad);
        mat.mat[1][2] = -sin(rad);
        mat.mat[2][1] = sin(rad);
        mat.mat[2][2] = cos(rad);
        return mat;
    }

    static Matrix rotationY(float angle) {
        Matrix mat;
        float rad = angle * (M_PI / 180.0f);
        mat.mat[0][0] = cos(rad);
        mat.mat[0][2] = sin(rad);
        mat.mat[2][0] = -sin(rad);
        mat.mat[2][2] = cos(rad);
        return mat;
    }

    static Matrix rotationZ(float angle) {
        Matrix mat;
        float rad = angle * (M_PI / 180.0f);
        mat.mat[0][0] = cos(rad);
        mat.mat[0][1] = -sin(rad);
        mat.mat[1][0] = sin(rad);
        mat.mat[1][1] = cos(rad);
        return mat;
    }

    // Escalado
    static Matrix scalingMatrix(float sx, float sy, float sz) {
        Matrix mat;
        mat.mat[0][0] = sx;
        mat.mat[1][1] = sy;
        mat.mat[2][2] = sz;
        return mat;
    }

    // Overload * operator to multiply a matrix by a vertex
    friend Vertex operator*(const Matrix& mat, const Vertex& v);
};

//Apply model matrix to a vertex (4D vector)
inline Vertex operator*(const Matrix& mat, const Vertex& v) {
    Vertex result;
    result.x = mat.mat[0][0] * v.x + mat.mat[0][1] * v.y + mat.mat[0][2] * v.z + mat.mat[0][3] * v.w;
    result.y = mat.mat[1][0] * v.x + mat.mat[1][1] * v.y + mat.mat[1][2] * v.z + mat.mat[1][3] * v.w;
    result.z = mat.mat[2][0] * v.x + mat.mat[2][1] * v.y + mat.mat[2][2] * v.z + mat.mat[2][3] * v.w;
    result.w = mat.mat[3][0] * v.x + mat.mat[3][1] * v.y + mat.mat[3][2] * v.z + mat.mat[3][3] * v.w;
    return result;
}
