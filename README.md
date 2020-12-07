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
