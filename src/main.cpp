// const float SCALE_X = 50.0f;   // Масштаб по X (увеличение)
// const float SCALE_Y = 100.0f;  // Масштаб по Y (увеличение)

// const double EPSILON = 0.1;

// double f(double x) {
//     // std::cout << "X= " << x << std::endl;
//     double radian = std::fmod(x, 2. * 3.14);

//     // std::cout << x << " " << radian << std::endl;
//     const double breakpoin = radian > 3.14 ? 3. * 3.14 / 2. : 3.14 / 2.;
//     if (radian > breakpoin - EPSILON && radian <= breakpoin + EPSILON) {
//         const double borderX = breakpoin - ((breakpoin - radian) > 0 ? -EPSILON : EPSILON);
//         return f(borderX);
//     }

//     return std::fabs(std::cos(radian));
// }

// void drawGraph(sf::RenderWindow* window, double k) {
//     // std::cout << "k = " << k << std::endl;
//     sf::VertexArray graph(sf::LineStrip);

//     for (float x = -8.0 * std::fabs(1 / k); x <= 8.0 * std::fabs(1 / k);
//          x += 0.1) {            // Диапазон X от -8 до 8
//         float y = std::sin(x);  // Функция

//         // Преобразование координат в пиксели
//         float pixelX = WIDTH / 2 + x * SCALE_X * k;
//         float pixelY = HEIGHT / 2 - y * SCALE_Y;

//         graph.append(sf::Vertex(sf::Vector2f(pixelX, pixelY), sf::Color::Magenta));
//     }

//     window->draw(graph);
// }

#include "app/app.hpp"

using namespace app;

int main(int, char**) {
    App* app = new App();
    app->Init();
    app->Start();
}
