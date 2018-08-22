# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
  
#Create a variable called linalg_SOURCES containing all .cpp files:

set(createCommunicator_Src 
    src/createCommunicator/createCommunicator.c
    src/createCommunicator/createColIdxMap.c
    PARENT_SCOPE
    )

