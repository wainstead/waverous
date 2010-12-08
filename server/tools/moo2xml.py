#!/usr/bin/python
#
# Moo Database to XML Converter 1.1
# Copyright (C) 2005 Neil Fraser, Scotland
# http://neil.fraser.name/software/moobrowser/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of version 2 of the GNU General Public License as
# published by the Free Software Foundation.  This program is
# distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.  http://www.gnu.org/

import sys
import os

# Standard list of error types.
Errors = 'E_NONE', 'E_TYPE', 'E_DIV', 'E_PERM', 'E_PROPNF', 'E_VERBNF', 'E_VARNF', 'E_INVIND', 'E_RECMOVE', 'E_MAXREC', 'E_RANGE', 'E_ARGS', 'E_NACC', 'E_INVARG', 'E_QUOTA', 'E_FLOAT'

# -2 is 'any', -1 is 'none', then the list is as defined in db_verbs.c
Prepositions = 'any', 'none', 'with/using', 'at/to', 'in front of', 'in/inside/into', 'on top of/on/onto/upon', 'out of/from inside/from', 'over', 'through', 'under/underneath/beneath', 'behind', 'beside', 'for/about', 'is', 'as', 'off/off of'

# Global repository of all objects.
ObjectDictionary = {}

# Current indentation level of XML.
Indent = 0

# MooObj is a class which contains all the data pertaining to one Moo Object.
class MooObj:

  # Initialise the object as blank, but with an object number.
  def __init__ (self, number):
    self.number = number
    self.flags = 0
    self.parent = -1
    self.owner = -1
    self.location = -1
    self.name = ''
    self.children = []
    self.properties = []
    self.verbs = []

  # Return this object's flags as a string, e.g. 'r programmer player'
  def flags2str(self):
    flags = ''
    objflags = self.flags
    if objflags >= 128:
      objflags -= 128
      flags += 'f '
    if objflags >= 64:
      objflags -= 64
      # obsolete
    if objflags >= 32:
      objflags -= 32
      flags += 'w '
    if objflags >= 16:
      objflags -= 16
      flags += 'r '
    if objflags >= 8:
      objflags -= 8
      # obsolete
    if objflags >= 4:
      objflags -= 4
      flags += 'wizard '
    if objflags >= 2:
      objflags -= 2
      flags += 'programmer '
    if objflags >= 1:
      objflags -= 1
      flags += 'player '
    if objflags != 0:
      # A remainder means unknown extensions have been used.
      flags = '???'
    return flags.rstrip()


# Return a property's flags as a string, e.g. 'r c'
def decodePropFlags(propperms):
  propflags = ''
  if propperms >= 4:
    propperms -= 4
    propflags += 'c '
  if propperms >= 2:
    propperms -= 2
    propflags += 'w '
  if propperms >= 1:
    propperms -= 1
    propflags += 'r '
  if propperms != 0:
    # A remainder means unknown extensions have been used.
    propflags = '???'
  return propflags.rstrip()


# Return a verb's flags, direct object, proposition and indirect object.
# e.g. ('r x', 'none', 'for/about' 'this')
def decodeVerbFlagsDobjPrepIobj(verbperms, verbprep):
  verbflags = ''
  verbdobj = 'none'
  verbiobj = 'none'
  if verbperms >= 1024:
    verbperms -= 1024
    verbflags += 'o ' # non-standard extension
  if verbperms >= 512:
    verbperms -= 512 # unused
  if verbperms >= 256:
    verbperms -= 256 # unused
  if verbperms >= 128:
    verbperms -= 128
    verbiobj = 'this'
  if verbperms >= 64:
    verbperms -= 64
    verbiobj = 'any'
  if verbperms >= 32:
    verbperms -= 32
    verbdobj = 'this'
  if verbperms >= 16:
    verbperms -= 16
    verbdobj = 'any'
  if verbperms >= 8:
    verbperms -= 8
    verbflags += 'd '
  if verbperms >= 4:
    verbperms -= 4
    verbflags += 'x '
  if verbperms >= 2:
    verbperms -= 2
    verbflags += 'w '
  if verbperms >= 1:
    verbperms -= 1
    verbflags += 'r '
  return (verbflags.rstrip(), verbdobj, Prepositions[verbprep+2], verbiobj)


# Read a line from the database file
def readstr():
  return file.readline().rstrip("\r\n")


# Read an integer from the database file
def readint():
  return int(readstr())


