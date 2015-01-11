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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "ack.h"
#include "tables.h"
#include "cursor.h"
#include <math.h>
#ifndef DEC_MAGIC_H
#include "magic.h"
#endif
#ifndef DEC_MONEY_H
#include "money.h"
#endif

void do_mapping args ( ( CHAR_DATA *ch, char *argument ) );
int reclass_lookup args ( ( const char *name, CHAR_DATA *ch ) );
char *get_title_nocol args ( ( CHAR_DATA *ch ) );


char *PERSON( CHAR_DATA *ch, CHAR_DATA *victim )
{
	CHAR_DATA *ppl;
	static char personbuffer[MSL];
	personbuffer[0] = '\0';

	if ( IS_NPC(victim) )
	{
 if ( !can_see(ch, victim) )
	 return "Someone";

		 return victim->short_descr;
	}


	if ( IS_IMMORTAL( ch ) )
	 return victim->name;

/* Stupid MOBS, this is a bit hacky, bah.
 * This is if CH is a MOB.
 * Need to do this because of fights. */

	/* Okay, let me explain this, this is if the LOOKER is a MOB
	 * This is hacky because of battles, but it works.
	 */
	 for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
	 {
	if ( ppl == ch )
	continue;
	if ( ppl == victim )
	continue;

		 if ( IS_NPC(ch) )
	 {
 
	if ( IS_IMMORTAL(ppl) )
		 return victim->name;


	 if ( ppl->hometown != victim->hometown  )
	 {
 if ( !can_see(ppl, victim) )
	 return "Someone";
	 if(victim->level < 50 )
		 return race_table[ch->race].racedesc[ch->hometown];
	 else
		 sprintf(personbuffer, "*%s %s*", get_title_nocol(victim), victim->name);
		 return personbuffer;
	 }

 
 if ( ppl->hometown == victim->hometown )
	 {
 if ( !can_see(ppl, victim) )
	 return "Someone";

	 if(victim->level < 50 )
		 return victim->name;
	 else
		 sprintf(personbuffer, "%s %s", get_title_nocol(victim), victim->name);
		 return personbuffer;
 }

	 }

	 }

	 if ( !IS_IMMORTAL( ch ) && IS_IMMORTAL( victim ) )
	 {
		 if ( !can_see(ch, victim) )
		 return "Someone";
		sprintf(personbuffer, "%s %s", get_title_nocol(victim), victim->name);
		return personbuffer;
	 }


	 if ( ch->hometown != victim->hometown  )
	 {
 if ( !can_see(ch, victim) )
	 return "Someone";
	 if(victim->level < 50 )
		 return race_table[ch->race].racedesc[ch->hometown];
	 else
		 sprintf(personbuffer, "*%s %s*", get_title_nocol(victim), victim->name);
		return personbuffer;
	 }

	 if ( ch->hometown == victim->hometown  )
	 {
		if ( !can_see(ch, victim) )
			 return "Someone";
	 if(victim->level < 50 )
		 return victim->name;
	 else
		 sprintf(personbuffer, "%s %s", get_title_nocol(victim), victim->name);
		return personbuffer;
}
return NULL;
 }

char *get_pkn( CHAR_DATA *ch )
{
	static char pperbuf[MSL];
	pperbuf[0] = '\0';

	if (ch->level < 50)
		return race_table[ch->race].racedesc[ch->hometown];
	else
		sprintf(pperbuf, "*%s %s*", get_title_nocol(ch), ch->name);
	return pperbuf;
}
extern  bool     deathmatch;


char *  const   where_name      [] =
{
		"                      ",   /* light  */  
		"-*floating above*-    ",
		"-*enveloped by*-      ",
		"-*worn on horns*-     ",
		"-*worn on head*-      ",
		"-*worn on face*-      ",
		"-*worn on beak*-      ",
		"-*worn on ear*-       ",
		"-*worn on ear*-       ",
		"-*worn on neck*-      ",
		"-*worn on neck*-      ",  /* 10 */
		"-*worn on wings*-     ",
		"-*worn on shoulders*- ",
		"-*worn on arms*-      ",
		"-*worn around wrist*- ",
		"-*worn around wrist*- ",  /* 15 */
		"-*worn on hands*-     ",
		"-*worn on finger*-    ",
		"-*worn on finger*-    ",
		"-*worn on claws*-     ",
		"-*held in left hand*- ",  /* 20 */
		"-*held in right hand*-",
		"-*worn as a cape*-    ",
		"-*worn about waist*-  ",
		"-*worn on body*-      ",
		"-*worn on tail*-      ",
		"-*worn on legs*-      ",
		"-*worn on feet*-      ",
		"-*worn on hooves*-    ",
		"-*Lodged in body!*-   ",
};




/*
 * Local functions.
 */
char *  format_obj_to_char      args( ( OBJ_DATA *obj, CHAR_DATA *ch,
						bool fShort ) );
void    show_list_to_char       args( ( OBJ_DATA *list, CHAR_DATA *ch,
						bool fShort, bool fShowNothing ) );
void    show_char_to_char_0     args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char_1     args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char       args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool    check_blind             args( ( CHAR_DATA *ch ) );



char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
		static char buf[MAX_STRING_LENGTH];

		sprintf( buf, "%s", color_string( ch, "objects" ) );
		if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   safe_strcat(MAX_STRING_LENGTH, buf, "(Invis) "     );

		if ( IS_OBJ_STAT(obj, ITEM_HIDDEN)    )   safe_strcat(MAX_STRING_LENGTH, buf, "(Hidden) "    );

		if ( ( IS_AFFECTED(ch, AFF_DETECT_EVIL) || item_has_apply( ch, ITEM_APPLY_DET_EVIL ) )
	 && IS_OBJ_STAT(obj, ITEM_EVIL)   )   safe_strcat(MAX_STRING_LENGTH, buf, "(Red Aura) "  );
		
		if ( ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)|| item_has_apply( ch, ITEM_APPLY_DET_MAG) )
	 && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   safe_strcat(MAX_STRING_LENGTH, buf, "(Magical) "   );
		
		if ( IS_OBJ_STAT(obj, ITEM_GLOW)      )   safe_strcat(MAX_STRING_LENGTH, buf, "{d({CGlowing{d){x "   );
		
		if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   safe_strcat(MAX_STRING_LENGTH, buf, "{d({gHumming{d){x "   );

		if ( fShort )
		{
	if ( obj->short_descr != NULL )
			safe_strcat(MAX_STRING_LENGTH, buf, obj->short_descr );
		}
		else
		{
	if ( obj->description != NULL )
			safe_strcat(MAX_STRING_LENGTH, buf, obj->description );
		}
		safe_strcat( MAX_STRING_LENGTH, buf, color_string( ch, "normal" ) );
		return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
		char buf[MAX_STRING_LENGTH];
		char **prgpstrShow;
		int *prgnShow;
		char *pstrShow;
		OBJ_DATA *obj;
		int nShow;
		int iShow;
		int count;
		bool fCombine;

		if ( ch->desc == NULL )
	return;
		buf[0] = '\0';


		/*
		 * Alloc space for output lines.
		 */
		count = 0;
		for ( obj = list; obj != NULL; obj = obj->next_in_carry_list )
	count++;
		prgpstrShow = qgetmem( count * sizeof(char *) );
		prgnShow    = qgetmem( count * sizeof(int)    );
		nShow       = 0;

		/*
		 * Format the list of objects.
		 */
		for ( obj = list; obj != NULL; obj = obj->next_in_carry_list )
		{ 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
			pstrShow = format_obj_to_char( obj, ch, fShort );
			fCombine = FALSE;

			if ( IS_NPC(ch) || IS_SET(ch->config, CONFIG_COMBINE) )
			{
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
				if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
				{
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
				}
		}
			}

			/*
			 * Couldn't combine, or didn't want to.
			 */
			if ( !fCombine )
			{
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
			}
	}
		}

		/*
		 * Output the formatted list.
		 */
		for ( iShow = 0; iShow < nShow; iShow++ )
		{
	if ( IS_NPC(ch) || IS_SET(ch->config, CONFIG_COMBINE) )
	{
			if ( prgnShow[iShow] != 1 )
			{
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		send_to_char( buf, ch );
			}
			else
			{
		send_to_char( "     ", ch );
			}
	}
	send_to_char( prgpstrShow[iShow], ch );
	send_to_char( "\n\r", ch );
	free_string( prgpstrShow[iShow] );
		}

		if ( fShowNothing && nShow == 0 )
		{
	if ( IS_NPC(ch) || IS_SET(ch->config, CONFIG_COMBINE) )
			send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
		}

		/*
		 * Clean up.
		 */
		qdispose( prgpstrShow, count * sizeof(char *) );
		qdispose( prgnShow,    count * sizeof(int)    );

		return;
}

void show_room_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
		char buf[MAX_STRING_LENGTH];
		char **prgpstrShow;
		int *prgnShow;
		char *pstrShow;
		OBJ_DATA *obj;
		int nShow;
		int iShow;
		int count;
		bool fCombine;

		if ( ch->desc == NULL )
	return;
		buf[0] = '\0';


		/*
		 * Alloc space for output lines.
		 */
		count = 0;
		for ( obj = list; obj != NULL; obj = obj->next_in_room )
	count++;
		prgpstrShow = qgetmem( count * sizeof(char *) );
		prgnShow    = qgetmem( count * sizeof(int)    );
		nShow       = 0;

		/*
		 * Format the list of objects.
		 */
		for ( obj = list; obj != NULL; obj = obj->next_in_room )
		{ 
				if (obj->sat == TRUE)
						continue;
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) && str_cmp( obj->description, "" ) )
	{
			pstrShow = format_obj_to_char( obj, ch, fShort );
			fCombine = FALSE;

			if ( IS_NPC(ch) || IS_SET(ch->config, CONFIG_COMBINE) )
			{
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
				if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
				{
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
				}
		}
			}

			/*
			 * Couldn't combine, or didn't want to.
			 */
			if ( !fCombine )
			{
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
			}
	}
		}

		/*
		 * Output the formatted list.
		 */
		for ( iShow = 0; iShow < nShow; iShow++ )
		{
	if ( ( IS_NPC(ch) || IS_SET(ch->config, CONFIG_COMBINE) ) && str_cmp( prgpstrShow[iShow], "" ) )
	{
			if ( prgnShow[iShow] != 1 )
			{
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		send_to_char( buf, ch );
			}
			else
			{
		send_to_char( "     ", ch );
			}
	}
	send_to_char( prgpstrShow[iShow], ch );
	send_to_char( "\n\r", ch );
	free_string( prgpstrShow[iShow] );
		}

		if ( fShowNothing && nShow == 0 )
		{
	if ( IS_NPC(ch) || IS_SET(ch->config, CONFIG_COMBINE) )
			send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
		}

		/*
		 * Clean up.
		 */
		qdispose( prgpstrShow, count * sizeof(char *));
		qdispose( prgnShow,    count * sizeof(int)   );

		return;
}




void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
		char buf[MAX_STRING_LENGTH];
		char buf2[MAX_STRING_LENGTH];


		sprintf( buf, "%s", color_string( ch, "mobiles" ) );
		buf2[0] = '\0';

 
/* This is temporary....
 * Zen  WOLF
 *
 */   
		if ( !IS_NPC( victim ) && IS_WOLF( victim ) )
		{
			if ( IS_SHIFTED( victim ) )
			{
				send_to_char( "A large wolf scampers before you.{x\n\r", ch );
				return;
			}
			else
			if ( IS_RAGED( victim ) )
			{
				send_to_char( "A {yWEREWOLF in {GFULL RAGE stands before you!\n\r", ch );
				return;
			}
		}

		if ( IS_NPC( victim ) )
		{
			 if ( IS_SET( victim->act, ACT_UNDEAD ) &&
		( is_affected( ch, skill_lookup( "detect undead" ) ) || item_has_apply( ch, ITEM_APPLY_DET_UNDEAD ) ) )
		safe_strcat(MAX_STRING_LENGTH, buf, "(UnDead) " );
		
			 if ( IS_SET( victim->act, ACT_INTELLIGENT ) )
		safe_strcat( MAX_STRING_LENGTH, buf, "(Int) ");
		
		}
		
		
		if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) safe_strcat(MAX_STRING_LENGTH, buf, "(Invis) "      );
		
		if ( IS_AFFECTED(victim, AFF_HIDE) || item_has_apply( victim, ITEM_APPLY_HIDE ) )
		 safe_strcat(MAX_STRING_LENGTH, buf, "(Hide) "       );
		
		if ( IS_AFFECTED(victim, AFF_CHARM)       ) safe_strcat(MAX_STRING_LENGTH, buf, "(Charm) "    );
		/*
		if ( IS_AFFECTED(victim, AFF_PASS_DOOR) || item_has_apply( victim, ITEM_APPLY_PASS_DOOR) )
	 safe_strcat(MAX_STRING_LENGTH, buf, "({bClear{x) ");
		*/
		if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) safe_strcat(MAX_STRING_LENGTH, buf, "(Pink) "  );
		
		if ( IS_EVIL(victim)
		&&   (IS_AFFECTED(ch, AFF_DETECT_EVIL)|| item_has_apply( ch, ITEM_APPLY_DET_EVIL) ) )
		safe_strcat(MAX_STRING_LENGTH, buf, "({RRed Aura{x){x "   );
		
		if ( IS_AFFECTED(victim, AFF_SANCTUARY) || item_has_apply( victim, ITEM_APPLY_SANC )  )
	 safe_strcat(MAX_STRING_LENGTH, buf, "({WWhite Aura{x){x " );

				if (  ( IS_AFFECTED( victim, AFF_CLOAK_FLAMING ) )
			 || ( IS_AFFECTED( victim, AFF_CLOAK_ABSORPTION ) ) 
	 || ( IS_AFFECTED( victim, AFF_CLOAK_REFLECTION ) ) 
			 || ( is_affected( victim, skill_lookup( "cloak:misery" ) )  ) )
		{
			if ( IS_AFFECTED( victim, AFF_CLOAK_FLAMING ) )
				safe_strcat( MAX_STRING_LENGTH, buf, "({RFlaming{x) " );
			if ( IS_AFFECTED( victim, AFF_CLOAK_ABSORPTION ) )
				safe_strcat( MAX_STRING_LENGTH, buf, "({BAbsorb{x) " );
			if ( IS_AFFECTED( victim, AFF_CLOAK_REFLECTION ) )
				safe_strcat( MAX_STRING_LENGTH, buf, "({mReflect{x) " );
			if ( is_affected( victim, skill_lookup( "cloak:misery" ) ) )
				safe_strcat( MSL, buf, "({rMisery{x) " );
		}

		if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER ) )
						safe_strcat(MAX_STRING_LENGTH, buf, "(KILLER) "     );
		
		if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF  ) )
						safe_strcat(MAX_STRING_LENGTH, buf, "(THIEF) "      );

	/* strcat to buf *linkdead* if not connected? */
		if ( !IS_NPC(victim)  )
			if ( (victim->desc) != NULL
		 && victim->desc->connected != CON_PLAYING )
			 safe_strcat( MAX_STRING_LENGTH, buf, "(LINKDEAD)" );

/*
		if ( !IS_NPC( victim ) && IS_SET( victim->pcdata->pflags, PFLAG_RULER ) )    
			safe_strcat( MSL, buf, 
			 get_ruler_title( victim->pcdata->ruler_rank, victim->login_sex ) );
*/
		if ( victim->position == POS_STANDING && victim->long_descr[0] != '\0' )
		{
	safe_strcat(MAX_STRING_LENGTH, buf, victim->long_descr );
	safe_strcat(MAX_STRING_LENGTH, buf, color_string( ch, "normal" ) );
/*
		if (  ( IS_AFFECTED( victim, AFF_CLOAK_FLAMING ) )
			 || ( IS_AFFECTED( victim, AFF_CLOAK_ABSORPTION ) ) 
	 || ( IS_AFFECTED( victim, AFF_CLOAK_REFLECTION ) ) 
			 || ( is_affected( victim, skill_lookup( "cloak:misery" ) )  ) )
		{
			safe_strcat( MAX_STRING_LENGTH, buf, "  {xCLOAK:" );
			if ( IS_AFFECTED( victim, AFF_CLOAK_FLAMING ) )
				safe_strcat( MAX_STRING_LENGTH, buf, " {RFLAMING{x" );
			if ( IS_AFFECTED( victim, AFF_CLOAK_ABSORPTION ) )
				safe_strcat( MAX_STRING_LENGTH, buf, " {BABSORB{x" );
			if ( IS_AFFECTED( victim, AFF_CLOAK_REFLECTION ) )
				safe_strcat( MAX_STRING_LENGTH, buf, " {mREFLECT{x" );
			if ( is_affected( victim, skill_lookup( "cloak:misery" ) ) )
				safe_strcat( MSL, buf, " {rMISERY{x" );
			safe_strcat( MAX_STRING_LENGTH, buf, "\n\r" );
		}
*/
/*
		if ( victim->first_shield != NULL )
		{
			MAGIC_SHIELD * this_shield;
			safe_strcat( MSL, buf, "   {WSHIELD: {x" );
			for ( this_shield = victim->first_shield; this_shield != NULL; this_shield = this_shield->next )
				safe_strcat( MSL, buf, this_shield->name );
			safe_strcat( MSL, buf, "\n\r" );
		}
*/
			safe_strcat( MSL, buf, "\n\r" );
	send_to_char( buf, ch );
	return;
		}
/* BINGO!! */
		safe_strcat(MAX_STRING_LENGTH, buf, PERSON(ch, victim) );
