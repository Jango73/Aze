## Repository structure

```
.aze  
|- info.xml  
|- branches  
|  |- trunk.xml  
|  |- some-branch.xml  
|- stash  
|- commits
|  |- 0123456789abcdef.xml  
|  |- fedbca9876543210.xml  
|  |- ...  
|- objects
```

## Branch file structure

```
<branch>  
  <info>  
    type : String  
    author : String  
    date : ISO 8601 Date String  
    root : Commit Id (link to commits/id.xmlc)  
    tip : Commit Id (link to commits/id.xmlc)  
```

## Commit file structure
```
<commit>  
  <info>  
    author : String  
    date : ISO 8601 Date String  
    parents : Comma sparated Ids  
  <message>  
    String  
  <user>  
    <some key 1>  
      some value 1  
    <some key 2>  
      some value 2  
    ...  
<files>  
  0011@userFolder1/userFile1.txt \n  
  0012@userFolder1/userFile2.txt \n  
  1004@userFolder2/userFile3.cpp \n  
  1020@userFolder2/userFile4.h  
```
