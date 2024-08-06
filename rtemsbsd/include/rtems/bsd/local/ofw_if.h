/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/ofw/ofw_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

/**
 * @defgroup OFW ofw - KObj methods for Open Firmware RTAS implementations
 * @brief A set of methods to implement the Open Firmware client side interface.
 * @{
 */

#ifndef _ofw_if_h_
#define _ofw_if_h_

/** @brief Unique descriptor for the OFW_INIT() method */
extern struct kobjop_desc ofw_init_desc;
/** @brief A function implementing the OFW_INIT() method */
typedef int ofw_init_t(ofw_t _ofw, void *_cookie);
/**
 * @brief Initialize OFW client interface
 *
 * @param _cookie	A handle to the client interface, generally the OF
 *			callback routine.
 */

static __inline int OFW_INIT(ofw_t _ofw, void *_cookie)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_init);
	rc = ((ofw_init_t *) _m)(_ofw, _cookie);
	return (rc);
}

/** @brief Unique descriptor for the OFW_PEER() method */
extern struct kobjop_desc ofw_peer_desc;
/** @brief A function implementing the OFW_PEER() method */
typedef phandle_t ofw_peer_t(ofw_t _ofw, phandle_t _node);
/**
 * @brief Return next sibling of node.
 *
 * @param _node		Selected node
 */

static __inline phandle_t OFW_PEER(ofw_t _ofw, phandle_t _node)
{
	kobjop_t _m;
	phandle_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_peer);
	rc = ((ofw_peer_t *) _m)(_ofw, _node);
	return (rc);
}

/** @brief Unique descriptor for the OFW_PARENT() method */
extern struct kobjop_desc ofw_parent_desc;
/** @brief A function implementing the OFW_PARENT() method */
typedef phandle_t ofw_parent_t(ofw_t _ofw, phandle_t _node);
/**
 * @brief Return parent of node.
 *
 * @param _node		Selected node
 */

static __inline phandle_t OFW_PARENT(ofw_t _ofw, phandle_t _node)
{
	kobjop_t _m;
	phandle_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_parent);
	rc = ((ofw_parent_t *) _m)(_ofw, _node);
	return (rc);
}

/** @brief Unique descriptor for the OFW_CHILD() method */
extern struct kobjop_desc ofw_child_desc;
/** @brief A function implementing the OFW_CHILD() method */
typedef phandle_t ofw_child_t(ofw_t _ofw, phandle_t _node);
/**
 * @brief Return first child of node.
 *
 * @param _node		Selected node
 */

static __inline phandle_t OFW_CHILD(ofw_t _ofw, phandle_t _node)
{
	kobjop_t _m;
	phandle_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_child);
	rc = ((ofw_child_t *) _m)(_ofw, _node);
	return (rc);
}

/** @brief Unique descriptor for the OFW_INSTANCE_TO_PACKAGE() method */
extern struct kobjop_desc ofw_instance_to_package_desc;
/** @brief A function implementing the OFW_INSTANCE_TO_PACKAGE() method */
typedef phandle_t ofw_instance_to_package_t(ofw_t _ofw, ihandle_t _handle);
/**
 * @brief Return package corresponding to instance.
 *
 * @param _handle	Selected instance
 */

static __inline phandle_t OFW_INSTANCE_TO_PACKAGE(ofw_t _ofw, ihandle_t _handle)
{
	kobjop_t _m;
	phandle_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_instance_to_package);
	rc = ((ofw_instance_to_package_t *) _m)(_ofw, _handle);
	return (rc);
}

/** @brief Unique descriptor for the OFW_GETPROPLEN() method */
extern struct kobjop_desc ofw_getproplen_desc;
/** @brief A function implementing the OFW_GETPROPLEN() method */
typedef ssize_t ofw_getproplen_t(ofw_t _ofw, phandle_t _node,
                                 const char *_prop);
/**
 * @brief Return length of node property.
 *
 * @param _node		Selected node
 * @param _prop		Property name
 */

static __inline ssize_t OFW_GETPROPLEN(ofw_t _ofw, phandle_t _node,
                                       const char *_prop)
{
	kobjop_t _m;
	ssize_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_getproplen);
	rc = ((ofw_getproplen_t *) _m)(_ofw, _node, _prop);
	return (rc);
}

/** @brief Unique descriptor for the OFW_GETPROP() method */
extern struct kobjop_desc ofw_getprop_desc;
/** @brief A function implementing the OFW_GETPROP() method */
typedef ssize_t ofw_getprop_t(ofw_t _ofw, phandle_t _node, const char *_prop,
                              void *_buf, size_t _size);