/*
		if ( !IS_NPC(victim) && !IS_SET(ch->config, CONFIG_BRIEF) )
	safe_strcat(MAX_STRING_LENGTH, buf, victim->pcdata->title );
*/

		switch ( victim->position )
		{
		case POS_DEAD:     safe_strcat(MAX_STRING_LENGTH, buf, " is DEAD!!" );              break;
		case POS_MORTAL:   safe_strcat(MAX_STRING_LENGTH, buf, " is mortally wounded." );   break;
		case POS_INCAP:    safe_strcat(MAX_STRING_LENGTH, buf, " is incapacitated." );      break;
		case POS_STUNNED:  safe_strcat(MAX_STRING_LENGTH, buf, " is lying here stunned." ); break;
		case POS_SLEEPING:
			if ( victim->sitting != NULL 
	 && victim->sitting->in_room == victim->in_room )
			{
	 char sit[MAX_INPUT_LENGTH];
	 sprintf( sit, " is here, sleeping on %s.", victim->sitting->short_descr );
	 safe_strcat( MAX_STRING_LENGTH, buf, sit );
			}
			else
	safe_strcat(MAX_STRING_LENGTH, buf, " is sleeping here." );       
			break;

		case POS_RESTING:  
			if ( victim->sitting != NULL 
	 && victim->sitting->in_room == victim->in_room )
			{
	 char sit[MAX_INPUT_LENGTH];
	 sprintf( sit, " is here, resting on %s.", victim->sitting->short_descr );
	 safe_strcat( MAX_STRING_LENGTH, buf, sit );
			}
			else
	safe_strcat(MAX_STRING_LENGTH, buf, " is resting here." );       
			break;
		case POS_SITTING:  
			if ( victim->sitting != NULL 
	 && victim->sitting->in_room == victim->in_room )
			{
	 char sit[MAX_INPUT_LENGTH];
	 sprintf( sit, " is here, sitting on %s.", victim->sitting->short_descr );
	 safe_strcat( MAX_STRING_LENGTH, buf, sit );
			}
			else
	safe_strcat(MAX_STRING_LENGTH, buf, " is sitting here." );       
			break;
		case POS_STANDING: 
				 {
					 safe_strcat(MAX_STRING_LENGTH, buf, " is here" );
						 safe_strcat( MSL, buf, "." );
					 break;
				 }
		case POS_WRITING:  safe_strcat(MAX_STRING_LENGTH, buf, " is writing a note." );     break;
		case POS_BUILDING: safe_strcat(MAX_STRING_LENGTH, buf, " is BUILDING!!" );          break;
		case POS_FIGHTING:
	safe_strcat(MAX_STRING_LENGTH, buf, " is here, fighting " );
	if ( victim->fighting == NULL )
			safe_strcat(MAX_STRING_LENGTH, buf, "thin air??" );
	else if ( victim->fighting == ch )
			safe_strcat(MAX_STRING_LENGTH, buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->in_room )
	{
			safe_strcat(MAX_STRING_LENGTH, buf, PERSON(ch, victim->fighting) );
			safe_strcat(MAX_STRING_LENGTH, buf, "." );
	}
	else
			safe_strcat(MAX_STRING_LENGTH, buf, "somone who left??" );
	break;
		}

		
		/* buf[0] = UPPER(buf[0]); */
		safe_strcat( MAX_STRING_LENGTH, buf, color_string( ch, "normal" ) );
		safe_strcat(MAX_STRING_LENGTH, buf, "\n\r" );
/*
		if (  ( IS_AFFECTED( victim, AFF_CLOAK_FLAMING ) )
			 || ( IS_AFFECTED( victim, AFF_CLOAK_ADEPT ) ) 
	 || ( IS_AFFECTED( victim, AFF_CLOAK_REGEN ) )  
			 || ( IS_AFFECTED( victim, AFF_CLOAK_ABSORPTION ) ) 
	 || ( IS_AFFECTED( victim, AFF_CLOAK_REFLECTION ) ) 
	 || ( is_affected( victim, skill_lookup( "cloak:misery" ) ) ) )
		{
			safe_strcat( MAX_STRING_LENGTH, buf, "  {xCLOAK:" );
			if ( IS_AFFECTED( victim, AFF_CLOAK_FLAMING ) )
	safe_strcat( MAX_STRING_LENGTH, buf, " {RFLAMING{x" );
			if ( IS_AFFECTED( victim, AFF_CLOAK_ABSORPTION ) )
	safe_strcat( MAX_STRING_LENGTH, buf, " {BABSORB{x" );
			if ( IS_AFFECTED( victim, AFF_CLOAK_REFLECTION ) )
	safe_strcat( MAX_STRING_LENGTH, buf, " {mREFLECT{x" );
			
			if ( IS_AFFECTED( victim, AFF_CLOAK_ADEPT ) )
	safe_strcat( MAX_STRING_LENGTH, buf, " {WADEPT{x" );
			if ( IS_AFFECTED( victim, AFF_CLOAK_REGEN ) )
	safe_strcat( MAX_STRING_LENGTH, buf, " {GREGEN{x" );
			if ( is_affected( victim, skill_lookup( "cloak:misery" ) ) )
				safe_strcat( MSL, buf, " {rMISERY{x" );      
			safe_strcat( MAX_STRING_LENGTH, buf, "\n\r" );
	 
		}
		if ( victim->first_shield != NULL )
		{
			MAGIC_SHIELD * this_shield;
			safe_strcat( MSL, buf, "   {WSHIELD: {x" );
			for ( this_shield = victim->first_shield; this_shield != NULL; this_shield = this_shield->next )
				safe_strcat( MSL, buf, this_shield->name );
			safe_strcat( MSL, buf, "\n\r" );
		}
*/
		if ( victim->riding != NULL )
		{
			sprintf( buf2, "  riding %s.\n\r", victim->riding->short_descr );
			safe_strcat( MSL, buf, buf2 );
		}

		send_to_char( buf, ch );
		return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
		char buf[MAX_STRING_LENGTH];
		OBJ_DATA *obj;
		int iWear;
		int pct;
		bool found;
		buf[0] = '\0';

		if ( ch == victim )
		{

		if ( can_see( victim, ch ) )
		{
	act( "$n looks at $Mself.",  ch, NULL, victim, TO_NOTVICT );
		}
		} else {
		if ( can_see( victim, ch ) )
		{
			act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
		}
		}
		
		if ( victim->description[0] != '\0' )
		{
	send_to_char( victim->description, ch );
		}
		else
		{
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
		}

		if ( victim->max_hit > 0 )
	pct = ( 100 * victim->hit ) / victim->max_hit;
		else
	pct = -1;

		strcpy( buf, PERSON(ch, victim) );

	 if ( pct >= 100 ) safe_strcat(MAX_STRING_LENGTH, buf, " is in pristine condition.\n\r" );
		else if ( pct >=  90 ) safe_strcat(MAX_STRING_LENGTH, buf, " is slightly scratched.\n\r" );
		else if ( pct >=  80 ) safe_strcat(MAX_STRING_LENGTH, buf, " has some light bruising.\n\r" );
		else if ( pct >=  70 ) safe_strcat(MAX_STRING_LENGTH, buf, " has some shallow cuts.\n\r" );
		else if ( pct >=  60 ) safe_strcat(MAX_STRING_LENGTH, buf, " has several weeping wounds.\n\r" );
		else if ( pct >=  50 ) safe_strcat(MAX_STRING_LENGTH, buf, " looks like a traffic accident.\n\r" );
		else if ( pct >=  40 ) safe_strcat(MAX_STRING_LENGTH, buf, " is bleeding slowly into a puddle.\n\r" );
		else if ( pct >=  30 ) safe_strcat(MAX_STRING_LENGTH, buf, " is spraying blood all over.\n\r" );
		else if ( pct >=  20 ) safe_strcat(MAX_STRING_LENGTH, buf, " is having trouble living.\n\r" );
		else if ( pct >=  10 ) safe_strcat(MAX_STRING_LENGTH, buf, " looks ready to kick the bucket.\n\r"        );
		else                   safe_strcat(MAX_STRING_LENGTH, buf, " is DYING.\n\r" );

		buf[0] = UPPER(buf[0]);
		send_to_char( buf, ch );

		found = FALSE;

		if ( !IS_NPC( victim ) && IS_WOLF( victim )
			 && ( IS_SHIFTED( victim ) || IS_RAGED( victim )  )  )
		{
		}
		else
		{
			for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
			{
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
			if ( !found )
			{
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
			}
			send_to_char( where_name[iWear], ch );
			send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
			send_to_char( "\n\r", ch );
	}
			}

			if ( victim != ch
			&&   !IS_NPC(ch)
			&&   number_percent( ) < ch->pcdata->learned[gsn_peek] )
			{
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	show_list_to_char( victim->first_carry, ch, TRUE, TRUE );
			}
		}
		return;
}


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
		CHAR_DATA *rch;

		for ( rch = list; rch != NULL; rch = rch->next_in_room )
		{
	if ( rch == ch )
			continue;

	if ( !IS_NPC(rch)
	&&   IS_SET(rch->act, PLR_WIZINVIS)
	&&   get_trust( ch ) < rch->invis )
			continue;

				if (  ( rch->rider != NULL ) 
					 && ( rch->rider != ch ) )
					continue;  /* show under the rider */

	if ( can_see( ch, rch ) )
	{
			show_char_to_char_0( rch, ch );
	}
	else if ( room_is_dark( ch->in_room )
	&&        (IS_AFFECTED(rch, AFF_INFRARED )|| item_has_apply( rch, ITEM_APPLY_INFRA)) )
	{
			send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
	}
		}

		return;
} 



bool check_blind( CHAR_DATA *ch )
{
		if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

		if ( IS_AFFECTED(ch, AFF_BLIND) )
		{
			send_to_char( "You cannot see a thing!\n\r", ch );
			return FALSE;
		}

		return TRUE;
}



void do_look( CHAR_DATA *ch, char *argument )
{
		char buf  [MAX_STRING_LENGTH];
		char arg1 [MAX_INPUT_LENGTH];
		char arg2 [MAX_INPUT_LENGTH];
		char out  [MAX_STRING_LENGTH];
		CHAR_DATA *ppl;
		ROOM_INDEX_DATA *room;
		EXIT_DATA *pexit;
		CHAR_DATA *victim;
		OBJ_DATA *obj;
		char pdesc[MSL];
		int door;
		bool found;
		buf[0] = '\0';
		out[0] = '\0';

		if ( !IS_NPC( ch ) && ch->desc == NULL ) 
	return;

		if ( ch->position < POS_SLEEPING )
		{
	send_to_char( "You cannot see anything but stars!\n\r", ch );
	return;
		}

		if ( ch->position == POS_SLEEPING )
		{
	send_to_char( "You are sleeping!\n\r", ch );
	return;
		}

		if ( !check_blind( ch ) )
	return;

		if ( !IS_NPC(ch)
		&&   !IS_SET(ch->act, PLR_HOLYLIGHT)
		&&   room_is_dark( ch->in_room ) )
		{
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->first_person, ch );
	return;
		}

		argument = one_argument( argument, arg1 );
		argument = one_argument( argument, arg2 );

		if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) || !str_cmp( arg1, "zxcvbnm") )
		{
	/* 'look' or 'look auto' */
/* To hell with the old map.
 *

	if ( IS_SET(ch->config, CONFIG_NEWMAP) )
	{
*/
	if (  ( IS_SWITCHED( ch )  || ( !IS_NPC(ch) && IS_SET(ch->config, CONFIG_MAPPER)  ) ) && str_cmp( arg1, "zxcvbnm" ) )
	{
		send_to_char( "\n\r", ch );
		do_mapping( ch, "8" );
		send_to_char( "\n\r", ch );
		sprintf(buf, "{d[{m%d{x{d]{x", ch->in_room->vnum);
		sprintf( out, "%s %s\n\r",
				ch->in_room->name, (IS_IMMORTAL(ch) ? buf : "") );
	send_to_char( out, ch );
		buf[0] = '\0';
		if (  IS_SWITCHED( ch )  || ( !IS_NPC(ch) && IS_SET(ch->config, CONFIG_AUTOEXIT)  ) )
		 do_exits(ch, "auto" );
		if ( !IS_SET(ch->config, CONFIG_BRIEF) )
	{
	sprintf( out, "%s", ch->in_room->description );
	send_to_char( out, ch );
	send_to_char( "\n\r", ch);
	 }
	}

	else 
 {
		sprintf(buf, "{d[{m%d{x{d]{x", ch->in_room->vnum);
		sprintf( out, "%s %s\n\r",
				ch->in_room->name, (IS_IMMORTAL(ch) ? buf : "") );
	send_to_char( out, ch );
		buf[0] = '\0';
	
	send_to_char( out, ch );
		if (  IS_SWITCHED( ch )  || ( !IS_NPC(ch) && IS_SET(ch->config, CONFIG_AUTOEXIT)  ) )
			do_exits(ch, "auto" );

		if ( !IS_SET(ch->config, CONFIG_BRIEF) )
 {
			
			sprintf( out, "\n\r%s", ch->in_room->description );
			send_to_char( out, ch );
		send_to_char("\n\r", ch);
	}
 } 

	/* Show any room-affects */
	if (  ( ch->in_room->affected_by != 0 ) 
	 && ( ( is_affected( ch, skill_lookup( "detect magic" ) ) )  
			|| ( item_has_apply( ch, ITEM_APPLY_DET_MAG ) )  ) )
				{
//	   sprintf( out, "%s", color_string( ch, "rooms" ) );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_SHADE ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA menacing {ddark shadow{x hangs in the sky above you.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_ENCAPS ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA barely visible {Genergy web{x is blocking all exits here.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_FIRE_RUNE ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA mystical {RFire {xRune{x hangs in the air above you.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_SHOCK_RUNE ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA mystical {BShock{x Rune{x hangs in the air above you.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_POISON_RUNE ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA mystical {dPoison{x Rune hangs in the air above you.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_HEAL_REGEN ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA majestic {mHealing Light{x encompasses the room.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_HEAL_STEAL ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA menacing {dWithering shadow{x enfolds the room.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_MANA_REGEN ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA powerful {RMana Flare{x empowers the room.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_MANA_STEAL ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA mind sapping {dMana Drain{x enfolds the room.\n\r" );
		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_HOLD ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xThe magical bars of a {GCage{x surround the room.\n\r" );         

		 if ( IS_SET( ch->in_room->affected_by, ROOM_BV_SOUL_NET ) )
				safe_strcat( MAX_STRING_LENGTH,  out, "{xA demonic {dSoul Net{x enshrouds the room.\n\r" );
			 
		 
		 
//	   safe_strcat( MAX_STRING_LENGTH,  out, color_string( ch, "normal" ) );
		 send_to_char( out, ch );
				}

				if ( !IS_NPC( ch ) && ( IS_VAMP( ch ) || IS_WOLF( ch ) )  )
					if ( ch->in_room->first_mark_list != NULL )
					{
						MARK_LIST_MEMBER	*marks;
						sh_int	num_marks = 0;
						
						for ( marks = ch->in_room->first_mark_list; marks != NULL; marks = marks->next )
							
							if (  ( ( marks->mark->type == WOLF ) && IS_WOLF( ch ) )
								 || ( ( marks->mark->type == VAMP ) && IS_VAMP( ch ) )  )
							 num_marks++;
						if ( num_marks > 0 )
						{
							char  marksbuf[MSL];

							if ( IS_WOLF( ch ) )
							{
								sprintf( marksbuf, "{CThis room has {W%d {C%s{W.{x\n\r", num_marks, ( num_marks > 1 ) ? "scents" : "scent" );
							}
							else if ( IS_VAMP( ch ) )
							{
								sprintf( marksbuf, "{mThis room has {W%d {RBlood%s{x.\n\r", num_marks, ( num_marks > 1) ? "Signs" : "Sign" );
							}
							send_to_char( marksbuf, ch );
						}
 
					}
		
	
	if ( str_cmp( arg1, "zxcvbnm") )
			 {
	show_room_list_to_char( ch->in_room->first_content, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->first_person,   ch );
				
	{
		char money_show[MSL];
		sprintf( money_show, "%s lie in a pile.\n\r", money_string( ch->in_room->treasure ) );
		if ( str_prefix( " lie", money_show ) )
			send_to_char( money_show, ch );
	}
				}
	return;
		}
		
		
		if ( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
		{
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
			send_to_char( "Look in what?\n\r", ch );
			return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
			send_to_char( "You do not see that here.\n\r", ch );
			return;
	}

	switch ( obj->item_type )
	{
	default:
			send_to_char( "That is not a container.\n\r", ch );
			break;

	case ITEM_PORTAL:
		 if ( obj->value[1] == 0 )
		 {
				act( "You don't seem to be able to look in $p.", ch, obj, NULL, TO_CHAR );
				return;
		 }
		 
		 if ( ( room = get_room_index( obj->value[0] ) ) == NULL )
		 {
				send_to_char( "You see nothing but blackness!\n\r", ch );
				return;
		 }
		 
		 act( "$n looks into $p.", ch, obj, NULL, TO_ROOM );
		 
		 if ( !IS_NPC(ch)
				&&   !IS_SET(ch->act, PLR_HOLYLIGHT)
				&&   room_is_dark( ch->in_room ) )
		 {
				act( "$p comes out into a dark place.  You see nothing!\n\r", ch, obj, NULL, TO_CHAR );
				return;
		 }
		 
		 sprintf( buf, "You look in $p and see: %s%s.%s", color_string( ch, "rooms" ), 
				room->name, color_string( ch, "normal" ) );
		 act( buf, ch, obj, NULL, TO_CHAR );
		 
		 found = FALSE;
		 if ( room->first_person != NULL )
		 {
				send_to_char( "You see the following beings:\n\r", ch );
				for ( ppl = room->first_person; ppl != NULL; ppl = ppl->next_in_room )
				{
		 if ( can_see( ch, ppl ) )
		 {
				found = TRUE;
				sprintf( buf, "%s%s%s\n\r", color_string( ch, "mobiles" ),
					 (IS_NPC(ppl) ? ppl->short_descr : ppl->name ),
					 color_string( ch, "normal" ) );
				send_to_char( buf, ch );
		 }
				}
		 }
		 if ( !found )
				act( "You see no beings through $p.", ch, obj, NULL, TO_CHAR );
		 
		 if ( room->first_content != NULL )
		 {
				send_to_char( "Some objects are visible:\n\r", ch );
				show_room_list_to_char( room->first_content, ch, TRUE, FALSE );
		 }
		 else
				act( "You see no objects through $p.", ch, obj, NULL, TO_CHAR );   
		 
		 break;
	
	case ITEM_DRINK_CON:
			if ( obj->value[1] <= 0 )
			{
		send_to_char( "It is empty.\n\r", ch );
		break;
			}

			sprintf( buf, "It's %s full of a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
				? "less than" :
		obj->value[1] < 3 * obj->value[0] / 4
				? "about"     : "more than",
		liq_table[obj->value[2]].liq_color
		);

			send_to_char( buf, ch );
			break;
				case ITEM_SPELL_MATRIX:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
		case ITEM_RANDOM_CHEST:
			if ( IS_SET(obj->value[1], CONT_CLOSED) )
			{
		send_to_char( "It is closed.\n\r", ch );
		break;
			}

			act( "$p contains:", ch, obj, NULL, TO_CHAR );
			show_list_to_char( obj->first_in_carry_list, ch, TRUE, TRUE );
			{
				char money_show[MSL];
				sprintf( money_show, "%s lie within.\n\r", money_string( obj->money ) );
				if ( str_prefix( " lie", money_show ) )
					send_to_char( money_show, ch );
			}
			break;
	}
	return;
		}

		if ( !str_cmp( arg1, "board" ) || !str_cmp( arg2, "board" ) )
		{
	/*int bnum;*/
	
	if ( ( obj = get_obj_here( ch, "board" ) ) == NULL )
	{
			send_to_char( "You do not see that here.\n\r", ch );
			return;

	}
	
	do_show_contents( ch, obj );
	return;
		}
		
		if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
		{
	show_char_to_char_1( victim, ch );
	return;
		}

		for ( obj = ch->first_carry; obj != NULL; obj = obj->next_in_carry_list)
		{
	if ( can_see_obj( ch, obj ) )
	{
			sprintf( pdesc, "\n\r%s",  get_extra_descr( arg1, obj->first_exdesc ) );
			if ( str_cmp( pdesc, "\n\r(null)" ) )
			{
							send_to_char( pdesc, ch );
							act( "$L$n closely examines $p.", ch, obj, NULL, TO_ROOM );
		return;
			}

			sprintf( pdesc, "\n\r%s",  get_extra_descr( arg1, obj->pIndexData->first_exdesc ) );

			if ( str_cmp( pdesc, "\n\r(null)" ) )
			{
							send_to_char( pdesc, ch );
							act( "$L$n closely examines $p.", ch, obj, NULL, TO_ROOM );
		return;
			}
	}

	if ( is_name( arg1, obj->name ) )
	{
			send_to_char( obj->description, ch );
			return;
	}
		}
		 
		for ( obj = ch->in_room->first_content; obj != NULL; obj = obj->next_in_room)
		{
	if ( can_see_obj( ch, obj ) )
	{
			sprintf( pdesc, "\n\r%s",  get_extra_descr( arg1, obj->first_exdesc ) );

			if ( str_cmp( pdesc, "\n\r(null)" ) )
			{
							send_to_char( pdesc, ch );
							act( "$L$n closely examines $p.", ch, obj, NULL, TO_ROOM );
		return;
			}

			sprintf( pdesc, "\n\r%s",  get_extra_descr( arg1, obj->pIndexData->first_exdesc ) );
			if ( str_cmp( pdesc, "\n\r(null)" ) )
			{
							send_to_char( pdesc, ch );
							act( "$L$n closely examines $p.", ch, obj, NULL, TO_ROOM );
		return;
			}
	}

	if ( is_name( arg1, obj->name ) )
	{
					send_to_char( obj->description, ch );
					act( "$L$n closely examines $p.", ch, obj, NULL, TO_ROOM );
		return;
	}
		}

		sprintf( pdesc, "\n\r%s",  get_extra_descr( arg1, ch->in_room->first_exdesc ) );
		if ( str_cmp( pdesc, "\n\r(null)" ) )
		{
			send_to_char( pdesc, ch );
			act( "$L$n closely examines the $t.", ch, arg1, NULL, TO_ROOM );
			return;
		}

	 if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
		else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
		else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
		else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
		else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
		else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
		else
		{
	send_to_char( "You do not see that here.\n\r", ch );
	return;
		}

		/* 'look direction' */
		if ( ( pexit = ch->in_room->exit[door] ) == NULL )
		{
	send_to_char( "Nothing special there.\n\r", ch );
	return;
		}

		if (  ( pexit->description != NULL    )
			 && ( pexit->description[0] != '\0' )
			 && (  ( !str_cmp( pexit->keyword, ""      ) )
					|| (  ( str_cmp( pexit->keyword, ""    ) )
						 && ( !str_cmp( pexit->keyword, arg1 ) ) )  )  )
	send_to_char( pexit->description, ch );
		else
	send_to_char( "Nothing special there.\n\r", ch );

		if (  ( pexit->keyword    != NULL )
			 && ( pexit->keyword[0] != '\0' )
			 && ( pexit->keyword[0] != ' ' )
			 && ( !str_cmp( pexit->keyword, arg1 ) )  )
		{
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
			act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
			act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	}
		}

		return;
}



