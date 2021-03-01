add_rules("mode.debug", "mode.release")
add_requires("raylib 3.5.*")

target("sim")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("raylib")
