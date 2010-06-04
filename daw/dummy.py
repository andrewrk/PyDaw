from .exceptions import LoadError

class Dummy():
    "the interface used for a daw object"

    # if true, can use the functions that provide data about the file
    canReadFile = False

    # if true, can use merge function
    canMerge = False

    # if true, the library can render a project file into a wave file.
    canRender = False

    # the file extensions used by this daw
    fileExtensions = ()
    
    # a unique identifier string for this daw.
    identifier = "dummy"

    @staticmethod
    def isValid(inProject):
        "returns whether the project is of this daw"
        return False

    @staticmethod
    def merge(inProject1, inProject2, outProject):
        "merge the project files into 1 file"
        pass

    def load(self, inProject):
        """
        load all the information about the project file.
        raises a LoadError if there is a problem.
        """
        pass

    def save(self, outProject):
        """
        saves only the project file to outProject
        you should build the filename using the extension function.
        """
        pass

    def extension(self):
        """
        Returns the best extension for this file.
        """
        return None

    def render(self, outWave):
        "render the project into a wave file."
        pass

    def samples(self):
        "return a list of samples required by this project"
        return None

    def effects(self):
        "return a list of effects required by this project"
        return None

    def generators(self):
        "return a list of generators required by this project"
        return None

    def tempo(self):
        return None

    def title(self):
        "returns the title of the project"
        return None

