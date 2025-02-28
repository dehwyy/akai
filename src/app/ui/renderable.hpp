namespace renderable {
    template <typename T, class S>
    class Renderable {
        public:
            virtual ~Renderable() = default;
            virtual T compile(S state) = 0;
    };
}  // namespace renderable
