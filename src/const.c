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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "ack.h"
#ifndef DEC_MAGIC_H
#include "magic.h"
#endif



/*
 * color table for say, gossip, shout, etc.
 * Put here for ease of editing. -S-
 */
 
const   struct color_type      color_table    [MAX_color]            =
{
   { "say",              0 },
   { "tell",             1 },
   { "gossip",           2 },
   { "auction",          3 },
   { "music",            4 },
   { "shout",            5 },
   { "yell",             6 },
   { "clan",             7 },
   { "race",             8 },
   { "flame",            9 },
   { "info",            10 },
   { "stats",           11 },
   { "rooms",           12 },
   { "objects",         13 },
   { "mobiles",         14 }

};

const   struct  ansi_type       ansi_table      [MAX_ANSI]              =
{
   { "gray",            "\033[0;37m", 	 0,	'w',	7 },
   { "red",             "\033[0;31m",	 1,	'r',	7 },
   { "green",           "\033[0;32m",	 2,	'g',	7 },
   { "brown",           "\033[0;33m",	 3,	'y',	7 },
   { "blue",            "\033[0;34m",	 4,	'b',	7 },
   { "magenta",         "\033[0;35m",	 5,	'm',	7 },
   { "cyan",            "\033[0;36m",	 6,	'c',	7 },
   { "black",           "\033[0;30m",	 7,	'k',	7 }, /* was 0;33 */
   { "yellow",          "\033[1;33m",	 8,	'Y',	7 },
   { "white",           "\033[1;37m",    9,	'W',	7 },
   { "normal",          "\033[0;0m\033[0;40m",    10,	'x',	13 },
   { "purple",		"\033[1;35m",   11,	'M',	7 },
   { "dark_grey",	"\033[1;30m",   12,	'd',	7 },
   { "light_blue",	"\033[1;34m",   13,	'B',	7 },
   { "light_green",	"\033[1;32m",   14,	'G',	7 },
   { "light_cyan",	"\033[1;36m",   15,	'C',	7 },
   { "light_red",	"\033[1;31m",   16,	'R',	7 },
   { "bold",		"\033[1m",    17,	'l',	4 },
   { "flashing",	"\033[5m",    18,	'f',	4 },
   { "inverse",		"\033[7m",    19,	'i',	4 },
   { "back_red",             "\033[0;41m",	 20,	'2',	7 },
   { "back_green",           "\033[0;42m",	 21,	'3',	7 },
   { "back_yellow",           "\033[0;43m",	 22,	'4',	7 },
   { "back_blue",            "\033[0;44m",	 23,	'1',	7 },
   { "back_magenta",         "\033[0;45m",	 24,	'5',	7 },
   { "back_cyan",            "\033[0;46m",	 25,	'6',	7 },
   { "back_black",           "\033[0;40m",	 26,	'0',	7 }, 
   { "back_white",           "\033[1;47m",    27,	'7',	7 },

};

