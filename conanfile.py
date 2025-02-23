from conan import ConanFile

class ImGuiConan(ConanFile):
    name = "CSProject"
    version = "1.0"
    settings = "os", "compiler", "build_type"
    requires = [
        "nlohmann_json/3.11.3",
        "sfml/2.6.2",
        # "imgui/1.89.9",
        # "imgui-sfml/2.5@bincrafters/stable"
    ]
    generators = "CMakeDeps", "CMakeToolchain"
