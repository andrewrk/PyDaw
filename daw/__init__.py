__version__ = '0.2'

def dawFor(projectFile):
    """
    Picks the daw class that is best suited for opening projectFile.
    """
    from flstudio import FlStudio
    from dummy import Dummy

    _class_list = (
        FlStudio,
    )

    for item in _class_list:
        if item.isValid(projectFile):
            return item

    from exceptions import LoadError
    raise LoadError("File not recognized with any studio.")

def load(projectFile):
    DawClass = dawFor(projectFile)
    f = DawClass()
    f.load(projectFile)
    return f
