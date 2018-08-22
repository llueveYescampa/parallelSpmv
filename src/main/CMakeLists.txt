# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
  
#Create a variable called linalg_SOURCES containing all .cpp files:

set(main_Src 
    src/main/parallelSpmv.c
    PARENT_SCOPE
    )

