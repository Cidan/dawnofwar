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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "ack.h"
#include "tables.h"
#include <math.h>
#include <ctype.h>

void do_sdelete( CHAR_DATA *ch, char *argument )
{
DESCRIPTOR_DATA *d;
char strsave[MAX_INPUT_LENGTH];
char arg1[MAX_INPUT_LENGTH];
char *pArg;
char cEnd;
char buf[MAX_INPUT_LENGTH];
char modbuf[MSL];
char nums[MSL];
CLAN_DATA *clan;

	if ( IS_NPC(ch) )
	    return;
strcpy(buf,ch->name);
sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( buf ), "/", capitalize( buf ) );

	pArg = arg1;
	while ( isspace(*argument) )
	    argument++;
	    
	cEnd = ' ';
	if ( *argument =='\'' || *argument == '"' )
	    cEnd = *argument++;
	    
	while ( *argument != '\0' )
	{
	    if ( *argument == cEnd )
	    {
	        argument++;
	        break;
	    }
	    *pArg++ = *argument++;
	}
	*pArg = '\0';
	
	if (  ( ch->pcdata->pwd != '\0' )
	   && ( arg1[0] == '\0' )  )
	{
	   send_to_char( "Syntax: pdelete <password>.\n\r", ch );
	   return;
	}
	if (  ( ch->pcdata->pwd != '\0' )
	   && ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )  )
	{
	   WAIT_STATE( ch, 40 );
	   send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	   return;
	}

	if (ch->pcdata->clan != 0)
	{
		for( clan = first_clan; clan != NULL; clan = clan->next)
         if( clan->clan_numb == ch->pcdata->clan)
			 break;
		 if (get_char_world(ch, clan->leader) == ch)
		 {
			 sprintf(nums, "%d", clan->clan_numb);
			 raw_delclan(ch, nums);
		 } else
		 {
            sprintf(modbuf, "-%s", ch->name);
            clan->members = str_mod(clan->members, modbuf);
            fix_hack(clan);
		 }
	}


unlink(strsave);
send_to_char("Character deleted.\n\r",ch);

    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d != NULL )
        close_socket( d );
}


void do_nuke( CHAR_DATA *ch, char *argument )
{
char strsave[MAX_INPUT_LENGTH];
char arg[MAX_INPUT_LENGTH];
char buf[MAX_STRING_LENGTH];
char modbuf[MSL];
char nums[MSL];
CHAR_DATA *victim;
CLAN_DATA *clan;
   one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:nuke  <character name> \n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not logged on.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
        send_to_char( "Can't nuke your self. \n\r", ch );
	  return;
    }
    if ( victim->level >= ch->level )
    {
      send_to_char( "Can't nuke someone who's your elder :( \n\r", ch );
      return;
    }
   if (IS_NPC(victim))
    {
     send_to_char( "This command only works on players.\n\r", ch);
      return;
    }

	if (victim->pcdata->clan != 0)
	{
		for( clan = first_clan; clan != NULL; clan = clan->next)
         if( clan->clan_numb == victim->pcdata->clan)
			 break;
		 if (get_char_world(ch, clan->leader) == victim)
		 {
			 sprintf(nums, "%d", clan->clan_numb);
			 raw_delclan(ch, nums);

		 } else
		 {
            sprintf(modbuf, "-%s", victim->name);
            clan->members = str_mod(clan->members, modbuf);
            fix_hack(clan);
		 }
	}

strcpy(buf,victim->name);
sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( buf ), "/", capitalize( buf ) );
sprintf( buf,"%s has been purged by %s!",victim->name,ch->name);

info( buf, 1 );
do_quit(victim,"");	
unlink(strsave);
}
