/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Ack 2.2 improvements copyright (C) 1994 by Stephen Dooley              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *       _/          _/_/_/     _/    _/     _/    ACK! MUD is modified    *
 *      _/_/        _/          _/  _/       _/    Merc2.0/2.1/2.2 code    *
 *     _/  _/      _/           _/_/         _/    (c)Stephen Zepp 1998    *
 *    _/_/_/_/      _/          _/  _/             Version #: 4.3          *
 *   _/      _/      _/_/_/     _/    _/     _/                            *
 *                                                                         *
 *                        http://ackmud.nuc.net/                           *
 *                        zenithar@ackmud.nuc.net                          *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/* This code inspired by a snippet from :                                  */

/************************************************************************/
/* mlkesl@stthomas.edu	=====>	Ascii Automapper utility                */
/* Let me know if you use this. Give a newbie some _credit_,            */
/* at least I'm not asking how to add classes...                        */
/* Also, if you fix something could ya send me mail about, thanks       */
/* PLEASE mail me if you use this or like it, that way I will keep it up*/
/************************************************************************/

#include <ctype.h>  /* for isalpha */
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
#include <time.h> 
#include "mapper.h"


char *map[MAX_MAP][MAX_MAP];
int offsets[4][2] ={ {-1, 0},{ 0, 1},{ 1, 0},{ 0,-1} };

void MapArea
(ROOM_INDEX_DATA *room, CHAR_DATA *ch, int x, int y, int min, int max){
ROOM_INDEX_DATA *prospect_room;EXIT_DATA *pexit;int door;

/* marks the room as visited */
switch (room->sector_type){
case SECT_CITY:		map[x][y]="{W+{x";		break;
case SECT_FIELD:		map[x][y]="{G.{x";		break;
case SECT_FOREST:		map[x][y]="{g@";		break;
case SECT_HILLS:		map[x][y]="{g^{x";		break;
case SECT_MOUNTAIN:	map[x][y]="{y^{x";		break;
case SECT_WATER_SWIM:	map[x][y]="{b~{x";		break;
case SECT_WATER_NOSWIM:	map[x][y]="{B~{x";		break;
case SECT_AIR:		map[x][y]="{c.{x";		break;
case SECT_DESERT:		map[x][y]="{Y.{x";		break;
case SECT_INSIDE:	map[x][y]="{W#{x";		break;
case SECT_ROAD:         map[x][y]="{Y+{x";          break;
case SECT_INN:          map[x][y]="{d+{x";          break;
case SECT_STORE:        map[x][y]="{Y#{x";          break;
case SECT_JUNGLE:	map[x][y]="{gY";		break;
case SECT_BEACH:	map[x][y]="{Y=";		break;
case SECT_SWAMP:	map[x][y]="{g~";		break;
case SECT_TUNDRA:	map[x][y]="{wT";		break;
case SECT_WASTELAND:	map[x][y]="{WW";		break;
case SECT_DENSE_FOREST:	map[x][y]="{GF";		break;
case SECT_LAVA:	map[x][y]="{R=";		break;
case SECT_ROCK_MOUNTAIN:	map[x][y]="{d^";		break;
case SECT_SNOW_MOUNTAIN:	map[x][y]="{W^";		break;
case SECT_BOG:	map[x][y]="{GB";		break;
case SECT_RUINS:		map[x][y]="{R#";		break;
case SECT_UNDERGROUND:		map[x][y]="{w#";		break;
case SECT_HOUSE:		map[x][y]="{rH";		break;
case SECT_PYRAMID:		map[x][y]="{Y^";		break;
case SECT_OCEAN:	map[x][y]="{BO";		break;
case SECT_STREET:	map[x][y]="{w+";		break;
case SECT_ALLEY:		map[x][y]="{d+";		break;
case SECT_CAVE:	map[x][y]="{dC";		break;
case SECT_FARMLAND:	map[x][y]="{y:";		break;
case SECT_CAVERN:	map[x][y]="{dO";		break;
case SECT_BRIDGE:	map[x][y]="{y=";		break;
case SECT_ICE:	map[x][y]="{W=";		break;
case SECT_BLOOD_RIVER:	map[x][y]="{R~";		break;
case SECT_WOODED_ROAD:	map[x][y]="{G+";		break;
case SECT_FROZEN_SEA:	map[x][y]="{W~";		break;
case SECT_DOCK:         map[x][y]="{y%";                break;
case SECT_ICEMOUNTAIN:  map[x][y]="{W^";                break;
case SECT_SNOWTREES:     map[x][y]="{W@";                break;
case SECT_DIRTPATH:     map[x][y]="{y+";		break;
case SECT_WHEATFIELD:   map[x][y]="{y.";		break;
 case SECT_DARKWOOD:    map[x][y]="{d@";                break;
 case SECT_WIND:        map[x][y]="{C~";                break;
 case SECT_TORNADO:     map[x][y]="{C@";                break;
 case SECT_WHIRLPOOL:   map[x][y]="{B@";                break;
 case SECT_LAVACHUTE:   map[x][y]="{R@";                break;
 case SECT_EARTH:       map[x][y]="{y~";                break;
 case SECT_QUICKSAND:   map[x][y]="{y@";                break;
 case SECT_CAVE_FLOOR:   map[x][y]="{d=";                break;
 case SECT_FIRE_ROOM:   map[x][y]="{RX";                break;
 case SECT_DARK_SWAMP:   map[x][y]="{d~";                break;
 case SECT_BRICK_ROAD:   map[x][y]="{r+";                break;
default:
map[x][y]="{yo{x";
}


    for ( door = 0; door < MAX_MAP_DIR; door++ )     {
           if ((pexit = room->exit[door]) != NULL
	     &&   pexit->to_room != NULL
	     &&   !IS_SET(pexit->exit_info, EX_CLOSED)
         &&   pexit->is_named == FALSE                     )
        { /* if exit there */	prospect_room = pexit->to_room;
        if ( prospect_room->exit[rev_dir[door]] &&
	 prospect_room->exit[rev_dir[door]]->to_room!=room)
	{
		/* if not two way */
		if ((prospect_room->sector_type==SECT_CITY)
		||  (prospect_room->sector_type==SECT_INSIDE))
			map[x][y]="{W@";
			else
			map[x][y]="{d?";
			return;
		}
		/* end two way */
        if ((x<=min)||(y<=min)||(x>=max)||(y>=max)) return;
        if (map[x+offsets[door][0]][y+offsets[door][1]]==NULL) {
                MapArea (pexit->to_room,ch,
                    x+offsets[door][0], y+offsets[door][1],min,max);
	}
	} /* end if exit there */
	}
return;
}
void ShowMap( CHAR_DATA *ch, int min, int max){int x,y;


  for (x = min; x < max; ++x)
     {
	send_to_char("     ",ch);
         for (y = min; y < max; ++y)
         {
	  if (map[x][y]==NULL)
		send_to_char(" ",ch);
		   else
 		send_to_char(map[x][y],ch);
         }
      send_to_char("\n\r",ch);
     }
return;
}

