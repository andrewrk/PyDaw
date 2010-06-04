from .flstudio import FlStudio
from .lmms import Lmms
from .dummy import Dummy

__version__ = '0.2'

_class_tuple = (
    FlStudio,
    Lmms,
)

def dawFor(projectFile):
    """
    Picks the daw class that is best suited for opening projectFile.
    """

    for item in _class_tuple:
        if item.isValid(projectFile):
            return item

    from exceptions import LoadError
    raise LoadError("File not recognized with any studio.")

def dawIds():
    return [X.identifier for X in _class_tuple]

def load(projectFile):
    DawClass = dawFor(projectFile)
    f = DawClass()
    f.load(projectFile)
    return f

