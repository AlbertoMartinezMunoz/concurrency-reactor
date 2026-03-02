# concurrency-reactor
Reactor. An Event Handling Pattern for Concurrent Programming

## Setup

### Google Test

Google test can be added to your git repo as a git submodule.

```sh
git submodule add https://github.com/google/googletest.git ./vendor/gtest
```

Also if we are using C++20 in our project, we have to ensure that C++20 is used adding the following line in the root CMakeList file

```cmake
set(CMAKE_CXX_STANDARD 20)
```

### Fake Function Framework (fff)

FFF can be added to your git repo as a git submodule.

```sh
git submodule add https://github.com/meekrosoft/fff.git ./vendor/fff
```

### Valgrind

#### Installation

```sh
sudo apt-get update
sudo apt-get -y install valgrind
```

### CppCheck

Cppcheck is a command-line tool that tries to detect bugs that your C/C++ compiler doesn’t see. It is versatile, and can check non-standard code including various compiler extensions, inline assembly code, etc. Its internal preprocessor can handle includes, macros, and several preprocessor commands. While Cppcheck is highly configurable, you can start using it just by giving it a path to the source code.

#### Installation

```console
sudo apt-get update
sudo apt-get -y install cppcheck
```

#### Launch

```console
mkdir buildcppcheck
rm ./buildcppcheck/*.a1
rm ./buildcppcheck/*.s1
cppcheck --cppcheck-build-dir=buildcppcheck --std=c++20 --language=c++ --xml --error-exitcode=2 --enable=style --output-file=./buildcppcheck/basic_report.xml --file-list=cppcheck-input.txt
rm ./buildcppcheck/*.a1
rm ./buildcppcheck/*.s1
cppcheck --cppcheck-build-dir=buildcppcheck --std=c++20 --language=c++ --xml --error-exitcode=2 --enable=style --output-file=./buildcppcheck/bug_h_report.xml --file-list=cppcheck-input.txt --bug-hunting
cppcheck-htmlreport --source-dir=. --report-dir=./buildcppcheck/basic --file=./buildcppcheck/basic_report.xml --title=BASIC
cppcheck-htmlreport --source-dir=. --report-dir=./buildcppcheck/bug --file=./buildcppcheck/bug_h_report.xml --title=BUG_HUNTING
```

With the following options for cppcheck:

- '--bug-hunting' Enable noisy and soundy analysis. The normal Cppcheck analysis is turned off.
- '--cppcheck-build-dir=<dir>' Cppcheck work folder.
- '--language=<language>' Forces cppcheck to check all files as the given languaje. Valid values are: c, c++
- '--std=<id>' Set standard.
- '--error-exitcode=<n>' If errors are found, integer [n] is returned instead of the default '0'. '1' is returned if arguments are not valid or if no input files are provided. Note that your operating system can modify this value, e.g. '256' can become '0'.
- '--xml' Write results in xml format to error stream (stderr).
- '--enable=<id>' Enable additional checks. The available ids are:
    * all Enable all checks. It is recommended to only use --enable=all when the whole program is scanned, because this enables unusedFunction.
    * warning Enable warning messages
    * style Enable all coding style checks. All messages with the severities 'style', 'warning', 'performance' and 'portability' are enabled.
    * performance Enable performance messages
    * portability Enable portability messages
    * information Enable information messages
    * unusedFunction Check for unused functions. It is recommended to only enable this when the whole program is scanned.
    * missingInclude Warn if there are missing includes. For detailed information, use '--check-config'.
    Several ids can be given if you separate them with commas. See also --std
- '--output-file=<file>' Write results to file, rather than standard error.
- '--file-list=<file>' Specify the files to check in a text file. Add one filename per line. When file is '-,' the file list will be read from standard input.

### clang-format

The `clang-format` tool will be used to format the c++ files. To install clang-format:

```console
sudo apt install clang-format
```

There some default styles when formatting, but you can modify them using the `.clang-format` configuration file and the option `-style=file`

It will be used to format the c++ files before commiting them to the repo. To manually run it:

```console
find ./src -iname '*.h' -o -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i -style=file
```

### pre-commit

Git hook scripts are useful for identifying simple issues before submission to code review. We run our hooks on every commit to automatically point out issues in code such as missing semicolons, trailing whitespace, and debug statements. By pointing these issues out before code review, this allows a code reviewer to focus on the architecture of a change while not wasting time with trivial style nitpicks.

The whole documentation in on the website [pre-commit](https://pre-commit.com/)

#### Installation

Before you can run hooks, you need to have the pre-commit package manager installed.

Using pip:

```console
pip install pre-commit
```

#### Setup

1. Add a pre-commit configuration

> create a file named .pre-commit-config.yaml you can generate a very basic configuration using `pre-commit sample-config > .pre-commit-config.yaml`

3. Run against all the files

> Before installing, it's usually a good idea to run the hooks against all of the files when adding new hooks

3. Install the git hook scripts

> run `pre-commit install` to set up the git hook scripts

To run the hooks manually

```console
pre-commit run --all-files
```

## Build

### Release

To build the release, first we launch CMake and then make.

```console
cmake . -H. -Bbuild
cd build
make -j3
```

All in one line:

```console
cmake . -H. -Bbuild; cd build; make -j3; cd ..
```

### Unitary Testing

The unit tests will be launched using **ctest**

To build the unitary tests, first we launch CMake with the UTEST custom option and then make.

```console
cmake . -DUTEST=ON -H. -Bbuildtest
cd buildtest
make -j3
```

To launch **all** the tests:

```console
ctest -VV;
```

To launch only a testsuite:

```console
ctest -R "testsuite_name" -VV
```

To use **valgrind** for checking memory leaks:

```console
ctest -T memcheck -VV
```

All in one line:

```console
cmake . -DUTEST=ON -H. -Bbuildtest; cd buildtest; make -j3 && ctest -T memcheck -VV; cd ..
```

### Documentation Diagrams

The C4 diagrams are made using [PlantUML](https://plantuml.com/). First, you have to install the `plantuml` package:

```console
sudo apt-get update
sudo apt-get install plantuml
```

Then, to build the diagrams, you only have to run the `plantuml` command:

```console
plantuml -tsvg <path-to-the-files>
```
