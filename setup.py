try:
    from distutils.core import setup, Extension
except:
    raise RuntimeError("\n\nPython distutils not found!\n")

# Definition of extension modules
cBaumerCamera = Extension('cBaumerCamera',
                          include_dirs=[r'C:\Program Files\Python37\include ', r'C:\Program Files\Baumer GAPI SDK\Components\Dev\C++\Inc', r'C:\opencv\build\include'],
                          libraries=[r'C:\opencv\build\x64\vc15\lib\opencv_world341', r'bgapi2_genicam'],
                          library_dirs=[r'C:\Program Files\Python37\libs', r'C:\Program Files\Baumer GAPI SDK\Components\Dev\C++\Lib\x64', r'C:\opencv\build\x64\vc15\lib'],
                          sources=['BaumerCamera.cpp', 'BaumerCamera2py.cpp'])

# Compile Python module
setup (ext_modules = [cBaumerCamera],
       name = 'cBaumerCamera',
       description = 'cBaumerCamera Python module',
       version = '1.0')