// Supported with union (c) 2020 Union team

// User API for zMAT4
// Add your methods here


void GetTranslation(zVEC3& t) const;
int IsUpper3x3Orthonormal(void) const;



void PostScaleNew(float scaleFactor) {
    zMAT4 scaleMatrix;
    scaleMatrix.MakeIdentity(); // �������� � ��������� �������
    scaleMatrix[0][0] = scaleFactor; // ��������������� �� X
    scaleMatrix[1][1] = scaleFactor; // ��������������� �� Y
    scaleMatrix[2][2] = scaleFactor; // ��������������� �� Z

    // �������� ������� ������� �� ������� ���������������
    *this = *this * scaleMatrix;
}

std::string ToString();