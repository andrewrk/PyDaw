from .exceptions import LoadError
from .dummy import Dummy
import flp
import shutil

class FlStudio(Dummy):
    "FL Studio project"

    canReadFile = True
    canMerge = False
    canRender = False
    fileExtensions = (u'flp',)
    identifier = u"flstudio"

    def _yield_list(self, count, getter):
        for i in xrange(count):
            try:
                name = getter(i).decode()
            except UnicodeDecodeError:
                continue
            if len(name) == 0:
                continue
            yield name

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
            raise LoadError(u"Error loading the project")
        if not self.flp.good():
            raise LoadError(self.flp.errorMessage().decode())

    def save(self, outProject):
        """
        saves only the project file to outProject
        you should build the filename using the extension function.
        """
        # no processing necessary
        shutil.copy(self.filename, outProject)

    def extension(self):
        return u'flp'

    def samples(self):
        def formatFileName(windowsPath):
            "changes backslashes into forward slashes and removes c:"
            if len(windowsPath) >= 2 and windowsPath[1] == ':':
                windowsPath = windowsPath[2:]
            return windowsPath.replace("\\", "/")

        return self._yield_list(self.flp.sampleCount(), \
            lambda index: formatFileName(self.flp.sampleFileName(index)))

    def effects(self):
        "return a list of effects required by this project"
        return self._yield_list(self.flp.effectCount(), self.flp.effectPluginName)

    def generators(self):
        "return a list of generators required by this project"
        return self._yield_list(self.flp.channelCount(), self.flp.channelPluginName)

    def tempo(self):
        return self.flp.tempo()

    def title(self):
        try:
            return self.flp.title().decode()
        except UnicodeDecodeError:
            return u''
