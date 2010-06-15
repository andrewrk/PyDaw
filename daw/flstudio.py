from .exceptions import LoadError
from .dummy import Dummy
import flp
import shutil

class FlStudio(Dummy):
    "FL Studio project"

    canReadFile = True
    canMerge = False
    canRender = False
    fileExtensions = ('flp',)
    identifier = "flstudio"

    @staticmethod
    def isValid(projectFile):
        "returns whether the project is of this daw"
        return flp.isValid(projectFile)

    def load(self, inProject):
        """
        load all the information about the project file.
        raises a LoadError if there is a problem.
        """
        self.filename = inProject
        try:
            self.flp = flp.new(inProject)
        except:
            raise LoadError("Error loading the project")
        if not self.flp.good():
            raise LoadError(self.flp.errorMessage())

    def save(self, outProject):
        """
        saves only the project file to outProject
        you should build the filename using the extension function.
        """
        # no processing necessary
        shutil.copy(self.filename, outProject)

    def extension(self):
        return 'flp'

    def samples(self):
        def formatFileName(windowsPath):
            "changes backslashes into forward slashes and removes c:"
            if len(windowsPath) >= 2 and windowsPath[1] == ':':
                windowsPath = windowsPath[2:]
            return windowsPath.replace("\\", "/")

        return [formatFileName(self.flp.sampleFileName(i)) for i in xrange(self.flp.sampleCount())]

    def effects(self):
        "return a list of effects required by this project"
        return [self.flp.effectPluginName(i) for i in xrange(self.flp.effectCount())]

    def generators(self):
        "return a list of generators required by this project"
        return [self.flp.channelPluginName(i) for i in xrange(self.flp.channelCount())]

    def tempo(self):
        return self.flp.tempo()

    def title(self):
        return self.flp.title()
