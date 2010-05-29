
import flp

def load(path):
    for func in (load_flp,):
        result = func(path)
        if result != None:
            return result
    return None

def load_flp(path):
    f = flp.new(path)
    if not f.good():
        return None
    class Wrapper(object):
        def plugins(self):
            for i in range(f.channelCount()):
                yield f.channelPluginName(i)
        def samples(self):
            for i in range(f.sampleCount()):
                yield f.sampleFileName(i)
        def effects(self):
            for i in range(f.effectCount()):
                yield f.effectPluginName(i)
    return Wrapper()









