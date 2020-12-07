# qonche 
A tiny quake-like console written in C with no dependencies.

This lib implements a stripped-down quake-like console for real-time apps (i.e. games): 
* editing:
   - cursor movement
   - erase characters at the cursor
   - insert string at the cursor

* printing:
   - add string to the pager 
   - paging 

This lib doesn't provide:
* utf-8 / multibyte support
* advanced cursor movement i.e. between words
* commands parsing, autocompletion, history, etc.
* printf-like formatting
