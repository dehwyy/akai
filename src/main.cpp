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

#include "app/app.hpp"

using namespace app;

int main(int, char**) {
    App app;
    app.Init();
    app.Start();
}
