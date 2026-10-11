#include "Transform2D.h"
#include <cmath>

Matrix3x3::Matrix3x3() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

Matrix3x3 Matrix3x3::identity() {
    return Matrix3x3();
}

// 1. TRASLACIÓN: T(dx, dy)
Matrix3x3 Matrix3x3::translation(float dx, float dy) {
    Matrix3x3 res = identity();
    res.m[0][2] = dx;
    res.m[1][2] = dy;
    return res;
}

// 2. ESCALAMIENTO: S(sx, sy)
Matrix3x3 Matrix3x3::scaling(float sx, float sy) {
    Matrix3x3 res = identity();
    res.m[0][0] = sx;
    res.m[1][1] = sy;
    return res;
}

// 3. ROTACIÓN: R(theta) (grados convertidos a radianes)
Matrix3x3 Matrix3x3::rotation(float angleDegrees) {
    Matrix3x3 res = identity();
    float thetaRad = angleDegrees * (float)M_PI / 180.0f;
    float c = std::cos(thetaRad);
    float s = std::sin(thetaRad);
    res.m[0][0] = c;  res.m[0][1] = -s;
    res.m[1][0] = s;  res.m[1][1] =  c;
    return res;
}

// Multiplicación matricial: C = A * B
Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const {
    Matrix3x3 res;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) {
                sum += m[i][k] * other.m[k][j];
            }
            res.m[i][j] = sum;
        }
    }
    return res;
}

// Multiplicación Matriz * Punto homogéneo: P' = M * P
Point Matrix3x3::transformPoint(const Point& p) const {
    float x = m[0][0] * p.x + m[0][1] * p.y + m[0][2];
    float y = m[1][0] * p.x + m[1][1] * p.y + m[1][2];
    float w = m[2][0] * p.x + m[2][1] * p.y + m[2][2];

    if (w != 0.0f && std::abs(w - 1.0f) > 1e-5f) {
        x /= w;
        y /= w;
    }
    return { (int)std::round(x), (int)std::round(y) };
}

// Inversa de la composición de pivoteo:
// M^(-1) = T(C) * S(1/sx, 1/sy) * R(-theta) * T(-F)
Matrix3x3 Matrix3x3::inversePivot(Point center, float tx, float ty, float sx, float sy, float angleDegrees) {
    float safeSx = (std::abs(sx) < 1e-4f) ? 1.0f : (1.0f / sx);
    float safeSy = (std::abs(sy) < 1e-4f) ? 1.0f : (1.0f / sy);

    float fx = center.x + tx;
    float fy = center.y + ty;

    Matrix3x3 invTFinal  = translation(-fx, -fy);
    Matrix3x3 invR       = rotation(-angleDegrees);
    Matrix3x3 invS       = scaling(safeSx, safeSy);
    Matrix3x3 invTCentro = translation((float)center.x, (float)center.y);

    // M^(-1) = invTCentro * invS * invR * invTFinal
    return invTCentro * (invS * (invR * invTFinal));
}

// Convierte matriz 3x3 a formato columna principal 4x4 de OpenGL
void Matrix3x3::toOpenGLMatrix(float out16[16]) const {
    // Columna 0
    out16[0]  = m[0][0];
    out16[1]  = m[1][0];
    out16[2]  = 0.0f;
    out16[3]  = 0.0f;

    // Columna 1
    out16[4]  = m[0][1];
    out16[5]  = m[1][1];
    out16[6]  = 0.0f;
    out16[7]  = 0.0f;

    // Columna 2
    out16[8]  = 0.0f;
    out16[9]  = 0.0f;
    out16[10] = 1.0f;
    out16[11] = 0.0f;

    // Columna 3 (traslación)
    out16[12] = m[0][2];
    out16[13] = m[1][2];
    out16[14] = 0.0f;
    out16[15] = 1.0f;
}

/*
 * 4. COMPOSICIÓN DE TRANSFORMACIONES (PIVOTEO)
 * Trasladar al centro -> Escalar -> Rotar -> Trasladar a posición final
 *
 * M = TFinal * R * S * TCentro
 */
Matrix3x3 buildPivotMatrix(Point center, float tx, float ty, float sx, float sy, float angleDegrees) {
    float cx = (float)center.x;
    float cy = (float)center.y;
    float fx = cx + tx;
    float fy = cy + ty;

    Matrix3x3 TCentro = Matrix3x3::translation(-cx, -cy);
    Matrix3x3 S       = Matrix3x3::scaling(sx, sy);
    Matrix3x3 R       = Matrix3x3::rotation(angleDegrees);
    Matrix3x3 TFinal  = Matrix3x3::translation(fx, fy);

    // Multiplicación de derecha a izquierda
    // P' = TFinal * R * S * TCentro * P
    return TFinal * (R * (S * TCentro));
}

Matrix3x3 composeTransforms(const std::vector<Matrix3x3>& matricesInExecutionOrder) {
    Matrix3x3 M = Matrix3x3::identity();
    for (const auto& A : matricesInExecutionOrder) {
        M = A * M;
    }
    return M;
}

/*
 * CÁLCULO DEL CENTRO DE UNA FIGURA (PROMEDIO DE VÉRTICES)
 */
Point computeCentroid(const std::vector<Point>& vertices) {
    if (vertices.empty()) return {0, 0};
    long long sumX = 0;
    long long sumY = 0;
    for (const auto& p : vertices) {
        sumX += p.x;
        sumY += p.y;
    }
    int n = (int)vertices.size();
    return { (int)(sumX / n), (int)(sumY / n) };
}

std::vector<Point> transformPolygon(const std::vector<Point>& vertices, const Matrix3x3& M) {
    std::vector<Point> res;
    res.reserve(vertices.size());
    for (const auto& p : vertices) {
        res.push_back(M.transformPoint(p));
    }
    return res;
}
