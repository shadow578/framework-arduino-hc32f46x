"""
Arduino
Arduino Wiring-based Framework allows writing cross-platform software to
control devices attached to a wide range of Arduino boards to create all
kinds of creative coding, interactive objects, spaces or physical experiences.
http://arduino.cc/en/Reference/HomePage
"""

from os.path import isdir, join

from SCons.Script import DefaultEnvironment

print("platformio-build-arduino.py running")

# get the environment
env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()
build_core = board.get("build.core", "")

# ensure framework is installed
FRAMEWORK_DIR = platform.get_package_dir("framework-arduino-hc32f46x")
assert isdir(FRAMEWORK_DIR)

# prepare basic compile environment
# as far as i understood, the flags in 'CCFLAGS' should be added to all the other flags, but it doesn't seem to work that way...
# as such, i'm adding them to all the other flags manually, which is kinda hacky, but seems to work just fine
common_gcc_flags = [
    "-mcpu=cortex-m4",
	"-mthumb",
	"-mthumb-interwork",
	"-Os",
	"-fmessage-length=0",
	"-fsigned-char",
	"-ffunction-sections",
	"-fdata-sections",
	"-Wall",
	"-g"
]
env.Append(
    # common gcc (?)
    CCFLAGS=common_gcc_flags + [],

    # c
    CFLAGS=common_gcc_flags + [
        "-std=gnu17"
    ],
    
    # c++
    CXXFLAGS=common_gcc_flags + [
        "-std=gnu++17",
        "-fabi-version=0"
    ],

    # asm
    ASFLAGS=common_gcc_flags + [
        "-x", "assembler-with-cpp"
    ],

    # linker
    LINKFLAGS=common_gcc_flags + [
        "-Xlinker",
        "--gc-sections",
        ("-Wl,--default-script", board.get("build.ldscript", join(FRAMEWORK_DIR, "ld", "hc32f46x_param.ld"))),
        #"-Wl,--no-warn-rwx-segment", # introduced in gcc 12, and thus not supported by the version platformio uses
        "-Wl,--print-memory-usage",
        "--specs=nano.specs",
        "--specs=nosys.specs",
    ],

    # c/c++ defines
    CPPDEFINES=[
        "HC32F46x",
        ("ARDUINO", 100),
        "ARDUINO_ARCH_HC32",
        "USE_DEVICE_DRIVER_LIB",
        "__TARGET_FPU_VFP",
	    "__FPU_PRESENT=1",
	    "_MPU_PRESENT=1",
	    "ARM_MATH_CM4",
	    "ARM_MATH_MATRIX_CHECK",
	    "ARM_MATH_ROUNDING"
    ],

    # c/c++ defines
    CPPPATH=[
        join(FRAMEWORK_DIR, "cores", "arduino"),

        # TODO: fix hdsc ddl to not need this hacky include path
        join(FRAMEWORK_DIR, "cores", "arduino", "hdsc", "addon"),
        join(FRAMEWORK_DIR, "cores", "arduino", "hdsc", "common"),
        join(FRAMEWORK_DIR, "cores", "arduino", "hdsc", "include"),
        join(FRAMEWORK_DIR, "cores", "arduino", "hdsc", "library", "inc"),

    ]
)


# resolve and append linker script parameters to ld command line
# parameters are passed into the linker script via the --defsym flag, as symbols
# this allows the linker script to be generic, and the parameters to be passed in using the board manifest
# however, this could cause issues as it's not *exactly* what this flag is for, but it should work for a while...
def get_ld_params():
    # define keys to symbol def names
    ld_args = []
    ld_arg_keys_to_def_mapping = {
        "build.ld_args.flash_start": "FLASH_START",
        "build.ld_args.flash_size": "FLASH_SIZE",
        "build.ld_args.ram_start": "RAM_START",
        "build.ld_args.ram_size": "RAM_SIZE",
        "build.ld_args.ram_ret_start": "RAM_RET_START",
        "build.ld_args.ram_ret_size": "RAM_RET_SIZE",
    }

    # get all the keys from the board manifest, append to arguments list
    for key, def_name in ld_arg_keys_to_def_mapping.items():
        if key in board:
            ld_args.append("-Wl,'--defsym={0}={1}'".format(def_name, board.get(key)))
    
    return ld_args

env.Append(LINKFLAGS=get_ld_params())

#
# Target: Build Core Library
#
libs = []

#if "build.variant" in board:
#    variants_dir = join(
#        "$PROJECT_DIR", board.get("build.variants_dir")) if board.get(
#            "build.variants_dir", "") else join(FRAMEWORK_DIR, "variants")
#
#    env.Append(
#        CPPPATH=[
#            join(variants_dir, board.get("build.variant"))
#        ]
#    )
#    libs.append(env.BuildLibrary(
#        join("$BUILD_DIR", "FrameworkArduinoVariant"),
#        join(variants_dir, board.get("build.variant"))
#    ))

env.BuildSources(
    join("$BUILD_DIR", "FrameworkArduino"), join(FRAMEWORK_DIR, "cores", "arduino")
)

env.Prepend(LIBS=libs)
