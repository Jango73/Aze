## XML files
The xml files should be compressed in production environment.
The extension of a compressed XML file is xmlc, which contains UTF8 text data compressed using Qt's built-in compression functions (qCompress and qUncompress).

## Repository structure

```
.aze  
|- info.xml  
|- branches  
|  |- trunk.xml (.xmlc)  
|  |- some-branch.xml (.xmlc)  
|- stash  
|- commits
|  |- 0123456789abcdef.xml (.xmlc)  
|  |- fedbca9876543210.xml (.xmlc)  
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
    root : Commit Id (link to commits/id.xml (.xmlc))  
    tip : Commit Id (link to commits/id.xml (.xmlc))  
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
