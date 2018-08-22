# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
  
#Create a variable called linalg_SOURCES containing all .cpp files:

add_subdirectory(main)
add_subdirectory(spmv)
add_subdirectory(reader)
add_subdirectory(createCommunicator)
add_subdirectory(startCommunication)

set(parallelSpmv_Src 
    ${main_Src}
    ${spmv_Src}
    ${reader_Src}
    ${createCommunicator_Src}
    ${startCommunication_Src}
    PARENT_SCOPE
    )

