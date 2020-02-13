# Aze
A versioning system

## Dependencies

* Qt 5.13
* QtCreator 4.9.1
* qt-plus - Copyright myself
  * https://github.com/Jango73/qt-plus
* Diff Match and Patch - Copyright 2018 The diff-match-patch Authors.
  * Qt/C++ port by mikeslemmer@gmail.com (Mike Slemmer)
  * Original : https://github.com/google/diff-match-patch

## Building

* Make sure to update submodules
```
    git submodule init
    git submodule update
```
* Open /Aze.pro in QtCreator
* Build and run

## Things it does

* Initialize a repository
* Create a branch
* Add files to stage
* Remove files from stage
* Commmit staged files
* Diff between two commits
* Merge a branch
