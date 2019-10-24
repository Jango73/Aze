# Aze
A versioning system

## Dependencies

* Qt 5.13
* QtCreator 4.9.1
* qt-plus - Copyright myself
  * https://github.com/Jango73/qt-plus
* Diff Template Library (dtl) - Copyright (c) 2015 Tatsuhiko Kubo <cubicdaiya@gmail.com> All rights reserved.
  * https://github.com/cubicdaiya/dtl

## Building

* Make sure to update submodules
```
    git submodule init
    git submodule update
```
* Open /CuteGit.pro in QtCreator
* Build and run

## Things it does

* Initialize a repository
* Create a branch
* Add files to stage
* Remove files from stage
* Commmit staged files
* Diff between two commits
