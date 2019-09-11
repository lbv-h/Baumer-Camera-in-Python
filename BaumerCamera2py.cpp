#include <Python.h>
#include "BaumerCamera.h"


PyObject* construct(PyObject* self, PyObject* args) 
{
	BaumerCamera* camera = new BaumerCamera();

	PyObject* cameraCapsule = PyCapsule_New((void *)camera, "CameraPtr", NULL);
	PyCapsule_SetPointer(cameraCapsule, (void *)camera);

	return Py_BuildValue("O", cameraCapsule);
}

PyObject* CameraStart(PyObject* self, PyObject* args) 
{
	PyObject* cameraCapsule_;

	PyArg_ParseTuple(args, "O", &cameraCapsule_);

	BaumerCamera* camera = (BaumerCamera*)PyCapsule_GetPointer(cameraCapsule_, "CameraPtr");

	camera->CameraStart();

	return Py_BuildValue("");
}

PyObject* GetImageFromCamera(PyObject* self, PyObject* args)
{
	PyObject* cameraCapsule_;

	PyArg_ParseTuple(args, "O", &cameraCapsule_);

	BaumerCamera* camera = (BaumerCamera*)PyCapsule_GetPointer(cameraCapsule_, "CameraPtr");

	camera->GetImageFromCamera();

	return Py_BuildValue("");
}

PyObject* CameraStop(PyObject* self, PyObject* args)
{
	PyObject* cameraCapsule_;

	PyArg_ParseTuple(args, "O", &cameraCapsule_);

	BaumerCamera* camera = (BaumerCamera*)PyCapsule_GetPointer(cameraCapsule_, "CameraPtr");

	camera->CameraStop();

	return Py_BuildValue("");
}

PyObject* delete_object(PyObject* self, PyObject* args)
{
	PyObject* cameraCapsule_;

	PyArg_ParseTuple(args, "O", &cameraCapsule_);

	BaumerCamera* camera = (BaumerCamera*)PyCapsule_GetPointer(cameraCapsule_, "CameraPtr");

	delete camera;

	return Py_BuildValue("");
}

PyMethodDef cBaumerCameraFunctions[] =
{
	{"construct", construct, METH_VARARGS, "Create 'BaumerCamera' object"},
	{"CameraStart", CameraStart, METH_VARARGS, "Start Camera"},
	{"GetImageFromCamera", GetImageFromCamera, METH_VARARGS, "Get an image from camera"},
	{"CameraStop", CameraStop, METH_VARARGS, "Stop Camera"},
	{"delete_object", delete_object, METH_VARARGS, "Delete 'BaumerCamera' object"},
	{NULL, NULL, 0, NULL}
};

struct PyModuleDef cBaumerCameraModule =
{
	PyModuleDef_HEAD_INIT, "cBaumerCamera", NULL, -1, cBaumerCameraFunctions
};

PyMODINIT_FUNC PyInit_cBaumerCamera(void)
{
	return PyModule_Create(&cBaumerCameraModule);
}