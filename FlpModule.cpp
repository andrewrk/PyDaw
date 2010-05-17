#include <Python.h>

#include "Flp.h"

static PyObject * ErrorObject;

typedef struct {
    PyObject_HEAD
    PyObject * x_attr; // attributes dictionary
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
    self->x_attr = NULL;
    return self;
}

// Flp methods

static void Flp_dealloc(FlpObject * self)
{
    Py_XDECREF(self->x_attr);
    PyMem_DEL(self);
}

static PyObject * Flp_demo(FlpObject * self, PyObject * args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Flp_methods[] = {
    {"demo",    (PyCFunction)Flp_demo,  1},
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
    
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    rv = newFlpObject(args);
    if ( rv == NULL )
        return NULL;
    return (PyObject *)rv;
}

/* List of functions defined in the module */

static PyMethodDef flp_methods[] = {
    {"new",     flp_new,     1},
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