# Read a Moo value from the database file
def readvalue():
  type = readint() # e.g. 1
  if type == 0: # integer
    value = readstr() # e.g. 123
  elif type == 1: # object
    value = "#"+readstr() # e.g. 5
  elif type == 2: # string
    value = readstr() # e.g. Hello!
    value = value.replace('\\', '\\\\')
    value = value.replace('"', '\\\"')
    value = '"'+value+'"'
  elif type == 3: # error
    value = readint() # e.g. 10
    value = Errors[value]
  elif type == 4: # list
    elements = readint() # e.g. 2
    value = ""
    while elements > 0:
      if value != "":
        value += ", "
      value += readvalue()
      elements = elements - 1
    value = "{"+value+"}"
  elif type == 5: # clear
    value = None
  elif type == 9: # float
    value = readstr() # e.g. 2.5
  return value


# Read all the lines associated with an object
def readobject():
  objnumber = readstr() # e.g. #5
  objnumber.index('#')  # Blow up if we're out of sync
  if objnumber.find('recycled') != -1:
    return
  objnumber = int(objnumber[1:])
  object = MooObj(objnumber)

  object.name   = readstr() # e.g. generic thing
  dummy         = readstr() # blank line
  object.flags  = readint() # e.g. 152
  object.owner  = readint() # e.g. 2
  object.location = readint() # e.g. -1
  objcontents1  = readint() # e.g. -1 (not used here)
  objneighbour  = readint() # e.g. -1 (not used here)
  object.parent = readint() # e.g. 1
  objchild1     = readint() # e.g. 8  (not used here)
  objsibling    = readint() # e.g. 6  (not used here)

  # Verb Definitions
  objverbs = readint() # e.g. 6
  while objverbs > 0:
    verbname  = readstr() # e.g. g*et t*ake
    verbowner = readint() # e.g. 2
    verbperms = readint() # e.g. 45
    verbprep  = readint() # e.g. -1
    object.verbs.append([verbname, verbowner, verbperms, verbprep, ''])
    objverbs = objverbs - 1

  # Property Names
  propnames = []
  objpropnames = readint() # e.g. 4
  while objpropnames > 0:
    propname = readstr() # e.g. drop_failed_msg
    propnames.append(propname)
    objpropnames = objpropnames - 1

  # Property Definitions
  objprops = readint() # e.g. 12
  while objprops > 0:
    propvalue = readvalue()
    propowner = readint() # e.g. 2
    propperms = readint() # e.g. 5
    if propnames:
      object.properties.append([propnames.pop(0), propowner, propperms, propvalue])
    else:
      object.properties.append([None, propowner, propperms, propvalue])
    objprops = objprops - 1

  # Add this object to the dictionary
  ObjectDictionary[objnumber] = object


# Read all the lines associated with a verb
def readverb():
  verblocation = readstr() # e.g. #5:0
  colon = verblocation.index(':')
  objnum = int(verblocation[1:colon])
  verbnum = int(verblocation[colon+1:])
  code = ''
  lastline = readstr()
  while lastline != '.':
    code += lastline + '\n'
    lastline = readstr()
  ObjectDictionary[objnum].verbs[verbnum][4] = code


# Recursively walk through the dictionary, completing the property definitions.
def walkProperties(objnum):
  object = ObjectDictionary[objnum]
  offset = 0
  for i in range(len(object.properties)):
    if object.properties[i][0] == None:
      object.properties[i][0] = ObjectDictionary[object.parent].properties[i-offset][0]
    else:
      offset += 1
  for childnum in object.children:
    walkProperties(childnum)


