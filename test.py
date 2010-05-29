#!/usr/bin/env python

import sys

import daw

def main(paths):
    printPath = 1 < len(paths)
    indentation = ""
    if printPath:
        indentation = "\t"
    for path in paths:
        if printPath: print(path + ":")
        f = daw.load(path)
        if f == None:
            print(indentation + "ERROR")
            continue
        categories = (
            ("plugins", f.plugins()), 
            ("samples", f.samples()), 
            ("effects", f.effects()),
        )
        for (name, items) in categories:
           print(indentation + name + ":")
           for item in items:
               print(indentation + "\t" + item)


if __name__ == "__main__":
    paths = sys.argv[1:]
    if paths == []:
        sys.exit("usage: %s <path> ..." % sys.argv[0])
    main(paths)

