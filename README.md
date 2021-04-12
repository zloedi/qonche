# qonche 
A minimal quake style debug console for games in a single C header with no dependencies. Try the live demo here: https://zloedi.github.io/qonche/

This lib PROVIDES: 
* editing:
   - cursor movement
   - erase characters at the cursor
   - insert string at the cursor

* printing:
   - add string to the pager 
   - paging 

This lib DOES NOT PROVIDE:
* (font) rendering of any kind
* utf-8 / multibyte support
* advanced cursor movement i.e. between words
* commands parsing, autocompletion, history, etc.
* printf-like formatting
* multithreading support

### Usage

* Invoke `QON_Draw()` somewhere in your game/render loop, which will eventually call back your version of `QON_DrawChar()`.
* Wire up the input routines i.e. to the matching key events in your code.
* See [qonche.h](https://github.com/zloedi/qonche/blob/main/qonche.h) for details.

### Example usage with embedded GUI
![Sample image](https://github.com/zloedi/qonche/blob/main/qonche.gif)
