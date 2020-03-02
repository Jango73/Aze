## XML files
The xml files should be compressed in production environment.
The extension of a compressed XML file is xmlc, which contains UTF8 text data compressed using Qt's built-in compression functions (qCompress and qUncompress).

## Identifiers
All identifiers are expressed using a SHA256 hex string.

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
   |- abcdef0123456789  
   |- 9876543210fedbca  
   |- ...  
```

## General information file structure (.aze/info.xml)

```
<info>  
  <branches>
    current : String
  <stashList>
    <stash>
      id : Identifier
```

## Branch file structure
Those files reside in .aze/branches.

```
<branch>  
  <info>  
    type   : String  
    author : String  
    date   : ISO 8601 Date String  
    root   : Identifier (link to .aze/commits/<id>.xml (.xmlc))  
    tip    : Identifier (link to .aze/commits/<id>.xml (.xmlc))  
```

## Commit file structure
Those files reside in .aze/commits.

```
<commit>  
  <info>  
    author  : String  
    date    : ISO 8601 Date String  
    parents : Comma separated Identifiers  
  <message>  
    String  
  <user>  
    <some key 1>  
      some value 1  
    <some key 2>  
      some value 2  
    ...  
<files>  
  Identifier@some-folder-1/some-file-1.txt \n  
  Identifier@some-folder-1/some-file-2.txt \n  
  Identifier@some-folder-2/some-file-3.cpp \n  
  Identifier@some-folder-2/some-file-4.h  
  ...  
```

## Object files structure
These files reside in .aze/objects.

Each object file holds data for a single file in the repository at a specific moment in time.
A single object file may be referenced by many commits.
