#编译模块
OPTION(BUILD_trans_checker "Buile the trans_checker of cmake of fsdk" ON)
IF(BUILD_trans_checker)
	ADD_SUBDIRECTORY(trans_checker)
ENDIF()