/**
 * @brief Read node property.
 *
 * @param _node		Selected node
 * @param _prop		Property name
 * @param _buf		Pointer to buffer
 * @param _size		Size of buffer
 */

static __inline ssize_t OFW_GETPROP(ofw_t _ofw, phandle_t _node,
                                    const char *_prop, void *_buf, size_t _size)
{
	kobjop_t _m;
	ssize_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_getprop);
	rc = ((ofw_getprop_t *) _m)(_ofw, _node, _prop, _buf, _size);
	return (rc);
}

/** @brief Unique descriptor for the OFW_NEXTPROP() method */
extern struct kobjop_desc ofw_nextprop_desc;
/** @brief A function implementing the OFW_NEXTPROP() method */
typedef int ofw_nextprop_t(ofw_t _ofw, phandle_t _node, const char *_prop,
                           char *_buf, size_t _size);
/**
 * @brief Get next property name.
 *
 * @param _node		Selected node
 * @param _prop		Current property name
 * @param _buf		Buffer for next property name
 * @param _size		Size of buffer
 */

static __inline int OFW_NEXTPROP(ofw_t _ofw, phandle_t _node, const char *_prop,
                                 char *_buf, size_t _size)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_nextprop);
	rc = ((ofw_nextprop_t *) _m)(_ofw, _node, _prop, _buf, _size);
	return (rc);
}

/** @brief Unique descriptor for the OFW_SETPROP() method */
extern struct kobjop_desc ofw_setprop_desc;
/** @brief A function implementing the OFW_SETPROP() method */
typedef int ofw_setprop_t(ofw_t _ofw, phandle_t _node, const char *_prop,
                          const void *_buf, size_t _size);
/**
 * @brief Set property.
 *
 * @param _node		Selected node
 * @param _prop		Property name
 * @param _buf		Value to set
 * @param _size		Size of buffer
 */

static __inline int OFW_SETPROP(ofw_t _ofw, phandle_t _node, const char *_prop,
                                const void *_buf, size_t _size)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_setprop);
	rc = ((ofw_setprop_t *) _m)(_ofw, _node, _prop, _buf, _size);
	return (rc);
}

/** @brief Unique descriptor for the OFW_CANON() method */
extern struct kobjop_desc ofw_canon_desc;
/** @brief A function implementing the OFW_CANON() method */
typedef ssize_t ofw_canon_t(ofw_t _ofw, const char *_path, char *_buf,
                            size_t _size);
/**
 * @brief Canonicalize path.
 *
 * @param _path		Path to canonicalize
 * @param _buf		Buffer for canonicalized path
 * @param _size		Size of buffer
 */

static __inline ssize_t OFW_CANON(ofw_t _ofw, const char *_path, char *_buf,
                                  size_t _size)
{
	kobjop_t _m;
	ssize_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_canon);
	rc = ((ofw_canon_t *) _m)(_ofw, _path, _buf, _size);
	return (rc);
}

/** @brief Unique descriptor for the OFW_FINDDEVICE() method */
extern struct kobjop_desc ofw_finddevice_desc;
/** @brief A function implementing the OFW_FINDDEVICE() method */
typedef phandle_t ofw_finddevice_t(ofw_t _ofw, const char *_path);
/**
 * @brief Return phandle for named device.
 *
 * @param _path		Device path
 */

static __inline phandle_t OFW_FINDDEVICE(ofw_t _ofw, const char *_path)
{
	kobjop_t _m;
	phandle_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_finddevice);
	rc = ((ofw_finddevice_t *) _m)(_ofw, _path);
	return (rc);
}

/** @brief Unique descriptor for the OFW_INSTANCE_TO_PATH() method */
extern struct kobjop_desc ofw_instance_to_path_desc;
/** @brief A function implementing the OFW_INSTANCE_TO_PATH() method */
typedef ssize_t ofw_instance_to_path_t(ofw_t _ofw, ihandle_t _handle,
                                       char *_path, size_t _size);
/**
 * @brief Return path for node instance.
 *
 * @param _handle	Instance handle
 * @param _path		Buffer for path
 * @param _size		Size of buffer
 */

static __inline ssize_t OFW_INSTANCE_TO_PATH(ofw_t _ofw, ihandle_t _handle,
                                             char *_path, size_t _size)
{
	kobjop_t _m;
	ssize_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_instance_to_path);
	rc = ((ofw_instance_to_path_t *) _m)(_ofw, _handle, _path, _size);
	return (rc);
}

