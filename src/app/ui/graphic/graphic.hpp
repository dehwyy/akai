#include "../renderable.hpp"

namespace graphic {
    // todo
    class Graphic : public renderable::Renderable<void*, void*> {
        public:
            void* compile(void* state) override { return state; }
    };
}  // namespace graphic
