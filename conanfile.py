from conan import ConanFile

class ImGuiConan(ConanFile):
    name = "CSProject"
    version = "1.0"
    settings = "os", "compiler", "build_type"
    requires = [
        "nlohmann_json/3.11.3"
    ]
    generators = "CMakeDeps", "CMakeToolchain"
