from distutils.core import setup, Extension

flp = Extension(
    'flpmodule',
    sources = [
        'Flp.cpp',
        'Utils.cpp',
        'FlpModule.cpp',
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
