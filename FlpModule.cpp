#include <Python.h>

#include "Flp.h"

static PyObject * ErrorObject;

typedef struct {
    PyObject_HEAD
    PyObject * x_attr; // attributes dictionary
} FlpObject;

staticforward PyTypeObject Flp_Type;

#define FlpObject_Check(v) ((v)->ob_type == &Flp_Type)

static FlpObject *
newFlpObject(arg)
    PyObject * arg;
{
    FlpObject * self;
    self = PyObject_NEW(FlpObject, &Flp_Type);
    if (self == NULL)
        return NULL;
    self->x_attr = NULL;
    return self;
}

// Flp methods

static void
Flp_dealloc(self)
    FlpObject * self;
{
    Py_XDECREF(self->x_attr);
    PyMem_DEL(self);
}

static PyObject *
Flp_demo(self, args)
    FlpObject * self;
    PyObject * args;
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

static PyObject *
Flp_getattr(self, name)
    FlpObject * self;
    char * name;
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

static int
Flp_setattr(self, name, v)
    FlpObject *self;
    char *name;
    PyObject *v;
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

statichere PyTypeObject Flp_Type = {
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
/* --------------------------------------------------------------------- */

/* Function of two integers returning integer */

static PyObject *
xx_foo(self, args)
    PyObject *self; /* Not used */
    PyObject *args;
{
    long i, j;
    long res;
    if (!PyArg_ParseTuple(args, "ll", &i, &j))
        return NULL;
    res = i+j; /* XXX Do something here */
    return PyInt_FromLong(res);
}


/* Function of no arguments returning new Flp object */

static PyObject *
xx_new(self, args)
    PyObject *self; /* Not used */
    PyObject *args;
{
    FlpObject *rv;
    
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    rv = newFlpObject(args);
    if ( rv == NULL )
        return NULL;
    return (PyObject *)rv;
}

/* Example with subtle bug from extensions manual ("Thin Ice"). */

static PyObject *
xx_bug(self, args)
    PyObject *self;
    PyObject *args;
{
    PyObject *list, *item;
    
    if (!PyArg_ParseTuple(args, "O", &list))
        return NULL;
    
    item = PyList_GetItem(list, 0);
    /* Py_INCREF(item); */
    PyList_SetItem(list, 1, PyInt_FromLong(0L));
    PyObject_Print(item, stdout, 0);
    printf("\n");
    /* Py_DECREF(item); */
    
    Py_INCREF(Py_None);
    return Py_None;
}

/* Test bad format character */

static PyObject *
xx_roj(self, args)
    PyObject *self; /* Not used */
    PyObject *args;
{
    PyObject *a;
    long b;
    if (!PyArg_ParseTuple(args, "O#", &a, &b))
        return NULL;
    Py_INCREF(Py_None);
    return Py_None;
}


/* List of functions defined in the module */

static PyMethodDef xx_methods[] = {
    {"roj",     xx_roj,     1},
    {"foo",     xx_foo,     1},
    {"new",     xx_new,     1},
    {"bug",     xx_bug,     1},
    {NULL,      NULL}       /* sentinel */
};


/* Initialization function for the module (*must* be called initxx) */

DL_EXPORT(void)
initxx()
{
    PyObject *m, *d;

    /* Initialize the type of the new type object here; doing it here
     * is required for portability to Windows without requiring C++. */
    Flp_Type.ob_type = &PyType_Type;

    /* Create the module and add the functions */
    m = Py_InitModule("xx", xx_methods);

    /* Add some symbolic constants to the module */
    d = PyModule_GetDict(m);
    ErrorObject = PyErr_NewException("xx.error", NULL, NULL);
    PyDict_SetItemString(d, "error", ErrorObject);
}

