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
    name='daw',
    version='0.4.6', # also update in daw/__init__.py
    author="Andrew Kelley",
    author_email="superjoe30@gmail.com",
    url="http://github.com/superjoe30/PyDaw",
    description='Mess with digital audio workstation project files',
    license="GPL",
    ext_modules=[flp],
    packages=["daw"],
)