/** @brief Unique descriptor for the OFW_PACKAGE_TO_PATH() method */
extern struct kobjop_desc ofw_package_to_path_desc;
/** @brief A function implementing the OFW_PACKAGE_TO_PATH() method */
typedef ssize_t ofw_package_to_path_t(ofw_t _ofw, phandle_t _node, char *_path,
                                      size_t _size);
/**
 * @brief Return path for node.
 *
 * @param _node		Package node
 * @param _path		Buffer for path
 * @param _size		Size of buffer
 */

static __inline ssize_t OFW_PACKAGE_TO_PATH(ofw_t _ofw, phandle_t _node,
                                            char *_path, size_t _size)
{
	kobjop_t _m;
	ssize_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_package_to_path);
	rc = ((ofw_package_to_path_t *) _m)(_ofw, _node, _path, _size);
	return (rc);
}

/** @brief Unique descriptor for the OFW_TEST() method */
extern struct kobjop_desc ofw_test_desc;
/** @brief A function implementing the OFW_TEST() method */
typedef int ofw_test_t(ofw_t _ofw, const char *_name);
/**
 * @brief Test to see if a service exists.
 *
 * @param _name		name of the service
 */

static __inline int OFW_TEST(ofw_t _ofw, const char *_name)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_test);
	rc = ((ofw_test_t *) _m)(_ofw, _name);
	return (rc);
}

/** @brief Unique descriptor for the OFW_CALL_METHOD() method */
extern struct kobjop_desc ofw_call_method_desc;
/** @brief A function implementing the OFW_CALL_METHOD() method */
typedef int ofw_call_method_t(ofw_t _ofw, ihandle_t _instance,
                              const char *_method, int _nargs, int _nreturns,
                              cell_t *_args_and_returns);
/**
 * @brief Call method belonging to an instance handle.
 *
 * @param _instance	Instance handle
 * @param _method	Method name
 * @param _nargs	Number of arguments
 * @param _nreturns	Number of return values
 * @param _args_and_returns	Values for arguments, followed by returns
 */

static __inline int OFW_CALL_METHOD(ofw_t _ofw, ihandle_t _instance,
                                    const char *_method, int _nargs,
                                    int _nreturns, cell_t *_args_and_returns)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_call_method);
	rc = ((ofw_call_method_t *) _m)(_ofw, _instance, _method, _nargs, _nreturns, _args_and_returns);
	return (rc);
}

/** @brief Unique descriptor for the OFW_INTERPRET() method */
extern struct kobjop_desc ofw_interpret_desc;
/** @brief A function implementing the OFW_INTERPRET() method */
typedef int ofw_interpret_t(ofw_t _ofw, const char *_cmd, int _nreturns,
                            cell_t *_returns);
/**
 * @brief Interpret a forth command.
 *
 * @param _cmd		Command
 * @param _nreturns	Number of return values
 * @param _returns	Values for returns
 */

static __inline int OFW_INTERPRET(ofw_t _ofw, const char *_cmd, int _nreturns,
                                  cell_t *_returns)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_interpret);
	rc = ((ofw_interpret_t *) _m)(_ofw, _cmd, _nreturns, _returns);
	return (rc);
}

/** @brief Unique descriptor for the OFW_OPEN() method */
extern struct kobjop_desc ofw_open_desc;
/** @brief A function implementing the OFW_OPEN() method */
typedef ihandle_t ofw_open_t(ofw_t _ofw, const char *_path);
/**
 * @brief Open node, returning instance handle.
 *
 * @param _path		Path to node
 */

static __inline ihandle_t OFW_OPEN(ofw_t _ofw, const char *_path)
{
	kobjop_t _m;
	ihandle_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_open);
	rc = ((ofw_open_t *) _m)(_ofw, _path);
	return (rc);
}

/** @brief Unique descriptor for the OFW_CLOSE() method */
extern struct kobjop_desc ofw_close_desc;
/** @brief A function implementing the OFW_CLOSE() method */
typedef void ofw_close_t(ofw_t _ofw, ihandle_t _instance);
/**
 * @brief Close node instance.
 *
 * @param _instance	Instance to close
 */

static __inline void OFW_CLOSE(ofw_t _ofw, ihandle_t _instance)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_close);
	((ofw_close_t *) _m)(_ofw, _instance);
}

