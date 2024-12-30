#include "Libraries/vectors.h"
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp" // Apenas para usar as matrizes de projeção (Renderização da Janela)
#include "glm/gtc/type_ptr.hpp" // Para poder usar value_ptr (também relacionado à renderização)
#include <vector>
#include <array>
#include <random>

/* Os cálculos e minha própria biblioteca de vetores estão em 3D, entretanto para a parte gráfica, trabalharemos
   apenas com 2D. 
*/

// Janela 800x800
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// Limites do Plano Cartesiano 2D -- Desenhado na Janela via OpenGL
float xMin, xMax, yMin, yMax;

bool vecsComplete = false; // Limite de 3 vetores por execução (Parte 2 - Visualizador Gráfico)
bool choose = false; // Escolha entre qual resultante desenhar (A + B + C ou C + B + A) --> Prova de Comutatividade.
std::vector<vec3> vecs;
std::vector<vec3> res;

void randomVecs(){
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib_x(xMin, xMax);
    std::uniform_real_distribution<> distrib_y(yMin, yMax);
    
    for(int i = 0; i < 3; ++i){
        double x = distrib_x(gen);
        double y = distrib_y(gen);
        vecs.push_back(vec3(x, y, 0.0));    
    }

}

// a + b + c ; c + b + a --> Iguais pela propriedade comutativa.
void calcualteResultant(){
    double x, y = 0;
    for(int i = 0; i < vecs.size(); ++i){
        x += vecs.at(i).get_x();
        y += vecs.at(i).get_y();
    }
    vec3 new_one(x, y, 0.0);
    res.push_back(new_one);
    x = 0;
    y = 0;
    for(int i = vecs.size() - 1; i >= 0; --i){
        x += vecs.at(i).get_x();
        y += vecs.at(i).get_y();
    }
    vec3 new_two(x, y, 0.0);
    res.push_back(new_two);

    std::cout << "A + B + C = " << new_one << std::endl;
    std::cout << "C + B + A = " << new_two << std::endl; 
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !vecsComplete) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // Mouse --> Coordenadas de Mundo.
        double x = static_cast<double>((xpos / WIDTH) * (xMax - xMin) + xMin);
        double y = static_cast<double>(((HEIGHT - ypos) / HEIGHT) * (yMax - yMin) + yMin);
        vecs.push_back(vec3(x, y, 0.0));
        if (vecs.size() == 3) {
            calcualteResultant();
            vecsComplete = true;
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        calcualteResultant();
        vecsComplete = true;
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        if (choose)
        {
            choose = false;
        }else{
            choose = true;
        }
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        if (vecsComplete) {return;}
        randomVecs();
        calcualteResultant();
        vecsComplete = true;
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        vecs.clear();
        res.clear();
        vecsComplete = false;
        choose = false;
    }
}

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Erro ao compilar shader: " << infoLog << std::endl;
    }

    return shader;
}

unsigned int setupCartesianPlane(float xMin, float xMax, float yMin, float yMax) {
    std::array<float, 12> planeVertices = {
        xMin, 0.0f, 0.0f,   xMax, 0.0f, 0.0f,  // Eixo X
        0.0f, yMin, 0.0f,   0.0f, yMax, 0.0f   // Eixo Y
    };

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}

void drawVector(const vec3& vector, unsigned int shaderProgram, glm::mat4 projection, float red, float green, float blue) {
    // Linha do vetor
    std::array<float, 6> vectorData = {
        0.0f, 0.0f, 0.0f,   vector.get_x(), vector.get_y(), vector.get_z() // Origem --> Ponta do Vetor
    };

    /* Calcular direção do vetor.
       Aqui, precisamos usar o glm::vec3 pois precisaremos usar funções que não implementei na minha própria
       biblioteca pessoal vec3, como o normalize. 
       Não é problema, pois essa função só é chamada na Parte 2 (Visualizador Gráfico). 
    */
    glm::vec3 dir(vector.get_x(), vector.get_y(), vector.get_z());
    float length = glm::length(dir);
    glm::vec3 unitDir = glm::normalize(dir);

    // Posição do triângulo (base na ponta do vetor)
    glm::vec3 arrowBase = dir - unitDir * (length * 0.1f); // 10% do tamanho do vetor para o triângulo

    // Criar vértices do triângulo
    glm::vec3 normal = glm::vec3(-unitDir.y, unitDir.x, 0.0f);
    std::array<float, 9> triangleData = {
        vector.get_x(), vector.get_y(), vector.get_z(), // Ponta do triângulo
        arrowBase.x + normal.x * (length * 0.05f), arrowBase.y + normal.y * (length * 0.05f), 0.0f, // Primeiro ponto da base
        arrowBase.x - normal.x * (length * 0.05f), arrowBase.y - normal.y * (length * 0.05f), 0.0f  // Segundo ponto da base
    };

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vectorData), vectorData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), red, green, blue);

    glDrawArrays(GL_LINES, 0, 2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleData), triangleData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUniform3f(glGetUniformLocation(shaderProgram, "color"), red, green, blue);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

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

    std::cout << "Continuar Para Parte 2? [y/n]" << std::endl;
    char a; std::cin >> a;
    if (a == 'y' || a == 'Y') {
        std::cout << "Informe os limites para x (xMin xMax): ";
        std::cin >> xMin >> xMax;
        std::cout << "Informe os limites para y (yMin yMax): ";
        std::cin >> yMin >> yMax;

        if (!glfwInit()) {
            std::cerr << "Erro ao inicializar GLFW" << std::endl;
            return -1;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Parte 2", nullptr, nullptr);
        if (!window) {
            std::cerr << "Erro ao criar janela GLFW." << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetKeyCallback(window, keyCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Erro ao carregar GLAD." << std::endl;
            return -1;
        }

        glViewport(0, 0, WIDTH, HEIGHT);

        const char* vertexShaderSource = R"(
            #version 430 core
            layout (location = 0) in vec3 aPos;
            uniform mat4 projection;
            void main() {
                gl_Position = projection * vec4(aPos, 1.0);
            }
        )";

        const char* fragmentShaderSource = R"(
            #version 430 core
            out vec4 FragColor;
            uniform vec3 color;
            void main() {
                FragColor = vec4(color, 1.0);
            }
        )";
        
        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        int success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Erro ao vincular shaders: " << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        unsigned int cartesianVAO = setupCartesianPlane(xMin, xMax, yMin, yMax);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = glm::ortho(xMin, xMax, yMin, yMax);

            glBindVertexArray(cartesianVAO);
            glUseProgram(shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_LINES, 0, 4);

            for (const auto& vec : vecs) {
                drawVector(vec, shaderProgram, projection, 1.0f, 1.0f, 1.0f);
            }

            if(!res.empty()){
                // O que prova a Comutatividade.
                if(choose){
                    drawVector(res.at(0), shaderProgram, projection, 0.0f, 0.0f, 1.0f);
                    drawVector(res.at(1), shaderProgram, projection, 1.0f, 0.0f, 0.0f);
                }else{
                    drawVector(res.at(1), shaderProgram, projection, 1.0f, 0.0f, 0.0f);
                    drawVector(res.at(0), shaderProgram, projection, 0.0f, 0.0f, 1.0f);
                }
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }else{
        std::cout << "Foi escolhido NÃO prosseguir para a parte 2" << std::endl;
    }

    return 0;
}