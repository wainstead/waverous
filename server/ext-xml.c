/***********************************************************************

This file (ext-xml.c) was originally obtained from this URL:
http://www.xythian.com/moo/expat-binding/

The copyright notice for this file is given by the URL:
http://www.xythian.com/moo/expat-binding/docs/ext-xml.README

and it reads:


  This is a binding of expat to MOO providing two functions for
  parsing XML.
  
  It was written by Ken Fox <fox@mars.org>.
  
  This patch's "home page" (for now) is
     http://www.xythian.com/moo/expat-binding/
  
  expat is an XML Parser Toolkit available at 
     http://www.jclark.com/xml/expat.html
  
  The binding is covered under the following license:
  
  --- 
  
  
  Copyright 2000 by Ken Fox.
  
  Permission to use, copy, modify, and distribute this software and
  its documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and
  that both that copyright notice and this permission notice appear in
  supporting documentation.
  
  KEN FOX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
  NO EVENT SHALL KEN FOX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
  OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
  NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
  WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  
***********************************************************************/

/*
 * XML for the MOO Server using the expat library
 */

#include "config.h"

#ifdef HAVE_EXPAT_H

#include "bf_register.h"
#include "functions.h"
#include "db_tune.h"
#include "storage.h"
#include "list.h"
#include "streams.h"
#include "utils.h"

#include "exceptions.h"
#include "tasks.h"

#include <expat.h>


/*
 * quick'n'dirty
 * <foo a="1">
 *   <bar>11</bar>
 * </foo> 
 * =
 * {"foo", {{"a", "1"}}, {{"bar", {}, {"11"}}}}
 */

#define NS_DELIMITER '\t'

typedef struct XMLdata XMLdata;

struct XMLdata {
    XMLdata *parent;
    Stream *body;
    Var element;
};

static XMLdata *
new_node(XMLdata * parent, const char *name)
{
    /* TODO: may want a suballocator here; gonna be needing a lot of
     * these 2-ptr + 1 var nodes
     */
    XMLdata *node;
    Var element;

    char *nametemp = NULL;
    const char *nodename = NULL;
    char *nsname = NULL;
    char *delim = NULL;

    if ((delim = strchr((char *) name, NS_DELIMITER)) != NULL) {
	int index = delim - name;
	nametemp = str_dup(name);
	nametemp[index] = '\0';
	nodename = nametemp + index + 1;
	nsname = nametemp;
    } else {
	nodename = name;
    }

    node = (XMLdata *) mymalloc(1 * sizeof(XMLdata), M_XML_DATA);
    element = new_list(nsname ? 5 : 4);

    /* {name, attribs, body, children} */
    element.v.list[1].type = TYPE_STR;
    element.v.list[1].v.str = str_dup(nodename);
    element.v.list[2] = new_list(0);
    element.v.list[3].type = TYPE_INT;
    element.v.list[3].v.num = 0;
    element.v.list[4] = new_list(0);
    if (nsname) {
	element.v.list[5].type = TYPE_STR;
	element.v.list[5].v.str = str_dup(nsname);
    }
    node->body = NULL;
    node->element = element;
    node->parent = parent;

    if (nametemp)
	free_str(nametemp);

    return node;
}

static void
finish_node(XMLdata * data)
{
    XMLdata *parent = data->parent;
    Var element = data->element;
    Var body;
    Stream *s = data->body;
    body.type = TYPE_STR;
    if (s == NULL) {
	body.v.str = str_dup("");
    } else {
	body.v.str = str_dup(reset_stream(s));
    }
    element.v.list[3] = body;
    if (parent != NULL) {
	Var pelement = parent->element;
	pelement.v.list[4] =
	    listappend(pelement.v.list[4], var_ref(element));
    }
}

static void
free_node(XMLdata * data)
{
    free_var(data->element);
    if (data->body != NULL)
	free_stream(data->body);
    myfree(data, M_XML_DATA);
}

static void
flush_nodes(XMLdata * bottom)
{
    XMLdata *parent = bottom->parent;
    free_node(bottom);
    if (parent != NULL) {
	flush_nodes(parent);
    }
}