void do_examine( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		char arg[MAX_INPUT_LENGTH];
		OBJ_DATA *obj;
		buf[0] = '\0';

		one_argument( argument, arg );

		if ( arg[0] == '\0' )
		{
	send_to_char( "Examine what?\n\r", ch );
	return;
		}

		do_look( ch, arg );

		if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
		{
	switch ( obj->item_type )
	{
	default:
			break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
			send_to_char( "When you look inside, you see:\n\r", ch );
			sprintf( buf, "in %s", arg );
			do_look( ch, buf );
	}
		if ( ch->position > POS_RESTING )
			trigger_handler( ch, obj, TRIGGER_EXAMINE );
		}

		return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
		extern char * const compass_name[];
		char buf[MAX_STRING_LENGTH];
		char buf2[MAX_STRING_LENGTH];
		EXIT_DATA *pexit;
		bool found;
		bool fAuto;
		int door;

		buf[0] = '\0';
		buf2[0] = '\0';


		fAuto  = !str_cmp( argument, "auto" );

		if ( !check_blind( ch ) )
	return;

		strcpy( buf, fAuto ? "[{cExits:" : "Obvious exits:\n\r" );

		found = FALSE;
		for ( door = 0; door <= 5; door++ )
		{
			/* check for players that are blind (irl) */
				if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
				{
					if (  ( pexit = ch->in_room->exit[door] ) != NULL
			 &&   pexit->to_room != NULL )
					{
						found = TRUE;
						if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
						{
							 sprintf( buf2, " closed %s", ( !str_cmp( pexit->keyword, "" ) 
																						? compass_name[door] 
																						: pexit->keyword )  );
						}
						else
						{
							 sprintf( buf2, " %s", ( !str_cmp( pexit->keyword, "" ) 
																		 ? compass_name[door] 
																		 : pexit->keyword )  );
						}
			safe_strcat( MAX_STRING_LENGTH, buf, buf2 );
						continue;
					}
				}


	/* Check for thieves with find_doors... */
	if ( !IS_NPC( ch ) )
		 if ( ( pexit = ch->in_room->exit[door] ) != NULL
		 &&  pexit->to_room != NULL
		 && IS_SET( pexit->exit_info, EX_CLOSED )
		 && !IS_SET( pexit->exit_info, EX_NODETECT ) 
		 && ( ch->pcdata->learned[gsn_find_doors] > number_percent() ) 
					 && ( !str_cmp( pexit->keyword, "" ) )  )
		 {
				 found = TRUE;
				 if ( fAuto )
			sprintf( buf2, " (%s)", compass_name[door] );
				 else
			sprintf( buf2, "%-5s - Door.\n\r",
				 capitalize( compass_name[door] ) );
				
				 safe_strcat( MAX_STRING_LENGTH, buf, buf2 );
				 continue;
			}
	
	if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED) 
				&&   ( !str_cmp( pexit->keyword, "" ) )  )
	{
			found = TRUE;
			if ( fAuto )
			{
		safe_strcat(MAX_STRING_LENGTH, buf, " " );
		safe_strcat(MAX_STRING_LENGTH, buf, compass_name[door] );
			}
			else
			{
		sprintf( buf + strlen(buf), "%-5s - %s\n\r",
				capitalize( compass_name[door] ),
				room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name
				);
			}
	}
		}

		if ( !found )
	safe_strcat(MAX_STRING_LENGTH, buf, fAuto ? " none" : "None.\n\r" );

		if ( fAuto )
	safe_strcat(MAX_STRING_LENGTH, buf, "{x]\n\r" );

		send_to_char( buf, ch );
		return;
}


void spsk_num(CHAR_DATA *ch)
{
	int sn;
	/* This addes up how many spells and skills a player has and stores them in a variable, :D
	 */
		ch->num_of_skills = 0;
	ch->num_of_spells = 0;

	for (sn = 0; sn < MAX_SKILL; sn++)
	{
		if (ch->pcdata->learned[sn] > 0)
		{
			if (skill_table[sn].spell_fun == spell_null)
				ch->num_of_skills++;
			else
				ch->num_of_spells++;
		}
	}
	return;
}

void do_score( CHAR_DATA *ch, char *argument )
{
		
		/* Score text has been re-formatted to look a little nicer ;)
		 * Affected by now dealt with by command 'affected'
		 * Also now added color
		 *  -- Stephen
		 */

		char buf[MAX_STRING_LENGTH];
		char buf2[MAX_STRING_LENGTH];
		char buf3[MAX_STRING_LENGTH];
		char buf4[MSL];
	char cbuf[MSL];
		int cost;
					cost = exp_to_level( ch, 0, ch->pcdata->points );
		buf[0] = '\0';
		buf2[0] = '\0';
		buf3[0] = '\0';
		buf4[0] = '\0';
	cbuf[0] = '\0';
	sprintf( buf,
		"{r+{d=============={r+{x\n\r");
	safe_strcat( MSL, buf4, buf);
	sprintf( buf,
		"{d|{x {mLuck:{x %2d     {d|{x\n\r", get_curr_lck(ch) );
	safe_strcat( MSL, buf4, buf);

		sprintf( buf,
	"{r+{d=============={r+{d================================================{r+{d\n\r");
	safe_strcat( MSL, buf4, buf );

		sprintf( buf,
	"{d| {mExp:{x%8d {d| {mClass:{x %s                                     {d|\n\r",
	ch->total_exp, "Fix");
			safe_strcat( MSL, buf4, buf );
		
		sprintf( buf3,
	"{mMoney:{x" );
	
	sprintf( buf, " %-38s {B",
	 money_string(ch->money));
	sprintf( buf2, "{d| {mTnl:{x%8d {d| %s%s {d|\n\r", ( cost - ch->exp ),  buf3, center_text(buf, 40) );
	safe_strcat( MSL, buf4, buf2 );

		sprintf( cbuf, get_clan_info(ch->pcdata->clan, CNAME) );
		sprintf( buf,
	"{r+{d=============={r+{d==={r+{d============================================{r+{d\n\r");
	safe_strcat( MSL, buf4, buf );
				
		sprintf( buf,
			"{d| {mName:{x %10s{d | {mClan:{x %-s{d|\n\r",
			 IS_NPC(ch)  ? ch->short_descr : ch->name,
			 IS_NPC(ch)  ? "n/a" : center_text (cbuf, 37) );
	safe_strcat( MSL, buf4, buf );
		
		sprintf( buf,
	"{d| {mRace:{x %10s{d | {mAge:{x  ",
	IS_NPC(ch) ? race_table[ch->race].race_title : race_table[ch->race].race_title );
		
		my_get_age( ch, buf );
	safe_strcat( MSL, buf4, buf );
		
		sprintf( buf,
			"{d    |{x\n\r" );
	safe_strcat( MSL, buf4, buf );

		sprintf( buf,
	"{d| {mLevel:{x       %3d {d| {mHours Played:{x %-4d {d                        |{x\n\r",
	ch->level,
	my_get_hours( ch ) );
	safe_strcat( MSL, buf4, buf );
		sprintf( buf,
	"{r+{d=================={r+{d=============={r+{d============================={r+{x\n\r");
	 safe_strcat( MSL, buf4, buf );
		sprintf( buf,
			"{d| {mHP:{x %4d/%4d {d   | {mAC:{x ",
			ch->hit, ch->max_hit );
	safe_strcat( MSL, buf4, buf );            
					sprintf( buf, " %5d  {d | {x", GET_AC( ch ));
	 safe_strcat( MSL, buf4, buf );
			
			sprintf( buf, " {mStr:{x %2d/%2d  {d               |{x\n\r",
			get_curr_str(ch), ch->pcdata->max_str );
	safe_strcat( MSL, buf4, buf );
			
		sprintf( buf,
			"{d| {mMP:{x %4d/%4d {d   | {mHit:{x ",
			ch->mana, ch->max_mana );
				safe_strcat( MSL, buf4, buf );            
					sprintf( buf, "%5d {d  | {x", GET_HITROLL(ch));
				safe_strcat( MSL, buf4, buf );
			
			sprintf( buf, " {mInt:{x %2d/%2d  {d               |{x\n\r",
			get_curr_int(ch), ch->pcdata->max_int );
				safe_strcat( MSL, buf4, buf );

		sprintf( buf,
			"{d| {mMV:{x %4d/%4d {d   | {mDam:{x ",
			ch->move, ch->max_move );
				safe_strcat( MSL, buf4, buf );            
					sprintf( buf, "%5d {d  | {x", GET_DAMROLL(ch));
				safe_strcat( MSL, buf4, buf );
			
			sprintf( buf, " {mWis:{x %2d/%2d  {d               |{x\n\r",
			get_curr_wis(ch), ch->pcdata->max_wis );
			 safe_strcat( MSL, buf4, buf );

		sprintf( buf,
			"{d| {mQP:{x %7d {d     | {mTrains:{x ",
			ch->quest_points );
				safe_strcat( MSL, buf4, buf );            
					sprintf( buf, "%2d {d  | {x", ch->train);
				safe_strcat( MSL, buf4, buf );
			
			sprintf( buf, " {mDex:{x %2d/%2d  {d               |{x\n\r",
			get_curr_dex(ch), ch->pcdata->max_dex );
				safe_strcat( MSL, buf4, buf );

		sprintf( buf,
			"{d| {mWimpy:{x %4d {d     | {mPracs:{x ",
			ch->wimpy );
				safe_strcat( MSL, buf4, buf );            
					sprintf( buf, "%3d {d  | {x", ch->practice);
				safe_strcat( MSL, buf4, buf );
			
			sprintf( buf, " {mCon:{x %2d/%2d  {d               |{x\n\r",
			get_curr_con(ch), ch->pcdata->max_con );
				safe_strcat( MSL, buf4, buf );
		sprintf( buf,
	"{r+{d=================={r+{d=============={r+{d============================={r+{x\n\r");
			safe_strcat( MSL, buf4, buf );
		sprintf( buf,
			"{d|                    {mKilled By Players: {x%6d                 {d |{x\n\r",
			ch->pcdata->pkilled );
			safe_strcat( MSL, buf4, buf );
				sprintf( buf,
			"{d|                    {mPlayer Kills:      {x%6d                 {d |{x\n\r",
			ch->pcdata->pkills );
			safe_strcat( MSL, buf4, buf );

		sprintf( buf,
			"{d|                    {mFinal Blows:          {xN/A                  {d|{x\n\r" );
			safe_strcat( MSL, buf4, buf );

		sprintf( buf,
			"{d|                    {mWar Score:         {x%6d                 {d |{x\n\r",
			( ch->pcdata->pkills - ch->pcdata->pkilled ) );
			safe_strcat( MSL, buf4, buf );

		sprintf( buf,
	"{r+{d==============================================================={r+{x\n\r");
			safe_strcat( MSL, buf4, buf );
		buf2[0] = '\0';

		 sprintf( buf,
		 "{d|                    {mWeight:{x {Y%7.2f/%7d {Wkg.{d                |{x\n\r",
		 ch->carry_weight, can_carry_w(ch) );
	safe_strcat( MSL, buf4, buf );
		 
		switch ( ch->position )
		{
		case POS_DEAD:     
	sprintf( buf, " {mYou are {YDEAD!!" );
	break;
		case POS_MORTAL:
	sprintf( buf, " {mYou are {Ymortally wounded." ); 
	break;
		case POS_INCAP:
	sprintf( buf, " {mYou are {Yincapaciated." );
	break;
		case POS_STUNNED:
	sprintf( buf, " {mYou are {Ystunned." );
	break;
		case POS_SLEEPING:
	sprintf( buf, " {mYou are {Ysleeping." );
	break;
		case POS_SITTING:
	sprintf( buf, " {mYou are {Ysitting." );
	break;
		case POS_RESTING:
	sprintf( buf, " {mYou are {Yresting." );
	break;
		case POS_STANDING:
	sprintf( buf, " {mYou are {Ystanding." );
	break;
		case POS_FIGHTING:
	sprintf( buf, " {mYou are {Yfighting." );
	break;
		}
		sprintf( buf2, "{d|{m%30s     {d|\n\r", center_text(buf,58) );
safe_strcat( MSL, buf4, buf2 );
				sprintf( buf,
	"{r+{d==============================================================={r+{x\n\r");
		 safe_strcat( MSL, buf4, buf );
	 send_to_char(buf4, ch);
		return;
}

