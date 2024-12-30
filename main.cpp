#include "Libraries/vectors.h"

int main(){

    double x1, y1, z1, x2, y2, z2, s;
    char c;

    std::cout << "Vetor U" << std::endl;
    std::cout << "X: ";
    std::cin >> x1;
    std::cout << "Y: ";
    std::cin >> y1;
    std::cout << "Z: ";
    std::cin >> z1;

    std::cout << "Vetor V" << std::endl;
    std::cout << "X: ";
    std::cin >> x2;
    std::cout << "Y: ";
    std::cin >> y2;
    std::cout << "Z: ";
    std::cin >> z2;
    std::cout << std::endl;

    vec3 u(x1, y1, z1);
    vec3 v(x2, y2, z2);

    std::cout << "U: " << u << std::endl;
    std::cout << "V: " << v << std::endl << std::endl;

    std::cout << "Soma (u+v): " << u + v << std::endl;
    std::cout << "Soma (v+u): " << v + u << std::endl;
    std::cout << "Podemos ver que vale a propriedade comutativa!" << std::endl << std::endl;

    std::cout << "Subtração (u-v): " << u - v << std::endl;
    std::cout << "Subtração (v-u): " << v - u << std::endl << std::endl;

    std::cout << "Digite um Escalar: ";
    std::cin >> s;
    std::cout << "Multiplicação de 'u' pelo Escalar: " << u * s << std::endl;
    std::cout << "Multiplicação de 'v' pelo Escalar: " << v * s << std::endl << std::endl;

    std::cout << "Inverso de 'u': " << u.inverse() << std::endl;
    std::cout << "Inverso de 'v': " << v.inverse() << std::endl << std::endl;

    std::cout << "u . v = " << u.dot(v) << std::endl;
    std::cout << "u x v = " << u.cross(v) << std::endl << std::endl;

    std::cout << "Projeção de 'u' em 'v': " << u.projection(v) << std::endl << std::endl;

    std::cout << "Escolha a Reflexão (Escolha entre x, y ou z): ";
    std::cin >> c;
    std::cout << "Reflexão de 'v' em relação à " << "'" << c << "': " << v.reflect(c) << std::endl << std::endl;

    return 0;
}