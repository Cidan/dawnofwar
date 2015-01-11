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
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "ack.h"
#define NOWHERE -1

char *const distance[4]=
{
"is right here.", "is nearby.", "is not far.", "is off in the distance."
};
char *const filler[4]=
{
"","{y%s{x: ","{y%s{x: ","{y%s{x: "
};

void scan_list           args((ROOM_INDEX_DATA *scan_room, CHAR_DATA *ch,
                               sh_int depth, sh_int door));
void scan_char           args((CHAR_DATA *victim, CHAR_DATA *ch,
                               sh_int depth, sh_int door));


bool    check_valid_ride ( CHAR_DATA *  ch );
bool        deathmatch;
char *  const	compass_name	[]		=
{
    "north", "east", "south", "west", "up", "down"
};

char *  const   dir_name        []              =
{
    "to the North", "to the East", "to the South", "to the West", "Up", "Down"
};

/* rev_name used to indicate where ch has come from when entering -S- */

char * const    rev_name        []              =
{
   "the South", "the West", "the North", "the East", "Below", "Above"
};


const   sh_int  rev_dir         []              =
{
    2, 3, 0, 1, 5, 4
};

const   sh_int  movement_loss   [SECT_MAX]      =
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6, 1
};



/*
 * Local functions.
 */
int     find_door       args( ( CHAR_DATA *ch, char *arg ) );
bool    has_key         args( ( CHAR_DATA *ch, int key ) );
/*
struct fol_data
{
  struct fol_data *next;
  CHAR_DATA *ch;
};

struct fol_data *fol_stack;
struct fol_data *fol_free;

*/

void move_char( CHAR_DATA *ch, int door )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    CHAR_DATA *ch2;
    char buf[MAX_STRING_LENGTH];
    char gbuf[MSL];
    char move_buf[MAX_STRING_LENGTH];
