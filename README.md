# 4211_Project1
Billy Wencl

5544600

Wencl010@umn.edu



### Build
To compile the whole project at once use: 
```bash
    cd /path/to/repo/project
    make
 ```



To compile/recompile only the server files use:
```bash
    cd /path/to/repo/project
    make server
```



To compile/recompile only the client files use:
```bash
    cd /path/to/repo/project
    make client
```



### Run
To start the server:
```bash
    make run_server
```



To start the client:
```bash
    make run_client
```

## Current Limitations
- Topic paths must be less than 70 characters in length
- Messages must be less than 948 characters in length