const  struct  group_type  group_table [MAX_GROUP] =
{

   {
     "Attack",
     {
	 /* Reis */
	    2,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,


	   /* Thantaras */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   

	   /* That damn other town */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,


	   /* Azterra */
	   10,            10,            10,            10,            10,            10,            10,
       10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10
	 },
	 {
	   /* CAPITALIZE SPELLS ON THIS LIST!
		* DO NOT FORGET!
	    */
		 "Animate",
	     "Bless",
		 "Earthquake",
		 "Armor"
	 }
   },

   {
     "Benedictions",
     {	 /* Reis */
	    2,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,


	   /* Thantaras */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   

	   /* That damn other town */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,


	   /* Azterra */
	   10,            10,            10,            10,            10,            10,            10,
       10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10
	 },
	 {
	   /* CAPITALIZE SPELLS ON THIS LIST!
		* DO NOT FORGET!
	    */
		 "Animate",
	     "Bless",
		 "Earthquake",
		 "Armor"
	 }
   },

   {
     "Necromancy",
     {	 /* Reis */
	    2,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */ 10,/* <-Nec */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,


	   /* Thantaras */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   

	   /* That damn other town */
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10,


	   /* Azterra */
	   10,            10,            10,            10,            10,            10,            10,
       10,            10,            10,            10,            10,            10,            10,
	   10,            10,            10,            10,            10,            10,            10
	 },
	 {
	   /* CAPITALIZE SPELLS ON THIS LIST!
		* DO NOT FORGET!
	    */
		 "Animate",
	     "Bless",
		 "Earthquake",
		 "Armor"
	 }
   },
   {
     "Minor Attack",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Maladictions",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Evocation",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Major Evocation",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Transportation",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Major Transportation",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Illusion",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Creation",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Minor Detection",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Detection",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Healing",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Major Detection",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Major Attack",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Draconian",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Bone Magic",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Protections",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
   {
     "Enchantments",
     { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
       10, 10, 10, 10 }
   },
};
   

/*
 * Class table.
 */
const   struct  class_type      class_table     [MAX_CLASS]     =
{
    {
	"Nec",  "Necromancer",   APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_DAGGER,
	1107,  90,  18, 10,  1,  2, TRUE,
	"faerie fire", 1, -1, FALSE,  /* "Skill learned", remort to, remort from, is remort class? */
      { TRUE, FALSE, FALSE, FALSE, TRUE }, /* FIRST ONE MUST ALWAYS BE TRUE */ "Practitioner of the dark arts."
    },

    {
	"Lic",  "Lich",    APPLY_WIS, "Wis",  OBJ_VNUM_SCHOOL_MACE,
	1105,  90,  18, 12,  2, 2, TRUE,
	"bless", -1, 1, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "An evil, dead being, with great control over the dark forces."
    },

    {
	"DkP",  "Dark Priest",    APPLY_DEX, "Dex",  OBJ_VNUM_SCHOOL_DAGGER,
	1106,  90,  18,  8,  1, 3, FALSE,
	"steal", -1, 1, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "A Necromancer who has prefected the healing arts."
    },

    {
	"Enc",  "Enchanter",  APPLY_STR, "Str",  OBJ_VNUM_SCHOOL_SWORD,
	1108,  90,  18,  6,  3, 4, FALSE,
	"punch", 2, -1, FALSE,
     { TRUE, FALSE, FALSE, FALSE, TRUE }, "A powerful mage dealing with enchantment."
    },

    {
	"Ill",  "Illusionist",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 2, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "The most powerful manipulator of minds."
    },
    {
	"Wit",  "Witch",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 2, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Solitary mages who have devoted their lives to alchemy and dark enchantment."
    },
    {
	"Sor",  "Sorceror",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 3, -1, FALSE,
      { TRUE, FALSE, FALSE, FALSE, TRUE }, "A practicer of the offensive magics."
    },
    {
	"Arc",  "Archmage",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 3, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "The highest Mage, well versed in all fields of magic, but specializing in attack magics."
    },
    {
	"BnM",  "Bonemage",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 3, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "A practicer of the offensive magics."
    },
    {
	"Kni",  "Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 4, -1, FALSE,
      { TRUE, FALSE, FALSE, FALSE, TRUE }, "A warrior who has experience in the healing arts."
    },
    {
	"Cru",  "Crusader",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 4, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "A Holy Knight that has perfected their craft."
    },
    {
	"Rai",  "Raider",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 4, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Forsaken Knights that have turned to the necromantic arts."
    },
    {
	"Thi",  "Thief",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 5, -1, FALSE,
      { TRUE, FALSE, FALSE, FALSE, TRUE }, "A sly warrior that thrives off the riches of others."
    },
    {
	"Ass",  "Assassin",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 5, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Fearsome fighters that have perfected their craft."
    },
    {
	"Nin",  "Ninja",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 5, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "A stealthy warrior that is one with the night."
    },
    {
	"Ran",  "Ranger",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 6, -1, FALSE,
      { TRUE, FALSE, FALSE, FALSE, TRUE }, "Someone who has ventured into the magics of nature."
    },
    {
	"Crf",  "Craftsmen",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 6, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "A refined Ranger, dealing more with the physical aspects of nature."
    },
    {
	"Dru",  "Druid",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 6, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "A Ranger that has chosen a more magical path."
    },
    {
	"Sol",  "Soldier",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 7, -1, FALSE,
      { TRUE, FALSE, FALSE, FALSE, TRUE }, "The most basic fighter, skilled in all forms of combat."
    },
    {
	"Bld",  "Blademaster",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 7, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Someone who has mastered all the weapon skills."
    },
    {
	"Mrc",  "Mercenary",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 7, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Tough soldiers that live as nomads, relying on brute strength."
    },
    {
	"Mag",  "Mage",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 8, -1, FALSE,
      { TRUE, TRUE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Wiz",  "Wizard",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 8, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Arc",  "Archmage",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 8, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Cle",  "Cleric",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 9, -1, FALSE,
      { TRUE, TRUE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Pri",  "Priest",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 9, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Acl",  "Acolyte",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 9, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Kni",  "Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 10, -1, FALSE,
      { TRUE, TRUE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"ShK",  "Shield Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 10, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"SwK",  "Sword Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 10, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Thi",  "Thief",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 11, -1, FALSE,
      { TRUE, TRUE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Sam",  "Samurai",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 11, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Nin",  "Ninja",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 11, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Dru",  "Druid",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 12, -1, FALSE,
      { TRUE, TRUE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Ele",  "Elemental",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 12, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Geo",  "Geomancer",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 12, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Ill",  "Illusionist",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 13, -1, FALSE,
      { TRUE, TRUE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Brd",  "Bard",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 13, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Sum",  "Summoner",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 13, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"War",  "Warrior",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 14, -1, FALSE,
      { TRUE, TRUE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"BlM",  "Blademaster",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 14, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Bar",  "Barbarian",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 14, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
/* BEGIN CRONUS */
    {
	"Kni",  "Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 15, -1, FALSE,
      { TRUE, FALSE, FALSE, TRUE, FALSE }, "Class Description goes here!"
    },
    {
	"AqK",  "Aqua Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 15, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"DtK",  "Death Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 15, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Cle",  "Cleric",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 16, -1, FALSE,
      { TRUE, FALSE, FALSE, TRUE, FALSE }, "Class Description goes here!"
    },
    {
	"Pri",  "Priest",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 16, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Rea",  "Reaper",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 16, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Dru",  "Druid",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 17, -1, FALSE,
      { TRUE, FALSE, FALSE, TRUE, FALSE }, "Class Description goes here!"
    },
    {
	"Geo",  "Geomancer",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 17, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Mon",  "Monk",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 17, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"War",  "Warrior",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 18, -1, FALSE,
      { TRUE, FALSE, FALSE, TRUE, FALSE }, "Class Description goes here!"
    },
    {
	"BlM",  "Blademaster",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 18, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Bar",  "Barbarian",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 18, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Thi",  "Thief",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 19, -1, FALSE,
      { TRUE, FALSE, FALSE, TRUE, FALSE }, "Class Description goes here!"
    },
    {
	"Sam",  "Samurai",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 19, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Nin",  "Ninja",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 19, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Mgs",  "Magus",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 20, -1, FALSE,
      { TRUE, FALSE, FALSE, TRUE, FALSE }, "Class Description goes here!"
    },
    {
	"Wiz",  "Wizard",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 21, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Sor",  "Sorrcerer",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 21, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Brd",  "Bard",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 22, -1, FALSE,
      { TRUE, FALSE, FALSE, TRUE, FALSE }, "Class Description goes here!"
    },
    {
	"Ill",  "Illusionist",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 22, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Mis",  "Mistic",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 22, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
/* BEGIN KHAD */
    {
	"Kni",  "Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 23, -1, FALSE,
      { TRUE, FALSE, TRUE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"HvK",  "Heaven Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 23, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"DtK",  "Death Knight",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 23, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Cle",  "Cleric",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 24, -1, FALSE,
      { TRUE, FALSE, TRUE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Pri",  "Priest",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 24, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Tai",  "Tainted",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 24, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Dru",  "Druid",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 25, -1, FALSE,
      { TRUE, FALSE, TRUE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Geo",  "Geomancer",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 25, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Mon",  "Monk",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 25, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"War",  "Warrior",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 26, -1, FALSE,
      { TRUE, FALSE, TRUE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Bld",  "Blademaster",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 26, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Bar",  "Barbarian",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 26, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Thi",  "Thief",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 27, -1, FALSE,
      { TRUE, FALSE, TRUE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Sam",  "Samurai",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 27, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Nin",  "Ninja",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 27, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Mgs",  "Magus",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 28, -1, FALSE,
      { TRUE, FALSE, TRUE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Wiz",  "Wizard",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 28, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Sor",  "Sorrcerer",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 28, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Brd",  "Bard",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", 29, -1, FALSE,
      { TRUE, FALSE, TRUE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Ill",  "Illusionist",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 29, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    },
    {
	"Mis",  "Mistic",  APPLY_INT, "Int",  OBJ_VNUM_SCHOOL_SWORD,
	1125,   90,   18, 10,  2,  3, TRUE,
	"mind flail", -1, 29, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE }, "Class Description goes here!"
    }

};




/* Table for remort classes.... same format as class_table 
 * Note that alot of stuff is not needed... 
 */
const	struct	remort_class_type	remort_table	[MAX_RCLASS] =
{
     {
     	"Sor",	"Sorcerer",	APPLY_INT,   "Int", 0,
        /* guild room -> */ 0, 0, 0, 0, 2, 3, TRUE,
        "", 6, 1
     },
     
     {
     	"Ass",	"Assassin",	APPLY_DEX,   "Dex", 0,
     	/* guild room -> */ 0, 0, 0, 0, 2, 3, FALSE,
     	"", 6, 2
     },
     
     {
     	"Kni",	"Knight",	APPLY_STR,   "Str", 0,
     	/* guild room -> */ 0, 0, 0, 0, 4, 4, FALSE,
     	"", 6, 3
     },
     
     {
     	"Nec",	"Necromancer",	APPLY_WIS,   "Wis", 0,
     	/* guild room -> */ 0, 0, 0, 0, 2, 3, TRUE,
     	"", 6, 4
     },
     
     { 
        "Mon",	"Monk",		APPLY_CON,   "Con", 0,
        /* guild_room -> */ 0, 0, 0, 0, 2, 3, TRUE,
        "", 6, 5
     }
};



const   struct  race_type      race_table     [MAX_RACE]     =
{
  {
    "Hmn",  "Human","{xHuman{x",        101,   0,
    18,     18,     18,     18,     17,   17,
    RACE_MOD_NONE,
     /* racial wear slots  */
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
        5, { 4, 5, 3, 1, 2 },
    "War, Thi/Psi,  Mag, Cle",
    "",
    REALM_NONE, /* strong magic realms for race */
    REALM_NONE, /* weak */
    REALM_NONE, /* resist */
    REALM_NONE, /* suscept */ /* FIRST MUST ALWAYS BE TRUE */
    TRUE, { TRUE, TRUE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },

  {
    "Hlf",  "Halfling","{xHalfling{x",    3001,   0,
    13,     17,     18,     21,     18,    17,
    RACE_MOD_SMALL | RACE_MOD_WOODLAND | RACE_MOD_RESIST_SPELL,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
    5, { 2, 3, 1, 4, 5 },
    "Thi, Mag, Cle, War, Psi",
    "steal",
    REALM_NONE,
    REALM_NONE,
    REALM_MIND | REALM_ACID | REALM_FIRE | REALM_COLD | REALM_HOLY,
    REALM_NONE,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },

  {
    "Dwf",  "Dwarf","{xDwarf{x",        7536,   0,
    18,     12,     19,     12,     20,     17,
    RACE_MOD_RESIST_SPELL | RACE_MOD_SMALL | RACE_MOD_IMMUNE_POISON,

        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
         5, { 5, 2, 4, 1, 3 },
    "War, Cle, Psi, Thi, Mag",
    "smash",
    REALM_NONE,
    REALM_ACID,
    REALM_ACID | REALM_FIRE | REALM_HOLY | REALM_POISON,
    REALM_NONE,
    TRUE, { TRUE, TRUE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },

  {
    "Elf",  "Elf","{xElf{x",          9201,   0,
    12,     19,     12,     20,     14,    17,
    RACE_MOD_SMALL | RACE_MOD_WOODLAND | RACE_MOD_STRONG_MAGIC,
       { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
        5, { 1, 5, 2, 3, 4 },
    "Mag, Thi, War, Psi, Cle", 
    "",
    REALM_MIND | REALM_COLD,
    REALM_DRAIN | REALM_ACID,
    REALM_MIND,
    REALM_DRAIN,
    TRUE, { TRUE, TRUE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },

  {
    "Gno",  "Gnome","{xGnome{x",       6015,   0,
    12,     20,     18,     20,     15,    17,
    RACE_MOD_TINY | RACE_MOD_IMMUNE_POISON | RACE_MOD_DARKNESS,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
    5, { 3, 1, 4, 5, 2 },
    "Cle, Psi, Mag, Thi, War",
    "sneak",
    REALM_POISON | REALM_MIND | REALM_FIRE,
    REALM_IMPACT | REALM_ACID,
    REALM_MIND,
    REALM_ACID,
    TRUE, { TRUE, TRUE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },

  {
    "Ogr",  "Ogre","{xOgre{x",         3430,   0,
    21,     10,     12,     15,     20,    17,
    RACE_MOD_WEAK_MAGIC | RACE_MOD_RESIST_SPELL| RACE_MOD_LARGE | RACE_MOD_TOUGH_SKIN,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
    5, { 2, 4, 3, 1, 5 },
    "War, Mag, Thi, Cle, Psi",
    "",
    REALM_POISON | REALM_ACID,
    REALM_MIND | REALM_SOUND,
    REALM_NONE,
    REALM_MIND,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
  {
    "Drw",  "Drow","{xDrow{x",         9201,   0,
    13,     19,     11,     19,     11,    17,
    RACE_MOD_STRONG_MAGIC | RACE_MOD_SLOW_HEAL | RACE_MOD_DARKNESS,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
    5, { 1, 5, 4, 3, 2 },
    "Psi, Mag, War, Thi, Cle",
    "\'find doors\'",
    REALM_DRAIN |  REALM_SHOCK | REALM_MIND | REALM_FIRE,
    REALM_HOLY | REALM_LIGHT,
    REALM_DRAIN,
    REALM_HOLY,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
    
  {
    "Lam",  "Lamia","{xLamia{x",        3001,   0,
    19,     13,     11,     20,     14,    17,
    RACE_MOD_LARGE | RACE_MOD_IMMUNE_POISON | RACE_MOD_TAIL,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE       },
    5, { 4, 5, 1, 2, 3 },
    "Thi, War, Psi, Mag, Cle",
    "dodge",
    REALM_SOUND | REALM_ACID,
    REALM_MIND | REALM_FIRE,
    REALM_NONE,
    REALM_MIND | REALM_FIRE | REALM_HOLY,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
    
  {
    "Drc",  "Dracon","{xDracon{x",       3001,   0,
    20,     19,     11,     9,     19,    17,
    RACE_MOD_STRONG_MAGIC | RACE_MOD_TOUGH_SKIN | RACE_MOD_SLOW_HEAL | RACE_MOD_LARGE | RACE_MOD_TAIL | RACE_MOD_RESIST_SPELL,
        { TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE,
          TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE       },
    5, { 1, 2, 5, 3, 4 },
    "Mag, Cle, War, Psi, Thi",
    "fly",
    REALM_FIRE | REALM_GAS | REALM_SHOCK | REALM_POISON | REALM_ACID,
    REALM_COLD | REALM_MIND,
    REALM_FIRE | REALM_SHOCK | REALM_ACID,
    REALM_MIND | REALM_SHOCK,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
    
  {
    "Cen",  "Centaur","{xCentaur{x",       3001,   0,
    19,     15,     19,     12,     19,    17,
    RACE_MOD_LARGE | RACE_MOD_TOUGH_SKIN | RACE_MOD_WOODLAND,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE       },
    5, { 2, 4, 3, 1, 5 },
    "War/Cle,  Thi, Mag, Psi",
    "hunt",
    REALM_SOUND | REALM_FIRE,
    REALM_FIRE | REALM_ACID,
    REALM_MIND | REALM_SHOCK | REALM_GAS,
    REALM_FIRE,
    TRUE, { TRUE, TRUE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
    
  {
    "Ttn",  "Titan","{xTitan{x",        3001,  0,
    21,     18,    12,      18,    21,    17,
    RACE_MOD_NO_MAGIC | RACE_MOD_HUGE | RACE_MOD_RESIST_SPELL,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
    5, { 3, 4, 5, 1, 2 },
    "War, Psi, Mag, Cle, Thi",
    "",
    REALM_MIND,
    REALM_DRAIN | REALM_GAS,
    REALM_MIND,
    REALM_NONE,
    TRUE, { TRUE, TRUE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
    
  {
    "Sde",  "Sidhe","{xSidhe{x",        3001,  0,
    12,     20,    15,      22,    12,    17,
    RACE_MOD_STRONG_MAGIC | RACE_MOD_TINY | RACE_MOD_RESIST_SPELL,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE      },
    5, { 3, 4, 2, 5, 1 },
    "Psi, Thi, Mag, Cle, War",
    "fly",
    REALM_MIND | REALM_SHOCK | REALM_FIRE | REALM_DRAIN,
    REALM_FIRE | REALM_GAS | REALM_DRAIN,
    REALM_MIND | REALM_SHOCK,
    REALM_IMPACT | REALM_FIRE,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
    
  {
    "Min",  "Minotaur", "{xMinotaur{x",     3001,  0,
    21,     12,    11,      16,    18,    17,
    RACE_MOD_LARGE | RACE_MOD_TOUGH_SKIN | RACE_MOD_DARKNESS,
        { TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE       },
    5, { 2, 3, 5, 1, 4 },
    "War, Mag, Cle, Psi, Thi",
    "",
    REALM_POISON | REALM_SOUND,
    REALM_SHOCK | REALM_COLD,
    REALM_NONE,
    REALM_COLD,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
  {
    "Trl",  "Troll","{xTroll{x",     3001,  0,
    20,     11,    11,      16,    21,    17,
    RACE_MOD_FAST_HEAL | RACE_MOD_LARGE | RACE_MOD_DARKNESS | RACE_MOD_STONE_SKIN,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
        5, { 2, 3, 5, 1, 4 },
    "War, Cle, Thi, Psi, Mag",
    "",
    REALM_NONE, 
    REALM_LIGHT | REALM_FIRE | REALM_HOLY,
    REALM_IMPACT,
    REALM_LIGHT | REALM_FIRE,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
  {
    "Und",  "Undead","{xUndead{x",     3001,  0,
    20,     11,    11,      15,    21,    17,
    RACE_MOD_DARKNESS | RACE_MOD_IMMUNE_POISON,
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
    5, { 2, 3, 5, 1, 4 },
    "War, Cle, Thi, Psi, Mag",
    "",
    REALM_DRAIN | REALM_POISON, 
    REALM_LIGHT |  REALM_HOLY,
    REALM_DRAIN | REALM_POISON,
    REALM_LIGHT | REALM_HOLY,
    FALSE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },

  {
    "Gar",  "Gargoyle","{xGargoyle{x",       3001,   0,
    19,     19,     11,     11,     17,    17,
    RACE_MOD_IRON_SKIN | RACE_MOD_SLOW_HEAL | RACE_MOD_LARGE,
        { TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE,
          TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, FALSE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
    5, { 1, 2, 5, 3, 4 },
    "Mag, War, Psi, Cle, Thi",
    "fly",
    REALM_FIRE | REALM_GAS | REALM_SHOCK | REALM_POISON,
    REALM_COLD | REALM_MIND | REALM_ACID,
    REALM_FIRE | REALM_SHOCK | REALM_MIND,
    REALM_COLD | REALM_ACID,
    TRUE, { TRUE, FALSE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  },
  {
    "Fae",  "Faerie", "{xFaerie{x",        101,   0,
    21,     23,     18,     18,     17,    17,
    RACE_MOD_NONE,
     /* racial wear slots  */
        { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
          TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
          TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE       },
        5, { 4, 5, 3, 1, 2 },
    "War, Thi/Psi,  Mag, Cle",
    "",
    REALM_NONE, /* strong magic realms for race */
    REALM_NONE, /* weak */
    REALM_NONE, /* resist */
    REALM_NONE, /* suscept */
    TRUE, { TRUE, TRUE, TRUE, TRUE, TRUE },
    { "*A lanky looking Human from PRISON*",
      "*A lanky looking Human from Thantaras*",
      "*A lanky looking Human from Azterra*",
      "*A lanky looking Human from Luminous*",
      "*A lanky looking Human from Other Town*" }, FALSE
  }

};


const   struct  clan_type      clan_table     [MAX_CLAN]     = 
{
	{
		"None", "None ",       0, 0, 
		"N/A", " ", { -1, -1, -1, -1 }
		
	},

	{
		"{RA{Yo{gW{x {BK{Wi{Bl{Wl{Be{Wr{x", "{R^{W_{R^{x  ",       167, 168, 
		"None", " ", { -1, -1, -1, -1 }
		
	},
	
	{
		"None", "None ",       0, 0, 
		"N/A", " ", { -1, -1, -1, -1 }
		
	},

	{
		"None", "None ",       0, 0, 
		"N/A", " ", { -1, -1, -1, -1 }
		
	},
	
	{
		"None", "None ",       0, 0, 
		"N/A", " ", { -1, -1, -1, -1 }
		
	},
	
	{
		"None", "None ",       0, 0, 
		"N/A", " ", { -1, -1, -1, -1 }
		
	},
		
	{
		"None", "None ",       0, 0, 
		"N/A", " ", { -1, -1, -1, -1 }
		
	},
	{
		"None", "None ",       0, 0, 
		"N/A", " ", { -1, -1, -1, -1 }
		
	},
	{
		"None", "None ",       0, 0, 
		"N/A", " ", { -1, -1, -1, -1 }
		
	},		
			
};

const   struct  hometown_type   hometown_table [] =
{
/*      {       "name",       color name        recall, school, morgue, death    wname         mdonate  adonate  wdonate  },    */
        {       "Prison",   "{dPrison{x",       3001,   3001,   3001,   3001, "{dPrison{x",     3001,    3001,    3001    },      
        {       "Thantaras","{cThantaras{x",    3063,   3700,   3001,   3064, "{cThants{x",     3017,    3018,    3021    },
        {       "Azterra",  "{gAzterra{x",      1,   1,   1,   1, "{gAzterr{x",     1,    1,    1    },
        {       "Kha'dar",  "{CKha'dar{x",      1,   1,   1,   1, "{CKhadar{x",     1,    1,    1    },
        {       "Reis",     "{rReis{x",         1,      1,      1,      1,    "{rReis  {x",     1,       1,       1    },
        {       NULL,       NULL,           3001,   3001,   3001,   3001, NULL, 3001, 3001, 3001,              }
};


const struct exp_type exp_table[141]  = 
{
   {      0, {    1000,    1000,    1000,    1000,    1000 } }, /*  0 */
   {    100, {    3050,    3023,    3022,    3021,    3020 } }, /*  1 */
   {    200, {    8081,    8080,    8084,    8083,    8082 } }, /*  2 */
   {    300, {   11183,   11182,   11181,   11184,   11185 } }, /*  3 */
   {    450, {   20326,   20322,   20323,   20324,   20325 } }, /*  4 */
   {    600, {   35504,   35505,   35506,   35507,   35503 } }, /*  5 */
   {    850, {   38350,   38725,   38726,   38727,   38728 } }, /*  6 */
   {   1080, {   54000,   53000,   54500,   53500,   53800 } }, /*  7 */
   {   1300, {   65000,   65500,   64500,   65900,   64300 } }, /*  8 */
   {   1550, {   77500,   77600,   77000,   78000,   77200 } }, /*  9 */
   {   1800, {  108000,  108500,  109000,  107500,  107000 } }, /* 10 */
   {   2100, {  126000,  126500,  125500,  125000,  127000 } }, /* 11 */
   {   2450, {  147000,  148000,  146000,  146500,  147500 } }, /* 12 */
   {   2800, {  168000,  167500,  168500,  169000,  167000 } }, /* 13 */
   {   3150, {  189000,  188000,  190000,  188500,  189500 } }, /* 14 */
   {   3500, {  210000,  211000,  205000,  210500,  210000 } }, /* 15 */
   {   3900, {  234000,  233000,  234500,  235000,  239000 } }, /* 16 */
   {   4350, {  261000,  262000,  261500,  260000,  260500 } }, /* 17 */
   {   4800, {  288000,  287000,  287500,  289000,  288000 } }, /* 18 */
   {   5300, {  318000,  317000,  317500,  318500,  319000 } }, /* 19 */
   {   5800, {  348000,  347000,  349000,  348500,  347500 } }, /* 20 */
   {   6350, {  381000,  382000,  380000,  380500,  381500 } }, /* 21 */
   {   6950, {  417000,  416500,  417500,  418000,  418500 } }, /* 22 */
   {   7550, {  453000,  452000,  454000,  453500,  452500 } }, /* 23 */
   {   8200, {  492000,  491000,  493000,  492500,  491500 } }, /* 24 */
   {   8800, {  528000,  527000,  528500,  529000,  527500 } }, /* 25 */
   {   9500, {  570000,  575000,  565000,  560000,  580000 } }, /* 26 */
   {  10200, {  612000,  611000,  613000,  612500,  611500 } }, /* 27 */
   {  11000, {  660000,  655000,  665000,  670000,  650000 } }, /* 28 */
   {  11900, {  714000,  715000,  714500,  713500,  714000 } }, /* 29 */
   {  13000, {  780000,  785000,  790000,  770000,  775000 } }, /* 30 */
   {  14000, {  840000,  841000,  840500,  841500,  840500 } }, /* 31 */
   {  15300, {  918000,  917500,  918500,  919000,  918200 } }, /* 32 */
   {  16500, {  990000,  985000,  987000,  995000,  993000 } }, /* 33 */
   {  18000, { 1080000, 1090000, 1075000, 1085000, 1087000 } }, /* 34 */
   {  19500, { 1170000, 1165000, 1175000, 1180000, 1160000 } }, /* 35 */
   {  22000, { 1320000, 1315000, 1330000, 1325000, 1320000 } }, /* 36 */
   {  24500, { 1470000, 1465000, 1475000, 1477000, 1460000 } }, /* 37 */
   {  27500, { 1650000, 1660000, 1655000, 1657000, 1645000 } }, /* 38 */
   {  30000, { 1800000, 1790000, 1810000, 1815000, 1795000 } }, /* 39 */
   {  33000, { 1980000, 1975000, 1985000, 1983000, 1977000 } }, /* 40 */
   {  34000, { 2040000, 2035000, 2045000, 2039000, 2050000 } }, /* 41 */
   {  35500, { 2130000, 2135000, 2140000, 2125000, 2133000 } }, /* 42 */
   {  37500, { 2250000, 2240000, 2255000, 2260000, 2245000 } }, /* 43 */
   {  40000, { 2400000, 2450000, 2480000, 2350000, 2390000 } }, /* 44 */
   {  43000, { 2580000, 2570000, 2585000, 2589000, 2575000 } }, /* 45 */
   {  46000, { 2760000, 2755000, 2765000, 2770000, 2763000 } }, /* 46 */
   {  49500, { 2970000, 2975000, 2980000, 2965000, 2973000 } }, /* 47 */
   {  52000, { 3120000, 3115000, 3125000, 3200000, 3128000 } }, /* 48 */
   {  55000, { 3300000, 3290000, 3310000, 3295000, 3300000 } }, /* 49 */
   {  56000, { 3360000, 3355000, 3365000, 3370000, 3368000 } }, /* 50 */
   {  58000, { 3480000, 3470000, 3485000, 3488000, 3475000 } }, /* 51 */
   {  60000, { 3600000, 3610000, 3590000, 3605000, 3640000 } }, /* 52 */
   {  63000, { 3780000, 3790000, 3770000, 3775000, 3785000 } }, /* 53 */
   {  66000, { 3960000, 3940000, 3970000, 3965000, 3962000 } }, /* 54 */
   {  68000, { 4080000, 4095000, 4085000, 4083000, 4073000 } }, /* 55 */
   {  71000, { 4260000, 4250000, 4265000, 4255000, 4300000 } }, /* 56 */
   {  73000, { 4380000, 4390000, 4385000, 4375000, 4387000 } }, /* 57 */
   {  76000, { 4560000, 4570000, 4550000, 4555000, 4565000 } }, /* 58 */
   {  79000, { 4740000, 4750000, 4745000, 4755000, 4760000 } }, /* 59 */
   {  81000, { 4860000, 4840000, 4850000, 4855000, 4870000 } }, /* 60 */
   {  0, { 4980000, 4990000, 4975000, 4985000, 4990000 } }, /* 61 */
   {  0, { 5100000, 5110000, 5150000, 5090000, 5095000 } }, /* 62 */
   {  0, { 5280000, 5270000, 5285000, 5290000, 5275000 } }, /* 63 */
   {  0, { 5345000, 5350000, 5360000, 5350000, 5343000 } }, /* 64 */
   {  0, { 5460000, 5470000, 5450000, 5465000, 5456000 } }, /* 65 */
   {  0, { 5580000, 5560000, 5590000, 5575000, 5590000 } }, /* 66 */
   {  0, { 5640000, 5650000, 5630000, 5640000, 5620000 } }, /* 67 */
   {  0, { 5700000, 5710000, 5690000, 5720000, 5680000 } }, /* 68 */
   {  0, { 5820000, 5800000, 5830000, 5820000, 5810000 } }, /* 69 */
   {  0, { 5880000, 5820000, 5900000, 5860000, 5870000 } }, /* 70 */
   {  0, { 5940000, 5920000, 5980000, 5960000, 5930000 } }, /* 71 */
   {  0, { 5970000, 5980000, 6000000, 5800000, 5970000 } }, /* 72 */
   { 0, { 6000000, 6100000, 5950000, 6150000, 6050000 } }, /* 73 */
   { 0, { 6180000, 6190000, 6170000, 6190000, 6185000 } }, /* 74 */
   { 0, { 6360000, 6350000, 6400000, 6340000, 6360000 } }, /* 75 */
   { 0, { 6600000, 6610000, 6620000, 6590000, 6600000 } }, /* 76 */
   { 0, { 6910000, 6920000, 6900000, 6850000, 6900000 } }, /* 77 */
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 78 */
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 79 */
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 80 */
   { 0, { 6600000, 6610000, 6620000, 6590000, 6600000 } }, /* 81 */
   { 0, { 6910000, 6920000, 6900000, 6850000, 6900000 } }, /* 82 */
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 83 */
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 84 */
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 85 */
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 86 */
   { 0, { 6600000, 6610000, 6620000, 6590000, 6600000 } }, /* 87 */
   { 0, { 6910000, 6920000, 6900000, 6850000, 6900000 } }, /* 88 */
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 89 */
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 90 */
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 91 */
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 92 */
   { 0, { 6600000, 6610000, 6620000, 6590000, 6600000 } }, /* 93 */
   { 0, { 6910000, 6920000, 6900000, 6850000, 6900000 } }, /* 94 */
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 95 */
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 96 */
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 97 */
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 98 */
   { 0, { 6600000, 6610000, 6620000, 6590000, 6600000 } }, /* 99 */
   { 0, { 6910000, 6920000, 6900000, 6850000, 6900000 } }, /* 100*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 101*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 102*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 103*/ 
   { 0, { 6910000, 6920000, 6900000, 6850000, 6900000 } }, /* 104*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 105*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 106*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 107*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 108*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 109*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 110*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 111*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 112*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 113*/ 
   { 0, { 6910000, 6920000, 6900000, 6850000, 6900000 } }, /* 114*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 115*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 116*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 117*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 118*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 119*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 120*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 121*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 122*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 123*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 124*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 125*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 126*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 127*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 128*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 129*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 130*/

   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 131*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 132*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 133*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 134*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 135*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 136*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 137*/
   { 0, { 7200000, 7250000, 7230000, 7190000, 7200000 } }, /* 138*/
   { 0, { 7800000, 7700000, 7750000, 7780000, 7850000 } }, /* 139*/
   { 0, { 8000000, 8000000, 8000000, 8000000, 8000000 } }, /* 140*/









};

#define CLASS_MAGE       0
#define CLASS_CLERIC     1
#define CLASS_THIEF      2
#define CLASS_WARRIOR    3
#define CLASS_PSI        4
#define CLASS_CONJURER   5
#define CLASS_TEMPLAR    6  /* Yes, i _know_ it's now paladin :P */
#define CLASS_RANGER     7


/*
 * Attribute bonus tables.
 */
const   struct  str_app_type    str_app         [26]            =
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 140, 12 },
    {  0,  0, 140, 13 }, /* 13  */
    {  0,  1, 170, 14 },
    {  1,  1, 170, 15 }, /* 15  */
    {  1,  2, 195, 16 },
    {  2,  3, 220, 22 },
    {  2,  4, 250, 25 }, /* 18  */
    {  3,  5, 300, 30 },
    {  3,  6, 350, 35 }, /* 20  */
    {  4,  7, 400, 40 },
    {  5,  7, 450, 45 },
    {  6,  8, 500, 50 },
    {  8, 10, 550, 55 },
    { 10, 12, 600, 60 }  /* 25   */
};



const   struct  int_app_type    int_app         [26]            =
{
    {  3, -50, 0 },     /*  0 */
    {  6, -50, 0 },     /*  1 */
    {  6, -50, 0 },
    {  7, -50, 0 },     /*  3 */
    {  7, -50, 0 },
    {  8, -50, 0 },     /*  5 */
    {  8, -40, 2 },
    {  9, -40, 2 },
    {  9, -40, 2 },
    { 10, -35, 2 },
    { 10, -35, 3 },     /* 10 */
    { 11, -30, 4 },
    { 12, -30, 5 },
    { 13, -20, 6 },
    { 13, -20, 7 },
    { 15, -15, 8 },     /* 15 */
    { 20,  -5, 9 },
    { 23,  0, 10 },
    { 25, 10, 10 },     /* 18 */
    { 28, 10, 11 },
    { 34, 15, 11 },     /* 20 */
    { 39, 20, 12 },
    { 45, 25, 13 },
    { 52, 30, 15 },
    { 60, 35, 17 },
    { 70, 50, 20 }      /* 25 */
};



const   struct  wis_app_type    wis_app         [26]            =
{
    { 0, 50 },      /*  0 */
    { 0, 50 },      /*  1 */
    { 0, 50 },
    { 0, 50 },      /*  3 */
    { 0, 50 },
    { 1, 50 },      /*  5 */
    { 1, 50 },
    { 1, 45 },
    { 1, 45 },
    { 1, 40 },
    { 1, 35 },      /* 10 */
    { 2, 30 },
    { 2, 25 },
    { 2, 15 },
    { 2,  5 },
    { 3,  0 },      /* 15 */
    { 3, -5 },
    { 4,-15 },
    { 4,-20 },      /* 18 */
    { 5,-25 },
    { 5,-30 },      /* 20 */
    { 6,-35 },
    { 7,-45 },
    { 7,-55 },
    { 7,-70 },
    { 8,-80 }       /* 25 */
};



const   struct  dex_app_type    dex_app         [26]            =
{
    {60, -4},			/* 0 */
    {50, -3},			/* 1 */
    {50, -3},
    {40, -2},
    {30. - 2},
    {20, -1},			/* 5 */
    {10, -1},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},			/* 10 */
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {-10, 1},			/* 15 */
    {-15, 2},
    {-20, 3},
    {-30, 4},
    {-40, 5},
    {-50, 6},			/* 20 */
    {-60, 8},
    {-75, 12},
    {-90, 18},
    {-105, 22},
    {-120, 26}			/* 25 */
};



const   struct  con_app_type    con_app         [26]            =
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },   /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const   struct  liq_type        liq_table       [LIQ_MAX]       =
{
    { "water",                  "clear",        {  0, 1, 10 }   },  /*  0 */
    { "beer",                   "amber",        {  3, 2,  5 }   },
    { "wine",                   "rose",         {  5, 2,  5 }   },
    { "ale",                    "brown",        {  2, 2,  5 }   },
    { "dark ale",               "dark",         {  1, 2,  5 }   },

    { "whisky",                 "golden",       {  6, 1,  4 }   },  /*  5 */
    { "lemonade",               "pink",         {  0, 1,  8 }   },
    { "firebreather",           "boiling",      { 10, 0,  0 }   },
    { "local specialty",        "everclear",    {  3, 3,  3 }   },
    { "slime mold juice",       "green",        {  0, 4, -8 }   },

    { "milk",                   "white",        {  0, 3,  6 }   },  /* 10 */
    { "tea",                    "tan",          {  0, 1,  6 }   },
    { "coffee",                 "black",        {  0, 1,  6 }   },
    { "blood",                  "red",          {  0, 2, -1 }   },
    { "salt water",             "clear",        {  0, 1, -2 }   },

    { "{ychocolate milk{x",   "creamy",       {  0, 3,  6 }   },  /* 15 */
    { "mountain dew",           "bubbly",       {  0, 1,  5 }   }
};

const float hr_damTable[121] = {
   0.311,
   0.325,
   0.341,
   0.358,
   0.378,
   0.399,
   0.423,
   0.449,
   0.478,
   0.509,
   0.543,
   0.579,
   0.618,
   0.659,
   0.703,
   0.749,
   0.797,
   0.846,
   0.897,
   0.948,
   1.000,
   1.012,
   1.024,
   1.036,
   1.048,
   1.060,
   1.071,
   1.083,
   1.094,
   1.105,
   1.117,
   1.127,
   1.138,
   1.149,
   1.159,
   1.169,
   1.178,
   1.188,
   1.197,
   1.206,
   1.215,
   1.223,
   1.231,
   1.239,
   1.247,
   1.254,
   1.261,
   1.268,
   1.274,
   1.281,
   1.287,
   1.292,
   1.298,
   1.303,
   1.308,
   1.313,
   1.317,
   1.322,
   1.326,
   1.330,
   1.333,
   1.337,
   1.340,
   1.344,
   1.347,
   1.350,
   1.352,
   1.355,
   1.357,
   1.360,
   1.362,
   1.364,
   1.366,
   1.368,
   1.370,
   1.372,
   1.373,
   1.375,
   1.376,
   1.377,
   1.379,
   1.380,
   1.381,
   1.382,
   1.383,
   1.384,
   1.385,
   1.386,
   1.387,
   1.387,
   1.388,
   1.389,
   1.389,
   1.390,
   1.391,
   1.391,
   1.392,
   1.392,
   1.393,
   1.393,
   1.393,
   1.394,
   1.394,
   1.395,
   1.395,
   1.395,
   1.395,
   1.396,
   1.396,
   1.396,
   1.396,
   1.397,
   1.397,
   1.397,
   1.397,
   1.397,
   1.397,
   1.398,
   1.398,
   1.398,
   1.398 };



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n) n

const   struct  skill_type      skill_table     [MAX_SKILL]     =
{

/*
 * Magic spells.
 */

    {
	MORTAL, NORM,      // Prime Attrib,  Apt Needed, Number of skills needed in attrib.
	"reserved",             STAT_NON,        0,      0,
	0,                      TAR_IGNORE,             POS_STANDING,
	NULL,                   SLOT( 0),        0,      0,
	"",                     "",
	"",
    },

    {
	MORTAL, NORM,
	"acid blast",           STAT_INT,       20,     0,
	spell_acid_blast,       TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SLOT(70),       20,     12,
	"acid blast",           "!Acid Blast!", 
	""
    }

};