/*    struct fol_data *fol = NULL;   */
    char tmp[MAX_STRING_LENGTH];
    int need_movement;
    char door_name_leave[MSL];
    char door_name_enter[MSL];
    int dam = 0;
    OBJ_DATA * weapon = NULL;
	int curvnum;
	char buff2[MSL];
    AREA_DATA *area;
    bool have_rooms = FALSE;

    buf[0] = '\0';
    move_buf[0] = '\0';
    door_name_leave[0] = '\0';
    door_name_leave[0] = '\0';
    tmp[0] = '\0';

    if ( door < 0 || door > 5 )
    {
	bug( "Do_move: bad door %d.", door );
        ch->using_named_door = FALSE;
	return;
    }
    if ( IS_NPC( ch ) )
      ch->using_named_door = TRUE;

	    in_room = ch->in_room;
		if (!IS_NPC(ch))
        if ( ch->act_build == ACT_BUILD_REDIT && ch->pcdata->autobuild == TRUE && IS_IMMORTAL(ch) )
		/* Every god damned safeguard... */
		{
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->to_room      ) == NULL 
    ||   (  ( str_cmp( pexit->keyword, ""   ) )
         && ( ch->using_named_door == FALSE   ) )   )
  {

	            area = ch->in_room->area;
	           for ( curvnum = area->min_vnum; curvnum <= area->max_vnum; curvnum++ )
			   {
		           if ( get_room_index( curvnum ) != NULL )
			       continue;
					 sprintf(buff2, "dig %s %d", compass_name[door], curvnum);
                     build_interpret(ch,buff2);
					 send_to_char("{Y***{RAUTOBUILD IS ENABLED!{Y***{x\n\r", ch);
					 have_rooms = TRUE;
					 break;
			   }
		  
	} else
	{
		send_to_char("{YYou go that way because there is already an exit there!{x\n\r", ch);
		return;
	}
		}
	  if(!IS_NPC(ch))
      if (!have_rooms && ch->pcdata->autobuild == TRUE && ch->act_build == ACT_BUILD_REDIT && IS_IMMORTAL(ch))
	  {
       send_to_char("{YYOU HAVE NO MORE VNUMS TO BUILD WITH. MOVING HALTED TILL YOU\n\rTURN OFF AUTOBUILD!{x\n\r", ch);
	   return;
	  }

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->to_room      ) == NULL 
    ||   (  ( str_cmp( pexit->keyword, ""   ) )
         && ( ch->using_named_door == FALSE   ) )   )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
    if(!IS_NPC(ch))
	if(ch->pcdata->autobuild)
    send_to_char("\n\r{Y***{RAUTOBUILD IS ENABLED!{Y***{x\n\r", ch);
        ch->using_named_door = FALSE;
	return;
    }
    if ( str_cmp( pexit->keyword, "" ) )
    {
      if ( pexit->keyword[0] == '^' )
      {
        sprintf( door_name_leave, "through the %s", pexit->keyword + 1);
        sprintf( door_name_enter, "the %s", pexit->keyword + 1);
      }
      else
      {
        sprintf( door_name_leave, "through the %s", pexit->keyword );
        sprintf( door_name_enter, "the %s", pexit->keyword );
      }
 
    }
    else
    {
      sprintf( door_name_leave, "%s", dir_name[door] );
      sprintf( door_name_enter, "%s", rev_name[door] );
    }

    if (    IS_SET( in_room->affected_by, ROOM_BV_ENCAPS )
         || IS_SET( to_room->affected_by, ROOM_BV_ENCAPS ) )
    {
      if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_INTELLIGENT )  )
      {
        if ( ch->mana > mana_cost( ch, skill_lookup( "room dispel" ) )  )
          do_cast( ch, "room dispel" );
      }
      else
      {
         send_to_char( "A barely visible energy web stops your movement!\n\r", ch );
         ch->using_named_door = FALSE; return;
      }
    }
    if ( IS_SET( in_room->affected_by, ROOM_BV_HOLD ) )
    {
      if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_INTELLIGENT )  )
      {
        if ( ch->mana > mana_cost( ch, skill_lookup( "room dispel" ) )  )
          do_cast( ch, "room dispel" );
      }
      else
      {
        send_to_char( "A fleeting vision of bars appears before the exit, and stops your movement!\n\r", ch );
        ch->using_named_door = FALSE; return;
      }
    }

    if ( IS_SET(pexit->exit_info, EX_CLOSED) )
    {
       if ( !IS_AFFECTED(ch, AFF_PASS_DOOR) && ( !item_has_apply( ch, ITEM_APPLY_PASS_DOOR ) ) )
       {
          if ( IS_SET( pexit->exit_info, EX_NODETECT ) && !IS_IMMORTAL( ch ) )
            send_to_char( "Alas, you cannot go that way.\n\r", ch );
          else if ( !IS_IMMORTAL( ch ) )
          {
           act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR ); 
          ch->using_named_door = FALSE; return;
          }
       }
       else if ( IS_SET( pexit->exit_info, EX_PASSPROOF ) && !IS_IMMORTAL( ch ) )
       {
          if ( IS_SET( pexit->exit_info, EX_NODETECT ) && !IS_IMMORTAL( ch ) )
            send_to_char( "Alas, you cannot go that way.\n\r", ch );
          else if ( !IS_IMMORTAL( ch ) )
          {
          act( "You don't seem to be able to pass through $d.", ch, NULL, pexit->keyword, TO_CHAR );
          ch->using_named_door = FALSE; return;
          }
         } 
        }    
    for ( ch2 = ch->in_room->first_person; ch2 != NULL;
          ch2 = ch2->next_in_room )
        {
         if ( ( IS_IMMORTAL(ch) || IS_NPC(ch) ) )
           continue;

         if ( ( !(str_cmp (rev_spec_lookup(ch2->spec_fun), "spec_thant_guard") || !(str_cmp (rev_spec_lookup(ch2->spec_fun), "spec_thant_road") ) ) ) && ch->hometown != 1  )
             {
             sprintf( gbuf, "Oh no! %s is blocking your path!\n\r", ch2->name);
             send_to_char( gbuf, ch );
             return;
             }
         if ( ( !(str_cmp (rev_spec_lookup(ch2->spec_fun), "spec_azt_guard")  || !(str_cmp (rev_spec_lookup(ch2->spec_fun), "spec_azt_road") ))) && ch->hometown != 2  )
             {
             sprintf( gbuf, "Oh no! %s is blocking your path!\n\r", ch2->name);
             send_to_char( gbuf, ch );
             return;
             }
         if ( ( !(str_cmp (rev_spec_lookup(ch2->spec_fun), "spec_lum_guard")  || !(str_cmp (rev_spec_lookup(ch2->spec_fun), "spec_lum_road") ))) && ch->hometown != 3  )
             {
             sprintf( gbuf, "Oh no! %s is blocking your path!\n\r", ch2->name);
             send_to_char( gbuf, ch );
             return;
             }
         if ( !(str_cmp (rev_spec_lookup(ch2->spec_fun), "spec_oth_guard") ) && ch->hometown != 4  )
             {
             sprintf( gbuf, "Oh no! %s is blocking your path!\n\r", ch2->name);
             send_to_char( gbuf, ch );
             return;
             }

         }
    if ( IS_AFFECTED(ch, AFF_CHARM)
    &&   ch->master != NULL
    &&   in_room == ch->master->in_room )
    {
      send_to_char( "What?  And leave your beloved master?\n\r", ch );
      ch->using_named_door = FALSE; return;
    }

    if ( room_is_private( to_room ) && !IS_IMMORTAL(ch) )
    {
      send_to_char( "That room is private right now.\n\r", ch );
      ch->using_named_door = FALSE; return;
    }
  /* need 1 move if riding - Celestian */
  if (ch->position == POS_RIDING )
  {
    need_movement = 1;
    if (IS_RIDING(ch))
    {
      if (!IS_AWAKE(ch->riding))
      {
        send_to_char("Your mount ignores you completely.\r\n", ch);
        ch->using_named_door = FALSE; return;
      }
      if ((ch->riding)->position != POS_STANDING) {
        act("$N twists $S head around and looks at you quizzically.",
            ch, 0, ch->riding, TO_CHAR);
        act("$N looks quizzically at $n.",
            ch, 0, ch->riding, TO_CHAR);
        ch->using_named_door = FALSE; return;
      }
    } else {
      send_to_char("Bummer, looks like your mount gave you the slip.\r\n", ch);
      ch->position = POS_STANDING;
      update_pos(ch);
      ch->using_named_door = FALSE; return;
    }
  }
  
    
    if ( IS_SET( pexit->exit_info, EX_IMMORTAL ) && !IS_IMMORTAL( ch ) )
    {
       send_to_char( "Only an Immortal may use that exit.\n\r", ch );
       ch->using_named_door = FALSE; return;
    }
    
    
  /*  Int Mobs need to be able to handle climbs  */
    if ( IS_SET( pexit->exit_info, EX_CLIMB ) && IS_NPC(ch) && !IS_AFFECTED( ch, AFF_FLYING )  )
    {
      if  ( ch->mana > mana_cost( ch, skill_lookup( "fly" ) )   )
      {
        do_cast( ch, "fly" );
      }
      else
      {
       send_to_char( "Looks like you'd have to climb up there.\n\r", ch );
       ch->using_named_door = FALSE; return;
      }
    }
    
    if (!IS_NPC(ch) && get_skill(ch,gsn_climb) == 0 
        && IS_SET( pexit->exit_info, EX_CLIMB )
        && !IS_AFFECTED( ch, AFF_FLYING )
        && !item_has_apply( ch, ITEM_APPLY_FLY )   )    
    {
       send_to_char( "You'd need to be able to fly or climb to go that way!\n\r", ch );
       ch->using_named_door = FALSE; return;
    }
    
    
    {
      int iRace;
      int iClan;
      int move;


	/* Check if room is a race-only one... */
	
     for ( iRace = 0; iRace < MAX_RACE; iRace++ )
     {
       if (  ( to_room->vnum == race_table[iRace].race_room )
          && ( iRace != ch->race )
          && ( !IS_IMMORTAL( ch )  )  )
	     {
         send_to_char( "You aren't allowed in there.\n\r", ch );
         ch->using_named_door = FALSE; return;
	     }
     }

	/* Check if room is a clan-only one... */
	
     for ( iClan = 0; iClan < MAX_CLAN; iClan++ )
     {
       if ( to_room->vnum == get_clan_numin(iClan, CROOM) )
       {
         if (  ( IS_NPC( ch ) )
            || (  ( !IS_IMMORTAL( ch ) )
               && ( iClan != ch->pcdata->clan ) ) )
	       {
           send_to_char( "You aren't allowed in there.\n\r", ch );
           ch->using_named_door = FALSE; return;
	       }
       }
     }
	
     if ( to_room->vnum == ROOM_VNUM_BUILDER 
        && ( !IS_IMMORTAL( ch ) && !IS_SET( ch->act, PLR_BUILDER ) ) )
     {
       send_to_char( "The Portal allows entrance to builders only.\n\r", ch );
       ch->using_named_door = FALSE; return;
     }
	
	
     if (  to_room->vnum == ROOM_VNUM_CLAN
        && (  IS_NPC( ch ) 
           || (  ( !IS_IMMORTAL( ch ) )
              && ( !IS_SET( ch->pcdata->pflags, PFLAG_CLAN_BOSS ) ) ) ) )
     {
       send_to_char( "Only Clan Bosses may enter this room.\n\r", ch );
       ch->using_named_door = FALSE; return;
     }
	
	
	
     if ( in_room->sector_type == SECT_AIR
        ||   to_room->sector_type == SECT_AIR  )
     {
       if (  (  ( !IS_AFFECTED(ch, AFF_FLYING) )
             && ( !item_has_apply( ch, ITEM_APPLY_FLY ) ) )
          && (  ( ch->riding != NULL ) 
             && ( !IS_SET( ch->riding->affected_by, AFF_FLYING ) ) ) )
	     {
            
         if (  ( IS_NPC( ch ) )
            && ( IS_SET( ch->act, ACT_INTELLIGENT ) )
            && ( ch->mana > mana_cost( ch, skill_lookup( "fly" ) )  )     )
         {
           do_cast( ch, "fly" );
         }
         else if ( !IS_IMMORTAL( ch ) )
         {
           send_to_char( "You can't fly.\n\r", ch );
           ch->using_named_door = FALSE; return;
         }
       }
     }

     if ( in_room->sector_type == SECT_WATER_NOSWIM
        ||   to_room->sector_type == SECT_WATER_NOSWIM )
     {
       OBJ_DATA *obj;
       bool found;

	    /*
	     * Look for a boat.
	     */
       found = FALSE;

	    /*
	     * Suggestion for flying above water by Sludge
	     */
       if (  ( IS_AFFECTED( ch, AFF_FLYING ) )
          || ( item_has_apply( ch, ITEM_APPLY_FLY) )  )
         found = TRUE;
       if (  ( ch->riding != NULL )
          && ( IS_SET( ch->riding->affected_by, AFF_FLYING ) ) )
         found = TRUE;
       for ( obj = ch->first_carry; obj != NULL; obj = obj->next_in_carry_list )
       {
         if ( obj->item_type == ITEM_BOAT )
         {
           found = TRUE;
           break;
         }
       }
       if ( !found )
       {
         if (  ( IS_NPC( ch ) )
            && ( IS_SET( ch->act, ACT_INTELLIGENT ) )
            && ( ch->mana > mana_cost( ch, skill_lookup( "fly" ) )  )     )
         {
           do_cast( ch, "fly" );
         }
         else if ( !IS_IMMORTAL( ch ) )
         {
           send_to_char( "You need a boat to go there.\n\r", ch );
           ch->using_named_door = FALSE; return;
         }
       }
     }

     move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
          + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
        ;

     if ( IS_AFFECTED( ch, AFF_FLYING )|| item_has_apply( ch, ITEM_APPLY_FLY) )
       move=1;


     if ( ch->move < move )
     {
       send_to_char( "You are too exhausted.\n\r", ch );
       ch->using_named_door = FALSE; return;
     }

	WAIT_STATE( ch, 1 );
    if (!IS_IMMORTAL(ch))
	ch->move -= move;
    }
