"Global daw exceptions"

class LoadError(Exception):
    "Error loading the project file"
    def __init__(self, message):
        self.message = message

    def __str__(self):
        return repr(self.message)
        
    def __repr__(self):
        return 'LoadError(%s)' % repr(self.message)

