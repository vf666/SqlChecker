# 设置模块名称
SET(MODULE_NAME	sql_trans_checker)

# 将导出头文件归组
FILE(GLOB_RECURSE ROOT_HEADERS 
	./*.hpp 
	./*.h
)
source_group("Include" FILES ${ROOT_HEADERS})
# 将源文件和内部头文件归组
FILE(GLOB_RECURSE ROOT_SOURCES
	./*.cpp 
	./*.c
	
)
source_group("Source" FILES ${ROOT_SOURCES})

# 项目输出运行文件
ADD_EXECUTABLE(${MODULE_NAME}  
			${ROOT_HEADERS}
			${ROOT_SOURCES} 
)
set_target_properties(${MODULE_NAME} 
				PROPERTIES FOLDER Demo
)

# 添加内部库
LINK_INTERNAL_LIB(${MODULE_NAME}
			# FsdkObj
			# FsdkKernel
)

# 查询外部模块
FIND_PACKAGE(MySql)
INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})
# 添加外部库
LINK_EXTERNAL_LIB(${MODULE_NAME}
				MYSQL_LIBRARY
)
