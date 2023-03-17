from conan import ConanFile
from conan.tools.cmake import CMakeDeps


class SiloRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    requires = [
        "boost/1.81.0",
        "poco/1.12.4",
        "onetbb/2021.7.0",
        "xz_utils/5.2.5",
        "rapidjson/cci.20220822",
        "nlohmann_json/3.11.2",
        "gtest/cci.20210126",
        "roaring/0.9.9"
    ]

    default_options = {
        "roaring/*:shared": False,

        "gtest/*:no_main": True,

        "boost/*:lzma": True,
        "boost/*:shared": False,

        "boost/*:without_iostreams": False,
        "boost/*:without_serialization": False,
        "boost/*:without_system": False,
        "boost/*:without_random": False,
        "boost/*:without_regex": False,

        "boost/*:without_atomic": True,
        "boost/*:without_chrono": True,
        "boost/*:without_container": True,
        "boost/*:without_context": True,
        "boost/*:without_contract": True,
        "boost/*:without_coroutine": True,
        "boost/*:without_date_time": True,
        "boost/*:without_exception": True,
        "boost/*:without_fiber": True,
        "boost/*:without_filesystem": True,
        "boost/*:without_graph": True,
        "boost/*:without_graph_parallel": True,
        "boost/*:without_json": True,
        "boost/*:without_locale": True,
        "boost/*:without_log": True,
        "boost/*:without_math": True,
        "boost/*:without_mpi": True,
        "boost/*:without_nowide": True,
        "boost/*:without_program_options": True,
        "boost/*:without_python": True,
        "boost/*:without_stacktrace": True,
        "boost/*:without_test": True,
        "boost/*:without_thread": True,
        "boost/*:without_timer": True,
        "boost/*:without_type_erasure": True,
        "boost/*:without_wave": True,

        "poco/*:shared": False,
        "poco/*:enable_json": True,
        "poco/*:enable_net": True,
        "poco/*:enable_util": True,

        "poco/*:enable_crypto": False,
        "poco/*:enable_activerecord": False,
        "poco/*:enable_active_record": False,
        "poco/*:enable_data": False,
        "poco/*:enable_data_mysql": False,
        "poco/*:enable_data_postgresql": False,
        "poco/*:enable_data_sqlite": False,
        "poco/*:enable_encodings": False,
        "poco/*:enable_jwt": False,
        "poco/*:enable_mongodb": False,
        "poco/*:enable_netssl": False,
        "poco/*:enable_redis": False,
        "poco/*:enable_xml": False,
        "poco/*:enable_zip": False,
    }

    def generate(self):
        deps = CMakeDeps(self)
        deps.set_property("boost", "cmake_find_mode", "both")
        deps.set_property("onetbb", "cmake_find_mode", "both")
        deps.set_property("xz_utils", "cmake_find_mode", "both")
        deps.set_property("rapidjson", "cmake_find_mode", "both")
        deps.set_property("poco", "cmake_find_mode", "both")
        deps.set_property("nlohmann_json", "cmake_find_mode", "both")
        deps.set_property("gtest", "cmake_find_mode", "both")
        deps.set_property("pcre2", "cmake_find_mode", "both")
        deps.set_property("roaring", "cmake_find_mode", "both")
        deps.generate()
