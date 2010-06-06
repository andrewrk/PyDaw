#include <Python.h>

#include "Flp.h"

#include <string>

static PyObject * ErrorObject;

typedef struct {
    PyObject_HEAD
    PyObject * x_attr; // attributes dictionary
    Flp * flp; // C++ object
} FlpObject;

extern "C" {
    static void Flp_dealloc(FlpObject * self);
    static PyObject * Flp_getattr(FlpObject * self, char * name);
    static int Flp_setattr(FlpObject * self, char * name, PyObject * v);
    DL_EXPORT(void) initflp();
}

static PyTypeObject Flp_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyObject_HEAD_INIT(NULL)
    0,          /*ob_size*/
    "Flp",          /*tp_name*/
    sizeof(FlpObject),  /*tp_basicsize*/
    0,          /*tp_itemsize*/
    /* methods */
    (destructor)Flp_dealloc, /*tp_dealloc*/
    0,          /*tp_print*/
    (getattrfunc)Flp_getattr, /*tp_getattr*/
    (setattrfunc)Flp_setattr, /*tp_setattr*/
    0,          /*tp_compare*/
    0,          /*tp_repr*/
    0,          /*tp_as_number*/
    0,          /*tp_as_sequence*/
    0,          /*tp_as_mapping*/
    0,          /*tp_hash*/
};

#define FlpObject_Check(v) ((v)->ob_type == &Flp_Type)

static FlpObject * newFlpObject(PyObject * arg)
{
    FlpObject * self;
    self = PyObject_NEW(FlpObject, &Flp_Type);
    if (self == NULL)
        return NULL;

    const char * filename;

    if (!PyArg_ParseTuple(arg, "s", &filename))
        return NULL;
    
    self->x_attr = NULL;
    self->flp = new Flp(filename);

    return self;
}


// Flp methods

static void Flp_dealloc(FlpObject * self)
{
    delete self->flp;
    Py_XDECREF(self->x_attr);
    PyObject_FREE(self);
}

static PyObject * Flp_good(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    if (self->flp->good())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject * Flp_errorMessage(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    return Py_BuildValue("s", self->flp->errorMessage().c_str());
}

static PyObject * Flp_tempo(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    return Py_BuildValue("i", self->flp->tempo());
}

static PyObject * Flp_title(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    return Py_BuildValue("s", self->flp->title().c_str());
}

static PyObject * Flp_versionString(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    return Py_BuildValue("s", self->flp->versionString().c_str());
}

static PyObject * Flp_channelCount(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    return Py_BuildValue("i", self->flp->channelCount());
}

static PyObject * Flp_channelName(FlpObject * self, PyObject * args)
{
    int i;

    if (! PyArg_ParseTuple(args, "i", &i))
        return NULL;

    return Py_BuildValue("s", self->flp->channelName(i).c_str());
}

static PyObject * Flp_channelPluginName(FlpObject * self, PyObject * args)
{
    int i;

    if (! PyArg_ParseTuple(args, "i", &i))
        return NULL;

    return Py_BuildValue("s", self->flp->channelPluginName(i).c_str());
}

static PyObject * Flp_effectCount(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    return Py_BuildValue("i", self->flp->effectCount());
}

static PyObject * Flp_effectPluginName(FlpObject * self, PyObject * args)
{
    int i;

    if (! PyArg_ParseTuple(args, "i", &i))
        return NULL;

    return Py_BuildValue("s", self->flp->effectPluginName(i).c_str());
}

static PyObject * Flp_sampleCount(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    return Py_BuildValue("i", self->flp->sampleCount());
}

static PyObject * Flp_sampleFileName(FlpObject * self, PyObject * args)
{
    int i;

    if (! PyArg_ParseTuple(args, "i", &i))
        return NULL;

    return Py_BuildValue("s", self->flp->sampleFileName(i).c_str());
}

static PyMethodDef Flp_methods[] = {
    {"good",    (PyCFunction)Flp_good,  METH_VARARGS},
    {"errorMessage",    (PyCFunction)Flp_errorMessage,  METH_VARARGS},
    {"tempo",    (PyCFunction)Flp_tempo,  METH_VARARGS},
    {"title",    (PyCFunction)Flp_title,  METH_VARARGS},
    {"versionString",    (PyCFunction)Flp_versionString,  METH_VARARGS},
    {"channelCount",    (PyCFunction)Flp_channelCount,  METH_VARARGS},
    {"channelName",    (PyCFunction)Flp_channelName,  METH_VARARGS},
    {"channelPluginName",    (PyCFunction)Flp_channelPluginName,  METH_VARARGS},
    {"effectCount",    (PyCFunction)Flp_effectCount,  METH_VARARGS},
    {"effectPluginName",    (PyCFunction)Flp_effectPluginName,  METH_VARARGS},
    {"sampleCount",    (PyCFunction)Flp_sampleCount,  METH_VARARGS},
    {"sampleFileName",    (PyCFunction)Flp_sampleFileName,  METH_VARARGS},
    {NULL,      NULL} // sentinel
};

static PyObject * Flp_getattr(FlpObject * self, char * name)
{
    if (self->x_attr != NULL) {
        PyObject * v = PyDict_GetItemString(self->x_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    return Py_FindMethod(Flp_methods, (PyObject *)self, name);
}

static int Flp_setattr(FlpObject * self, char * name, PyObject * v)
{
    if (self->x_attr == NULL) {
        self->x_attr = PyDict_New();
        if (self->x_attr == NULL)
            return -1;
    }
    if (v == NULL) {
        int rv = PyDict_DelItemString(self->x_attr, name);
        if (rv < 0)
            PyErr_SetString(PyExc_AttributeError,
                    "delete non-existing Flp attribute");
        return rv;
    }
    else
        return PyDict_SetItemString(self->x_attr, name, v);
}
/* --------------------------------------------------------------------- */

/* Function of no arguments returning new Flp object */

static PyObject * flp_new(PyObject * self, PyObject * args)
{
    FlpObject *rv;
    
    rv = newFlpObject(args);
    if ( rv == NULL )
        return NULL;
    return (PyObject *)rv;
}

static PyObject * flp_isValid(PyObject * self, PyObject * args)
{
    char * filename;

    if (! PyArg_ParseTuple(args, "s", &filename))
        return NULL;
    bool valid = Flp::isValid(filename);
    if (valid)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}


/* List of functions defined in the module */

static PyMethodDef flp_methods[] = {
    {"new",     flp_new,     METH_VARARGS},
    {"isValid",     flp_isValid,     METH_VARARGS},
    {NULL,      NULL}       /* sentinel */
};


/* Initialization function for the module (*must* be called initflp) */

DL_EXPORT(void) initflp()
{
    PyObject *m, *d;

    /* Initialize the type of the new type object here; doing it here
     * is required for portability to Windows without requiring C++. */
    Flp_Type.ob_type = &PyType_Type;

    /* Create the module and add the functions */
    m = Py_InitModule("flp", flp_methods);

    /* Add some symbolic constants to the module */
    d = PyModule_GetDict(m);
    ErrorObject = PyErr_NewException("flp.error", NULL, NULL);
    PyDict_SetItemString(d, "error", ErrorObject);
}
