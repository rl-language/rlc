#define Py_LIMITED_API
#include <Python.h>

#ifdef _WIN32
#	define EXPORT __declspec(dllexport)
#else
#	define EXPORT
#endif

static PyObject *get_py_none() { Py_RETURN_NONE; }
#include <Python.h>

static PyObject *wrap_c_pointer_in_ctypes(
		const char *module_name, const char *type_name, void *c_ptr)
{
	PyObject *type_module = NULL;
	PyObject *type_object = NULL;
	PyObject *from_address_method = NULL;
	PyObject *address_object = NULL;
	PyObject *struct_instance = NULL;
	PyObject *result = NULL;

	// Import the module where the ctypes.Structure subclass is defined
	type_module = PyImport_ImportModule(module_name);
	if (!type_module)
	{
		PyErr_Print();
		goto cleanup;
	}

	// Get the Structure subclass from the module
	type_object = PyObject_GetAttrString(type_module, type_name);
	if (!type_object)
	{
		PyErr_Print();
		goto cleanup;
	}

	// Get the 'from_address' class method
	from_address_method = PyObject_GetAttrString(type_object, "from_address");
	if (!from_address_method)
	{
		PyErr_Print();
		goto cleanup;
	}

	// Create a Python integer representing the C pointer's address
	address_object = PyLong_FromVoidPtr(c_ptr);
	if (!address_object)
	{
		PyErr_Print();
		goto cleanup;
	}

	// Call the 'from_address' method with the address
	struct_instance =
			PyObject_CallFunctionObjArgs(from_address_method, address_object, NULL);
	if (!struct_instance)
	{
		PyErr_Print();
		goto cleanup;
	}

	// Success: set the result
	result = struct_instance;

cleanup:
	// Clean up references
	Py_XDECREF(type_module);
	Py_XDECREF(type_object);
	Py_XDECREF(from_address_method);
	Py_XDECREF(address_object);

	return result;	// Returns a new reference or NULL on failure
}

static void call_python_print(PyObject *object)
{
	// Ensure that the GIL is held, if necessary
	PyGILState_STATE gstate = PyGILState_Ensure();

	// Get a reference to the built-in print function
	PyObject *print_function =
			PyObject_GetAttrString(PyImport_AddModule("builtins"), "print");
	if (!print_function || !PyCallable_Check(print_function))
	{
		PyErr_Print();
		fprintf(stderr, "Could not retrieve the print function\n");
		PyGILState_Release(gstate);
		return;
	}

	// Call print with the message
	PyObject *args = PyTuple_Pack(1, object);	 // Pack the message into a tuple
	PyObject *result = PyObject_CallObject(print_function, args);	 // Call print

	// Check for errors and clean up
	if (!result)
	{
		PyErr_Print();
		fprintf(stderr, "Error calling print\n");
	}

	// Clean up references
	Py_XDECREF(result);
	Py_DECREF(args);
	Py_DECREF(print_function);

	// Release the GIL
	PyGILState_Release(gstate);
}

EXPORT void rl_pyprint__PyObject(PyObject **obj) { call_python_print(*obj); }

EXPORT void rl_to_pyobject__int64_t_strlit_r_PyObject(
		PyObject **obj, void **ptr, const char **type_name)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	*obj = wrap_c_pointer_in_ctypes("wrapper", *type_name, *ptr);
	PyGILState_Release(gstate);
}

EXPORT void rl_pyinvoke__PyObject_PyObject_int64_t_r_bool(
		int8_t *out, PyObject **obj, PyObject **arg, int64_t *count)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	if (*obj && PyCallable_Check(*obj))
	{
		PyObject *args = PyTuple_New(*count);
		for (int i = 0; i != *count; i++)
		{
			Py_INCREF(arg[i]);
			PyTuple_SetItem(args, i, arg[i]);
		}

		PyObject *result = PyObject_CallObject(*obj, args);
		Py_DECREF(args);

		if (result == NULL)
		{
			PyErr_Print();
			fprintf(stderr, "Method call failed\n");
			*out = 0;
		}
		else
		{
			Py_DECREF(result);
			*out = 1;
		}
	}
	else
	{
		PyErr_Print();
		fprintf(stderr, "Could not call object\n");
		*out = 0;
	}

	PyGILState_Release(gstate);
}

EXPORT void rl_pyinvoke__PyObject_strlit_PyObject_int64_t_r_bool(
		int8_t *out,
		PyObject **obj,
		const char **method_name,
		PyObject **arg,
		int64_t *count)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject *method = PyObject_GetAttrString(*obj, *method_name);
	if (method && PyCallable_Check(method))
	{
		PyObject *args = PyTuple_New(*count);
		for (int i = 0; i != *count; i++)
		{
			Py_INCREF(arg[i]);
			PyTuple_SetItem(args, i, arg[i]);
		}

		PyObject *result = PyObject_CallObject(method, args);
		Py_DECREF(args);
		Py_DECREF(method);

		if (result == NULL)
		{
			PyErr_Print();
			fprintf(stderr, "Method call failed\n");
			*out = 0;
		}
		else
		{
			Py_DECREF(result);
			*out = 1;
		}
	}
	else
	{
		PyErr_Print();
		fprintf(stderr, "Method '%s' not found or not callable\n", *method_name);
		if (method)
			Py_DECREF(method);
		*out = 0;
	}

	PyGILState_Release(gstate);
}

EXPORT void rl_pyref_decrease__PyObject(PyObject **self)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	if (*self)
	{
		Py_DECREF(*self);
		*self = NULL;
	}
	PyGILState_Release(gstate);
}

EXPORT void rl_pyref_increase__PyObject(PyObject **self)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	if (*self)
		Py_INCREF(*self);
	PyGILState_Release(gstate);
}

EXPORT void rl_to_pyobject__int64_t_r_PyObject(PyObject **result, int64_t *in)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	*result = PyLong_FromLong(*in);
	Py_INCREF(*result);
	PyGILState_Release(gstate);
}

EXPORT void rl_to_pyobject__double_r_PyObject(PyObject **result, double *in)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	*result = PyFloat_FromDouble(*in);
	Py_INCREF(*result);
	PyGILState_Release(gstate);
}

static PyObject *to_bool(int8_t number)
{
	if (number)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

EXPORT void rl_to_pyobject__bool_r_PyObject(PyObject **result, int8_t *in)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	*result = to_bool(*in);
	PyGILState_Release(gstate);
}

EXPORT void rl_py_abort(char *message)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyErr_SetString(PyExc_RuntimeError, message);
	PyGILState_Release(gstate);
	return;
}