void do_affected( CHAR_DATA *ch, char *argument )
{
		
		char buf[MAX_STRING_LENGTH];
		AFFECT_DATA *paf;
		bool iaff;
		buf[0] = '\0';
		iaff = FALSE;

	send_to_char( 
	"\n\r{d+{B===== {dAffected By{B ============== {dHours Left {B= {dAff/Ammount {B========{d+{x\n\r", 
	ch );
		 
	if ( ch->first_affect != NULL )
	{
		 for ( paf = ch->first_affect; paf != NULL; paf = paf->next )
		 {
				 sprintf( buf, "{B|{xSpell: %-16s", skill_table[paf->type].name );
				 send_to_char( buf, ch );

								 if ( paf->location > APPLY_NONE )
								 {
			 sprintf( buf,
					 "    {d-{B={d-{x   %3d hours  %-13s %-3d     {B|{x",
									 paf->duration,
					 affect_loc_name( paf->location ),
					 paf->modifier
						);

								 }
								 else
									sprintf( buf, "    {d-{B={d-{x   %3d hours                        {B|{x", paf->duration  );
		 send_to_char( buf, ch );

				 send_to_char( "\n\r", ch );
		 }
		/* Check to see if items have applies... */
	}    

			if ( item_has_apply( ch, ITEM_APPLY_INFRA ) )
{
			 send_to_char( "{B|{xSpell: 'Infravision' permanent duration                           {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_INV ) )
{
			 send_to_char( "{B|{xSpell: 'Invisibilty' permanent duration                           {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_DET_INV ) )
{
			 send_to_char( "{B|{xSpell: 'Detect Invisibilty' permanent duration                    {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_SANC ) )
{
			 send_to_char( "{B|{xSpell: 'Sanctuary' permanent duration                             {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_SNEAK ) )
{
			 send_to_char( "{B|{xSkill: 'Sneak' permanent duration                                 {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_HIDE ) )
{
			 send_to_char( "{B|{xSkill: 'Hide' permanent duration                                  {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_PROT ) )
{
			 send_to_char( "{B|{xSpell: 'Protection' permanent duration                            {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_ENHANCED ) )
{
			 send_to_char( "{B|{xSkill: 'Enhanced Damage' permanent duration                       {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_DET_MAG ) )
{
			 send_to_char( "{B|{xSpell: 'Detect Magic' permanent duration                          {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_DET_HID ) )
{
			 send_to_char( "{B|{xSpell: 'Detect Hidden' permanent duration                         {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_DET_EVIL ) )
{
			 send_to_char( "{B|{xSpell: 'Detect Evil' permanent duration                           {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_PASS_DOOR ) )
{
			 send_to_char( "{B|{xSpell: 'Pass Door' permanent duration                             {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_DET_POISON ) )
{
			 send_to_char( "{B|{xSpell: 'Detect Posion' permanent duration                         {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_FLY ) )
{
			 send_to_char( "{B|{xSpell: 'Fly' permanent duration                                   {B|{x\n\r", ch );
			 iaff = TRUE;
}
		if ( item_has_apply( ch, ITEM_APPLY_KNOW_ALIGN ) )
{
			 send_to_char( "{B|{xSpell: 'Know Alignment' permanent duration                        {B|{x\n\r", ch );  
			 iaff = TRUE;
}

	if ( ch->first_affect == NULL && iaff == FALSE )
			 {
				send_to_char(
				 "{B|{x                                                                  {B|{x\n\r",
				 ch );
				}

	send_to_char( 
	"{d+{B=================================================================={d+{x\n\r", 
	ch );


		

		return;
}

char *  const   day_name        [] =
{
		"the Moon", "the Bull", "Deception", "Thunder", "Freedom",
		"the Great Gods", "the Sun"
};

/* 
 * Number of months reduced from 17 (!) to 8
 * This is to bring the time it takes a character to age on mud year
 * down from 200+ rl hrs to 64 rl hrs
 * (Player's age stats were screwed in base merc!)
 */

char *  const   month_name      [] =
{
		"the Frost Giant", "the Old Forces",
		"the Grand Struggle", "Futility", 
		"the Dark Shades", "the Long Shadows", 
		"the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
		extern char str_boot_time[];
		char buf[MAX_STRING_LENGTH];
		char *suf;
		int day;
		sprintf(buf,

				"\n\rIt is %d:%d%s.\n\r",

				(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
		 time_info.min,
				time_info.hour >= 12 ? "PM" : "AM");

/*	
	 if ( day > 4 && day <  20 ) suf = "th";
		else if ( day % 10 ==  1       ) suf = "st";
		else if ( day % 10 ==  2       ) suf = "nd";
		else if ( day % 10 ==  3       ) suf = "rd";
		else                             suf = "th";

		sprintf( buf,
	"It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\rAck! started up at %s\rThe system time is %s\r",

	(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month],
	str_boot_time,
	(char *) ctime( &current_time )
	);
*/
		send_to_char( buf, ch );
		return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		char buf2[MSL];

		static char * const sky_look[4] =
		{
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
		};
		buf[0] = '\0';
		buf2[0] = '\0';

		if ( !IS_OUTSIDE(ch) )
		{
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
		}

		if ( IS_SET( ch->in_room->affected_by, ROOM_BV_SHADE ) )
		{
			 send_to_char( "You see no weather - a dark shadow masks the sky.\n\r", ch );
			 return;
		}
		
		sprintf( buf, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
		switch ( weather_info.moon_loc )
		{
			case MOON_DOWN: safe_strcat( MSL, buf, "The moon is not in the sky.\n\r");
										 break;
			case MOON_RISE: sprintf( buf2, "A %s {YMoon{x is just rising.\n\r", get_moon_phase_name( ) );
											safe_strcat( MSL, buf, buf2 );
											break;
			case MOON_LOW: sprintf( buf2, "A %s {YMoon{x is low on the horizon.\n\r", get_moon_phase_name( ) );
											safe_strcat( MSL, buf, buf2 );
											break;
			case MOON_PEAK: sprintf( buf2, "A %s {YMoon{x is high above you.\n\r", get_moon_phase_name( ) );
											safe_strcat( MSL, buf, buf2 );
											break;
			case MOON_FALL: sprintf( buf2, "A %s {YMoon{x is falling.\n\r", get_moon_phase_name( ) );
											safe_strcat( MSL, buf, buf2 );
											break;

			case MOON_SET: sprintf( buf2, "A %s {YMoon{x has just set.\n\r", get_moon_phase_name( ) );
											safe_strcat( MSL, buf, buf2 );
											break;

		}

		send_to_char( buf, ch );
		return;
}



void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;

    if ( argument[0] == '\0' )
	argument = "summary";

    for ( pHelp = first_help; pHelp != NULL; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;

	if ( is_name( argument, pHelp->keyword ) )
	{
	    if ( pHelp->level >= 0 && str_cmp( argument, "imotd" ) )
	    {
		send_to_char( pHelp->keyword, ch );
		send_to_char( "\n\r", ch );
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( pHelp->text[0] == '.' )
		send_to_char( pHelp->text+1, ch );
	    else
		send_to_char( pHelp->text  , ch );
	    return;
	}
    }

    send_to_char( "No help on that word.\n\r", ch );
    return;
}



/*
 * New 'who' command originally by Alander of Rivers of Mud.
 * Changed a lot since then though :P
 * List comes in 4 parts: Imms, Adepts, remorts then morts
 ***********************************************************
 */

void do_who( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	char buf[MSL];
	char bufcat[MSL];
	char namebuf[MSL];
	char clara[MSL];
	char extra[MSL];
	int cnt;

	if (IS_NPC(ch) )
		return;
	buf[0] = '\0';
	bufcat[0] = '\0';
	clara[0] = '\0';
	namebuf[0] = '\0';
	extra[0] = '\0';

	sprintf(buf, "{r+{d=========================================================================={r+{x\n\r");
	safe_strcat(MSL, bufcat, buf);
	sprintf(buf, "{d|{x %s {d|{x\n\r", center_text( "Dawn of War", 72 ));
	safe_strcat(MSL, bufcat, buf);
	sprintf( buf, "{r+{d========={r+{d==========={r+{d============================================{r+{d======={r+{x\n\r");
	safe_strcat(MSL, bufcat, buf);
	send_to_char(bufcat, ch);
	buf[0] = '\0';
	bufcat[0] = '\0';

	for ( d = first_desc; d != NULL; d = d->next )
	{
		 if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
			continue;

	victim = d->character;
	if (victim->hometown != ch->hometown && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) )
		continue;
	if (IS_IMMORTAL(ch) && IS_IMMORTAL(victim) )
	{
		if (!IS_SET(victim->pcdata->pflags, PFLAG_AFK) && !IS_SET(victim->act, PLR_WIZINVIS) )
			sprintf(clara, "{d|{x[{Y%s %s{x]{d|{x", "Fix", race_table[victim->race].race_name);
		else
			if (IS_SET(victim->pcdata->pflags, PFLAG_AFK) && !IS_SET(victim->act, PLR_WIZINVIS))
					sprintf(clara, "{d|{x %s {d|{x", center_text( "{gAFK{x", 7) );
		else
			if (!IS_SET(victim->pcdata->pflags, PFLAG_AFK) && IS_SET(victim->act, PLR_WIZINVIS))
					sprintf(clara, "{d|{x %s {d|{x", center_text( "{BInv{x", 7) );
		else
			sprintf(clara, "{d|{x {gAFK{x {BInv{x {d|{x");
	} else
	if(IS_SET( victim->pcdata->pflags, PFLAG_AFK  ) )
	sprintf(clara, "{d|{x %s {d|{x", center_text( "{gAFK{x", 7) );
	else
	sprintf(clara, "{d|{x[{y%s %s{x]{d|{x", "Fix", race_table[victim->race].race_name);
	safe_strcat(MSL, bufcat, clara);
	if (IS_IMMORTAL(ch) )
		sprintf(buf, " %8s {d|{x", center_text(hometown_table[victim->hometown].colname, 9));
	 else
	 {
		if (IS_IMMORTAL(victim))
		{
			if (victim->level < 110)
				sprintf(buf, " %s {d|{x", center_text("{gImmortal{x", 9 ));
			 else	
				sprintf(buf, " %s {d|{x", center_text("{RHead Guy{x", 9 ));
		} else
			if( victim->level < 50)
			{
				if(!is_remort(victim))
					sprintf(buf, " %s {d|{x", center_text("{yMortal{x", 9));
				else
					sprintf(buf, " %s {d|{x", center_text("{mRemort{x", 9));
			} else
				if (victim->level < 52)
					sprintf(buf, " %s {d|{x", center_text("{gBaron{x", 9));
				else
				if (victim->level < 54)
					sprintf(buf, " %s {d|{x", center_text("{gDuke{x", 9));
				else
				if (victim->level < 56)
					sprintf(buf, " %s {d|{x", center_text("{gLord{x", 9));
				else
					sprintf(buf, " %s {d|{x", center_text("{gLegendary{x", 9));
	 }
	 safe_strcat(MSL, bufcat, buf);

	 for ( cnt = 0; cnt < strlen(victim->pcdata->title); cnt++ )
			 if ( victim->pcdata->title[cnt] == '{' )
			 strcat( extra, "  ");

		if ( IS_SET(victim->act, PLR_THIEF ) && !IS_SET(victim->act, PLR_KILLER ) )
		{
			sprintf(namebuf, "{y%s{x%s", victim->name, victim->pcdata->title);
			sprintf(buf, " %-46s%s {d|{x", namebuf, extra);
		}
		else
			if (IS_SET(victim->act, PLR_KILLER) && !IS_SET(victim->act, PLR_THIEF ) )
			{
			sprintf(namebuf, "{r%s{x%s", victim->name, victim->pcdata->title);
			sprintf(buf, " %-46s%s {d|{x", namebuf, extra);
			}
		else if (IS_SET(victim->act, PLR_KILLER) && IS_SET(victim->act, PLR_THIEF ) )
		{
			sprintf(namebuf, "{R%s{x%s", victim->name, victim->pcdata->title);
			sprintf(buf, " %-46s%s {d|{x", namebuf, extra);
		}
		else
		{
			sprintf(namebuf, "%s%s", victim->name, victim->pcdata->title);
			sprintf(buf, " %-42s%s {d|{x", namebuf, extra);
		}
			extra[0] = '\0';
	 safe_strcat(MSL, bufcat, buf);
		 sprintf(buf, " %s {d|{x", center_text(get_clan_info(victim->pcdata->clan, CABBR), 5));
	 safe_strcat(MSL, bufcat, buf);
	 safe_strcat(MSL, bufcat, "\n\r");
	} /* For loop end*/
	sprintf( buf, "{r+{d========={r+{d==========={r+{d============================================{r+{d======={r+{x\n\r");
	safe_strcat(MSL, bufcat, buf);
	send_to_char(bufcat, ch);
	return;
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
		if ( !IS_NPC( ch ) && IS_WOLF( ch )
			 && ( IS_SHIFTED( ch ) || IS_RAGED( ch )  )   )
		{
			send_to_char( "All your inventory has transformed into fur!!!{x\n\r", ch );
			return;
		}
 
		send_to_char( "You are carrying:\n\r", ch );
	 show_list_to_char( ch->first_carry, ch, TRUE, TRUE );
		return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
		do_wear( ch, "" );
		return;

}

void do_compare( CHAR_DATA *ch, char *argument )
{
		char arg1[MAX_INPUT_LENGTH];
		char arg2[MAX_INPUT_LENGTH];
		OBJ_DATA *obj1;
		OBJ_DATA *obj2;
		int value1;
		int value2;
		char *msg;
		if ( !IS_NPC( ch ) && IS_WOLF( ch )
			 && ( IS_SHIFTED( ch ) || IS_RAGED( ch )  )   )
		{
			send_to_char( "All your inventory has transformed into fur!!!{x\n\r", ch );
			return;
		}
 
		argument = one_argument( argument, arg1 );
		argument = one_argument( argument, arg2 );
		if ( arg1[0] == '\0' )
		{
	send_to_char( "Compare what to what?\n\r", ch );
	return;
		}

		if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
		{
	send_to_char( "You do not have that item.\n\r", ch );
	return;
		}

		if ( arg2[0] == '\0' )
		{
	for ( obj2 = ch->first_carry; obj2 != NULL; obj2 = obj2->next_in_carry_list )
	{
			if ( obj2->wear_loc != WEAR_NONE
			&&   can_see_obj( ch, obj2 )
			&&   obj1->item_type == obj2->item_type
			&& ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}
	if ( obj2 == NULL )
	{
			send_to_char( "You aren't wearing anything comparable.\n\r", ch );
			return;
	}
		}
		else
		{
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
			send_to_char( "You do not have that item.\n\r", ch );
			return;
	}
		}
			
		msg         = NULL;
		value1      = 0;
		value2      = 0;

		if ( obj1 == obj2 )
		{
	msg = "You compare $p to itself.  It looks about the same.";
		}
		else if ( obj1->item_type != obj2->item_type )
		{
	msg = "You can't compare $p and $P.";
		}
		else
		{
	switch ( obj1->item_type )
	{
	default:
			msg = "You can't compare $p and $P.";
			break;

	case ITEM_ARMOR:
			value1 = obj1->value[0];
			value2 = obj2->value[0];
			break;

	case ITEM_WEAPON:
			value1 = obj1->value[1] + obj1->value[2];
			value2 = obj2->value[1] + obj2->value[2];
			break;
	}
		}

		if ( msg == NULL )
		{
			 if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
		}

		act( msg, ch, obj1, obj2, TO_CHAR );
		return;
}


void do_credits( CHAR_DATA *ch, char *argument )
{
		do_help( ch, "diku" );
		do_help( ch, "ack" );
		return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		char arg[MAX_INPUT_LENGTH];
		CHAR_DATA *victim;
		DESCRIPTOR_DATA *d;
		bool found;
		buf[0] = '\0';

		one_argument( argument, arg );

		if ( arg[0] == '\0' )
		{
	send_to_char( "Players near you:\n\r", ch );
			sprintf( buf, "In %s {x:\n\r", ch->in_room->area->name );
			send_to_char( buf, ch );
	found = FALSE;
	for ( d = first_desc; d; d = d->next )
	{
			if ( d->connected == CON_PLAYING
			&& ( victim = d->character ) != NULL
			&&   !IS_NPC(victim)
			&&   victim->in_room != NULL
			&&   victim->in_room->area == ch->in_room->area
			&&   can_see( ch, victim ) 
				&& ( !IS_WOLF( victim ) || ( !IS_SHIFTED( victim ) && !IS_RAGED( victim ) )  )
		&& ch->hometown == victim->hometown )
			{
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
				PERSON(ch, victim), victim->in_room->name );
		send_to_char( buf, ch );
			}
	}
	if ( !found )
			send_to_char( "None\n\r", ch );
		}
		else
		{
	found = FALSE;
	for ( victim = first_char; victim != NULL; victim = victim->next )
	{
			/* &&   victim->in_room->area == ch->in_room->area 
			 * taken out from below to allow global where use
			 * and then put back in... global where no fun at all. ;)
			 * -- Stephen
			 */

			if ( victim->in_room != NULL
			&&   ( !IS_AFFECTED(victim, AFF_HIDE) && !item_has_apply( victim, ITEM_APPLY_HIDE ) )
			&&  ( victim->in_room->area == ch->in_room->area )
			&&  ( !IS_AFFECTED(victim, AFF_SNEAK) && !item_has_apply( victim, ITEM_APPLY_SNEAK ) )
			&&   can_see( ch, victim )
			&&   is_name( arg, victim->name )
		&&   ch->hometown == victim->hometown )
			{
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
				PERSON(ch, victim), victim->in_room->name );
		send_to_char( buf, ch );
		break;
			}
	}
	if ( !found )
			act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
		}

		return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
	return;	
}



void set_title( CHAR_DATA *ch, char *title )
{
		char buf[MAX_STRING_LENGTH];
		buf[0] = '\0';
		if ( IS_NPC(ch) )
		{
	bug( "Set_title: NPC.", 0 );
	return;
		}

		if ( isalpha(title[0]) || isdigit(title[0]) || title[0] == '{' )
		{
	buf[0] = ' ';
	strcpy( buf+1, title );
		}
		else
		{
	strcpy( buf, title );
		}

		free_string( ch->pcdata->title );
		ch->pcdata->title = str_dup( buf );
		return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
		/* Changed this to limit title length, and to remove and brackets. -S- */
		
		char buf[MAX_STRING_LENGTH];
		int cnt;
		bool changed;
		buf[0] = '\0';

		if ( IS_NPC(ch) )
	return;

		if ( argument[0] == '\0' )
		{
	send_to_char( "Change your title to what?\n\r", ch );
	return;
		}
		changed = FALSE;

		for ( cnt = 0; cnt < strlen(argument); cnt++ )
		{
			 if ( argument[cnt] == '[' || argument[cnt] == ']' )
			 {
		changed = TRUE;
		argument[cnt] = ( argument[cnt] == ']' ) ? '>' : '<';
			 }
		}

		if ( changed )
			 send_to_char( "You used either [ or ] in your title.  They have been removed!\n\r", ch );

		/* my_strlen handles color codes as zero length */
	/* Except we don't want that... */
		if ( strlen(argument) > 34 )
		{
	send_to_char( "Title too long.  Please try again.\n\r", ch );
	return;
		}

		smash_tilde( argument );
		set_title( ch, argument );
		sprintf( buf, "You are now: %s%s.\n\r", ch->name, ch->pcdata->title );
		send_to_char( buf, ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		buf[0] = '\0';

		if ( argument[0] != '\0' )
		{
	buf[0] = '\0';
	smash_tilde( argument );
//	smash_return( argument);
	if ( argument[0] == '+' )
	{
			if ( ch->description != NULL )
		safe_strcat(MAX_STRING_LENGTH, buf, ch->description );
			argument++;
			while ( isspace(*argument) )
		argument++;
	}

	if ( my_strlen(buf) + my_strlen(argument) >= MAX_STRING_LENGTH - 2 )
	{
			send_to_char( "Description too long.\n\r", ch );
			return;
	}

	safe_strcat(MAX_STRING_LENGTH, buf, argument );
	safe_strcat(MAX_STRING_LENGTH, buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
		}

		send_to_char( "Your description is:\n\r", ch );
		send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
	send_to_char( "\n\r", ch);
		return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_INPUT_LENGTH];

		sprintf( buf,
	"You report: %d/%d hp %d/%d mana %d/%d mv %d xp.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

		send_to_char( buf, ch );

		sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d/%d mv %d xp.",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

		act( buf, ch, NULL, NULL, TO_ROOM );

		return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
		return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		char arg[MAX_INPUT_LENGTH];
		int wimpy;
		if ( deathmatch )
		{
			send_to_char( "Not during a {RDeath MAtch{x!!!\n\r", ch );
			return;
		}
		buf[0] = '\0';

		one_argument( argument, arg );

		if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
		else
	wimpy = atoi( arg );

		if ( wimpy < 0 )
		{
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
		}

		if ( wimpy > ch->max_hit )
		{
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
		}

		ch->wimpy   = wimpy;
		sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
		send_to_char( buf, ch );
		return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
		char arg1[MAX_INPUT_LENGTH];
		char arg2[MAX_INPUT_LENGTH];
		char *pArg;
		char *pwdnew;
		char *p;
		char cEnd;

		if ( IS_NPC(ch) )
	return;
		arg1[0] = '\0';
		arg2[0] = '\0';

		/*
		 * Can't use one_argument here because it smashes case.
		 * So we just steal all its code.  Bleagh.
		 */
		pArg = arg1;
		while ( isspace(*argument) )
	argument++;

		cEnd = ' ';
		if ( *argument == '\'' || *argument == '"' )
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

		pArg = arg2;
		while ( isspace(*argument) )
	argument++;

		cEnd = ' ';
		if ( *argument == '\'' || *argument == '"' )
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
			 && ( arg1[0] == '\0' || arg2[0] == '\0' )  )
		{
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
		}

		if (  ( ch->pcdata->pwd != '\0' )
			 && ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )  )
		{
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
		}

		if ( strlen(arg2) < 5 )
		{
	send_to_char(
			"New password must be at least five characters long.\n\r", ch );
	return;
		}

		/*
		 * No tilde allowed because of player file format.
		 */
		pwdnew = crypt( arg2, ch->name );
		for ( p = pwdnew; *p != '\0'; p++ )
		{
	if ( *p == '~' )
	{
			send_to_char(
		"New password not acceptable, try again.\n\r", ch );
			return;
	}
		}

		free_string( ch->pcdata->pwd );
		ch->pcdata->pwd = str_dup( pwdnew );
		save_char_obj( ch );
		send_to_char( "Ok.\n\r", ch );
		return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		char out[MAX_STRING_LENGTH*2];
		int iSocial;
		int col;
		buf[0] = '\0';
 
		col = 0;
		out[0] = '\0';

		for ( iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++ )
		{
	sprintf( buf, "%-12s", social_table[iSocial].name );
	safe_strcat(MAX_STRING_LENGTH, out, buf );
	if ( ++col % 6 == 0 )
			safe_strcat(MAX_STRING_LENGTH, out, "\n\r" );
		}
 
		if ( col % 6 != 0 )
	safe_strcat(MAX_STRING_LENGTH, out, "\n\r" );
		send_to_char( out, ch );
		return;
}


/* for old command table--Aeria */
/*
 * Contributed by Alander.
 */
 /*
void do_commands( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		char buf1[MAX_STRING_LENGTH];
		int cmd;
		int col;
 
		buf[0] = '\0';
		
		if ( IS_NPC(ch) )
			 return;
		
		
		
		buf1[0] = '\0';
		col = 0;
		for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
		{
	if ( cmd_table[cmd].level <  LEVEL_HERO
	&&   cmd_table[cmd].level <= get_trust( ch ) )
	{
	
			 if ( cmd_table[cmd].level == CLAN_ONLY 
				&& ch->pcdata->clan == 0 )
		 continue;
			 
			 if ( cmd_table[cmd].level == BOSS_ONLY
				&& !IS_SET( ch->act, PLR_CLAN_LEADER ) )
		 continue;
	
			 if ( cmd_table[cmd].level == VAMP_ONLY
				&& !IS_VAMP( ch ) )
		 continue;
			 if ( cmd_table[cmd].level == WOLF_ONLY
				&& !IS_WOLF( ch ) )
		 continue;
	
			sprintf( buf, "%-12s", cmd_table[cmd].name );
			safe_strcat(MAX_STRING_LENGTH, buf1, buf );
			if ( ++col % 6 == 0 )
		safe_strcat(MAX_STRING_LENGTH, buf1, "\n\r" );
	}
		}
 
		if ( col % 6 != 0 )
	safe_strcat(MAX_STRING_LENGTH, buf1, "\n\r" );

		send_to_char( buf1, ch );
		return;
}
*/


struct show_cmds
{
	char buf[MSL];
	sh_int col;
};

static char * const cmd_group_names  [] =
{
	"{WMisc", "{CCommunication", "{mConfiguration", "{RInformation", "{GActions", 
	"{dItem Manipulation", "{cInterMud", "{YImm" 
};

void do_commands( CHAR_DATA *ch, char *argument )
{
		static struct show_cmds show_table[8];
		char buf[MAX_STRING_LENGTH];
		char buf1[MAX_STRING_LENGTH];
		char arg1[MSL];
		int cmd;
		int col = 0;
		sh_int      show_only = -1;


		buf[0] = '\0';
		buf1[0] = '\0';

		if ( IS_NPC(ch) )
	return;
/* NOTE: This is better coded via a build_tab.c style lookup, but this is
	 quicker to code right now :) Zen */

		argument = one_argument( argument, arg1 );
		if ( arg1[0] != '\0' )
		{
			if ( !str_prefix( arg1, "miscellaneous" ))
				show_only = 0;
			else if ( !str_prefix( arg1, "communication" ))
				show_only = 1;
			else if ( !str_prefix( arg1, "configuration" ))
				show_only = 2;
			else if ( !str_prefix( arg1, "information" ))
				show_only = 3;
			else if ( !str_prefix( arg1, "actions" ))
				show_only = 4;
			else if ( !str_prefix( arg1, "objects" )
							|| !str_prefix( arg1, "manipulation") )
				show_only = 5;
			else if ( !str_prefix( arg1, "intermud" )
							|| !str_prefix( arg1, "imc" ) )
				show_only = 6;
			else if ( !str_prefix( arg1, "imm" ))
				show_only = 7;     
		}

	for ( col = 0; col < 8; col++ )
	{
		sprintf( show_table[col].buf, "%s:\n\r",  cmd_group_names[col] );
		show_table[col].col = 0;
	}

	col = 0;


		for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
		{
	if ( cmd_table[cmd].level <=  L_GOD
	&&   cmd_table[cmd].level <= get_trust( ch ) )
	{
	
			 if ( cmd_table[cmd].level == CLAN_ONLY 
				&& ch->pcdata->clan == 0 )
		 continue;
			 
			 if ( cmd_table[cmd].level == BOSS_ONLY
				&& !IS_SET( ch->act, PLR_CLAN_LEADER ) )
		 continue;
	
			 if ( cmd_table[cmd].level == VAMP_ONLY
				&& !IS_VAMP( ch ) )
		 continue;
			 if ( cmd_table[cmd].level == WOLF_ONLY
				&& !IS_WOLF( ch ) )
		 continue;
/*	     if ( cmd_table[cmd].level == SYSTEM_ONLY
				&& get_trust( ch ) < L_SUP )
		 continue;
*/
			 if ( cmd_table[cmd].show == C_SHOW_NEVER )
							 continue;
						 if (  ( cmd_table[cmd].show == C_SHOW_SKILL )
								&& ( ch->pcdata->learned[skill_lookup( cmd_table[cmd].name )] < 10 )  )
							 continue;

			sprintf( buf, "%-12s", cmd_table[cmd].name );
						safe_strcat( MSL, show_table[cmd_table[cmd].type ].buf, buf );
						if ( ++show_table[cmd_table[cmd].type].col % 6 == 0 )
							safe_strcat(MSL, show_table[cmd_table[cmd].type].buf, "\n\r" );  
	}
		}
 
	 send_to_char( buf1, ch );
	for ( col = 0; col < 8; col++ )
	{
		if (  ( show_only > -1 )
			 && ( show_only != col ) )
			continue;
		safe_strcat(MAX_STRING_LENGTH, show_table[col].buf, "\n\r" );
		send_to_char( show_table[col].buf, ch );
	}

	return;
}



struct chan_type {
			 int    bit;
			 int    min_level;
			 char * name;
			 char * on_string;
			 char * off_string;
			 };
			 
struct chan_type channels[] = {
			{ CHANNEL_AUCTION, 0, "auction",
				"[ +AUCTION  ] You hear biddings.{x\n\r",
				"[ -auction  ] You don't hear biddings.\n\r" },
			{ CHANNEL_MUSIC,   0, "music", 
				"[ +MUSIC    ] You hear people's music quotes.\n\r",
				"[ -music    ] You don't hear people's music quotes.\n\r" },
			{ CHANNEL_IMMTALK, LEVEL_HERO, "immtalk",
				"[ +IMMTALK  ] You hear what other immortals have to say.\n\r",
				"[ -immtalk  ] You don't hear what other immortals have to say.\n\r" },
			{ CHANNEL_NOTIFY,  LEVEL_HERO, "notify",
				"[ +NOTIFY   ] You hear player information.\n\r",
				"[ -notify   ] You don't hear player information.\n\r" },
			{ CHANNEL_LOG,0, "log",
			"[ +LOG      ] You receive LOG_FILE details.\n\r",
			"[ -log      ] You don't receive LOG_FILE details.\n\r"},
	
			{ CHANNEL_QUEST,   0, "quest",
			"[  +QUEST   ] You hear roleplaying quests!\n\r",
			"[  -quest   ] You ignore roleplaying quests.\n\r" },
	
			{ CHANNEL_NEWBIE, 0, "newbie",
			 "[ +NEWBIE   ] You hear newbie's chit-chat.\n\r",
			 "[ -newbie   ] You don't hear newbie's chit-chat.\n\r" },
			

			{ CHANNEL_QUESTION, 0, "question",
			"[ +QUESTION ] You hear player's questions & answers.\n\r",
			"[ -question ] You don't hear player's questions & answers.\n\r"},
			

			{ CHANNEL_SHOUT,0, "shout",
			"[ +SHOUT    ] You hear people shouting.\n\r",
			"[ -shout    ] You don't hear people shouting.\n\r"},
			

			{ CHANNEL_YELL,0, "yell",
			"[ +YELL     ] You hear people yelling.\n\r",
			"[ -yell     ] You don't hear people yelling.\n\r"},
			

			{ CHANNEL_CLAN,0, "clan",
			"[ +CLAN     ] You hear clan chit-chat.\n\r",
			"[ -clan     ] You don't hear clan chit-chat.\n\r"},
			
						{ CHANNEL_GAME,0, "game",
						"[ +GAME     ] You hear players participating in games.\n\r",   
						"[ -game     ] You no longer hear people playing games.\n\r"},  
	
			{ CHANNEL_RACE,0, "race",
			"[ +RACE     ] You hear your race's chit-chat.\n\r",
			"[ -race     ] You don't hear your race's chit-chat.\n\r"},
	
			{ CHANNEL_FLAME,0, "flame",
			"[ +FLAME    ] You hear players flaming each other.\n\r",
			"[ -flame    ] You don't hear players flaming each other.\n\r"},
			

			{ CHANNEL_ZZZ,0, "zzz",
			"[ +ZZZ      ] You hear sleeping players chatting.\n\r",
			"[ -zzz      ] You don't hear sleeping players chatting.\n\r"},
			
	
			{ CHANNEL_INFO,0, "info",
			"[ +INFO     ] You hear information about deaths, etc.\n\r",
			"[ -info     ] You don't hear information about deaths, etc.\n\r"},
			
			{ CHANNEL_CREATOR, 85, "creator",
			"[ +CREATOR  ] You hear Creator's discussing Mud matters.\n\r",
			"[ -creator  ] You don't hear Creators discussing Mud matters\n\r" },
			
			{ CHANNEL_ALLCLAN, 85, "allclan",
			"[ +ALLCLAN  ] You hear ALL clan channels.\n\r",
			"[ -allclan  ] You don't hear ALL clan channels.\n\r" },
			
			{ CHANNEL_ALLRACE, 85, "allrace",
			"[ +ALLRACE  ] You hear ALL race channels.\n\r",
			"[ -allrace  ] You don't hear ALL race channels.\n\r" },
			
			{ CHANNEL_HERMIT, 0, "hermit",
			"[ +HERMIT   ] You are NOT ignoring all channels.\n\r",
			"[ -hermit   ] YOU ARE IGNORING ALL CHANNELS!\n\r" },
			
			{ CHANNEL_BEEP,   0, "beep",
			"[ +BEEP     ] You accept 'beeps' from other players.\n\r",
			"[ -beep     ] You are ignoring 'beeps' from other players.\n\r" },
			{ CHANNEL_ALLCLAN, 85, "allclan",
			"[ +ALLCLAN  ] You hear ALL clan channels.\n\r",
			"[ -allclan  ] You don't hear ALL clan channels.\n\r" },
			
			{ CHANNEL_FAMILY, 0, "vampyre",
			"",
			"" },
			
			{ CHANNEL_DIPLOMAT, 0, "diplomat",
			"[ +DIPLOMAT ] You hear diplomatic negotioations.\n\r",
			"[ -diplomat ] YOU do not hear diplomatic negotiations\n\r" },
			
			{ CHANNEL_REMORTTALK,   0, "{",
			"[ +{        ] You hear remorts gossiping amongst themselves.\n\r",
			"[ -{        ] You are ignoring idle remort chatter.\n\r" },

			{ CHANNEL_CRUSADE,   0, "crusade",
			"[ +CRUSADE  ] You hear quest information.\n\r",
			"[ -crusade  ] You are ignoring quest information.\n\r" },

			{ CHANNEL_ADEPT,   0, "adept",
			"[  +ADEPT   ] You hear Adepts plotting to take over the mud!\n\r",
			"[  -adept   ] You ignore those bored dudes and dudettes.\n\r" },

			{ CHANNEL_GAME,   0, "game",
			"[   +GAME   ] You hear non-roleplaying games.\n\r",
			"[   -game   ] You are ignoring non-roleplaying games.\n\r" },

/*
			{ CHANNEL_OOC,   0, "ooc",
			"[   +OOC    ] You hear non-roleplaying chit-chat.\n\r",
			"[   -ooc    ] You ignore those boring non-roleplaying fools!\n\r" },
*/			    
			{ 0,0, NULL, NULL }
			};
		 
			
				
				
			

				

void do_channels( CHAR_DATA *ch, char *argument )
{
		char arg[MAX_INPUT_LENGTH];
		char buffer[MAX_STRING_LENGTH];
		int a,trust;

		one_argument( argument, arg );

		if ( arg[0] == '\0' )
		{
			if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
			{
	send_to_char( "You are silenced.\n\r", ch );
	return;
			}

			trust=get_trust(ch);
			buffer[0]='\0';
			safe_strcat( MAX_STRING_LENGTH, buffer, "Channels:\n\r");
	
			for (a=0; channels[a].bit != 0; a++)
			{
	if (trust >= channels[a].min_level)
	{
					char  colbuf[10];
					colbuf[0] = '\0';

		if ( IS_SET(ch->deaf, channels[a].bit) )
					{
						if ( !IS_NPC( ch ) )
						{
							sprintf( colbuf, "@@%c", ch->pcdata->dimcol );
							safe_strcat( MSL, buffer, colbuf );
						}
			safe_strcat( MAX_STRING_LENGTH, buffer, channels[a].off_string);
					}
		else
					{
						if ( !IS_NPC( ch ) )
						{
							sprintf( colbuf, "@@%c", ch->pcdata->hicol );
							safe_strcat( MSL, buffer, colbuf );
						}
			safe_strcat( MAX_STRING_LENGTH, buffer, channels[a].on_string);
					}
	}
	
	safe_strcat( MAX_STRING_LENGTH, buffer,"{x");
			}
			send_to_char( buffer, ch );
		}
		else
		{
	bool fClear;
	int bit;

			 if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
			send_to_char( "Channels -channel or +channel?\n\r", ch );
			return;
	}

	/* Now check through table to set/unset channel... */
	bit = 0;
	for ( a = 0; channels[a].bit != 0; a++ )
	{
		 if ( channels[a].min_level > get_trust( ch ) )
				continue;
		 if ( !str_prefix( arg+1, channels[a].name ) )
		 {
				bit = channels[a].bit;
				break;
		 }
	}
	if ( bit == 0 )
	{
		 send_to_char( "Set or clear which channel?\n\r", ch );
		 return;
	}

	if ( fClear ) 
			REMOVE_BIT (ch->deaf, bit);
	else
			SET_BIT    (ch->deaf, bit);

	send_to_char( "Ok.\n\r", ch );
		}

		return;
}



/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
		char arg[MAX_INPUT_LENGTH];
		char buf[MSL];
		if ( IS_NPC(ch) )
	return;

		one_argument( argument, arg );

		if ( arg[0] == '\0' )
		{
	send_to_char( "[ Keyword  ] Option\n\r", ch );

	send_to_char( IS_SET( ch->act, PLR_NOSUMMON )
		 ? "[{g+NOSUMMON{x ] You may not be summoned.\n\r"
		 : "[{r-nosummon{x ] You may be summoned.\n\r"
		 , ch );

	send_to_char( IS_SET( ch->act, PLR_NOVISIT )
		 ? "[{g+NOVISIT{x  ] You may not be 'visited'.\n\r"
		 : "[{r-novisit{x  ] You may be 'visited'.\n\r"
		 , ch );
	
	send_to_char( IS_SET( ch->config, CONFIG_COLOR )
		 ? "[{g+COLOR{x    ] You recieve 'ANSI' color.\n\r"
		 : "[{g-color{x    ] You don't receive 'ANSI' color.\n\r"
		 , ch );
	
	send_to_char(  IS_SET(ch->config, CONFIG_AUTOEXIT)
			? "[{g+AUTOEXIT{x ] You automatically see exits.\n\r"
			: "[{r-autoexit{x ] You don't automatically see exits.\n\r"
			, ch );

	send_to_char(  IS_SET(ch->config, CONFIG_AUTOLOOT)
			? "[{g+AUTOLOOT{x ] You automatically loot corpses.\n\r"
			: "[{r-autoloot{x ] You don't automatically loot corpses.\n\r"
			, ch );

	send_to_char(  IS_SET(ch->config, CONFIG_AUTOSAC)
			? "[{g+AUTOSAC{x  ] You automatically sacrifice corpses.\n\r"
			: "[{r-autosac{x  ] You don't automatically sacrifice corpses.\n\r"
			, ch );
			
	send_to_char(  IS_SET(ch->config, CONFIG_AUTOASSIST)
			? "[{g+AUTOASS{x  ] You automatically assist in combat.\n\r"
			: "[{r-autoass{x  ] You don't automatically assist in combat.\n\r"
			, ch );
			
	send_to_char(  IS_SET(ch->config, CONFIG_BLANK)
			? "[{g+BLANK{x    ] You have a blank line before your prompt.\n\r"
			: "[{r-blank{x    ] You have no blank line before your prompt.\n\r"
			, ch );

	send_to_char(  IS_SET(ch->config, CONFIG_BRIEF)
			? "[{g+BRIEF{x    ] You see brief descriptions.\n\r"
			: "[{r-brief{x    ] You see long descriptions.\n\r"
			, ch );
	 
	send_to_char(  IS_SET(ch->config, CONFIG_COMBINE)
			? "[{g+COMBINE{x  ] You see object lists in combined format.\n\r"
			: "[{r-combine{x  ] You see object lists in single format.\n\r"
			, ch );

	send_to_char(  IS_SET(ch->config, CONFIG_PROMPT)
			? "[{g+PROMPT{x   ] You have a prompt.\n\r"
			: "[{r-prompt{x   ] You don't have a prompt.\n\r"
			, ch );

	send_to_char(  IS_SET(ch->config, CONFIG_TELNET_GA)
			? "[{g+TELNETGA{x ] You receive a telnet GA sequence.\n\r"
			: "[{r-telnetga{x ] You don't receive a telnet GA sequence.\n\r"
			, ch );
	send_to_char(  IS_SET(ch->config, CONFIG_FULL_ANSI)
			? "[{g+FULLANSI{x ] Your client supports FULL ANSI.\n\r"
			: "[{r-fullansi{x ] Your client does not support full ANSI (GMUD).\n\r"
			, ch );
	send_to_char(  IS_SET(ch->config, CONFIG_MAPPER)
			? "[{g+DISPLAY{x  ] You are viewing the ASCII display map!\n\r"
			: "[{r-display{x  ] Your are not viewing the ASCII display map.\n\r"
			, ch );
	send_to_char(  IS_SET(ch->config, CONFIG_JUSTIFY)
			? "[{g+JUSTIFY{x  ] You are viewing rooms in space justified format.\n\r"
			: "[{r-justify{x  ] Your are not viewing rooms space justified.\n\r"
			, ch );
	send_to_char(  IS_SET(ch->act, PLR_NO_PRAY)
			? "[{g+NOPRAY{x   ] You cannot use 'pray'.\n\r"
			: ""
			, ch );
	
	send_to_char(  IS_SET(ch->act, PLR_SILENCE)
			? "[{g+SILENCE{x  ] You are silenced.\n\r"
			: ""
			, ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_EMOTE)
			? ""
			: "[{r-emote{x    ] You can't emote.\n\r"
			, ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_TELL)
			? ""
			: "[{r-tell{x     ] You can't use 'tell'.\n\r"
			, ch );
	sprintf( buf, "Terminal set to:  %i Rows, %i Columns.\n\r", 
		ch->pcdata->term_rows, ch->pcdata->term_columns );
	send_to_char( buf, ch );
		}
		else
		{
	bool fSet;
	int bit;
	bool config_var= TRUE;
			 if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
			send_to_char( "Config -option or +option?\n\r", ch );
			return;
	}

			 if ( !str_cmp( arg+1, "autoexit" ) ) bit = CONFIG_AUTOEXIT;
	else if ( !str_cmp( arg+1, "autoloot" ) ) bit = CONFIG_AUTOLOOT;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit = CONFIG_AUTOSAC;
	else if ( !str_cmp( arg+1, "autoass"  ) ) bit = CONFIG_AUTOASSIST;
	else if ( !str_cmp( arg+1, "blank"    ) ) bit = CONFIG_BLANK;
	else if ( !str_cmp( arg+1, "brief"    ) ) bit = CONFIG_BRIEF;
	else if ( !str_cmp( arg+1, "combine"  ) ) bit = CONFIG_COMBINE;
	else if ( !str_cmp( arg+1, "prompt"   ) ) bit = CONFIG_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) bit = CONFIG_TELNET_GA;
	else if ( !str_cmp( arg+1, "nosummon" ) ) {config_var=FALSE;bit = PLR_NOSUMMON;}
	else if ( !str_cmp( arg+1, "novisit"  ) ) {config_var=FALSE;bit = PLR_NOVISIT;}
	else if ( !str_cmp( arg+1, "color"   ) ) bit = CONFIG_COLOR;
	else if ( !str_cmp( arg+1, "fullansi"   ) ) bit = CONFIG_FULL_ANSI;
	else if ( !str_cmp( arg+1, "display"   ) ) bit = CONFIG_MAPPER;
	else if (  IS_IMMORTAL( ch ) 
					&& ( !str_cmp( arg+1, "justify"   ) ) ) bit = CONFIG_JUSTIFY;

	else if ( !str_prefix("rows", arg+1 ) )
	{
		char arg1[MSL];
		argument = one_argument( argument, arg1 );
		argument = one_argument( argument, arg1 );
		if ( !is_number( arg1 ) )
		{
			send_to_char( "That is not a number.\n\r", ch );
			return;
		}
		ch->pcdata->term_rows = URANGE( 10, atoi( arg1 ), 80 );
		return;
	}
	else
	{
			send_to_char( "Config which option?\n\r", ch );
			return;
	}

	if ( fSet )
	{
			if (config_var)
				SET_BIT (ch->config, bit);
			else
				SET_BIT (ch->act, bit);
			//SET_BIT (config_var?ch->config:ch->act, bit);
			if ( bit == CONFIG_FULL_ANSI )
			{
				char scrollbuf[MSL];
				sprintf( scrollbuf, "%s%s%s%i;%ir%s%i;%iH",
					CRS_RESET,
					CRS_CLS,
					CRS_CMD,
					0,
					ch->pcdata->term_rows - 12,
					CRS_CMD,
					ch->pcdata->term_rows - 13,
					0 );
				send_to_char( scrollbuf, ch );
			}
	}
	else
	{
		if (config_var)
			REMOVE_BIT (ch->config, bit);
		else
			REMOVE_BIT (ch->act, bit);

			if ( bit == CONFIG_FULL_ANSI )
			{

					
				send_to_char( CRS_RESET, ch );
				send_to_char( "Done.\n\r", ch );

			}
	}

	send_to_char( "Ok.\n\r", ch );
		}

		return;
}

void do_wizlist ( CHAR_DATA *ch, char *argument )
{

		do_help ( ch, "wizlist" );
		return;

}

void do_race_list ( CHAR_DATA *ch, char *argument )
{
	 int          iRace;
	 char         buf[MAX_STRING_LENGTH];
		
	 send_to_char( "    Here follows a list of current races within ACK! Mud:\n\r", ch );
	 send_to_char( "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r", ch );
	 send_to_char( "\n\r", ch );
	 if ( IS_IMMORTAL( ch ) )
			send_to_char( "    No.   Room.    Abbr.    Name.   M/C   Classes: (Good->Bad)\n\r", ch );
	 else
			send_to_char( "   Abbr.    Name.  M/C  Classes: (Good->Bad)\n\r", ch );
	 
	 for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	 {
			if ( IS_IMMORTAL( ch ) )
			{
				sprintf( buf, "   %3d   %5d    %5s     %8s %2d %s %s\n\r", 
					iRace, race_table[iRace].recall,
					race_table[iRace].race_name,  race_table[iRace].race_title,
					race_table[iRace].classes, race_table[iRace].comment,
					( race_table[iRace].player_allowed == TRUE ?
						"{CPlayer{x" :
						"{RNPC ONLY{x" ) );
				send_to_char( buf, ch );
			}

			else  if ( race_table[iRace].player_allowed == TRUE )
			{   
				sprintf( buf, "   %5s    %8s    %2d %s\n\r",  
				race_table[iRace].race_name,  race_table[iRace].race_title,
				race_table[iRace].classes, race_table[iRace].comment );
				send_to_char( buf, ch );
			}


	 }
	 send_to_char( "\n\r", ch );
	 send_to_char( "M/C = Number of classes available.\n\r", ch );
	 send_to_char( "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r", ch );
	 return;
}

bool has_spell( CHAR_DATA *ch, int sn)
{

	return TRUE;
}

void do_spells ( CHAR_DATA *ch, char *argument )
{

	return;

}

void do_slist ( CHAR_DATA *ch, char *argument )
{
	return;

}

/* by passing the conf command - Kahn */

void do_autoexit ( CHAR_DATA *ch, char *argument )
{

		( IS_SET ( ch->config, CONFIG_AUTOEXIT )
		 ? do_config( ch, "-autoexit" )
		 : do_config( ch, "+autoexit" ) );

}

void do_autoloot ( CHAR_DATA *ch, char *argument )
{

		( IS_SET ( ch->config, CONFIG_AUTOLOOT )
		 ? do_config( ch, "-autoloot" )
		 : do_config( ch, "+autoloot" ) );

}

void do_autoass ( CHAR_DATA *ch, char *argument )
{

		( IS_SET ( ch->config, CONFIG_AUTOASSIST )
		 ? do_config( ch, "-autoass" )
		 : do_config( ch, "+autoass" ) );

}

void do_togmap ( CHAR_DATA *ch, char *argument )
{
			return;

}

void do_autosac ( CHAR_DATA *ch, char *argument )
{

		( IS_SET ( ch->config, CONFIG_AUTOSAC )
		 ? do_config( ch, "-autosac" )
		 : do_config( ch, "+autosac" ) );

}

void do_blank ( CHAR_DATA *ch, char *argument )
{

		( IS_SET ( ch->config, CONFIG_BLANK )
		 ? do_config( ch, "-blank" )
		 : do_config( ch, "+blank" ) );

}

void do_brief ( CHAR_DATA *ch, char *argument )
{

		( IS_SET ( ch->config, CONFIG_BRIEF )
		 ? do_config( ch, "-brief" )
		 : do_config( ch, "+brief" ) );

}

void do_combine ( CHAR_DATA *ch, char *argument )
{

		( IS_SET ( ch->config, CONFIG_COMBINE )
		 ? do_config( ch, "-combine" )
		 : do_config( ch, "+combine" ) );

}
 
void do_pagelen ( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		char arg[MAX_INPUT_LENGTH];
		int lines;

		one_argument( argument, arg );

		if ( arg[0] == '\0' )
	lines = 20;
		else
	lines = atoi( arg );

		if ( lines < 1 )
		{
	send_to_char(
		"Negative or Zero values for a page pause is not legal.\n\r",
				 ch );
	return;
		}

		ch->pcdata->pagelen = lines;
		sprintf( buf, "Page pause set to %d lines.\n\r", lines );
		send_to_char( buf, ch );
		return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];
		buf[0] = '\0';

	 if ( argument[0] == '\0' )
	 {
			 ( IS_SET ( ch->config, CONFIG_PROMPT )
	? do_config( ch, "-prompt" )
	: do_config( ch, "+prompt" ) );
			 return;
	 }

	 if( !strcmp( argument, "all" ) )
			strcpy( buf, "<%hhp %mm %vmv> ");
	 else
	 {
			if ( strlen(argument) > 200 )
	 argument[200] = '\0';
			strcpy( buf, argument );
			smash_tilde( buf );
	 }

	 free_string( ch->prompt );
	 ch->prompt = str_dup( buf );
	 send_to_char( "Ok.\n\r", ch );
	 return;
} 

void do_auto( CHAR_DATA *ch, char *argument )
{

		do_config( ch, "" );
		return;

}


void do_players( CHAR_DATA *ch, char *argument )
{
		
		send_to_char( "This command is no longer needed, as 'WHO' carries all details.\n\r", ch );
		return;
}

void do_diagnose( CHAR_DATA *ch, char *argument )
{
		char buf[MAX_STRING_LENGTH];
		char arg[MAX_INPUT_LENGTH];
		CHAR_DATA *victim;
		int pct;

		
		one_argument( argument, arg );

		if ( arg[0] == '\0' )
		{
	send_to_char( "Diagnose whom?\n\r", ch );
	return;
		}

		if ( ( victim = get_char_room( ch, arg ) ) == NULL )
		{
	send_to_char( "They're not here.\n\r", ch );
	return;
		}
		
		
		if ( can_see( victim, ch ) )
		{
	act( "$n gives you the once-over.", ch, NULL, victim, TO_VICT    );
	act( "$n gives $N the once-over.",  ch, NULL, victim, TO_NOTVICT );
		}
		else
		{
	send_to_char( "They're not here.\n\r", ch );
	return;
		}
		
		if ( victim->max_hit > 0 )
	pct = ( 100 * victim->hit ) / victim->max_hit;
		else
	pct = -1;

		strcpy( buf, PERS(victim, ch) );

	 if ( pct >= 100 ) safe_strcat(MAX_STRING_LENGTH, buf, " --  [5] 4  3  2  1   \n\r" );
		else if ( pct >=  80 ) safe_strcat(MAX_STRING_LENGTH, buf, " --   5 [4] 3  2  1   \n\r" );
		else if ( pct >=  60 ) safe_strcat(MAX_STRING_LENGTH, buf, " --   5  4 [3] 2  1   \n\r" );
		else if ( pct >=  40 ) safe_strcat(MAX_STRING_LENGTH, buf, " --   5  4  3 [2] 1   \n\r" );
		else                   safe_strcat(MAX_STRING_LENGTH, buf, " --   5  4  3  2 [1]  \n\r" );
		
		buf[0] = UPPER(buf[0]);
		send_to_char( buf, ch );


		return;
}




void do_heal( CHAR_DATA *ch, char *argument )
{
	 /* This function used when a player types heal when in a room with
		* a mob with ACT_HEAL set.  Cost is based on the ch's level.
		* -- Stephen
		*/

		CHAR_DATA *mob;
		char buf[MAX_STRING_LENGTH];
		int mult;           /* Multiplier used to calculate costs. */
		char costbuf[MSL];
		char changebuf[MSL];
		char * give;
		char givebuf[MSL];
		buf[0] = '\0';

		/* Check for mob with act->heal */
	 for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
	 {
			if ( IS_NPC(mob) && IS_SET(mob->act, ACT_HEAL) )
	 break;
	 }
 
	 if ( mob == NULL )
	 {
			send_to_char( "You can't do that here.\n\r", ch );
	 return;
	 }

	 mult = UMAX( 10, get_psuedo_level( ch )/2 );

	 if ( argument[0] == '\0' )
	 {
			/* Work out costs of different spells. */
			send_to_char( "Costs for spells ( approximate ):\n\r", ch );
			sprintf( costbuf, "%s", cost_to_money( (mult*100) ) );
			sprintf( buf, "Sanctuary:          %-*s.\n\r",
			 ccode_len( costbuf, 40 ), costbuf ); 
			send_to_char( buf, ch );
			sprintf( costbuf, "%s", cost_to_money( (mult*50) ) );
			sprintf( buf, "Heal:               %-*s.\n\r", 
			 ccode_len( costbuf, 40 ), costbuf ); 
			send_to_char( buf, ch );
			sprintf( costbuf, "%s", cost_to_money( (mult*20) ) );
			sprintf( buf, "Invisibilty:        %-*s.\n\r",
			 ccode_len( costbuf, 40 ), costbuf ); 
			send_to_char( buf, ch );
			sprintf( costbuf, "%s", cost_to_money( (mult*10) ) );
			sprintf( buf, "Detect Invisibilty: %-*s.\n\r",
			 ccode_len( costbuf, 40 ), costbuf ); 
			send_to_char( buf, ch );
			sprintf( costbuf, "%s", cost_to_money( (mult*10) ) );
			sprintf( buf, "Refresh:            %-*s.\n\r",
			 ccode_len( costbuf, 40 ), costbuf ); 
			send_to_char( buf, ch );
			sprintf( costbuf, "%s", cost_to_money( (mult*20) ) );
			sprintf( buf, "Night Vision:       %-*s.\n\r", 
			 ccode_len( costbuf, 40 ), costbuf ); 
			send_to_char( buf, ch );
			sprintf( costbuf, "%s", cost_to_money( (mult*200) ) );
			sprintf( buf, "Magical Dispel:     %-*s.\n\r",
			 ccode_len( costbuf, 40 ), costbuf ); 
			send_to_char( buf, ch );
			sprintf( costbuf, "%s", cost_to_money( (2000) ) );
			sprintf( buf, "Mana:     %-*s.\n\r", 
			 ccode_len( costbuf, 40 ), costbuf ); 
			send_to_char( buf, ch );


			send_to_char( "Type HEAL [S|H|I|D|R|N|M|P]\n\r", ch );
			send_to_char( "Eg: 'HEAL H' will result in the heal spell being cast.\n\r", ch );
			send_to_char( "\n\r**ALL** Spells will be cast on the buyer ONLY.\n\r", ch );
			return;
	 }

	 switch ( UPPER( argument[0] ) )
	 {
			case 'S':                                /* Sanc */
	 if ( money_value( ch->money ) < ( mult * 100 ) )
	 {
			send_to_char( "You don't have enough money...\n\r", ch );
			return;
	 }
	 act( "$N gestures towards $n.", ch, NULL, mob, TO_NOTVICT );
	 act( "$N gestures towards you.", ch, NULL, mob, TO_CHAR );
	 spell_sanctuary( skill_lookup( "sanc" ), mult, ch, ch, NULL );
	 give = take_best_coins( ch->money,  ( mult * 100 ) );
	 give = one_argument( give, changebuf );
	 sprintf( givebuf, "%s to %s", give, mob->name );
	 do_give( ch, givebuf ); 
	 join_money( round_money( atoi(changebuf), TRUE ), ch->money );
	 send_to_char( "The healer hands you some change.\n\r", ch );
	 break;
			case 'P':                                 /* mana */
	 if ( money_value( ch->money ) < 1000 )
	 {
			send_to_char( "You don't have enough money...\n\r", ch );
			return;
	 }
	 act( "$N gestures towards $n.", ch, NULL, mob, TO_NOTVICT );
	 act( "$N gestures towards you.", ch, NULL, mob, TO_CHAR );
	 give = take_best_coins( ch->money, 1000 );
	 ch->mana = UMIN( ch->max_mana, ch->mana + 50 );
	 give = one_argument( give, changebuf );
	 sprintf( givebuf, "%s to %s", give, mob->name );
	 do_give( ch, givebuf ); 
	 join_money( round_money( atoi(changebuf), TRUE ), ch->money );
	 send_to_char( "The healer hands you some change.\n\r", ch );
	 break;
			
			case 'H':                                 /* Heal */
	 if ( money_value( ch->money ) < ( mult * 50 ) )
	 {
			send_to_char( "You don't have enough money...\n\r", ch );
			return;
	 }
	 act( "$N gestures towards $n.", ch, NULL, mob, TO_NOTVICT );
	 act( "$N gestures towards you.", ch, NULL, mob, TO_CHAR );
	 spell_heal( skill_lookup( "heal" ), mult, mob, ch, NULL );
	 give = take_best_coins( ch->money,( mult * 50 ) );
	 give = one_argument( give, changebuf );
	 sprintf( givebuf, "%s to %s", give, mob->name );
	 do_give( ch, givebuf ); 
	 join_money( round_money( atoi(changebuf), TRUE ), ch->money );
	 send_to_char( "The healer hands you some change.\n\r", ch );
	 break;
			case 'I':                                 /* invis */
	 if ( money_value( ch->money ) < ( mult * 20 ) )
	 {
			send_to_char( "You don't have enough money...\n\r", ch );
			return;
	 }
	 act( "$N gestures towards $n.", ch, NULL, mob, TO_NOTVICT );
	 act( "$N gestures towards you.", ch, NULL, mob, TO_CHAR );
	 spell_invis( skill_lookup( "invis" ), mult, mob, ch, NULL );
	 give = take_best_coins( ch->money,( mult * 20 ));
	 give = one_argument( give, changebuf );
	 sprintf( givebuf, "%s to %s", give, mob->name );
	 do_give( ch, givebuf ); 
	 join_money( round_money( atoi(changebuf), TRUE ), ch->money );
	 send_to_char( "The healer hands you some change.\n\r", ch );
	 break;
			case 'D':                                 /* detect invis */
	 if ( money_value( ch->money ) < ( mult * 10 ) )
	 {
			send_to_char( "You don't have enough money...\n\r", ch );
			return;
	 }
	 act( "$N gestures towards $n.", ch, NULL, mob, TO_NOTVICT );
	 act( "$N gestures towards you.", ch, NULL, mob, TO_CHAR );
	 spell_detect_invis( skill_lookup( "detect invis" ), mult, mob, ch, NULL );        
	 give = take_best_coins( ch->money, ( mult * 10 ) );
	 give = one_argument( give, changebuf );
	 sprintf( givebuf, "%s to %s", give, mob->name );
	 do_give( ch, givebuf ); 
	 join_money( round_money( atoi(changebuf), TRUE ), ch->money );
	 send_to_char( "The healer hands you some change.\n\r", ch );
	 break;
			case 'R':                                 /* refresh */
	 if ( money_value( ch->money ) < ( mult * 10 ) )
	 {
			send_to_char( "You don't have enough money...\n\r", ch );
			return;
	 }
	 act( "$N gestures towards $n.", ch, NULL, mob, TO_NOTVICT );
	 act( "$N gestures towards you.", ch, NULL, mob, TO_CHAR );
	 spell_refresh( skill_lookup( "refresh" ), mult, mob, ch, NULL );      
	 give = take_best_coins( ch->money,( mult * 10 )); 
	 give = one_argument( give, changebuf );
	 sprintf( givebuf, "%s to %s", give, mob->name );
	 do_give( ch, givebuf ); 
	 join_money( round_money( atoi(changebuf), TRUE ), ch->money );
	 send_to_char( "The healer hands you some change.\n\r", ch );
	 break;      
			case 'N':                                 /* Infra */
	 if ( money_value( ch->money )< ( mult * 20 ) )
	 {
			send_to_char( "You don't have enough money...\n\r", ch );
			return;
	 }
	 act( "$N gestures towards $n.", ch, NULL, mob, TO_NOTVICT );
	 act( "$N gestures towards you.", ch, NULL, mob, TO_CHAR );
	 spell_infravision( skill_lookup( "infra" ), mult, ch, ch, NULL );
	 give = take_best_coins( ch->money,( mult * 20 ) );
	 give = one_argument( give, changebuf );
	 sprintf( givebuf, "%s to %s", give, mob->name );
	 do_give( ch, givebuf ); 
	 join_money( round_money( atoi(changebuf), TRUE ), ch->money );
	 send_to_char( "The healer hands you some change.\n\r", ch );
	 break;
			case 'M':                                 /* dispel */   
	 if ( money_value( ch->money ) < ( mult * 200 ) )
	 {
			send_to_char( "You don't have enough money...\n\r", ch );
			return;
	 }
	 /* No acts, as they are in spell_dispel_magic.  Doh. */
	 spell_dispel_magic( skill_lookup( "dispel magic" ), mult*5, mob, ch, NULL );
	 give = take_best_coins( ch->money,( mult * 200 ) );
	 give = one_argument( give, changebuf );
	 sprintf( givebuf, "%s to %s", give, mob->name );
	 do_give( ch, givebuf ); 
	 join_money( round_money( atoi(changebuf), TRUE ), ch->money );
	 send_to_char( "The healer hands you some change.\n\r", ch );
	 break;
			default:
	 send_to_char( "Are you sure you're reading the instructions right??\n\r", ch );
	 return;
	 }
	 return;
}

void do_gain( CHAR_DATA *ch, char *argument)
{
	char arg[MSL];
	char buf[MSL];
	char sendbuf[MSL];
	int col = 0;
	int cnt;
	buf[0] = '\0';
	sendbuf[0] = '\0';

	one_argument(argument, arg);

	if (IS_NPC(ch)) // May change? Perhaps have pets level up with skills?
		return;

	if (IS_IMMORTAL(ch))
	{
		send_to_char("You're an Immortal! You have no use for this command!\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char("\n\rYou can currentlly gain:\n\r\n\r", ch);
		for (cnt = 0; cnt < MAX_SKILL; cnt++)
		{
			if (!can_gain_skill(ch, cnt))
				continue;
			sprintf(buf, "{c%-12s{x", skill_table[cnt].name);
			strcat(sendbuf, buf);

					if ( ++col % 6 == 0 )
					strcat( sendbuf, "\n\r" );

		}
			if ( ++col % 6 != 0 )
		 strcat( sendbuf, "\n\r" );

		send_to_char(sendbuf, ch);
		return;
	} else {
		for (cnt = 0; cnt < MAX_SKILL; cnt++)
		{
			if (!str_cmp(argument, skill_table[cnt].name));
			{
				if (!can_gain_skill(ch, cnt))
					continue;

				ch->pcdata->learned[cnt] = 1;
				update_scale(ch, cnt, FALSE);
				sprintf(buf, "You now have the knowledge of %s!\n\r", skill_table[cnt].name);
				send_to_char(buf, ch);
				return;
			}
		}
			send_to_char("There is no such skill available here.\n\r", ch);
	}

	return;

}
void do_levelup( CHAR_DATA *ch, char *argument)
{
	 char buf[MSL];
	 int cost;
	 if ( ch->level == 110 )
	 {
		 send_to_char( "You are already 110! Highest possible for morts! Honest!\n\r", ch);
		 return;
	 }

	 if ( !str_cmp( argument, "imm") )
		{
		ch->level = 103;
		send_to_char( "You are now level 103.\n\r", ch );
		do_save( ch, "" );
		return;
		}
	 if ( !str_cmp( argument, "remort") )
		{
		ch->level = 50;
		send_to_char( "You are now level 50.", ch );
		return;
		}

	 cost = exp_to_level( ch, 0, ch->pcdata->points );
	 ch->exp -= cost;

	 sprintf( buf, "{YYou gain a level!{x\n\r" );
	 send_to_char( buf, ch );   
	 advance_level( ch, 0, TRUE, FALSE );

			ch->level += 1;
			if (ch->exp < 0 )
			ch->exp = 0;
			if (ch->level >= 102)
			send_to_char("You can use the \"Immtalk\" channel to talk.\n\r", ch);
			return;
}

void do_assassinate( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];
	 int  cost;
	 CHAR_DATA *mob;
	 CHAR_DATA *victim;
		buf[0] = '\0';


		 /* Check for mob with act->merc */
	 for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
	 {
			if ( IS_NPC(mob) && 
	 ( IS_SET(mob->act, ACT_MERCENARY ) ) ) 
	 break;
	 }
 
	 if ( mob == NULL )
	 {
			send_to_char( "There doesn't seem to be a mercenary around!\n\r", ch );
			return;
	 }

	 if ( mob->hunting != NULL || mob->hunt_obj != NULL )
	 {
	act( "$N looks to be busy already!", ch, NULL, mob, TO_CHAR );
	return;
	 }
	 
	 cost = ( mob->level * 10000 );

	 if ( argument[0] == '\0' )
	 {
			 sprintf( buf, "$N tells you, 'The cost is %d GP.'", cost );
			 act( buf, ch, NULL, mob, TO_CHAR );
			 return;
	 }

	 if ( (victim = get_char_world( ch, argument ) ) == NULL )
	 {
			act( "$N tells you, 'I can't locate the victim.  Sorry.'", ch, NULL, mob, TO_CHAR );
			 return;
	 }

	 if ( victim == ch )
		 return;


	 if ( IS_NPC(victim) )
	 {
			act( "$N tells you, 'I only go after REAL players, they're more fun!`",
			ch, NULL, mob, TO_CHAR );
			return;
		}

	 if ( victim->level < 12 )
	 {
			act( "$N tells you, 'I'm not interested in small fry.'", ch, NULL, mob, TO_CHAR );
			return;
	 }  


	 if ( ch->gold < cost )
	 {
			 act( "$N tells you, 'You can't afford my services!'", ch, NULL, mob, TO_CHAR );
			 return;
		}

	 
	 if ( !set_hunt(mob, ch, victim, NULL, HUNT_WORLD|HUNT_MERC,
					 HUNT_INFORM|HUNT_CR) )
	 {
		 act( "$N tells you, 'I can't seem to find the target!'", ch, NULL, mob,
		TO_CHAR );
		 return;
	 }
/*   if ( !make_hunt( mob, victim ) )
	 {
			act( "$N tells you, 'I can't seem to find the target!'", ch, NULL, mob, TO_CHAR );
			return;
	 }*/
	 

		ch->gold -= cost;
		act( "$n gives $N some gold coins.", ch, NULL, mob, TO_NOTVICT );
		act( "$n says '$N shall die by my hand!`", mob, NULL, victim, TO_ROOM );
/*
		sprintf( buf, "%s employs the services of %s to assassinate %s!!",
			 ch->name, mob->short_descr, victim->name );
		info( buf, 1 );
*/
	 return;
}


void do_alias( CHAR_DATA *ch, char *argument )
{
	/* Handle aliases - setting and clearing, as well as listing. */
	
	int cnt = 0;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	bool found = FALSE;
		buf[0] = '\0';

	
	if ( IS_NPC( ch ) )
	{
		 send_to_char( "Not a chance!\n\r", ch );
		 return;
	}
	smash_tilde( argument );
	if ( argument[0] == '\0' )
	{
		 send_to_char( "Defined Aliases:\n\r", ch );
		 
		 for ( cnt = 0; cnt < MAX_ALIASES; cnt++ )
		 {
		if (!str_cmp(ch->pcdata->alias_name[cnt], "<none>")
			&& !str_cmp(ch->pcdata->alias[cnt], "<none>") )
			continue;

		 sprintf( buf, "[Name:] %12s  [Alias:] %s\n\r",
		 ch->pcdata->alias_name[cnt],
		 ch->pcdata->alias[cnt] );
	send_to_char( buf, ch );
		 }
		 send_to_char( "\n\rTo Set an Alias:\n\r", ch );
		 send_to_char( "ALIAS <name> <alias>\n\r", ch );
		 send_to_char( "-enter 'CLEAR' as {r<alias>{x to clear an alias.\n\r", ch );
		 return;
	}
	/* Stupid ACK!MUD coders always want to complicate things...
	 * Just need a name and alias boys, number == retarded.
	 */
	 argument=one_argument( argument, arg1 );   /* Number */ /* Now name */
	 argument=one_argument( argument, arg2 );   /* name   */ /* now alias */

	 if (arg2[0] == '\0')
	 {
		 send_to_char( "\n\rTo Set an Alias:\n\r", ch );
		 send_to_char( "ALIAS <name> <alias>\n\r", ch );
		 send_to_char( "-enter 'CLEAR' as {r<alias>{x to clear an alias.\n\r", ch );
		 return;
	 }

	 if ( !str_cmp( "clear", arg2 ) )
	 {
	 for ( cnt = 0; cnt < MAX_ALIASES; cnt++ )
		if(!str_cmp(arg1, ch->pcdata->alias_name[cnt]) )
		{
			free_string( ch->pcdata->alias_name[cnt] );
			free_string( ch->pcdata->alias[cnt] );
	 
			ch->pcdata->alias_name[cnt] = str_dup( "<none>" );   
			ch->pcdata->alias[cnt]      = str_dup( "<none>" );
			send_to_char("Alias removed.\n\r", ch);
		found = TRUE;
			/* Clear the alias (enter <none> for name and alias */
			return;
		}
		if (!found)
		{
		send_to_char("There is no such alias.\n\r", ch);
		return;
		}
	 }
	 if (!str_cmp(arg1, "<none>") || !str_cmp(arg2, "<none>") )
	 {
		 send_to_char("Sorry, that word is restricted.\n\r", ch);
		 return;
	 }
	 /* Hopefully, now just set the (new) alias... */
	for ( cnt = 0; cnt < MAX_ALIASES; cnt++ )
	{
		if (!str_cmp(ch->pcdata->alias_name[cnt], arg1) )
		{
			send_to_char("You already have that word aliased.\n\r", ch);
			return;
		}

		if (str_cmp(ch->pcdata->alias_name[cnt], "<none>")
			&& str_cmp(ch->pcdata->alias[cnt], "<none>") )
			continue;

	 free_string( ch->pcdata->alias_name[cnt] );
	 free_string( ch->pcdata->alias[cnt] );
	 
	 ch->pcdata->alias_name[cnt] = str_dup( arg1 );   
	 ch->pcdata->alias[cnt]      = str_dup( arg2 );
	 send_to_char("Alias added.\n\r", ch);
	 return;
	}
	send_to_char("You have hit the limit of aliases allowed.\n\r", ch);
	return;
}
	

void do_color( CHAR_DATA *ch, char *argument )
{
			
	 if ( IS_NPC(ch) )
			return;

send_to_char("			 Remind me to fix this later. Will allow people to set color schemes for menues, etc.\n\r", ch);
			 return;
}

/* A simple, return the char sequence, function -S- */

char * color_string( CHAR_DATA *ch, char *argument )
{
	 int cnt;
	 int num;
	 
	 
	 /* if we don't want to send the string, return "" */
	 /* argument should be the string to find, eg "say" */
	 
	 if ( IS_NPC( ch ) || argument[0] == '\0' )
			return( "" );
	 
	 if ( !IS_SET( ch->config, CONFIG_COLOR ) )
			return( "" );
			
	 if ( !str_cmp( argument, "normal" ) )
			return( "\x1b[0m" );
	 
	 /* By here, ch is a PC and wants color */
	 
	 num = -1;
	 for ( cnt = 0; cnt < MAX_color; cnt ++ )
		 if ( !str_cmp( argument, color_table[cnt].name ) )
	num = cnt;
	
	 if ( num == -1 ) /* bug report? */
	 
			return( "" );
	 
	 return( ansi_table[ch->pcdata->color[num]].value );

} 

void do_worth( CHAR_DATA *ch, char *argument )
{        	
		send_to_char( "Huh?\n\r", ch );
		return;
}
 
void do_whois( CHAR_DATA *ch, char *argument )
{
	 /* Show ch some details about the 'victim'
		* Make sure ch can see victim!
		* -S-
		*/

	 CHAR_DATA *victim;
	 char buf[MAX_STRING_LENGTH];
	 char buf2[MSL];
	 char buf3[MSL];
	 char sender[MSL];
	 int s, r, h, m = 0;

	 if ( ( victim = get_char_connected( ch, argument ) ) == NULL )
	 {
			send_to_char( "No such player found.\n\r", ch );
			return;
	 }


	 /* Ok, so now show the details! */
	 buf[0] = '\0';
	 sender[0] = '\0';

	 sprintf(sender, "{r+{d=============={r+{x");
	 if (victim->pcdata->clan != 0 && str_cmp(get_clan_info(victim->pcdata->clan, CNAME), "None") )
		 strcat(sender, "{d==================================={r+{x\n\r");
	 else
		 strcat(sender, "\n\r");
	 sprintf(buf, "{d| {m%s{x {d|{x", center_text(capitalize(victim->name), 12) );
	 strcat(sender, buf);
		if (victim->pcdata->clan != 0 && str_cmp(get_clan_info(victim->pcdata->clan, CNAME), "None") )
		{
		sprintf(buf, " {mClan:{x %-19s {RClan %-2d{x {d|{x\n\r",
			center_text( get_clan_info(victim->pcdata->clan, CNAME), 19 ),
			victim->pcdata->clan );
		strcat(sender, buf);
		} else
		strcat(sender, "\n\r");

	 strcat(sender, "{r+{d=============={r+{d==================================={r+{x\n\r");
	 strcat(sender, "{d|{x                                                  {d|{x\n\r");
	 sprintf(buf,   "{d|{x %s {d|{x\n\r", center_text( get_title(victim), 48) );
	 strcat(sender, buf);
	 if(IS_IMMORTAL(ch) )
	 {
	 sprintf(buf2, "%d", victim->level);
	 sprintf(buf, "{d|{x %s {d|{x\n\r", center_text(buf2, 48));
	 strcat(sender, buf);
	 }
	 strcat(sender, "{d|{x                                                  {d|{x\n\r");
	 sprintf(buf,   "{d|{x    {mClass:{x %-15s {mPlayers killed:{x %-6d {d|{x\n\r",
		 "Fix", victim->pcdata->pkills);
	 strcat(sender, buf);
	 sprintf(buf,   "{d|{x     {mRace:{x %-15s {mMobiles killed:{x %-6d {d|{x\n\r",
		 race_table[victim->race].race_title, victim->pcdata->mkills);
	 strcat(sender, buf);
	 sprintf(buf,   "{d|{x      {mSex:{x %-12s {mKilled by players:{x %-6d {d|{x\n\r",
		 (victim->sex == 1 ? "Male" : "Female"), victim->pcdata->pkilled );
	 strcat(sender, buf);
	 sprintf(buf,   "{d|{x    {mHours:{x %-12d {mKilled by mobiles:{x %-6d {d|{x\n\r",
		 my_get_hours(victim),
		 victim->pcdata->mkilled);
	 strcat(sender, buf);
	 strcat(sender, "{d|{x                                                  {d|{x\n\r");
		s = current_time - victim->pcdata->laston;
		r = s / 86400;
		s -= r * 86400;
		h = s / 3600;
		s -= h * 3600;
		m = s / 60;
		s -= m * 60;

		 if (ch->hometown == victim->hometown || IS_IMMORTAL(ch) )
	 {
		 sprintf(buf3, "Last played %d days, %d hours and %d minutes ago", r, h, m );
		 sprintf(buf2, "%s %s", (can_see(ch, victim) ? capitalize(victim->name) : ""), ( can_see(ch, victim) ?
		 (victim->desc == NULL ? "is currentlly {Rlink dead{x." : "is {gplaying{x.") :
		 buf3) );
	 sprintf(buf,   "{d|{x %s {d|{x\n\r", center_text(buf2, 48) );
	 strcat(sender, buf);
	 }

	 if(IS_IMMORTAL(ch) )
	 {
		strcat(sender, "{d|{x                                                  {d|{x\n\r");
			sprintf(buf,   "{d|{x %s {d|{x\n\r", center_text(victim->pcdata->host, 48) );
			strcat(sender, buf);
		strcat(sender, "{d|{x                                                  {d|{x\n\r");
	 } else
	 strcat(sender, "{d|{x                                                  {d|{x\n\r");
	 strcat(sender, "{r+{d=================================================={r+{x\n\r");
	 if (!str_cmp(victim->description, "") )
	 {
	 send_to_char(sender, ch);
	 return;
	 }
	 strcat(sender, "  {mDescription:{x\n\r");
	 sprintf(buf, "    %s", victim->description);
	 strcat(sender, buf);
	 strcat(sender, "{r+{d=================================================={r+{x\n\r");
	 send_to_char(sender, ch);
	 return;

}

void do_shelp( CHAR_DATA *ch, char * argument )
{
	 /* Like help, except for spells and skills. */
	 int sn;
	 char buf[MAX_STRING_LENGTH];
	 HELP_DATA *pHelp;
	 bool found = FALSE;
		buf[0] = '\0';

	 if ( argument[0] == '\0' )
	 {
			do_help( ch, "shelp_summary" );   
			return;
	 }
			
	 if ( ( sn = skill_lookup( argument ) ) < 0 )
	 {
			sprintf( buf, "No sHelp found for argument:%s\n\r", argument );
			send_to_char( buf, ch );
			return;
	 }
	 sprintf( buf, "shelp_%s", skill_table[sn].name );
	 
	 /* Search help texts for 'shelp_<name>' as keyword.... */
	 for ( pHelp = first_help; pHelp != NULL; pHelp = pHelp->next )
			if ( !str_cmp( buf, pHelp->keyword ) )
			{
	 found = TRUE;
	 send_to_char( pHelp->text, ch );
	 break;
			}
			
	 if ( !found )
			send_to_char( "Couldn't find a sHelp for that skill/spell.\n\r", ch );   
			
	 return;
}     

void do_afk( CHAR_DATA *ch, char *argument )
{
		int value;
		
		if ( IS_NPC( ch ) )
			 return;
		
		value = table_lookup( tab_player_flags, "AFK" );
		
		if ( IS_SET( ch->pcdata->pflags, value ) )
		{
			 REMOVE_BIT( ch->pcdata->pflags, value );
			 send_to_char( "AFK flag turned off.\n\r", ch );
		}
		else
		{
			 SET_BIT( ch->pcdata->pflags, value );
			 send_to_char( "AFK flag turned on.\n\r", ch );
		}
		return;
}  


void do_colist( CHAR_DATA *ch, char *argument )
{
	 int col, n;
	 char buf[MAX_STRING_LENGTH];
	 
	 if IS_NPC( ch )
			return;
	 
	 send_to_char( "{Wcolor Codes: {YTo use color codes within a string, use the following\n\r", ch );
	 send_to_char( "characters in sequence: { <letter>.  Do not leave any spaces when you use\n\r", ch );
	 send_to_char( "the codes.  (Spaces are left above so that the characters are not interpreted\n\r", ch );
	 send_to_char( "as colors when you see this text!){w\n\r\n\r", ch );
	 
	 n = 0;
	 
	 for ( col = 0; col < MAX_ANSI; col++ )
	 {
			sprintf( buf, "%c - %s%-14s{x    ", 
	 ansi_table[col].letter, 
	 IS_SET( ch->config, CONFIG_COLOR ) ? ansi_table[col].value : "", 
	 ansi_table[col].name );
			send_to_char( buf, ch );
			if ( ++n % 3 == 0 )
	 send_to_char( "\n\r", ch );
	 }
	 if ( n % 3 != 0 )
			send_to_char( "\n\r", ch );
	 
	 send_to_char( "\n\r", ch );
	 return;
}
	 


void do_loot ( CHAR_DATA *ch, char *argument )
{

		char arg[MAX_INPUT_LENGTH];
		OBJ_DATA *corpse;
		OBJ_DATA *obj = NULL;
		int counter, num;

		one_argument( argument, arg );

		if ( IS_NPC( ch ) )
		{
				send_to_char( "NPC's cannot loot corpses.\n\r", ch );
				return;
		}

		if ( arg[0] == '\0' )
		{
				send_to_char( "Loot what?\n\r", ch );
				return;
		}

		if ( !str_cmp( arg, "all" ) || !str_prefix( "all." , arg ) )
		{
				send_to_char( "You can't do that.\n\r", ch );
				return;
		}

		corpse = get_obj_room( ch, arg, ch->in_room->first_content );
		if ( corpse == NULL )
		{
				act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
				return;
		}

		if ( corpse->item_type == ITEM_CORPSE_NPC )
		{
				send_to_char( "Just go ahead and take it.\n\r", ch );
				return;
		}

		if ( corpse->item_type != ITEM_CORPSE_PC )
		{
				send_to_char( "You cannot loot that.\n\r", ch );
				return;
		} 
		/* begin checking for lootability */
	 
		if ( ( ch->pcdata->clan == 0 )
			&& ( !IS_SET(ch->pcdata->pflags, PFLAG_PKOK)  )
			&& ( !IS_VAMP( ch ) && !IS_WOLF( ch )  )  )
		{
				send_to_char( "You cannot loot corpses.\n\r", ch );
				return;
		}

		if ( corpse->value[3] == 0 )
		{
				send_to_char( "You cannot loot this corpse.\n\r", ch );
				return;
		}

		if (  ( ch->pcdata->clan == corpse->value[2] )
			 || (  ( IS_SET( ch->pcdata->pflags, PFLAG_PKOK )  )
					&& ( corpse->value[0] == 1 )  )  
			 || (  ( IS_WOLF( ch ) || IS_VAMP( ch )  )  
					&& ( corpse->value[0] == 1 )   )   )
		{
				counter = number_range( 1, 100 );

				if ( counter >= 40 )
				{
						if ( corpse->first_in_carry_list == NULL )
						{
								send_to_char( "There isn't anything in the corpse.\n\r", ch);
								return;
						}

						num = 0;
						for (obj = corpse->first_in_carry_list;
								 obj != NULL;
								 obj = obj->next_in_carry_list) {
							++num;
						}

						counter = number_range( 1, num );
 
						obj = corpse->first_in_carry_list;
						for (num = 1; num < counter; ++num) {
							obj = obj->next_in_carry_list;
						}

						if ( !IS_SET( obj->extra_flags, ITEM_NOLOOT ) )
			{
					get_obj( ch, obj, corpse );
			
								/* just incase... */
							 if ( ch->level > 1 )
								{
									 do_save( ch, "" );
								}
						}
						else 
			{
		send_to_char( "You failed to loot the corpse.\n\r", ch );
		return;
			}

						corpse->value[3] = corpse->value[3] - 1; 
						return;
				}
				else
				{
						send_to_char( "You failed to loot the corpse.\n\r", ch );
						return;
				}
		 }
		 
		 send_to_char( "You cannot loot this corpse.\n\r", ch );
	return;
}

void do_search( CHAR_DATA *ch, char *argument )
 {
	 OBJ_DATA *obj;

	 if(IS_NPC(ch)) return;

	 if(ch->pcdata->learned[gsn_search] < 1) {
	 send_to_char("You search around clumsily.\n\r",ch);
	 return;
	 }

	 if(number_percent() < ch->pcdata->learned[gsn_search] )
	 {
		send_to_char("You search the room..\n\r",ch); 

		for(obj = ch->in_room->first_content; obj != NULL; obj = obj->next_content)
		{
		if(IS_SET(obj->extra_flags,ITEM_HIDDEN) )
		{
		 REMOVE_BIT(obj->extra_flags,ITEM_HIDDEN);
		 act("You find $p.",ch,obj,NULL,TO_CHAR);
		 get_obj( ch, obj, NULL );     
			 }
			}
/*
			 send_to_char("You have searched everywhere.\n\r",ch);
			 WAIT_STATE(ch,24);
*/  
		}
			else
			send_to_char("You didn't uncover anything unusual.\n\r",ch);
		WAIT_STATE(ch,24);
	 return;
 }
 void do_largemap( CHAR_DATA *ch )
 {
send_to_char( "\n\r", ch );
do_mapping( ch, "10" );
send_to_char( "\n\r", ch );
 }
 
void do_hsight( CHAR_DATA *ch, char *argument )
{
	char buf[MSL];
	if (!is_number(argument) )
		return;
	sprintf(buf, "%d", atoi(argument) );
send_to_char( "\n\r", ch );
do_mapping( ch, buf );
send_to_char( "\n\r", ch );
}


void do_remort ( CHAR_DATA *ch, char *argument )
{
			 return;
}


bool can_use_skill( CHAR_DATA *ch, int skill)
{
		 return TRUE;
}

char *get_clan_info(int num, int which)
{
	bool found = FALSE;
	CLAN_DATA *clan;

	for (clan = first_clan; clan != NULL; clan = clan->next)
	{
		if(num == clan->clan_numb)
		{
			found = TRUE;
			break;
		}
	}
	switch ( which )
	{
	case CNAME :
		if (!found)
		return "None";
		return clan->clan_name;
	case CABBR :
		if (!found)
		return "None";
		return clan->clan_abbr;
	case CLEAD :
		if (!found)
		return "None";
		return clan->leader;
	case CENEM :
		if (!found)
		return "None";
		return clan->enemies;
	default :
	return "{RERROR IN GET_CLAN_INFO{x";
	}

	
}

int get_clan_numin(int num, int which)
{
	bool found = FALSE;
	CLAN_DATA *clan;

	for (clan = first_clan; clan != NULL; clan = clan->next)
	{
		if(num == clan->clan_numb)
		{
			found = TRUE;
			break;
		}
	}
	switch ( which )
	{
	case CDONA :
		if (!found)
		return 0;
				return clan->donat_room;
	case CROOM :
		if (!found)
		return 0;
		return clan->clan_room;
	default :
	return 0;
	}
	return 0;
}


int get_clan_eq(int num, int which)
{
	bool found = FALSE;
	CLAN_DATA *clan;

	for (clan = first_clan; clan != NULL; clan = clan->next)
	{
		if(num == clan->clan_numb)
		{
			found = TRUE;
			break;
		}
	}
	switch ( which )
	{
	case 0  :
		if (!found)
		return -1;
		return clan->eq[0];
	case 1  :
		if (!found)
		return -1;
		return clan->eq[1];
	case 2  :
		if (!found)
		return -1;
		return clan->eq[2];
	case 3  :
		if (!found)
		return -1;
		return clan->eq[3];
	case 4  :
		if (!found)
		return -1;
		return clan->eq[4];
	case 5  :
		if (!found)
		return -1;
		return clan->eq[5];
	default :
	return -1;
	}
	return -1;
}

char *get_title( CHAR_DATA *ch )
{
	if (ch->level == 110)
		return "{RAdministrator{x";
	else if (ch->level > 101)
		return "{gImmortal{x";
	else if (ch->level > 56)
		return "{gLegendary{x";
	else if (ch->level > 54)
		return "{gLord{x";
	else if (ch->level > 51)
		return "{gDuke{x";
	else if (ch->level > 49)
		return "{gBaron{x";
	else if (is_remort(ch))
		return "{mRemort{x";
	else if (!is_remort(ch))
		return "{yMortal{x";
	else
		return "{RBUG!!{x";

	return "{RBIG BUG!!{x";
}

char *get_title_nocol( CHAR_DATA *ch )
{
	if (ch->level == 110)
		return "Administrator";
	else if (ch->level > 101)
		return "Immortal";
	else if (ch->level > 56)
		return "Legendary";
	else if (ch->level > 54)
		return "Lord";
	else if (ch->level > 51)
		return "Duke";
	else if (ch->level > 49)
		return "Baron";
	else if (is_remort(ch))
		return race_table[ch->race].racedesc[ch->hometown];
	else if (!is_remort(ch))
		return race_table[ch->race].racedesc[ch->hometown];
	else
		return "{RBUG!!{x";

	return "{RBIG BUG!!{x";
}
void do_finger( CHAR_DATA *ch, char * argument )
{
	 CHAR_DATA *victim;
	 char name[MAX_STRING_LENGTH];
	 char buf[MAX_STRING_LENGTH];
	 char buf2[MSL];
	 char buf3[MSL];
	 char buf4[MSL];
	 char sender[MSL];
	 int s, r, h, m = 0;
	 bool found = FALSE;
	 DESCRIPTOR_DATA d;
	 DESCRIPTOR_DATA * this_d = NULL;

	 argument = one_argument( argument, name );
	 for (this_d = first_desc; this_d != NULL; this_d = this_d->next )
	 {
		 if ( ( this_d->connected == CON_PLAYING ) && !str_cmp( this_d->character->name, name ) )
		 {
			 do_whois( ch, name );
			 return;
		 }
	 }
 
	 if ( name[0] == '\0' )
		{
		 send_to_char( "Finger <player name>\n\r", ch );
		 return;
		}

	 found = load_char_obj( &d, name, TRUE );
	 
	 if (!found)
	 {
			sprintf( buf, "Player not found: %s\n\r", name );
			send_to_char( buf, ch );
			return;
	 }
	 
	 victim = d.character;
	 d.character = NULL;
	 victim->desc = NULL;
	 buf[0] = '\0';
	 sender[0] = '\0';

	 sprintf(sender, "{r+{d=============={r+{x");
	 if (victim->pcdata->clan != 0 && str_cmp(get_clan_info(victim->pcdata->clan, CNAME), "None") )
		 strcat(sender, "{d==================================={r+{x\n\r");
	 else
		 strcat(sender, "\n\r");
	 sprintf(buf, "{d| {m%s{x {d|{x", center_text(capitalize(victim->name), 12) );
	 strcat(sender, buf);
		if (victim->pcdata->clan != 0 && str_cmp(get_clan_info(victim->pcdata->clan, CNAME), "None") )
		{
		sprintf(buf, " {mClan:{x %-19s {RClan %-2d{x {d|{x\n\r",
			center_text( get_clan_info(victim->pcdata->clan, CNAME), 19 ),
			victim->pcdata->clan );
		strcat(sender, buf);
		} else
		strcat(sender, "\n\r");

	 strcat(sender, "{r+{d=============={r+{d==================================={r+{x\n\r");
	 strcat(sender, "{d|{x                                                  {d|{x\n\r");
	 sprintf(buf,   "{d|{x %s {d|{x\n\r", center_text( get_title(victim), 48) );
	 strcat(sender, buf);
	 if(IS_IMMORTAL(ch) )
	 {
	 sprintf(buf2, "%d", victim->level);
	 sprintf(buf, "{d|{x %s {d|{x\n\r", center_text(buf2, 48));
	 strcat(sender, buf);
	 }
	 strcat(sender, "{d|{x                                                  {d|{x\n\r");
	 sprintf(buf,   "{d|{x    {mClass:{x %-15s {mPlayers killed:{x %-6d {d|{x\n\r",
		 "Fix", victim->pcdata->pkills);
	 strcat(sender, buf);
	 sprintf(buf,   "{d|{x     {mRace:{x %-15s {mMobiles killed:{x %-6d {d|{x\n\r",
		 race_table[victim->race].race_title, victim->pcdata->mkills);
	 strcat(sender, buf);
	 sprintf(buf,   "{d|{x      {mSex:{x %-12s {mKilled by players:{x %-6d {d|{x\n\r",
		 (victim->sex == 1 ? "Male" : "Female"), victim->pcdata->pkilled );
	 strcat(sender, buf);
	 sprintf(buf,   "{d|{x    {mHours:{x %-12d {mKilled by mobiles:{x %-6d {d|{x\n\r",
		 my_get_hours(victim),
		 victim->pcdata->mkilled);
	 strcat(sender, buf);
	 strcat(sender, "{d|{x                                                  {d|{x\n\r");
		s = current_time - victim->pcdata->laston;
		r = s / 86400;
		s -= r * 86400;
		h = s / 3600;
		s -= h * 3600;
		m = s / 60;
		s -= m * 60;

		 if (ch->hometown == victim->hometown || IS_IMMORTAL(ch) )
	 {
	 sprintf(buf3, "Last played %d days, %d hours and %d minutes ago", r, h, m);
	 sprintf(buf4, "%s is currentlly {Rlink dead{x.", victim->name );
		 sprintf(buf,   "{d|{x %s {d|{x\n\r",  center_text(buf3, 48) );
	 strcat(sender, buf);
	 }

	 if(IS_IMMORTAL(ch) )
	 {
		strcat(sender, "{d|{x                                                  {d|{x\n\r");
			sprintf(buf,   "{d|{x %s {d|{x\n\r", center_text(victim->pcdata->host, 48) );
			strcat(sender, buf);
		strcat(sender, "{d|{x                                                  {d|{x\n\r");
	 } else
	 strcat(sender, "{d|{x                                                  {d|{x\n\r");
	 strcat(sender, "{r+{d=================================================={r+{x\n\r");

	 if (!str_cmp(victim->description, "") )
	 {
	 send_to_char(sender, ch);
	 free_char(victim);
	 return;
	 }
	 strcat(sender, "  {mDescription:{x\n\r");
	 sprintf(buf, "    %s", victim->description);
	 strcat(sender, buf);
	 strcat(sender, "{r+{d=================================================={r+{x\n\r");
	 send_to_char(sender, ch);
	 free_char(victim);
	 return;

}
