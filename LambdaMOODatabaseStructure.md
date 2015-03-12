The database file is a flat text file delimited by newlines.

A complete description can be found here:
http://www.mars.org/home/rob/docs/lmdb.html

You can find a copy of the LambdaCore database file in the directory "databases" in the Waverous project.

Also, in the project directory waverous/server/tools, there is a Python script called `moo2xml.py` that will translate a LambdaMOO database file to XML. The script was written by Neil Fraser. The output is roughly three times larger than the native LambdaMOO database file format.