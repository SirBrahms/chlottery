# chlottery
This simple program is a useful utility for randomising file permissions, perfect
for those who would like a little more excitement in their boring, 755-filled life.

## Options
Chlottery has the following options:
* `-R`: Recurses into a given directory
* `-s`: Also sets the special bits
* `-v`: Increases output verbosity
* `-V`: Prints the current version and exits
* `-h`: Prints a (very) short usage summary and exits

-R, -s and -v can be combined, but all are optional. Chlottery also **expects** a
positional argument, namely the path to the file or directory that should have
its permissions changed.
