#pragma once
//#include "Vertex.h"
#include "Face.h"
#include "Matrix.h"
#include <Vector>

using namespace std;

class Object3D
{
public:
    std::vector<Vertex> vertices; // V�rtices del objeto
    std::vector<Face> caras;       // Caras del objeto
    Vertex center;                 // Centro de masas
    Matrix modelo;
    float t = 0.0f, incremento = 0.3f / 100;

    void changeIncr(float i) {
        incremento = i;
    }
    // M�todo para calcular el centro de masas
    void calculateCenter() {
        center = { 0.0f, 0.0f, 0.0f, 0, 0, 0 }; // Inicializar el centro
        float areaTotal = 0.0f;

        // Sumar centros de masas de cada cara ponderados por su �rea
        for (const auto& cara : caras) {
            const Vertex& v1 = vertices[cara.v1];
            const Vertex& v2 = vertices[cara.v2];
            const Vertex& v3 = vertices[cara.v3];

            Vertex centroT = triangleCenter(v1, v2, v3);
            float areaT = triangleArea(v1, v2, v3);

            center.x += centroT.x * areaT;
            center.y += centroT.y * areaT;
            center.z += centroT.z * areaT;
            areaTotal += areaT;
        }

        // Dividir entre el �rea total para obtener el centro de masas
        if (areaTotal > 0) {
            center.x /= areaTotal;
            center.y /= areaTotal;
            center.z /= areaTotal;
        }
    }

private:
    // Funci�n privada para calcular el centro de un tri�ngulo
    Vertex triangleCenter(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
        Vertex centro;
        centro.x = (v1.x + v2.x + v3.x) / 3.0f;
        centro.y = (v1.y + v2.y + v3.y) / 3.0f;
        centro.z = (v1.z + v2.z + v3.z) / 3.0f;
        return centro;
    }

    // Funci�n privada para calcular el �rea de un tri�ngulo
    float triangleArea(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
        Vertex ab = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z, 0, 0, 0 };
        Vertex ac = { v3.x - v1.x, v3.y - v1.y, v3.z - v1.z, 0, 0, 0 };

        Vertex cruz = {
            ab.y * ac.z - ab.z * ac.y,
            ab.z * ac.x - ab.x * ac.z,
            ab.x * ac.y - ab.y * ac.x,
            0, 0, 0
        };

        return 0.5f * std::sqrt(cruz.x * cruz.x + cruz.y * cruz.y + cruz.z * cruz.z);
    }
};
