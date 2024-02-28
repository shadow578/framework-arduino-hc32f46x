"""
Arduino
Arduino Wiring-based Framework allows writing cross-platform software to
control devices attached to a wide range of Arduino boards to create all
kinds of creative coding, interactive objects, spaces or physical experiences.
http://arduino.cc/en/Reference/HomePage
"""
import sys
from os.path import isfile, isdir, join
from SCons.Script import DefaultEnvironment, SConscript


# get the environment
env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()
build_core = board.get("build.core", "")


# ensure framework is installed
FRAMEWORK_DIR = platform.get_package_dir("framework-arduino-hc32f46x")
CORE_DIR = join(FRAMEWORK_DIR, "cores", "arduino")
assert isdir(CORE_DIR)

# get and check variant directory
board_variant = board.get("build.variant", "")
if not board_variant:
    sys.stderr.write("Error: build.variant is not set")
    env.Exit(1)

VARIANT_DIR = join(FRAMEWORK_DIR, "variants", board_variant)
assert isdir(VARIANT_DIR)

# app_config.h is a optional header file that is included via the command line in all source files
# it can be used to define custom configuration options for the application, as a replacement for the build_flags option
# it can be set in platformio.ini using the 'app_config' option
# it accepts a relative path to the file, which is resolved relative to the project root
app_config_path = board.get("build.app_config", "app_config.h")
app_config_path = join(env.subst("$PROJECT_DIR"), app_config_path)
if not isfile(app_config_path):
    app_config_path = None

# setup compile environment
env.Append(
    # C compiler options
    CFLAGS=[
        f"-include{app_config_path}" if app_config_path else "",
    ],

    # C++ compiler options
    CXXFLAGS=[
        f"-include{app_config_path}" if app_config_path else "",
    ],

    # c/c++ defines
    CPPDEFINES=[
        ("ARDUINO", 100),
        "ARDUINO_ARCH_HC32",
    ],

    # c/c++ include paths
    CPPPATH=[
        CORE_DIR,
        join(CORE_DIR, "drivers"),
        VARIANT_DIR,
    ],

    # add libraries path
    LIBSOURCE_DIRS=[
        join(FRAMEWORK_DIR, "libraries"),
    ]
)

# enable all drivers required by the core
core_requirements = [
    "adc",
    "clk",
    "dmac",
    "efm",
    "exint",
    "gpio",
    "interrupts",
    "pwc",
    "rmu",
    "sram",
    "usart",
    "timera"
]
for req in core_requirements:
    board.update(f"build.ddl.{req}", "true")

# build the ddl core
ddl_build_script = join(env.PioPlatform().get_package_dir("framework-hc32f46x-ddl"), "tools", "platformio", "platformio-build-ddl.py")
if not isfile(ddl_build_script):
    sys.stderr.write(f"Error: Missing PlatformIO build script %s! {ddl_build_script}")
    env.Exit(1)

SConscript(ddl_build_script)

#
# Target: Build Core Library
#
env.BuildSources(join("$BUILD_DIR", "FrameworkArduino", "CORE"), CORE_DIR)
env.BuildSources(join("$BUILD_DIR", "FrameworkArduino", "VARIANT"), VARIANT_DIR)