/** @brief Unique descriptor for the OFW_READ() method */
extern struct kobjop_desc ofw_read_desc;
/** @brief A function implementing the OFW_READ() method */
typedef ssize_t ofw_read_t(ofw_t _ofw, ihandle_t _instance, void *_buf,
                           size_t size);
/**
 * @brief Read from device.
 *
 * @param _instance	Device instance
 * @param _buf		Buffer to read to
 * @param _size		Size of buffer
 */

static __inline ssize_t OFW_READ(ofw_t _ofw, ihandle_t _instance, void *_buf,
                                 size_t size)
{
	kobjop_t _m;
	ssize_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_read);
	rc = ((ofw_read_t *) _m)(_ofw, _instance, _buf, size);
	return (rc);
}

/** @brief Unique descriptor for the OFW_WRITE() method */
extern struct kobjop_desc ofw_write_desc;
/** @brief A function implementing the OFW_WRITE() method */
typedef ssize_t ofw_write_t(ofw_t _ofw, ihandle_t _instance, const void *_buf,
                            size_t size);
/**
 * @brief Write to device.
 *
 * @param _instance	Device instance
 * @param _buf		Buffer to write from
 * @param _size		Size of buffer
 */

static __inline ssize_t OFW_WRITE(ofw_t _ofw, ihandle_t _instance,
                                  const void *_buf, size_t size)
{
	kobjop_t _m;
	ssize_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_write);
	rc = ((ofw_write_t *) _m)(_ofw, _instance, _buf, size);
	return (rc);
}

/** @brief Unique descriptor for the OFW_SEEK() method */
extern struct kobjop_desc ofw_seek_desc;
/** @brief A function implementing the OFW_SEEK() method */
typedef int ofw_seek_t(ofw_t _ofw, ihandle_t _instance, uint64_t _off);
/**
 * @brief Seek device.
 *
 * @param _instance	Device instance
 * @param _off		Offset to which to seek
 */

static __inline int OFW_SEEK(ofw_t _ofw, ihandle_t _instance, uint64_t _off)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_seek);
	rc = ((ofw_seek_t *) _m)(_ofw, _instance, _off);
	return (rc);
}

/** @brief Unique descriptor for the OFW_CLAIM() method */
extern struct kobjop_desc ofw_claim_desc;
/** @brief A function implementing the OFW_CLAIM() method */
typedef caddr_t ofw_claim_t(ofw_t _ofw, void *_addr, size_t _size,
                            u_int _align);
/**
 * @brief Claim virtual memory.
 *
 * @param _addr		Requested memory location (NULL for first available)
 * @param _size		Requested size in bytes
 * @param _align	Requested alignment
 */

static __inline caddr_t OFW_CLAIM(ofw_t _ofw, void *_addr, size_t _size,
                                  u_int _align)
{
	kobjop_t _m;
	caddr_t rc;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_claim);
	rc = ((ofw_claim_t *) _m)(_ofw, _addr, _size, _align);
	return (rc);
}

/** @brief Unique descriptor for the OFW_RELEASE() method */
extern struct kobjop_desc ofw_release_desc;
/** @brief A function implementing the OFW_RELEASE() method */
typedef void ofw_release_t(ofw_t _ofw, void *_addr, size_t _size);
/**
 * @brief Release virtual memory.
 *
 * @param _addr		Memory location
 * @param _size		Size in bytes
 */

static __inline void OFW_RELEASE(ofw_t _ofw, void *_addr, size_t _size)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_release);
	((ofw_release_t *) _m)(_ofw, _addr, _size);
}

/** @brief Unique descriptor for the OFW_ENTER() method */
extern struct kobjop_desc ofw_enter_desc;
/** @brief A function implementing the OFW_ENTER() method */
typedef void ofw_enter_t(ofw_t _ofw);
/**
 * @brief Temporarily return control to firmware.
 */

static __inline void OFW_ENTER(ofw_t _ofw)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_enter);
	((ofw_enter_t *) _m)(_ofw);
}

/** @brief Unique descriptor for the OFW_EXIT() method */
extern struct kobjop_desc ofw_exit_desc;
/** @brief A function implementing the OFW_EXIT() method */
typedef void ofw_exit_t(ofw_t _ofw);
/**
 * @brief Halt and return control to firmware.
 */

static __inline void OFW_EXIT(ofw_t _ofw)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_ofw)->ops,ofw_exit);
	((ofw_exit_t *) _m)(_ofw);
}

#endif /* _ofw_if_h_ */
