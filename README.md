# Aze
A versioning system (just for pleasure of coding)

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

## Concepts
[Here is information about architecture](AzeLib/concept/Aze.md)

## Implemented functionality

### Client side
#### Local
* Initialize a repository
* Show status of files
* Create a branch
* Switch branches
* Add files to stage
* Remove files from stage
* Commit staged files
* Show commit log
* Show commit graph => needs rework to show commits in correct order
* Diff between two commits
* Merge a branch onto the current one
* Save stash
* Pop stash

#### Remote
* Pull => WIP
* Push => WIP

### Server side
* WIP
