# qonche 
A minimal quake-like console in a single C header with no dependencies.

This lib PROVIDES: 
* editing:
   - cursor movement
   - erase characters at the cursor
   - insert string at the cursor

* printing:
   - add string to the pager 
   - paging 

This lib DOES NOT PROVIDE:
* utf-8 / multibyte support
* advanced cursor movement i.e. between words
* commands parsing, autocompletion, history, etc.
* printf-like formatting

### Usage

* Define `QONCHE_IMPLEMENTATION` above the include in the c/c++ file where you want the implementation to reside.
* Define the `QON_DrawChar` macro with your own implementation of `QON_DrawChar()`.
* Invoke `QON_Draw()` which will eventually call back `QON_DrawChar()`.
* Wire up the input routines i.e. to the matching key events in your code.
