TODO
=====

###Features

- port the private file-descriptor passing single threaded fork/execvp
- figure out the *best* way to stop/reload, probably saving server pid to $somelocation
- forward stdout back to the caller process?
- pass the errors to my own logging daemon via socket? what if the daemon is not there? revert to manual logging | crash?
