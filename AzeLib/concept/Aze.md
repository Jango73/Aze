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
These files reside in .aze/branches.

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
These files reside in .aze/commits.

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

## Branching and merging
In the following diagrams:
* Labels beginning with a C refer to a standard commit
* Labels beginning with a M refer to a merge commit

This sequence should be auto-tested.

### Step 1
Initial situation.

```
    Trunk root      Trunk tip
    v               v
  [CA1] - [CA2] - [CA3]
            |
            \---- [CB1] - [CB2] - [CB3]
            ^                       |^
            Branch B1 root          |Branch B1 tip
                                    |
                                    \---- [CC1] - [CC2]
                                    ^               ^
                                    Branch B2 root  Branch B2 tip
```

### Step 2
Merge B1 on trunk.
Common ancestor of CA3 and CB3 is CA2.

```
    Trunk root                              Trunk tip
    v                                       v
  [CA1] - [CA2] - [CA3] ----------------- [MA4]
            |                               |
            \---- [CB1] - [CB2] - [CB3] ----/
            ^                       |^
            Branch B1 root          |Branch B1 tip
                                    |
                                    \---- [CC1] - [CC2]
                                    ^               ^
                                    Branch B2 root  Branch B2 tip
```

### Step 3
Merge B2 on trunk.
Common ancestor of MA4 and CC2 is CB3.

```
    Trunk root                              Trunk tip
    v                                       v
  [CA1] - [CA2] - [CA3] ----------------- [MA4] --------- [MA5]
            |                               |               |
            \---- [CB1] - [CB2] - [CB3] ----/               |
            ^                       |^                      |
            Branch B1 root          |Branch B1 tip          |
                                    |                       |
                                    \---- [CC1] - [CC2] ----/
                                    ^               ^
                                    Branch B2 root  Branch B2 tip
```

### Step 4
Work on B1 and B2.

```
    Trunk root                              Trunk tip
    v                                       v
  [CA1] - [CA2] - [CA3] ----------------- [MA4] --------- [MA5]
            |                               |               |
            \---- [CB1] - [CB2] - [CB3] ----/-------------- | --- [CB4] - [CB5]
            ^                       |                       |               ^
            Branch B1 root          |                       |               Branch B1 tip
                                    |                       |
                                    \---- [CC1] - [CC2] ----/ --- [CC3]
                                    ^                               ^
                                    Branch B2 root                  Branch B2 tip
```

### Step 5
Merge B1 on trunk.
Common ancestor of MA5 and CB5 is MA4.

```
    Trunk root                                                                      Trunk tip
    v                                                                               v
  [CA1] - [CA2] - [CA3] ----------------- [MA4] --------- [MA5] ----------------- [MA6]
            |                               |               |                       |
            \---- [CB1] - [CB2] - [CB3] ----/-------------- | --- [CB4] - [CB5] ----/
            ^                       |                       |               ^
            Branch B1 root          |                       |               Branch B1 tip
                                    |                       |
                                    \---- [CC1] - [CC2] ----/ --- [CC3]
                                    ^                               ^
                                    Branch B2 root                  Branch B2 tip
```

### Step 6
Merge trunk on B2.
Common ancestor of CC3 and MA6 is MA5.

```
    Trunk root                                                                      Trunk tip
    v                                                                               v
  [CA1] - [CA2] - [CA3] ----------------- [MA4] --------- [MA5] ----------------- [MA6] ----\
            |                               |               |                       |       |
            \---- [CB1] - [CB2] - [CB3] ----/-------------- | --- [CB4] - [CB5] ----/       |
            ^                       |                       |               ^               |
            Branch B1 root          |                       |               Branch B1 tip   |
                                    |                       |                               |
                                    \---- [CC1] - [CC2] ----/ --- [CC3] ----------------- [MC4]
                                    ^                                                       ^
                                    Branch B2 root                                          Branch B2 tip
```

## Workflow samples

```
Working directory                Local database                   Remote database

```
