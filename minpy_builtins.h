#ifndef h_minpy_builtins
#define h_minpy_builtins

#define IMPORT_THIS_MESSAGE "The Zen of Python, by Tim Peters\n\nBeautiful is better than ugly.\nExplicit is better than implicit.\nSimple is better than complex.\nComplex is better than complicated.\nFlat is better than nested.\nSparse is better than dense.\nReadability counts.\nSpecial cases aren't special enough to break the rules.\nAlthough practicality beats purity.\nErrors should never pass silently.\nUnless explicitly silenced.\nIn the face of ambiguity, refuse the temptation to guess.\nThere should be one-- and preferably only one --obvious way to do it.\nAlthough that way may not be obvious at first unless you're Dutch.\nNow is better than never.\nAlthough never is often better than *right* now.\nIf the implementation is hard to explain, it's a bad idea.\nIf the implementation is easy to explain, it may be a good idea.\nNamespaces are one honking great idea -- let's do more of those!"

#define HELP_MESSAGE "This calculator console tries its best to follow Python 3.X expression evaluation semantics. If there is only an expression by itself, it will be printed to the console. If there is an assignment statement, there will not be a console print.\n\nThis isn't really a Python 3.X interpreter, so to actually learn Python, check out the tutorial at https://docs.python.org/3.12/tutorial/.\n\nType 'quit()' to exit the console."

#define USAGE_MESSAGE "usage: calc [<filename>]\n\nDon't give a filename to enter console mode. Give a filename to interpret a file."

#define FILE_NOT_FOUND_MESSAGE(filename) "FileNotFoundError: Can't open file \"" << filename << "\""

#define CONSOLE_PROMPT ">>> "

#endif