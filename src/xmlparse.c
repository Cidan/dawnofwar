#include "ack.h"
#include "config.h"
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

xmlDocPtr readXMLFile(const char *filename) {
    xmlDocPtr doc; /* the resulting document tree */
	char buf[MSL];

	sprintf(buf, "%s/%s", QUEST_DIR, filename);

    doc = xmlReadFile(buf, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", filename);
	return NULL;
    }
    return doc;
//    xmlFreeDoc(doc);
}

void nodes_to_char(xmlNode * a_node, CHAR_DATA *ch, int depth)
{
    xmlNode *cur_node = NULL;
	char buf[MSL];
	int i = 0;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            sprintf(buf, "{y<{c%s{y>{x\n\r", cur_node->name);
			send_to_char(buf, ch);
			//while (i++ != depth || i == 20)
				send_to_char("   ", ch);

			send_to_char(cur_node->children->content, ch);
			send_to_char("\n\r",ch);
        }

        nodes_to_char(cur_node->children, ch, ++depth);
		depth--;
		if (cur_node->type == XML_ELEMENT_NODE)
		{
		sprintf(buf, "{y<{c/%s{y>{x\n\r", cur_node->name);
		send_to_char(buf, ch);
		}

    }
}
