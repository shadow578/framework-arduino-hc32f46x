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

print("platformio-build-arduino.py running")

# get the environment
env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()
build_core = board.get("build.core", "")

# ensure framework is installed
FRAMEWORK_DIR = platform.get_package_dir("framework-arduino-hc32f46x")
CORE_DIR = join(FRAMEWORK_DIR, "cores", "arduino")
assert isdir(CORE_DIR)

# setup compile environment
env.Append(
    # c/c++ defines
    CPPDEFINES=[
        "HC32F46x",
        ("ARDUINO", 100),
        "ARDUINO_ARCH_HC32",
    ],

    # c/c++ include paths
    CPPPATH=[
        CORE_DIR,
        join(CORE_DIR, "drivers"),
    ]
)

# enable all drivers required by the core
core_requirements = [
    "adc",
    "clk",
    "dmac",
    "efm",
    "extint",
    "gpio",
    "interrupts",
    "pwc",
    "rmu",
    "sdioc",
    "sram",
    "usart",
    "wdt",
]
for req in core_requirements:
    board.update("build.ddl.%s" % req, "true")

# build the ddl core
ddl_build_script = join(env.PioPlatform().get_package_dir("framework-hc32f46x-ddl"), "tools", "platformio", "platformio-build-ddl.py")
if not isfile(ddl_build_script):
    sys.stderr.write("Error: Missing PlatformIO build script %s!\n" % ddl_build_script)
    env.Exit(1)

SConscript(ddl_build_script)


#
# Target: Build Core Library
#
env.BuildSources(join("$BUILD_DIR", "FrameworkArduino"), CORE_DIR)