/* Mount code - Celestian */
    if ( ch->position == POS_RIDING)
    {
      if (check_valid_ride(ch)) {
        sprintf(tmp, "You ride %s on %s.\n\r",
          door_name_leave, ch->riding->short_descr);
        send_to_char(tmp, ch);
        sprintf(tmp, "$n rides %s on %s.",
          door_name_leave, ch->riding->short_descr);
      }
    }

/*    if ( ( !IS_AFFECTED(ch, AFF_SNEAK) && !item_has_apply( ch, ITEM_APPLY_SNEAK ) )
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )  */
    {
       if ( IS_SET( pexit->exit_info, EX_CLIMB ) )  /* dir_name[door] */
	  act( "$n climbs $T.", ch, NULL, door_name_leave, TO_ROOM );
       else
          if ( IS_SET( pexit->exit_info, EX_CLOSED ) )	/* using passdoor */
          {   
             sprintf( buf, "$L$n floats %s.", door_name_leave ); 
             act( buf, ch, NULL, pexit->keyword, TO_ROOM );
          }
          else
          {
            if ( IS_NPC( ch ) 
            || (  !IS_NPC( ch ) 
               && (  IS_WOLF( ch ) 
                  && ( IS_SHIFTED( ch ) || IS_RAGED( ch ) ) ) ) )
              sprintf( move_buf, "$L$n %s $T.", "wanders" );
            else if ( ch->riding == NULL )
            {
              sprintf( move_buf, "$L%s$n %s $T.",
                get_ruler_title( ch->pcdata->ruler_rank, ch->login_sex ),
                ch->pcdata->room_exit );
            }
            else if ( ch->riding != NULL )
            {
              sprintf( move_buf, "$L%s$n rides $T on %s.",
                get_ruler_title( ch->pcdata->ruler_rank, ch->login_sex ),
                ch->riding->short_descr );
            }
            act( move_buf, ch, NULL, door_name_leave, TO_ROOM );
          }
    }
    char_from_room( ch );
    char_to_room( ch, to_room );
/* Mount code - Celestian */
    if (IS_RIDING(ch))
    {
      char_from_room( ch->riding );
      char_to_room( ch->riding, to_room );
      if ((ch->riding)->in_room->vnum == NOWHERE)
      {
      ch->using_named_door = FALSE; return;
      }
    }

 /*   if ( ( !IS_AFFECTED(ch, AFF_SNEAK) && !item_has_apply( ch, ITEM_APPLY_SNEAK ) )
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )   */
    {
            if ( IS_NPC( ch ) 
            || (  !IS_NPC( ch ) 
               && (  IS_WOLF( ch ) 
                  && ( IS_SHIFTED( ch ) || IS_RAGED( ch ) ) ) ) )
        sprintf( move_buf, "$L$n %s $T.", "wanders in from" );
            else if ( ch->riding == NULL )
            {
              sprintf( move_buf, "$L%s$n %s $T.",
                get_ruler_title( ch->pcdata->ruler_rank, ch->login_sex ),
                ch->pcdata->room_enter );
            }
            else if ( ch->riding != NULL )
            {
              sprintf( move_buf, "$L%s$n rides in from $T on %s.",
                get_ruler_title( ch->pcdata->ruler_rank, ch->login_sex ),
                ch->riding->short_descr );
            }
            act( move_buf, ch, NULL, door_name_enter, TO_ROOM );
          
    }


    if ( in_room != to_room )
    {
      for ( fch = in_room->first_person; fch != NULL; fch = fch_next )
      {
        fch_next = fch->next_in_room;
        if ( fch->master == ch && fch->position == POS_STANDING )
        {
           act( "You follow $N $t.", fch, dir_name[door], ch, TO_CHAR );
            fch->using_named_door = TRUE;
           move_char( fch, door );
        }
      }
    }
    do_look( ch, "auto" );

   if ( (  weapon = get_eq_char( ch, WEAR_LODGED ) ) != NULL )
       {
       dam  =  dice(( weapon->value[1]), (2 * weapon->value[2]));
       send_to_char( "The item lodged in your body twists around!\n\r", ch );
       damage( ch, ch, dam, TYPE_UNDEFINED );
       }
    if ( !IS_NPC( ch ) && IS_VAMP( ch ) && !IS_IMMORTAL( ch ) )
       check_vamp( ch );	/* burn the vampire! */
    mprog_entry_trigger(ch);
    mprog_greet_trigger(ch);
	  if(!IS_NPC(ch))
	if (ch->pcdata->autobuild)
		send_to_char("{Y***{RAUTOBUILD IS ENABLED!{Y***{x\n\r", ch);
    ch->using_named_door = FALSE;
    return;
}