static void
xml_startElement(void *userData, const char *name, const char **atts)
{
    XMLdata **data = (XMLdata **) userData;
    XMLdata *parent = *data;

    XMLdata *node = new_node(parent, name);
    const char **patts = atts;

    char *delim;

    while (*patts != NULL) {
	Var pair;
	if ((delim = (char *) strchr(patts[0], NS_DELIMITER))) {
	    int index = delim - patts[0];
	    char *nametemp = str_dup(patts[0]);
	    nametemp[index] = '\0';

	    pair = new_list(3);
	    pair.v.list[3].type = TYPE_STR;
	    pair.v.list[3].v.str = str_dup(nametemp);
	    pair.v.list[1].type = TYPE_STR;
	    pair.v.list[1].v.str = str_dup(nametemp + index + 1);
	    if (nametemp)
		free_str(nametemp);
	} else {
	    pair = new_list(2);
	    pair.v.list[1].type = TYPE_STR;
	    pair.v.list[1].v.str = str_dup(patts[0]);
	}
	pair.v.list[2].type = TYPE_STR;
	pair.v.list[2].v.str = str_dup(patts[1]);
	patts += 2;
	node->element.v.list[2] =
	    listappend(node->element.v.list[2], pair);
    }

    *data = node;
}

static void
xml_characterDataHandler(void *userData, const XML_Char * s, int len)
{
    XMLdata **data = (XMLdata **) userData;
    XMLdata *node = *data;
    Stream *sp = node->body;

    if (sp == NULL) {
	node->body = new_stream(len);
	sp = node->body;
    }

    stream_add_string(sp, raw_bytes_to_binary(s, len));
}

static void
xml_streamCharacterDataHandler(void *userData, const XML_Char * s, int len)
{
    XMLdata **data = (XMLdata **) userData;
    XMLdata *node = *data;
    Var element = node->element;
    Var v;
    v.type = TYPE_STR;
    v.v.str = str_dup(raw_bytes_to_binary(s, len));
    element.v.list[4] = listappend(element.v.list[4], v);
}


static void
xml_endElement(void *userData, const char *name)
{
    XMLdata **data = (XMLdata **) userData;
    XMLdata *node = *data;
    XMLdata *parent = node->parent;
    finish_node(node);
    free_node(node);
    *data = parent;
}

/**
 * Parse an XML string into a nested list.
 * The second parameter indicates if body text (text within XML tags)
 * should show up among the children of the tag or in its own
 * section.
 *
 * See documentation (ext-xml.README) for examples.
 */
static package
parse_xml(const char *data, int bool_stream, int bool_parsens)
{
    /*
     * FIXME: Feed expat smaller chunks of the string and 
     * check for task timeout between chunks
     *
     */
    int decoded_length;
    const char *decoded;
    package result;
    XML_Parser parser;
    XMLdata *root = new_node(NULL, "");
    XMLdata *child = root;

    if (bool_parsens) {
	parser = XML_ParserCreateNS(NULL, NS_DELIMITER);
    } else {
	parser = XML_ParserCreate(NULL);
    }

    decoded_length = strlen(data);
    decoded = data;
    XML_SetUserData(parser, &child);
    XML_SetElementHandler(parser, xml_startElement, xml_endElement);
    if (bool_stream) {
	XML_SetCharacterDataHandler(parser,
				    xml_streamCharacterDataHandler);
    } else {
	XML_SetCharacterDataHandler(parser, xml_characterDataHandler);
    }
    if (!XML_Parse(parser, decoded, decoded_length, 1)) {
	Var r;
	r.type = TYPE_INT;
	r.v.num = XML_GetCurrentByteIndex(parser);
	flush_nodes(child);
	result = make_raise_pack(E_INVARG,
				 XML_ErrorString(XML_GetErrorCode(parser)),
				 r);
    } else {
	finish_node(root);
	result = make_var_pack(var_ref(root->element.v.list[4].v.list[1]));
	free_node(root);
    }
    XML_ParserFree(parser);
    return result;
}


static package
bf_parse_xml_document(Var arglist, Byte next, void *vdata, Objid progr)
{
    int bool_parsens = (arglist.v.list[0].v.num >= 2
			&& is_true(arglist.v.list[2]));
    package result = parse_xml(arglist.v.list[1].v.str, 1, bool_parsens);
    free_var(arglist);
    return result;
}

static package
bf_parse_xml_tree(Var arglist, Byte next, void *vdata, Objid progr)
{
    int bool_parsens = (arglist.v.list[0].v.num >= 2
			&& is_true(arglist.v.list[2]));
    package result = parse_xml(arglist.v.list[1].v.str, 0, bool_parsens);
    free_var(arglist);
    return result;
}

void
register_xml()
{
    register_function("xml_parse_tree", 1, 2, bf_parse_xml_tree, TYPE_STR,
		      TYPE_ANY);
    register_function("xml_parse_document", 1, 2, bf_parse_xml_document,
		      TYPE_STR, TYPE_ANY);
}


#endif /* HAVE_EXPAT_H */
