The role of the virtual filesystem (VFS) is to describe the hierarchy (e.g. location) of a file relative to others and store
info about the file itself. The hierarchy can either be saved statically with some system (e.g. annotating files with
their parent folder) or by saving the data in the same structure (e.g. children are inside a list of the parent).

The most common operations (that should be fast) for the system are:

- Locating a file
- Collecting all files inside the same directory
- Adding and removing files from inside the directory
- Moving files and directories

Additionally, there are some assumption to be made:
- A user much more often accesses the state (refresh or just login and display) than actively adding/removing files
  - "More reading than writing" or "State change is the exception"
- All operations in the VFS are atomic and instant 
  - No callbacks or waiting, if a change happens update is instant

When taking all of these points into account it seems the best option is to:

- Lay the data out in the same hierarchical way than the folder structure
    - Fast
- Use a continuous datastructure to store the children inside the parents (e.g. std::vector)
    - Brings fastest iteration speed (lookup)
    - Still O(1) insertion and removal (as order doesn't matter)

But this also has downsides:

- Allocation behavior is not clearly defined
    - std::vector does dynamic resizing and memory location changes
    - Because of this references to elements cant (is made more complicated) be cached (reference invalidation)

Note that the intuitive idea of saving the files content at that position as well is not even entertained.
This has several reasons:
- It couples the metadata of a file with its content
  - But a big part of the workload only needs indices
    - Displaying all files (Name, Accessed, Permissions) => Only metadata
    - Showing the content of a file (Given metadata) => Content
- Even more unpredictable allocation behavior

But also just purely practical as the files are not held in memory but directly written to disk.


### Virtual File (VF)

A virtual file describes only the metadata of a physical file and (most importantly) its position in the hierarchy.


### Virtual Directory (VD)

In most ways the same as a VF but can hold other VD's or VF's inside.