void do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTH );
    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST );
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH );
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST );
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN );
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

	 if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET(pexit->exit_info, EX_ISDOOR)
	    &&   pexit->keyword != NULL
	    &&   is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if (  ( ( pexit = ch->in_room->exit[door] ) == NULL )
       || ( str_cmp( pexit->keyword, "" ) && !is_name( arg, pexit->keyword ) )  )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *ppl;
    AFFECT_DATA af;
    int door;
    int sn;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'open object' */
        switch ( obj->item_type )
        {
        default:
         send_to_char( "That's not a container.\n\r", ch );
         return;
        case ITEM_CONTAINER:
        case ITEM_RANDOM_CHEST:
            break;
        }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	act( "You open $p.", ch, obj, NULL, TO_CHAR );
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );


    if ( obj->item_type == ITEM_RANDOM_CHEST && obj->trapped == TRUE)
    {
       if (IS_NPC(ch)) /* NPC's get insta killed and trap gets reset. :D */
       {
                send_to_char("Naughty Naughty, NPCs can't open traped chests. :D\n\r", ch);
                act( "A blade flies out and decapitates $n {RINSTANTLLY!{x", ch, NULL, NULL, TO_ROOM );
                raw_kill(ch, "");
                act( "$p resets it self with a click.", NULL, obj, NULL, TO_ROOM );
                SET_BIT(obj->value[1], CONT_CLOSED);
                return;
       }
       if (!IS_IMMORTAL(ch)) /* Imm's can't get hit by traps */
        switch (number_range(0, 10) )
        {
        default:
            break;
        case 0: /* No trap */
            break;
        case 1: /* Insta death, ouch. */
            send_to_char("A blade flies out, decapitating you {RINSTANTLLY{x!\n\r", ch);
            act( "A blade flies out and decapitates $n {RINSTANTLLY!{x", ch, NULL, NULL, TO_ROOM );
            raw_kill(ch, "");
            do_look(ch, "");
            break;
        case 2: /* Poison the whole room */
            act( "A {Gpoison{x cloud engulfs the room!", ch, NULL, NULL, TO_CHAR);
            act( "A {Gpoison{x cloud engulfs the room!", ch, NULL, NULL, TO_ROOM);
            sn = skill_lookup("poison");
            for (ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room)
            {
            af.type      = sn;
            af.duration  = 10;
            af.modifier  = 20;
            af.location  = APPLY_AC;
            af.bitvector = 0;
            affect_to_char( ppl, &af );
            }
            break;

        }
        obj->trapped = FALSE;
    }
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->first_person; rch != NULL; rch = rch->next_in_room )
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT(obj->value[1], CONT_CLOSED);
	send_to_char( "Ok.\n\r", ch );
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit   = ch->in_room->exit[door];
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->first_person; rch != NULL; rch = rch->next_in_room )
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->first_carry; obj != NULL; obj = obj->next_in_carry_list )
    {
	if ( obj->pIndexData->vnum == key )
	    return TRUE;
    }

    return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit   = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* unlock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
	    act( "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    if ( !IS_NPC(ch) && number_percent( ) > get_skill(ch,gsn_pick_lock) )
    {
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* pick the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

	send_to_char( "You wake and stand up.\n\r", ch );
	act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_RESTING:
	send_to_char( "You stand up.\n\r", ch );
	act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
	if ( ch->sitting != NULL )
	{
	   ch->sitting->value[1]--;
	   ch->sitting = NULL;
	}   
	ch->position = POS_STANDING;
	break;

    case POS_SITTING:
	send_to_char( "You stand up.\n\r", ch );
	act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
	if ( ch->sitting != NULL )
	{
	   ch->sitting->value[1]--;
       if (ch->sitting->value[1] == 0)
           ch->sitting->sat = FALSE;
       ch->sitting = NULL;
           
	}   
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    case POS_RIDING:
	send_to_char( "Dismount first!\n\r", ch );
	break;

    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    
    OBJ_DATA *furn=NULL;
    bool sit;
    
    sit = FALSE;
    
    if ( argument[0] != '\0' )
    {
       furn = get_obj_here( ch, argument );
       if ( furn == NULL || furn->item_type != ITEM_FURNITURE )
       {
          send_to_char( "You can't rest on that!\n\r", ch );
          return;
       }
       if ( furn->value[1] >= furn->value[0] )
       {
          send_to_char( "It's full already!\n\r", ch );
          return;
       }
       sit = TRUE;
       
    }
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	if ( sit==FALSE )
	{
	   send_to_char( "You rest.\n\r", ch );
	   act( "$n rests.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
           ch->sitting = furn;
           act( "$n rests on $p", ch, furn, NULL, TO_ROOM );
           act( "You rest on $p", ch, furn, NULL, TO_CHAR );
           ch->sitting->value[1]++;
           ch->sitting->sat = TRUE;
        }
        ch->position = POS_RESTING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    case POS_RIDING:
	send_to_char( "Dismount first!\n\r", ch );
	break;

    }

    return;
}

void do_sit( CHAR_DATA *ch, char *argument )
{
    
    OBJ_DATA *furn=NULL;
    bool sit;
    
    sit = FALSE;
    
    if ( argument[0] != '\0' )
    {
       furn = get_obj_here( ch, argument );
       if ( furn == NULL || furn->item_type != ITEM_FURNITURE )
       {
          send_to_char( "You can't sit on that!\n\r", ch );
          return;
       }
       if ( furn->value[1] >= furn->value[0] )
       {
          send_to_char( "There's no room!\n\r", ch );
          return;
       }
       sit = TRUE;
       
    }
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You're already sleeping, remember?\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	if ( sit==FALSE )
	{
	   send_to_char( "You sit.\n\r", ch );
	   act( "$n sits.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
           ch->sitting = furn;
           act( "$n sit on $p.", ch, furn, NULL, TO_ROOM );
           act( "You sit on $p.", ch, furn, NULL, TO_CHAR );
           ch->sitting->value[1]++;
           ch->sitting->sat = TRUE;
        }
        ch->position = POS_SITTING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    case POS_RIDING:
	send_to_char( "Dismount first!\n\r", ch );
	break;

    }

    return;
}


void do_sleep( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_STANDING: 
	send_to_char( "You sleep.\n\r", ch );
	act( "$n sleeps.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    case POS_RIDING:
	send_to_char( "Dismount first!\n\r", ch );
	break;

    }

    return;
}


void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

    act( "You wake $M.", ch, NULL, victim, TO_CHAR );
    act( "$n wakes you.", ch, NULL, victim, TO_VICT );
     if ( victim->sitting != NULL )
	{
	   victim->sitting->value[1]--;
       if (victim->sitting->value[1] == 0)
           victim->sitting->sat = FALSE;
       victim->sitting = NULL;
           
	}
    victim->position = POS_STANDING;
    return;
}




