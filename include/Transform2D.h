#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Representación de un punto 2D
struct Point {
    int x;
    int y;
};

/*
 * =========================================================================
 * TRANSFORMACIONES GEOMÉTRICAS 2D
 * Referencia: TransformacionesGeometricas_Algoritmos.txt
 *
 * Coordenadas homogéneas: P = [x, y, 1]^T, P' = M * P
 * Composición matricial (vectores columna, derecha a izquierda):
 *   P' = C * B * A * P  =>  M = C * B * A
 * =========================================================================
 */
struct Matrix3x3 {
    float m[3][3];

    // Constructor: Inicializa en matriz identidad
    Matrix3x3();

    static Matrix3x3 identity();

    // 1. TRASLACIÓN: T(dx, dy)
    // [ 1  0  dx ]
    // [ 0  1  dy ]
    // [ 0  0   1 ]
    static Matrix3x3 translation(float dx, float dy);

    // 2. ESCALAMIENTO: S(sx, sy)
    // [ sx  0   0 ]
    // [  0 sy   0 ]
    // [  0  0   1 ]
    static Matrix3x3 scaling(float sx, float sy);

    // 3. ROTACIÓN: R(theta) en grados sexagesimales (antihorario)
    // [ cos(theta) -sin(theta)  0 ]
    // [ sin(theta)  cos(theta)  0 ]
    // [     0           0       1 ]
    static Matrix3x3 rotation(float angleDegrees);

    // Multiplicación matricial: M = this * other
    Matrix3x3 operator*(const Matrix3x3& other) const;

    // Multiplicación Matriz * Punto homogéneo: P' = M * P
    Point transformPoint(const Point& p) const;

    // Inversa de la composición de pivoteo:
    // M^(-1) = T(C) * S(1/sx, 1/sy) * R(-theta) * T(-F)
    static Matrix3x3 inversePivot(Point center, float tx, float ty, float sx, float sy, float angleDegrees);

    // Conversión a matriz 4x4 (column-major) para OpenGL
    void toOpenGLMatrix(float out16[16]) const;
};

/*
 * 4. COMPOSICIÓN DE TRANSFORMACIONES (PIVOTEO)
 * Trasladar al centro -> Escalar -> Rotar -> Trasladar a posición final
 *
 * M = TFinal * R * S * TCentro
 */
Matrix3x3 buildPivotMatrix(Point center, float tx, float ty, float sx, float sy, float angleDegrees);

/*
 * Composición de una lista de matrices en orden de ejecución: M = An * ... * A1
 */
Matrix3x3 composeTransforms(const std::vector<Matrix3x3>& matricesInExecutionOrder);

/*
 * CÁLCULO DEL CENTRO DE UNA FIGURA (PROMEDIO DE VÉRTICES)
 * cx = (P0.x + ... + Pn-1.x) / n
 * cy = (P0.y + ... + Pn-1.y) / n
 */
Point computeCentroid(const std::vector<Point>& vertices);

/*
 * Transforma una lista completa de vértices
 */
std::vector<Point> transformPolygon(const std::vector<Point>& vertices, const Matrix3x3& M);

#endif // TRANSFORM2D_H