# Recursively walk through the dictionary, printing out the XML.
def walkDictionary(objnum):
  object = ObjectDictionary[objnum]
  if object.number != -1:
    flags = object.flags2str()
    if flags:
      flags = 'FLAGS="%s"' % flags
    xml = '<OBJECT ID="obj%i" %s OWNER="obj%i" LOCATION="obj%i">\n' % (object.number, flags, object.owner, object.location)
    xml += '<NAME>'+escapexml(object.name)+'</NAME>\n'
    writexml(xml)
    for (propname, propowner, propperms, propvalue) in object.properties:
      perms = decodePropFlags(propperms)
      if perms:
        perms = 'PERMS="%s"' % perms
      xml = '<PROPERTY OWNER="obj%i" %s>\n' % (propowner, perms)
      xml += '<NAME>'+escapexml(propname)+'</NAME>\n'
      if propvalue:
        xml += '<VALUE>'+escapexml(propvalue)+'</VALUE>\n'
      # Clear properties don't have a value.
      xml += '</PROPERTY>\n'
      writexml(xml)
    for (verbname, verbowner, verbperms, verbprep, verbcode) in object.verbs:
      (verbflags, verbdobj, verbprep, verbiobj) = decodeVerbFlagsDobjPrepIobj(verbperms, verbprep)
      if verbflags:
        verbflags = 'PERMS="%s"' % verbflags
      xml = '<VERB OWNER="obj%i" %s DOBJ="%s" PREP="%s" IOBJ="%s">\n' % (verbowner, verbflags, verbdobj, verbprep, verbiobj)
      xml += '<NAME>'+escapexml(verbname)+'</NAME>\n'
      xml += '<CODE>\n'+escapexml(verbcode)+'</CODE>\n'
      xml += '</VERB>\n'
      writexml(xml)
  for childnum in object.children:
    walkDictionary(childnum)
  if object.number != -1:
    writexml('</OBJECT>')


# Write to standard output, with XML indentation.
def writexml(str):
  global Indent
  for line in str.splitlines():
    if line.startswith('</'):
      Indent = Indent - 1
    spaces = "	".expandtabs(Indent*2) # Must be a better way...
    print spaces + line
    if line.startswith('<') and -1 == line.find('</') and -1 == line.find('<?') and -1 == line.find('<!'):
      Indent = Indent + 1


# Escape a string for use with XML
def escapexml(str):
  str = str.replace("&", "&amp;")
  str = str.replace("<", "&lt;")
  str = str.replace(">", "&gt;")
  return str


# Obtain the database's filename from the command line argument
if len(sys.argv) != 2:
  print >> sys.stderr, "Usage:  "+sys.argv[0]+" <DATABASE>"
  sys.exit(1)
filename = sys.argv[1]

# Open the database file
try:
  file=open(filename, 'r')
except IOError, err:
  print >> sys.stderr, str(err)
  sys.exit(1)

# Intro Block
versionstring = readstr() # e.g. ** LambdaMOO Database, Format Version 4 **
totalobjects  = readint() # e.g. 95
totalverbs    = readint() # e.g. 1698
dummy         = readstr() # e.g. 0
totalplayers  = readint() # e.g. 4

# Player Block
while totalplayers > 0:
  dummy = readstr() # e.g. 71
  totalplayers = totalplayers - 1

# Object Block
while totalobjects > 0:
  readobject()
  totalobjects = totalobjects - 1

# Crosslink objects to their children
ObjectDictionary[-1] = (MooObj(-1))
for object in ObjectDictionary.keys():
  parent = ObjectDictionary[object].parent
  ObjectDictionary[parent].children.append(object)
ObjectDictionary[-1].children.remove(-1)

# Look up and link inherited property names
walkProperties(-1)

# Verb Block
while totalverbs > 0:
  readverb()
  totalverbs = totalverbs - 1

# Finished reading the database file
file.close()

# Output the XML
writexml('<?xml version="1.0"?>')
#writexml('<!DOCTYPE MOO PUBLIC "MOO 1.0//EN" "http://neil.fraser.name/software/moobrowser/moo.dtd">')
writexml("""
<!DOCTYPE MOO [
 
<!ELEMENT MOO (OBJECT*)>
<!ELEMENT OBJECT (NAME, PROPERTY*, VERB*, OBJECT*)>
<!ELEMENT VERB (NAME, CODE)>
<!ELEMENT PROPERTY (NAME, VALUE?)>
<!ELEMENT CODE (#PCDATA)>
<!ELEMENT VALUE (#PCDATA)>
<!ELEMENT NAME (#PCDATA)>

<!ATTLIST OBJECT
  ID ID #REQUIRED
  FLAGS NMTOKENS #IMPLIED
  OWNER CDATA #REQUIRED
  LOCATION CDATA #REQUIRED
>
<!ATTLIST VERB
  OWNER CDATA #REQUIRED
  PERMS NMTOKENS #IMPLIED
  DOBJ CDATA #REQUIRED
  IOBJ CDATA #REQUIRED
  PREP CDATA #REQUIRED
>
<!ATTLIST PROPERTY
  OWNER CDATA #REQUIRED
  PERMS NMTOKENS #IMPLIED
>

]>
""")
writexml('<!-- %s -->' % versionstring)
writexml('<MOO>')
walkDictionary(-1)
writexml('</MOO>')