void do_shadowform( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    send_to_char( "You attempt to take on shadow form.\n\r", ch );
    act( "$n tries to take on shadow form.", ch, NULL, NULL, TO_ROOM );
    affect_strip( ch, gsn_sneak );

    if ( IS_NPC(ch) || number_percent( ) < get_skill(ch,gsn_shadow) )
    {
	af.type      = gsn_sneak;
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }

    return;
}

void do_sneak( CHAR_DATA *ch, char *argument )
{
/*
    AFFECT_DATA af;
*/
    if ( IS_SET(ch->affected_by, AFF_SNEAK) )
       {
        send_to_char( "You no longer are trying to move silently.\n\r", ch );
        REMOVE_BIT(ch->affected_by, AFF_SNEAK);
        return;
       }

    send_to_char( "You attempt to move silently.\n\r", ch );
    if ( IS_NPC(ch) || number_percent( ) < get_skill(ch,gsn_sneak) )
    {
	SET_BIT(ch->affected_by, AFF_SNEAK);
    }
    return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj; 
  char arg[MAX_INPUT_LENGTH];  

   argument = one_argument(argument, arg );

   if(arg[0] != '\0')
    {
     if(( obj = get_obj_carry(ch,arg)) == NULL)
     {
     send_to_char("You don't have that item.\n\r",ch);
     return;
     }
    obj_from_char(obj);
    SET_BIT(obj->extra_flags,ITEM_HIDDEN);
    obj_to_room(obj,ch->in_room);
    act("$n hides $p.",ch,obj,NULL,TO_ROOM);
    act("You hide $p.",ch,obj,NULL,TO_CHAR);
	/* Bitchin, finally made it so hidden objects on the
	 * ground save, and then load up again on reboot!
	 */
    return;
   }
 else
    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if ( IS_NPC(ch) || number_percent( ) < get_skill(ch,gsn_hide) )
	SET_BIT(ch->affected_by, AFF_HIDE);

    return;
}



/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis                        );
    affect_strip ( ch, gsn_mass_invis                   );
    affect_strip ( ch, gsn_sneak                        );
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE            );
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE       );
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK           );
    send_to_char( "You become more visible again.\n\r", ch );
    return;
}

void do_clan_recall( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *location;
   
   if ( IS_NPC( ch ) )
      return;
 
   if (deathmatch)
   { 
      send_to_char("Not during a DEATHMATCH!!!\n\r", ch );
      return;
    }

   if ( ch->pcdata->clan == 0 )
   {
      send_to_char( "You must be in a clan to use this command!!\n\r", ch );
      return;
   }
   else
      location = get_room_index( get_clan_numin(ch->pcdata->clan, CROOM) );
      
   if ( location == NULL )
   {
      send_to_char( "You are well lost, like.\n\r", ch );
      return;
   }
   
    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE) )
    {
	send_to_char( "Some strange force prevents your transport.\n\r", ch );
	return;
    }
   
   if ( ch->in_room == location )
   {
      send_to_char( "You didn't have far to go, did you?\n\r", ch );
      return;
   }
   
   if ( ch->fighting != NULL )
   {
      send_to_char( "You can't use this command when fighting!!\n\r", ch );
      return;
   }
   
   act( "$n is engulfed in a stream of red light!!", ch, NULL, NULL, TO_ROOM );
   send_to_char( "You are engulfed in a stream of red light, carrying you home!\n\r", ch );
   char_from_room( ch );
   char_to_room( ch, location );
    if ( ch->riding != NULL )
    {
      char_from_room( ch->riding );
      char_to_room( ch->riding, location );
    }

   act( "$n arrives, carried upon a stream of red light!!", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return;
}
   
