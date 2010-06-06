from .dummy import Dummy
from .exceptions import LoadError
from xml.dom import minidom
import zlib
import struct

def _uncompress(data):
    return zlib.decompress(data[4:])

def _compress(data):
    return struct.pack(">I", len(data)) + zlib.compress(data)

def _getdoc(projectFile):
    f = open(projectFile, 'rb')
    data = f.read()
    f.close()

    # try to parse as xml
    try:
        doc = minidom.parseString(data)
    except:
        # try to decompress
        try:
            data = _uncompress(data)
        except zlib.error:
            return None
        # try to parse result as xml
        try: 
            doc = minidom.parseString(data)
        except:
            return None

    return doc

class Lmms(Dummy):
    canReadFile = True
    canMerge = False
    canRender = False
    fileExtensions = ('mmp', 'mmpz')
    identifier = 'lmms'

    @staticmethod
    def isValid(inProject):
        try:
            doc = _getdoc(inProject)
        except:
            return False

        if doc is None:
            return False

        # cool, we have an xml document. now make sure it's lmms
        if doc.doctype.name != u'multimedia-project':
            return False

        doms = doc.getElementsByTagName('multimedia-project')
        if len(doms) != 1:
            return False

        dom = doms[0]
        if dom.getAttribute('creator') != u'Linux MultiMedia Studio (LMMS)':
            return False

        if dom.getAttribute('type') != u'song':
            return False

        return True

    def load(self, inProject):
        self.doc = _getdoc(inProject)
        assert self.doc is not None

    def save(self, outProject):
        f = open(outProject, 'w')
        f.write(_compress(self.doc.toxml(encoding='utf-8')))
        f.close()

    def extension(self):
        return 'mmpz'

    def samples(self):
        doms = self.doc.getElementsByTagName('audiofileprocessor')
        return list(set([dom.getAttribute('src') for dom in doms]))

    def effects(self):
        def effectName(dom):
            name = dom.getAttribute('name')
            if name == u'ladspaeffect':
                children = dom.getElementsByTagName('key')
                assert len(children) == 1
                child = children[0]
                children = child.getElementsByTagName('attribute')
                for node in children:
                    if node.getAttribute('name') == u'plugin':
                        name = u'ladspaeffect-' + node.getAttribute('value')
                        break
            return name
        doms = self.doc.getElementsByTagName('effect')
        return list(set([effectName(dom) for dom in doms]))

    def generators(self):
        def generatorName(dom):
            name = dom.getAttribute('name')
            if name == u'vestige':
                children = dom.getElementsByTagName('vestige')
                assert len(children) == 1
                child = children[0]
                fullPath = child.getAttribute('plugin')
                # get only file title
                title = fullPath.split('/')[-1]
                # strip extension
                name = title.split('.')[0]
            return name
        doms = self.doc.getElementsByTagName('instrument')
        return list(set([generatorName(dom) for dom in doms]))

    def tempo(self):
        doms = self.doc.getElementsByTagName('head')
        for dom in doms:
            if dom.parentNode.localName == u'multimedia-project':
                return int(dom.getAttribute('bpm'))
                break
        raise LoadError("Project does not contain <head/>")

