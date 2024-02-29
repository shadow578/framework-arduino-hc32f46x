# for some reason, the 'platform_packages' directive in platformio.ini
# does not apply to 'DefaultEnvironment().PioPlatform().get_package_dir()', which is used 
# by the hc32f46x build system...
#
# this script is a workaround, patching the 'get_package_dir' method to return the project directory
# for the 'framework-arduino-hc32f46x' package.
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()
platform = env.PioPlatform()
original_get_package_dir = platform.get_package_dir

def get_package_dir_override(name):
    if name == "framework-arduino-hc32f46x":
        return env.subst("$PROJECT_DIR")
    else:
        return original_get_package_dir(name)

platform.get_package_dir = get_package_dir_override