void do_recall( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    int recall_index;
    act( "$n prays for transportation!", ch, 0, 0, TO_ROOM );
    buf[0] = '\0';


    if ( IS_AFFECTED(ch, AFF_NOQUIT) && !IS_IMMORTAL(ch) )
       {
        send_to_char( "You are no quit!\n\r", ch );
        return;
       }
    /* FIXME: Need to check if NPC is charmed - if so find
     * owner's recall location..... -S- */
    
    /* if ( IS_NPC( ch ) || ch->level < 6 ) */
    
       if ( IS_VAMP( ch ) )
          location = get_room_index( VAMPIRE_RECALL );
       else 
          location = get_room_index( hometown_table[ch->hometown].recall );

    if ( !IS_NPC( ch )  )
      location = get_room_index( hometown_table[ch->hometown].recall );
       
       if ( deathmatch )
       {
          recall_index = number_range( ROOM_VNUM_DM_RECALL, ROOM_VNUM_DM_RECALL + 35);
          location = get_room_index( recall_index );
          
          if ( ch->hit > 250 )
          {
            send_to_char( "{BYou wince in pain as you feel your molecules disperse, then reform.{x\n\r", ch );
            ch->hit -= 200;
          }
          else
          {
            send_to_char( "Your molecules begin to disperse, then reform quickly, leaving you here.\n\r", ch );
            return;
          }

        }
          
       
       /* Only recall to midgaard now... */
    /*
    else
       location = get_room_index( race_table[ch->race].recall );
    */
    

    if ( location == NULL )
    {
	send_to_char( "Eeep, you have no recall location for some reason! Contact an Immortal ASAP!\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE) )
    {
	send_to_char( "Some strange force prevents your recall.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) != NULL )
    {
      send_to_char( "You can't recall in battle! \n\r", ch );
      return;
    }
/*
	int lose;

	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    lose = ( ch->level / 4 ) +1;
	    lose = UMIN( lose, ch->exp );
	    gain_exp( ch, 0 - lose );
	    sprintf( buf, "You failed!  You lose %d exps.\n\r", lose );
	    send_to_char( buf, ch );
	    return;
	}

	lose = ( ch->level / 4 ) +25;
	lose = UMIN( lose, ch->exp );
	gain_exp( ch, 0 - lose );
	sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }
*/
    ch->move = (ch->move > 10) ? ( ch->move - 10 ) : 0;
    act( "$n is engulfed by a stream of blue light!", ch, NULL, NULL, TO_ROOM );
    
    char_from_room( ch );
    char_to_room( ch, location );
    if ( ch->riding != NULL )
    {
      char_from_room( ch->riding );
      char_to_room( ch->riding, location );
    }
    
       act( "$n arrives, carried by a stream of blue light!", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    return;
}


void do_train( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[20];
    CHAR_DATA *mob;
    int hp_gain = 0;
    int mana_gain = 0;
    sh_int *pAbility;
    int pMax = 0;
	bool prime = FALSE;
    char *pOutput;
    int cost,cost1,cost2,cost3,cost4,cost5;  /* Urrgghh */
    buf[0] = '\0';
    buf2[0] = '\0';

 
    if ( IS_NPC(ch) )
    {
	send_to_char( "Not for NPCs.  Sorry.\n\r", ch );
	return;
    }
 
    /*
     * Check for trainer.
     */
    
    for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
      {
	if ( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) )
	      break;
      }
 
    if ( mob == NULL )
      {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
      }
 
    if ( argument[0] == '\0' )
      {
	sprintf( buf, "You have %d training sessions.\n\r", ch->train );
	send_to_char( buf, ch );
	argument = "foo";
      }


    cost = 1;
 
    if ( !str_cmp( argument, "str" ) )
      {
	pAbility    = &ch->pcdata->perm_str;
	pMax        = ch->pcdata->max_str;
	pOutput     = "strength";
      }
 
    else if ( !str_cmp( argument, "int" ) )
      {
	pAbility    = &ch->pcdata->perm_int;
	pMax        = ch->pcdata->max_int;
	pOutput     = "intelligence";
      }
 
    else if ( !str_cmp( argument, "wis" ) )
      {
	pAbility    = &ch->pcdata->perm_wis;
	pMax        = ch->pcdata->max_wis;
	pOutput     = "wisdom";
      }
 
    else if ( !str_cmp( argument, "dex" ) )
      {
	pAbility    = &ch->pcdata->perm_dex;
	pMax        = ch->pcdata->max_dex;
	pOutput     = "dexterity";
      }
 
    else if ( !str_cmp( argument, "con" ) )
      {
	pAbility    = &ch->pcdata->perm_con;
	pMax        = ch->pcdata->max_con;
	pOutput     = "constitution";
      }

	else if ( !str_cmp( argument, "luck" ) )
      {
	pAbility    = &ch->pcdata->perm_lck;
	pMax        = ch->pcdata->max_lck;
	pOutput     = "luck";
      }
 
    else if ( !str_cmp( argument, "hp" ) )
      {
	pAbility = &ch->max_hit;
	pOutput = "number of hit points";
	cost = 1;    /* this is pracs per "train hp" */
	hp_gain = 10; /* this is hp gained per "train hp" */
      }
 
    else if ( !str_cmp( argument, "mana" ) )
      {
	pAbility = &ch->max_mana;
	pOutput = "amount of mana";
	cost =1;
	mana_gain = 10;
      }
 
    else
      {
	/* Work out the costs... */
	cost1=1 ;cost2=1 ;cost3=1 ;cost4=1 ;cost5=1;        
	
	strcpy( buf, "You can train: " );
	
	if ( ch->pcdata->perm_str < ch->pcdata->max_str ) 
	    strcat( buf, "str" );
	if ( ch->pcdata->perm_int < ch->pcdata->max_int ) 
	    strcat( buf, " int" );
	if ( ch->pcdata->perm_wis < ch->pcdata->max_wis ) 
	    strcat( buf, " wis" );
	if ( ch->pcdata->perm_dex < ch->pcdata->max_dex ) 
	    strcat( buf, " dex" );
	if ( ch->pcdata->perm_con < ch->pcdata->max_con ) 
	    strcat( buf, " con" );
	if ( ch->pcdata->perm_lck < ch->pcdata->max_lck ) 
	    strcat( buf, " luck" );
	    strcat( buf, " hp mana" );

	if ( buf[strlen(buf)-1] != ':' )
	  {
		strcat( buf, ".\n\r" );
		    send_to_char( buf, ch );
	      }
	 return;
      }  
   
      
 
    if ( !str_cmp( argument, "hp" ) )
      {
      
	if ( cost > ch->train )
	  {
	    send_to_char( "You don't have enough trains.\n\r", ch );
	    return;
	  }
 
	     ch->train        -= cost;
	     *pAbility           += hp_gain;
             ch->pcdata->hp_from_gain += 10;
	     act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
	     act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
	     return;
      }
 
if ( !str_cmp( argument, "mana" ) )
  {
 
	if ( cost > ch->train )
	  {
	    send_to_char( "You don't have enough trains.\n\r", ch );
	    return;
	  }
 
	     ch->train        -= cost;
	     *pAbility           += mana_gain;
             ch->pcdata->mana_from_gain += 10;

	     act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
	     act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
	     return;
      }
    if ( *pAbility >= pMax )
      {
	act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
      }
 
    if ( cost > ch->train )
      {
	send_to_char( "You don't have enough trains.\n\r", ch );
	return;
      }
 
    ch->train -= cost;
	
	if (prime)
      *pAbility+= 2;
	else
	  *pAbility+= 1;
	act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
    return;
  }



void do_guild( CHAR_DATA *ch, char *argument )
{
    send_to_char( "This command is no longer employed here!\n\r", ch );
    return;
}



void do_halls( CHAR_DATA *ch, char *argument )

    /* Quite a lot of the following code derives from the original
     * MERC2.2 code, although the tweaks are my own.
     * -- Stephen
     */

{

    CHAR_DATA *victim;

  
    /* Remember to limit use of this command to immorts! */    
    
    if (ch->level < 102)
    {
       send_to_char( "Huh?\n\r", ch);
       return;
    }
    
    act( "$n prepares to return to the Halls of the Immortals!", ch, 0, 0, TO_ROOM );

    /* Check is immort is fighting.  
     * Stop fight if they are!
     */

    if ( ( victim = ch->fighting ) != NULL )
    {
	stop_fighting( ch, TRUE );
    }
    
      act( "$n vanishes into a beam of energy!", ch, NULL, NULL, TO_ROOM );
      char_from_room( ch );
      char_to_room( ch, get_room_index(1015) );
      act( "$n steps out of the energy beam!", ch, NULL, NULL, TO_ROOM );
    
    do_look( ch, "auto" );
    return;
}


void do_smash( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int door;
    int best;
    bool joke;		/* Was it unlocked to start with? */
    
    joke = FALSE;
    one_argument( argument, arg );
    
    best = -1;

    if ( !IS_NPC( ch ) )
    best = get_skill(ch, gsn_smash);

    if ( best == -1 || best == 0 )
    {
	send_to_char(
	    "You don't know of such a skill.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Smash what?\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	
	if ( ( 2*number_percent() > get_skill(ch,gsn_smash) ) 
	   || ( IS_SET(pexit->exit_info, EX_SMASHPROOF) ) )
	{
	   act( "$n tries to smash the $d, but fails.", ch, NULL, pexit->keyword, TO_ROOM );
	   send_to_char( "You failed!\n\r", ch );
	   return;
	}
	
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    REMOVE_BIT( pexit->exit_info, EX_LOCKED );
        else
           joke = TRUE;
           
	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n smashes opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	act( "You smash open the $d.", ch, NULL, pexit->keyword, TO_CHAR );

        if ( joke )
        {
           act( "The $d was already unlocked!!!", ch, NULL, pexit->keyword, TO_ROOM );
           act( "The $d was already unlocked!!!", ch, NULL, pexit->keyword, TO_CHAR );
           act( "$n tumbles through $d!!!", ch, NULL, pexit->keyword, TO_ROOM );
           act( "You tumble through $d!!!", ch, NULL, pexit->keyword, TO_CHAR );
              
        }



	/* open the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;
	
	    if (  IS_SET(pexit_rev->exit_info, EX_LOCKED) )
	       REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->first_person; rch != NULL; rch = rch->next_in_room )
		act( "The $d is smashed open.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	
	    if ( joke )
	    {
	       char_from_room( ch );
	       char_to_room( ch, to_room );
	       act( "$n tumbles into the room, head over heels!", ch, NULL, NULL, TO_ROOM );
	       do_look( ch, "" );
	    }	
	
	}
    }

    return;
}

void do_scan(CHAR_DATA *ch, char *argument)
{
   extern char *const dir_name[];
   char arg1[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *scan_room;
   EXIT_DATA *pExit;
   sh_int door, depth;

   argument = one_argument(argument, arg1);

   if (arg1[0] == '\0')
   {

      act("$n scans around.", ch, NULL, NULL, TO_ROOM);

      send_to_char("Scanning:\n\r", ch);
                scan_list(ch->in_room, ch, 0, -1);

      for (door=0;door<6;door++)
      {
         if ( (pExit = ch ->in_room->exit[door]) != NULL)
 /* Make sure they can not scan through walls and doors but can through
  * windows and arrowslits, Marcus */
            scan_list(pExit->to_room, ch, 1, door);
      }
      return;
   }
   else if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north")) door = 0;
   else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))  door = 1;
   else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south")) door = 2;
   else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))  door = 3;
   else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up" ))   door = 4;
   else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))  door = 5;
   else { send_to_char("Which way do you want to scan?\n\r", ch); return; }

   act("You peer intently $T.", ch, NULL, dir_name[door], TO_CHAR);
   act("$n peers intently $T.", ch, NULL, dir_name[door], TO_ROOM);
   sprintf(buf, "Looking %s you see:\n\r", dir_name[door]);

   scan_room = ch->in_room;

   for (depth = 1; depth < 4; depth++)
   {
      if ( (pExit = scan_room->exit[door]) != NULL && !IS_SET(pExit->exit_info,EX_CLOSED) )
      {
         scan_room = pExit->to_room;
         scan_list(pExit->to_room, ch, depth, door);
      }
   }
   return;
}

