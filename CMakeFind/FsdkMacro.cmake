# 链接外部库
MACRO(LINK_EXTERNAL_LIB TRGTNAME)
    FOREACH(varname ${ARGN})
        IF(${varname}_DEBUG)
            TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}}" debug "${${varname}_DEBUG}")
        ELSE(${varname}_DEBUG)
            TARGET_LINK_LIBRARIES(${TRGTNAME} "${${varname}}" )
        ENDIF(${varname}_DEBUG)
    ENDFOREACH(varname)
ENDMACRO(LINK_EXTERNAL_LIB TRGTNAME)

# 链接内部库
MACRO(LINK_INTERNAL_LIB TRGTNAME)
    IF(NOT CMAKE24)
        TARGET_LINK_LIBRARIES(${TRGTNAME} ${ARGN})
    ELSE(NOT CMAKE24)
        FOREACH(LINKLIB ${ARGN})
            IF(MSVC)
                # TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${OUTPUT_LIBDIR}/${LINKLIB}${CMAKE_RELEASE_POSTFIX}.lib" debug "${OUTPUT_LIBDIR}/${LINKLIB}${CMAKE_DEBUG_POSTFIX}.lib")
				TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${OUTPUT_LIBDIR}/Release/${LINKLIB}${CMAKE_RELEASE_POSTFIX}.lib" debug "${OUTPUT_LIBDIR}/Debug/${LINKLIB}${CMAKE_DEBUG_POSTFIX}.lib")
                # ADD_DEPENDENCIES(${TRGTNAME} ${LINKLIB})
            ELSE(MSVC)
                TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${OUTPUT_LIBDIR}/Release/${LINKLIB}${CMAKE_RELEASE_POSTFIX}" debug "${OUTPUT_LIBDIR}/Debug/${LINKLIB}${CMAKE_DEBUG_POSTFIX}")
            ENDIF(MSVC)
        ENDFOREACH(LINKLIB)
    ENDIF(NOT CMAKE24)
ENDMACRO(LINK_INTERNAL_LIB TRGTNAME)

