from conan import ConanFile
from conan.tools.microsoft import vs_layout

import os
from typing import NamedTuple, Optional

class CMakeInfo(NamedTuple):
    # Name of the package to find with find_package
    package: str
    # CMake target to use with target_link_libraries, if needed
    target: str
    # CMake variable to use in CMakeLists, defaults to ucase "${package}_LIB"
    var: Optional[str] = None

class Library(NamedTuple):
    # Name of library from conan-center
    name: str
    # Version of package from conan-center
    version: str
    # CMake package info
    info: Optional[CMakeInfo] = None


libraries = [
    Library("boost", "1.83.0", CMakeInfo("Boost", "boost::boost")),
    Library("crowcpp-crow", "1.0+3", CMakeInfo("Crow", "Crow::Crow")),
    Library("gtest", "1.12.1", CMakeInfo("GTest", "gtest::gtest")),   
    Library("libjpeg", "9e"),
    Library("libpng", "1.6.40", CMakeInfo("PNG", "PNG::PNG")),
    Library("libtiff", "4.6.0", CMakeInfo("TIFF", "TIFF::TIFF")),
    Library("libmediainfo", "22.03", CMakeInfo("MediaInfoLib", "MediaInfoLib::MediaInfoLib")),
    Library("zlib", "1.3.1", CMakeInfo("ZLIB", "ZLIB::ZLIB")),
]

class CassetteRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def init(self):
        # emit the find_package calls in the conanbuildinfo.cmake,
        # along with the variables for later use in the CMakeLists
        with open(os.path.join(self.recipe_folder, "cmake/ConanLibImports.cmake"), "w") as f:
            f.write('# Generated file: DO NOT EDIT or COMMIT!\n')
            f.write('# Add your library to the conanfile.py libraries list\n\n')
            for library in libraries:
                if library.info:
                  f.write(f'find_package({library.info.package} REQUIRED)\n')
                  var_name = library.info.var if library.info.var else f'{library.info.package.upper()}_LIB'
                  f.write(f'set({var_name} {library.info.target})\n\n')
                
    def requirements(self):
        for requirement in libraries:
            self.requires(requirement.name+"/"+requirement.version)

    def layout(self):
        vs_layout(self)