void scan_list(ROOM_INDEX_DATA *scan_room, CHAR_DATA *ch, sh_int depth,
               sh_int door)
{
   CHAR_DATA *rch;

   if (scan_room == NULL) return;
   for (rch=scan_room->first_person; rch != NULL; rch=rch->next_in_room)
   {
      if (rch == ch) continue;
      if (can_see(ch, rch))
	{
		scan_char(rch, ch, depth, door);
   	}
   }
   return;
}

void scan_char(CHAR_DATA *victim, CHAR_DATA *ch, sh_int depth, sh_int door)
{
   extern char *const distance[];
   char buf[MAX_INPUT_LENGTH];
  /* char buf2[MAX_INPUT_LENGTH];
   */
   buf[0] = '\0';
   sprintf(buf, filler[depth], compass_name[door]);

	
   strcat(buf, PERSON(ch, victim));

   strcat(buf," ");
   strcat(buf, distance[depth]);
   strcat(buf,"\n\r");

   send_to_char(buf, ch);
   return;
}


void do_enter( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *portal;
    ROOM_INDEX_DATA *to_room;
    OBJ_DATA *foo;
    OBJ_DATA * foo_next;
    int	door;
    
    if ( argument[0] == '\0' )
    {
       send_to_char( "Enter what?\n\r", ch );
       return;
    }
    
    portal = get_obj_here( ch, argument );
    if ( portal == NULL || portal->item_type != ITEM_PORTAL 
         || portal->value[2] == 0 )
    {
/* look for a keyworded door */

      if ( ( door = find_door( ch, argument ) ) >= 0 )
      {
   /* enterable exit */
        ch->using_named_door = TRUE;
        move_char( ch, door );
        return;
      }
      else
      {
/*        send_to_char( "You can't enter that!\n\r", ch );  */
        return;
      }
      
       return;
    }
    
    if ( ( to_room = get_room_index( portal->value[0] ) ) == NULL )
    {
       send_to_char( "Nothing happens.\n\r", ch );
       return;
    }
    if (  ( IS_SET( ch->in_room->room_flags, ROOM_NO_PORTAL ) || IS_SET( to_room->room_flags, ROOM_NO_PORTAL )  )
       && number_range( 0, 100 ) < 75 )
    {
      act( "The anti-magic zone causes the portal to {RIMPLODE{x!!!", ch, NULL, NULL, TO_ROOM);
      send_to_char( "The anti-magic zone causes the portal to {RIMPLODE{x!!!", ch);

      extract_obj( portal );

    /* Now check if there is a corresponding portal in to_room */
      for ( foo = to_room->first_content; foo != NULL; foo = foo_next )
      {
         foo_next = foo->next_in_room;

         if (  foo->item_type == ITEM_PORTAL
            && foo->value[0] == ch->in_room->vnum )
              break;
         extract_obj ( foo );
    
      }
      return;
    }


    act( "$n enters $p.", ch, portal, NULL, TO_ROOM );
    act( "You enter $p.", ch, portal, NULL, TO_CHAR );
    
    /* Now check if there is a corresponding portal in to_room */
    for ( foo = to_room->first_content; foo != NULL; foo = foo->next_in_room )
       if (  foo->item_type == ITEM_PORTAL
          && foo->value[0] == ch->in_room->vnum )
            break;
    
    char_from_room( ch );
    char_to_room( ch, to_room );
    
    if ( foo == NULL )
       act( "A glowing sphere forms, and $n steps out!", ch, NULL, NULL, TO_ROOM );
    else
       act( "$n steps out of $p.", ch, foo, NULL, TO_ROOM );
    
    do_look( ch, "" );
    return;
 }  

