# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
  
#Create a variable called linalg_SOURCES containing all .cpp files:

set(startCommunication_Src 
    src/startCommunication/startCommunication.c
    PARENT_SCOPE
    )

