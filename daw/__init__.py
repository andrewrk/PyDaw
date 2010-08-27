from .flstudio import FlStudio
from .lmms import Lmms
from .exceptions import LoadError

__version__ = '0.4.6'

_class_tuple = (
    FlStudio,
    Lmms,
)

_class_dict = dict([(X.identifier, X) for X in _class_tuple])

def dawFor(projectFile):
    """
    Picks the daw class that is best suited for opening projectFile.
   """

    for item in _class_tuple:
        if item.isValid(projectFile):
            return item

    raise LoadError("File not recognized with any studio.")

def ids():
    return [X.identifier for X in _class_tuple]

def dawForId(identifier):
    return _class_dict[identifier]

def load(projectFile):
    DawClass = dawFor(projectFile)
    f = DawClass()
    f.load(projectFile)
    return f