void do_scout ( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA 	*loc;
   ROOM_INDEX_DATA	*tmploc;
   int			door;
   int			depth = 0;
   int			counter;
   CHAR_DATA		*target;
   char			buf[MAX_INPUT_LENGTH];
   char			pre[MAX_INPUT_LENGTH];
   buf[0] = '\0';
   pre[0] = '\0';

    if ( !IS_NPC(ch) && get_skill(ch,gsn_scout) == 0 )
    {
       send_to_char( "You are not trained in this skill!\n\r", ch );
       return;
    }

    if ( !IS_NPC(ch) )
    {
   /* depth is the number of rooms AWAY from your present location */
       if ( get_skill(ch,gsn_scout) > 78 )
           depth = 4;
       else if ( get_skill(ch,gsn_scout) > 64 )
           depth = 3;
       else if ( get_skill(ch,gsn_scout) > 40 )
           depth = 2;
       else 
           depth = 1;
    }

   act( "$n looks all around for anyone else.", ch, NULL, NULL, TO_ROOM );
   send_to_char( "You look around.\n\r", ch );

   while ( depth > 0 )
   {
      loc = ch->in_room;
      for ( door = 0; door < 6; door++ )
      {
         tmploc = loc;
     
         /* get the room */
         for ( counter = 0; counter < depth; counter++ )
         {
            if (  tmploc->exit[door] == 0 
               || tmploc->exit[door]->to_room == NULL
               || IS_SET( tmploc->exit[door]->exit_info, EX_CLOSED )
               || ( str_cmp( tmploc->exit[door]->keyword, "" ) )  )
                   break;
  
            tmploc = tmploc->exit[door]->to_room;
         }
      
         /* if room didn't exist */
         if ( counter < depth )
            continue;

        
         for ( target = tmploc->first_person; target != NULL; target = target->next_in_room )
         {
            if ( target->in_room->vnum == ch->in_room->vnum )
               break;

            if ( !can_see( ch, target ) )
               continue;

            if ( depth == 1 )
               sprintf( pre, "%s : ", dir_name[door] );
            else if ( depth == 2 )
               sprintf( pre, "Far %s : ", dir_name[door] );
            else if ( depth == 3 )
               sprintf( pre, "Very Far %s : ", dir_name[door] );
            else if ( depth == 4 )
               sprintf( pre, "Distant %s : ", dir_name[door] );

            if ( IS_NPC( target ) )
               sprintf( buf, "%s%s\n\r", pre, target->short_descr );
            else
	       sprintf( buf, "%s%s\n\r", pre, target->name );

            send_to_char( buf, ch );
         }
      }
      
      depth = depth - 1;
   }
}
        
void do_nofollow ( CHAR_DATA *ch, char *argument )
{
CHAR_DATA *ppl;
    if ( IS_SET(ch->config, CONFIG_NOFOL) )
       {
        send_to_char( "You accept followers again.\n\r", ch );
        REMOVE_BIT(ch->config, CONFIG_NOFOL);
        return;
       }
    SET_BIT(ch->config, CONFIG_NOFOL);
    send_to_char ( "You no longer accept followers.\n\r", ch );
    for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
    if   ( ( ppl != ch ) && ( ppl->master == ch ) && ( ppl->hometown == ch->hometown ) && !IS_IMMORTAL ( ppl ) )
    {
    do_follow( ppl, "self" );
    break;
    }
 return;
 }

void do_noefollow ( CHAR_DATA *ch, char *argument )
{
CHAR_DATA *ppl;
    if ( IS_SET(ch->config, CONFIG_NOEFOL) )
       {
        send_to_char( "You accept enemy followers again.\n\r", ch );
        REMOVE_BIT(ch->config, CONFIG_NOEFOL);
        return;
       }
 SET_BIT(ch->config, CONFIG_NOEFOL);
 send_to_char ( "You no longer accept enemy followers.\n\r", ch );
    for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
    if   ( ( ppl != ch ) && ( ppl->master == ch ) && ( ppl->hometown != ch->hometown ) && !IS_IMMORTAL ( ppl ) )
    {
    do_follow( ppl, "self" );
    break;
    } 
 return;
 }


void do_cdebug( CHAR_DATA *ch, char *argument )
{
 char buf[MSL];
 char buf2[MSL];
 char buf3[MSL];
 char buf4[MSL];

 sprintf(buf, "Buffer 1\n\r");
 send_to_char( "Buffer 1 printed.\n\r", ch);
 log_f( "Buffer one printed");

 sprintf( buf2, "Buffer 2\n\r");
 send_to_char( "Buffer 2 printed.\n\r", ch);
 log_f( "Buffer two printed");

sprintf(buf, "Buffer 1\n\r");
sprintf( buf2, "Buffer 2\n\r");

 log_f( "about to call str_cmp" );
 
 if ( str_cmp(buf, buf2) )
  {
  send_to_char( "Buffers don't match, String compare works.\n\r", ch);
  log_f( "str_cmp works");
  }
  send_to_char( "Str_cmp works.\n\r", ch);

  sprintf(buf3, PERSON(ch,ch) );
  log_f( "PERSON works");

  sprintf(buf4, "PERSON returned %s", buf3);
  send_to_char( buf4, ch);
  log_f( "PERSON returned correctlly");

return;
}
