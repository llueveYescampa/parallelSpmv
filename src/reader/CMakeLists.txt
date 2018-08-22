# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
  
#Create a variable called linalg_SOURCES containing all .cpp files:

set(reader_Src 
    src/reader/reader.c
    src/reader/getRowsNnzPerProc.c
    src/reader/vectorReader.c
    PARENT_SCOPE
    )

