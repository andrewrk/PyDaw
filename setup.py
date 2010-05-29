from distutils.core import setup, Extension

# http://docs.python.org/distutils/apiref.html#distutils.core.Extension
flp = Extension(
    'flpmodule',
    sources = [
        'daw/Flp.cpp',
        'daw/Utils.cpp',
        'daw/FlpModule.cpp',
    ],
    include_dirs = [
        'daw'
    ],
)

setup(
    name='flp',
    version='0.1',
    author="Andrew Kelley",
    author_email="superjoe30@gmail.com",
    url="http://github.com/superjoe30/PyFlp",
    description='Mess with FL Studio project files',
    license="GPL",
    ext_modules=[flp],
)