void do_mapping( CHAR_DATA *ch, char *argument )
{
int size,center,x,y,min,max;
char arg1[10];

one_argument( argument, arg1 );
size = atoi (arg1);

if (size < 7 || size > 40)
return;

size=URANGE(7,size,72);
center=MAX_MAP/2;

min = MAX_MAP/2-size/2;
max = MAX_MAP/2+size/2;

for (x = 0; x < MAX_MAP; ++x)
	for (y = 0; y < MAX_MAP; ++y)                  map[x][y]=NULL;

MapArea(ch->in_room, ch, center, center, min, max);


map[center][center]="{R*";
ShowMap (ch, min, max);
return;
}

void do_mapping2( CHAR_DATA *ch, char *argument )
{
int size,center,x,y,min,max;
char arg1[10];

one_argument( argument, arg1 );
size = 19;

size=URANGE(7,size,72);
center=MAX_MAP/2;

min = MAX_MAP/2-size/2;
max = MAX_MAP/2+size/2;

for (x = 0; x < MAX_MAP; ++x)
        for (y = 0; y < MAX_MAP; ++y)                  map[x][y]=NULL;

MapArea(ch->in_room, ch, center, center, min, max);

map[center][center]="{R*";
ShowMap (ch, min, max);
return;
}

void do_mapping3( CHAR_DATA *ch, char *argument )
{
int size,center,x,y,min,max;
char arg1[10];

one_argument( argument, arg1 );
size = atoi (arg1);

size=URANGE(7,size,72);
center=MAX_MAP/2;

min = MAX_MAP/2-size/2;
max = MAX_MAP/2+size/2;

for (x = 0; x < MAX_MAP; ++x)
        for (y = 0; y < MAX_MAP; ++y)                  map[x][y]=NULL;

MapArea(ch->in_room, ch, center, center, min, max);
map[center][center]="{R*";
ShowMap (ch, min, max);
return;
}


