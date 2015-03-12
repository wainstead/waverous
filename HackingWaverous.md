### Hacking on the Waverous server code ###

Waverous LambdaMOO is a fork of the original LambdaMOO server code, written in C in the 1990s. The last formal release of the LambdaMOO server was version 1.8.1 in the year 2000.

Waverous has been modified to compile under g++, the GNU C++ compiler; as of this writing I don't know if it compiles with any other compilers.

In <a href='http://www.aristeia.com/books.html'><i>Effective C++</i></a>, Scott Meyers describes C++ as a federation of four languages:

  1. **C**
  1. **Object-Oriented C++**: "Classes (including constructors and destructors), encapsulation, inheritance, polymorphism, virtual functions (dynamic binding), etc."
  1. **Template C++**: "This is the generic programming part of C++, the one that most programmers have the least experience with."
  1. **The STL**: the Standard Template Library

For this project I am focusing on only items one and two. The goal is to introduce some OO into the server code but not to rewrite the entire project. If, at some distant date, there is a strong case to be made for using templates and the STL I'll be glad to have that debate. But for now it's enough to gradually introduce classes, new() and delete().



### Indentation Style ###

I've tried to stick with the original code format used by the LambdaMOO maintainers; that being K&R, with four spaces indentation. For emacs you can use this:

```
(setq c-default-style
      '((c-mode . "k&r")))
(setq c-basic-offset 4)
```

### Style Guide ###

Google's C++ Style Guide is the one I am following:

http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

with the notable exception of indentation (Google recommends two spaces, the server code uses four).

See also: DebuggingWaverous