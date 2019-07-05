# Feedback

## Cyril Soldani

### Project 3

* The `makefile` is dumb : it compiles all files at once.
* Triangle named points aren't correctly positioned.
* Include guard macros should be more specific to avoid potentential collisions. The format `MYFIlE_H` is pretty standard.
* Style is sometimes inconsistent which makes it harder to follow. Try to stay with uppercase or lowercase variables and to use explicit names. Avoid especially one-letter variables.
* The file decompostion is confusing and doesn't allow modularity. Source files (`cpp` & `hpp`) should serve one purpose only and should be as independant as possible (See the [single responsibility principle](https://en.wikipedia.org/wiki/Single_responsibility_principle)).