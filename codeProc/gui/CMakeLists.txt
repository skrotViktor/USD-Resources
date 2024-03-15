file(GLOB sources "*.py")

get_py_module_name(${USDPLUGIN_NAME} _pyname)

install(FILES 
    ${sources}
    DESTINATION lib/python/${PYPACKAGE_NAME}/${_pyname}/gui
)