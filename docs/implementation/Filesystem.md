The role of the virtual filesystem (VFS) is to describe the hierarchy (e.g. location) of a file relative to others and
store
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

## Transactions

Transaction are a very important concept as they give certain guarantees such as:

- Specific operations either always happen together or not at all
- If transaction doesn't finish the state is reverted as if it never happened

Having those guarantees is crucial for e.g. creating files or deleting files.

Implementing the transaction is made more difficult by different timing requirements.
While the virtual file systems operations are instant, file transfer from clients is not. This is why transactions
operate on an async basis.

The transaction for creating a new file in a directory in the following way (others have similar steps):
- Validate the conditions of creating a files:
  - Validate file name
  - Validate user access control
  - Check if the directory exists
- Add the file to the VFS immediately 
  - Now the file exists in the actual sense (although no physical data is there yet)
    - Queries will return the file with its size and name etc.
- Create a transaction that saves relevant state e.g. fileid and physical datastore etc.
  - The transaction object then handles adding the physical data 
  - Should the physical transfer fail the entry in the VFS is removed

