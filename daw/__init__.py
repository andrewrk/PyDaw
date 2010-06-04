
import flp

def load(path):
    try:
        guess = _extensionToLoadFunc[path[path.rfind("."):]]
    except KeyError:
        guess = None
    if guess != None:
        result = guess(path)
        if result != None:
            return result # guessed right
    for func in (load_flp, load_lmms):
        if func == guess:
            continue # already tried this one
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

def load_lmms(path):
    return None

_extensionToLoadFunc = {
    ".flp": load_flp,
    ".mmp": load_lmms,
    ".mmpz": load_lmms,
}







