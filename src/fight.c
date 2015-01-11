/****************************************************************************
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
#include <stdlib.h>     /* For div_t, div() */
#include <string.h>
#include <time.h>
#include "ack.h"
#include "tables.h"
#include <math.h>
#include "magic.h"
#ifndef DEC_MONEY_H
#include "money.h"
#endif

bool    deathmatch;

extern POL_DATA	politics_data;
extern CHAR_DATA * quest_target;
extern CHAR_DATA * quest_mob;



/*
 * Local functions.
 */
bool    check_dodge     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    check_killer    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_parry     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_skills	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    dam_message     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt, int sn ) );
void    death_message   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt,
                             int max_dt ) );
void    death_cry       args( ( CHAR_DATA *ch ) );
void    group_gain      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

bool    is_safe         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    make_corpse     args( ( CHAR_DATA *ch, char *argument ) );
void    one_hit         args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool dual ) );
void    raw_kill        args( ( CHAR_DATA *victim, char *argument ) );
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool check ) );
void    disarm          args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA * obj ) );
void    trip            args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	  check_adrenaline args( ( CHAR_DATA *ch, sh_int damage ) );
void obj_damage		args( ( OBJ_DATA * obj, CHAR_DATA * victim, int dam ) );
void check_improve  args( (CHAR_DATA * ch, int sn, bool success, int multiplier) );
/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;   
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    CHAR_DATA *rch_next;
 /*   CHAR_DATA *check_char;   */
/*    extern CHAR_DATA *violence_marker;
    CHAR_DATA	*marker;  */
    bool	has_cast = FALSE;



 CREF( ch_next, CHAR_NEXT );
 for ( ch = first_char; ch; ch = ch_next )
 {

   ch_next = ch->next;

     
	/* For stunning during combat
		-Damane-    4/26/96 */

   if ( ch->position == POS_STUNNED ) 
   {
     if ( ch->wait > 0 )
     {
       ch->wait-=1;
       continue;
     }
     else 
     {
//     ch->position = POS_STANDING;
         if (IS_NPC(ch))
             do_stand(ch, "");
       send_to_char( "You are no longer stunned.\n\r", ch );
       continue;
     }
   }
   has_cast = FALSE;

/* Healing rapidly for raged wolves  */

   if ( !IS_NPC( ch ) && IS_WOLF( ch ) && IS_RAGED( ch ) )
   {
     if ( !is_affected( ch, skill_lookup("Enraged" ) ) )
       REMOVE_BIT( ch->pcdata->pflags, PFLAG_RAGED );
     ch->hit = ( UMIN( ch->max_hit, ( ch->hit + ch->max_hit /150 )));
      
   }


/*
 *  for when wolves can't cast normal spells, 
 *  increase regen rate mucho to compensate.  
 *  let's make it a skill instead..hehehe
 *
   if ( ch->fighting != NULL )
     ch->hit = ( UMIN( ch->max_hit, ( ch->hit + ( number_range( (20 * ch->max_hit /100) , 

 */

/* Heated armor damage :) ZEN  */

   if ( ch->hit > 0 
      && ch->in_room != NULL 
      && get_room_index( ch->in_room->vnum ) != NULL 
      && item_has_apply( ch, ITEM_APPLY_HEATED ) )
   {
     OBJ_DATA * heated_item;
     int heat_damage = 0;
    
     for ( heated_item = ch->first_carry; heated_item != NULL; heated_item = heated_item->next_in_carry_list )
     {
       if ( IS_SET( heated_item->item_apply, ITEM_APPLY_HEATED ) && heated_item->wear_loc != WEAR_NONE  )
       {
         heat_damage = heated_item->level;
         if ( IS_SET( heated_item->extra_flags, ITEM_REMORT)  )
           heat_damage *= 2;
         obj_damage( heated_item, ch, heat_damage );
         act("{W   $p{x you are wearing are {RBURNING{x you!!!", ch, heated_item, NULL, TO_CHAR );
         act("{W   $p worn by $n is {RBURNING{x!!!", ch, heated_item, NULL, TO_ROOM);
         if ( IS_NPC( ch )  )
           do_remove( ch, heated_item->name );
       }
     }
   }
    
   if ( ch->stunTimer > 0 )
   {
     ch->stunTimer--;
     continue;
   }
      /*  heal check for  solos ( mob )..   Zen  */


   if (  ( ch->is_free == FALSE )
      && ( IS_NPC( ch ) ) 
      && IS_SET( ch->act, ACT_SOLO ) 
      && ch->hit > 0 )
   {
     if ( ( ch->hit < ch->max_hit *3 / 4 ) && ( ch->mana > mana_cost( ch, skill_lookup( "heal" ) ) ) )
     {  
       do_cast( ch,  "heal self"  );
       has_cast = TRUE; 
     }
   }
   else if ( IS_NPC( ch ) && ch->hit < 0 )
   { 
     ch->position = POS_DEAD;
     if ( ch->fighting == NULL )
       if ( ch->in_room != NULL )
          act( "Suddenly, $n is enveloped in a {mBeam of light{x, and is gone!", ch, NULL, NULL, TO_ROOM );
     stop_fighting( ch, TRUE );
     extract_char( ch, TRUE );
     continue;
   }
      
   area_resetting_global = TRUE;
   if (  ( ch->is_free == FALSE )
      && ( IS_NPC( ch ) ) 
      && ( !IS_SET( ch->def, DEF_NONE ) )  
      && ch->hit > 0 )
   {
     if ( ch->hit < ch->max_hit * 2 / 3 )
     {
       if ( IS_SET( ch->def, DEF_CURE_LIGHT ) )
       {
         if ( ch->mana > mana_cost( ch, skill_lookup( "cure light" ) ) )
         {
           do_cast( ch, "\'cure light\' self" );
           has_cast = TRUE;
         }
       }
       else
       if ( IS_SET( ch->def, DEF_CURE_SERIOUS ) )
       {
         if ( ch->mana > mana_cost( ch, skill_lookup( "cure serious" ) ) )
         {
           do_cast( ch, "\'cure serious\' self" );
           has_cast = TRUE;
         }
       }
       else
       if ( IS_SET( ch->def, DEF_CURE_CRITIC ) )
       {
         if ( ch->mana > mana_cost( ch, skill_lookup( "cure critical" ) ) )
         {
           do_cast( ch, "\'cure critical\' self" );
           has_cast = TRUE;
         }
       }
       else
       if ( IS_SET( ch->def, DEF_CURE_HEAL ) )
       {
         if ( ch->mana > mana_cost( ch, skill_lookup( "heal" ) ) )
         {
           do_cast( ch, "heal self" );
           has_cast = TRUE;
         }
       }
     }
   }
   else
   if ( IS_NPC( ch ) && ch->hit < 0 )
   {
     ch->position = POS_DEAD;
     if ( ch->fighting == NULL )
     if ( ch->in_room != NULL )
       act( "Suddenly, $n is enveloped in a {mBeam of light{x, and is gone!", ch, NULL, NULL, TO_ROOM );
     extract_char( ch, TRUE ); 
     continue;
   }     

   if (  ( ch->is_free == FALSE )
      && ( IS_NPC( ch ) ) 
      && ( !IS_SET( ch->def, DEF_NONE ) )  
      && ( ch->hit > 0 )
      && ( ch->first_shield == NULL )  
      && ( !has_cast )  
      && ( ch->fighting == NULL )  )
   {
      if (  ( IS_SET( ch->def, DEF_SHIELD_FIRE ) )
         && ( !is_affected( ch, skill_lookup( "fireshield" ) )   )
         && ( ch->mana > mana_cost( ch, skill_lookup( "fireshield" ) ) )   )
     {
       do_cast( ch, "fireshield" );
       has_cast = TRUE;
     }
     else
     if (  ( IS_SET( ch->def, DEF_SHIELD_ICE ) )
        && ( !is_affected( ch, skill_lookup( "iceshield" ) )   )
        && ( ch->mana > mana_cost( ch, skill_lookup( "iceshield" ) ) )   )
     {
       do_cast( ch, "iceshield" );
       has_cast = TRUE;
     }
     else
     if (  ( IS_SET( ch->def, DEF_SHIELD_SHOCK ) )
        && ( !is_affected( ch, skill_lookup( "shockshield" ) )   )
        && ( ch->mana > mana_cost( ch, skill_lookup( "shockshield" ) ) )   )
     {
       do_cast( ch, "shockshield" );
       has_cast = TRUE;
     }
   }
   area_resetting_global = FALSE;
/* Offensive spell handler, only use when actually fighting.. */

   if (  ( IS_NPC( ch ) )
      && ( ch->is_free == FALSE )
      && ( ch->cast > 1 )
      && ( !has_cast )
      && ( ch->position > POS_RESTING )  
      && ( ch->fighting != NULL )  
      && ( ch->fighting->is_free != TRUE ) 
      && ( ch->in_room != NULL )
      && ( ch->hit > 1 )
      && ( ch->position == POS_FIGHTING )  )

   {
     sh_int	cast_frequency;
     sh_int	index;

     cast_frequency = get_psuedo_level( ch ) / 2;  /* maybe set in olc later? */
     if (  ( number_range( 0, 99 ) < cast_frequency )
        && ( ch->mana >= ( 40 * ch->max_mana / 100 ) )   )
     {
       for ( index = 1; index < 32; index++ )
       {
         if (  ( IS_SET( ch->cast, ( 1 << index )  ) )
            && ( number_range( 0, 99 ) < ( index * 3 + number_range( 0, 25 ) )  )
            && ( ch->mana > mana_cost( ch, skill_lookup( rev_table_lookup( tab_cast_name, ( 1 << index ) ) ) )  )   )
         {
           char   cast_name[MSL];
           sprintf( cast_name, "%s %s", rev_table_lookup( tab_cast_name, ( 1 << index ) ), ch->fighting->name );
           do_cast( ch, cast_name );
           has_cast = TRUE;
           break;
         }
       }
     }
     
   }
        /*
         * Hunting mobs.
         * -S- Mod: use flags to work out what to do.... 
         */

   if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
   {
     continue;
   }
    

   if (  ( IS_AWAKE( ch ) )
      && ( ch->is_free == FALSE )
      && ( victim->is_free == FALSE ) 
      && ( ch->in_room != NULL )
      && ( victim->in_room != NULL )  
      && ( ch->in_room == victim->in_room ) )
    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

	mprog_hitprcnt_trigger( ch, victim );
	mprog_fight_trigger( ch, victim );

	/*
	 * Fun for the whole family!   RCH is a non-fighting mob
	 */
  if ( IS_NPC( victim ) && ( get_psuedo_level( victim ) > 15 )  )
  {
    CREF( rch_next, CHAR_NEXTROOM );
	  for ( rch = ch->in_room->first_person; rch != NULL; rch = rch_next )
	  {
	    rch_next = rch->next_in_room;
          if ( !IS_NPC( rch ) )
            continue; 

	    if ( IS_AWAKE(rch) && rch->fighting == NULL )
	    {
    /*
		 * NPC's assist NPC's of same type or 45% chance regardless.
		 */
        if ( !IS_AFFECTED(rch, AFF_CHARM) )
	      {
		      if (  ( rch->pIndexData == victim->pIndexData ) /* is it the same as a target here?  */
		         || (  ( number_percent() < 20 )  
                && ( abs( get_psuedo_level( rch ) - get_psuedo_level( victim ) ) < 35 )   )   )
          {
            CHAR_DATA *vch;
            CHAR_DATA *target;
            int number;

            target = NULL;
            number = 0;

                  /* vch is the target of the lazy mob...a player */
            for ( vch = ch->in_room->first_person; vch; vch = vch->next )
            {
              if (  ( can_see( rch, vch ) )
                 && (  !IS_NPC( vch )  )     )
              {
                target = vch;
                number++;
              }
            }

            if ( target != NULL )
            {
              if ( abs(  target->level - rch->level ) < 40 )
                multi_hit( rch, target, TYPE_UNDEFINED );
            }
          }
	      }
	    }
	  }
    CUREF( rch_next );
  }
 }
 CUREF( ch_next );
 return;
}


    
/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int     chance;
    OBJ_DATA * 	wield1 = NULL;
    OBJ_DATA *	wield2 = NULL;
    int		dual_chance = 0;
    bool multi_hit = FALSE;


    if ( ch->position == POS_RIDING )
    {
      if ( ch->riding && ( ch->riding->in_room == ch->in_room ) )
      {
        do_dismount( ch, "" );
      }
      else
      {
        ch->position = POS_FIGHTING;
      }
    }
    one_hit( ch, victim, dt, FALSE );
    if ( ch->fighting != victim )
	return;
    if (  (  ( ( wield1 = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  != NULL )
          && ( wield1->item_type == ITEM_WEAPON ) )
       && (  ( ( wield2 = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) != NULL )
          && ( wield2->item_type == ITEM_WEAPON ) )  )
      one_hit( ch, victim, dt, TRUE );
    if ( ch->fighting != victim )
	return;
    if (  (  ( ( wield1 = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  != NULL )
          && ( wield1->item_type == ITEM_WEAPON ) )
       && (  ( ( wield2 = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) != NULL )
          && ( wield2->item_type == ITEM_WEAPON ) )  )
      dual_chance = 15;


    chance = IS_NPC(ch) 
       ? ( IS_SET( ch->skills, MOB_SECOND ) ? 80 : 0 )
       : ( ( get_skill(ch, gsn_second_attack)/2 ) - ( dex_app[get_curr_dex( ch )].defensive /3 ) 
         + ( dual_chance ) );
    multi_hit = ( IS_NPC( ch ) ? FALSE : ( get_skill(ch, gsn_second_attack) > 0 ) );
  	
    if (  ( number_percent( ) < chance )
       && ( IS_NPC( ch ) || multi_hit ) )
    {
	one_hit( ch, victim, dt, FALSE );
	if ( ch->fighting != victim )
	    return;
    if (  (  ( ( wield1 = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  != NULL )
          && ( wield1->item_type == ITEM_WEAPON ) )
       && (  ( ( wield2 = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) != NULL )
          && ( wield2->item_type == ITEM_WEAPON ) )  )
      one_hit( ch, victim, dt, TRUE );
    if ( ch->fighting != victim )
	return;

    }

    chance = IS_NPC(ch) 
       ? ( IS_SET( ch->skills, MOB_THIRD ) ? 70 : 0 )
       : ( ( get_skill(ch, gsn_third_attack)/4 ) - ( dex_app[get_curr_dex( ch )].defensive /3 )
         + ( dual_chance ) );
    multi_hit = ( IS_NPC( ch ) ? FALSE : ( get_skill(ch, gsn_third_attack) > 0 ) );
  	
    if (  ( number_percent( ) < chance )
       && ( IS_NPC( ch ) || multi_hit ) )
    {
	one_hit( ch, victim, dt, FALSE );
	if ( ch->fighting != victim )
	    return;
    if (  (  ( ( wield1 = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  != NULL )
          && ( wield1->item_type == ITEM_WEAPON ) )
       && (  ( ( wield2 = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) != NULL )
          && ( wield2->item_type == ITEM_WEAPON ) )  )
      one_hit( ch, victim, dt, TRUE );
    if ( ch->fighting != victim )
	return;

    }


    chance = IS_NPC(ch) 
       ? ( IS_SET( ch->skills, MOB_FOURTH ) ? 60 : 0 )
       : (  ( get_skill(ch, gsn_fourth_attack)/7 ) - ( dex_app[get_curr_dex( ch )].defensive /3 )
         + ( dual_chance ) );
    multi_hit = ( IS_NPC( ch ) ? FALSE : ( get_skill(ch, gsn_fourth_attack) > 0 ) );
  	
    if (  ( number_percent( ) < chance )
       && ( IS_NPC( ch ) || multi_hit ) )
    {
	one_hit( ch, victim, dt, FALSE );
	if ( ch->fighting != victim )
	    return;
    if (  (  ( ( wield1 = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  != NULL )
          && ( wield1->item_type == ITEM_WEAPON ) )
       && (  ( ( wield2 = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) != NULL )
          && ( wield2->item_type == ITEM_WEAPON ) )  )
      one_hit( ch, victim, dt, TRUE );
    if ( ch->fighting != victim )
	return;

    }



    chance = IS_NPC(ch) 
       ? ( IS_SET( ch->skills, MOB_FIFTH ) ? 50 : 0 )
       : ( ( get_skill(ch, gsn_fifth_attack)/6 ) - ( dex_app[get_curr_dex( ch )].defensive /3 )
         + ( dual_chance ) );
    multi_hit = ( IS_NPC( ch ) ? FALSE : ( get_skill(ch, gsn_fifth_attack) > 0 ) );
  	
    if (  ( number_percent( ) < chance )
       && ( IS_NPC( ch ) || multi_hit ) )
    {
	one_hit( ch, victim, dt, FALSE );
	if ( ch->fighting != victim )
	    return;
    if (  (  ( ( wield1 = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  != NULL )
          && ( wield1->item_type == ITEM_WEAPON ) )
       && (  ( ( wield2 = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) != NULL )
          && ( wield2->item_type == ITEM_WEAPON ) )  )
      one_hit( ch, victim, dt, TRUE );
    if ( ch->fighting != victim )
	return;

    }


    chance = IS_NPC(ch) 
       ? ( IS_SET( ch->skills, MOB_SIXTH ) ? 30 : 0 )
       : ( ( get_skill(ch, gsn_sixth_attack)/7 ) - ( dex_app[get_curr_dex( ch )].defensive /3 )
         + ( dual_chance ) );
    multi_hit = ( IS_NPC( ch ) ? FALSE : ( get_skill(ch, gsn_sixth_attack) > 0 ) );
  	
    if (  ( number_percent( ) < chance )
       && ( IS_NPC( ch ) || multi_hit ) )
    {
	one_hit( ch, victim, dt, FALSE );
	if ( ch->fighting != victim )
	    return;
    if (  (  ( ( wield1 = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  != NULL )
          && ( wield1->item_type == ITEM_WEAPON ) )
       && (  ( ( wield2 = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) != NULL )
          && ( wield2->item_type == ITEM_WEAPON ) )  )
      one_hit( ch, victim, dt, TRUE );
    if ( ch->fighting != victim )
	return;

    }
    if ( !IS_NPC( ch ) && IS_SET( race_table[ch->race].race_flags, RACE_MOD_TAIL )  )
    {
      if ( number_percent( ) < 25 ) 
        one_hit( ch, victim, TYPE_HIT + 13, FALSE );
    }
    return;
}
#define ZERO_RATIO 20
#define ONE_STEPS 100l
#define MAX_IX 120
#define MAX_DAM_MOD 1.4


int get_weapon_sn(CHAR_DATA * ch, bool isDual)
{
    OBJ_DATA *wield;
    int sn;

    wield = get_eq_char(ch, WEAR_HOLD_HAND_L);

    if (isDual == TRUE)
	wield = get_eq_char(ch, WEAR_HOLD_HAND_R);

    if (wield == NULL || wield->item_type != ITEM_WEAPON)
	sn = gsn_martial_arts;
    else
	switch (wield->value[0]) {
	default:
	    sn = -1;
	    break;
	case (WEAPON_SWORD):
	    sn = gsn_sword;
	    break;
	case (WEAPON_DAGGER):
	    sn = gsn_dagger;
	    break;
	case (WEAPON_SPEAR):
	    sn = gsn_spear;
	    break;
	case (WEAPON_MACE):
	    sn = gsn_mace;
	    break;
	case (WEAPON_FIST):
		sn = gsn_fist;
		break;
	case (WEAPON_AXE):
	    sn = gsn_axe;
	    break;
	case (WEAPON_FLAIL):
	    sn = gsn_flail;
	    break;
	case (WEAPON_WHIP):
	    sn = gsn_whip;
	    break;
	case (WEAPON_POLEARM):
	    sn = gsn_polearm;
	    break;
	}
    return sn;
}
int get_weapon_skill(CHAR_DATA * ch, int sn)
{
    int skill;

    /* -1 is exotic */
    if (IS_NPC(ch)) {
	if (sn == -1)
	    skill = 3 * ch->level;
	else if (sn == gsn_martial_arts)
	    skill = 40 + 2 * ch->level;
	else
	    skill = 40 + 5 * ch->level / 2;
    } else {
	if (sn == -1)
	{
	    skill = 3 * ch->level;
	}
	else
	{ 
		if (!can_use_skill(ch, sn) )
		skill = 0;
		else
	    skill = ch->pcdata->learned[sn];
	}
    }

    return URANGE(0, skill, 100);
}

int get_skill(CHAR_DATA * ch, int sn)
{
    int skill;

    if (sn == -1) {		/* shorthand for level based skills */
	skill = ch->level * 5 / 2;
    } else if (sn < -1 || sn > MAX_SKILL) {
	bug("Bad sn %d in get_skill.", sn);
	skill = 0;
    } else if (!IS_NPC(ch)) {
		if (is_remort(ch) && skill_table[sn].flag1 == REMORT)
		{
	if (ch->pcdata->learned[sn] <= 0)
	    skill = 0;
	else
	    skill = ch->pcdata->learned[sn];
		} else if (!is_remort(ch))
		{
	if (ch->pcdata->learned[sn] <= 0)
	    skill = 0;
	else
	    skill = ch->pcdata->learned[sn];
		} else if (is_remort(ch))
		{
	 if (ch->pcdata->learned[sn] <= 0)
	    skill = 0;
	else
	    skill = ch->pcdata->learned[sn];
		} else
		{
			skill = 0;
		}

    } else {			/* mobiles */

	if (skill_table[sn].spell_fun != spell_null)
	    skill = 40 + 2 * ch->level;

	else if (sn == gsn_sneak || sn == gsn_hide)
	    skill = ch->level * 2 + 20;

//	else if ((sn == gsn_dodge && IS_SET(ch->off_flags, OFF_DODGE))
//		 || (sn == gsn_parry && IS_SET(ch->off_flags, OFF_PARRY)))
//	    skill = ch->level * 2;

	else if (sn == gsn_shield_block)
	    skill = 10 + 2 * ch->level;

	else if (sn == gsn_second_attack)
	    skill = 10 + 3 * ch->level;

	else if (sn == gsn_third_attack)
	    skill = 4 * ch->level - 40;

	else if (sn == gsn_martial_arts)
	    skill = 40 + 2 * ch->level;

	else if (sn == gsn_trip )
	    skill = 10 + 3 * ch->level;

	else if (sn == gsn_bash )
	    skill = 10 + 3 * ch->level;

	else if (sn == gsn_disarm)
	    skill = 20 + 3 * ch->level;

//	else if (sn == gsn_berserk )
//	    skill = 3 * ch->level;

	else if (sn == gsn_kick)
	    skill = 10 + 3 * ch->level;

	else if (sn == gsn_backstab )
	    skill = 20 + 2 * ch->level;

	else if (sn == gsn_rescue)
	    skill = 40 + ch->level;

//	else if (sn == gsn_recall)
//	    skill = 40 + ch->level;

	else if (sn == gsn_sword
		 || sn == gsn_dagger
		 || sn == gsn_spear
		 || sn == gsn_mace
		 || sn == gsn_axe
		 || sn == gsn_flail
		 || sn == gsn_whip
		 || sn == gsn_polearm
	     || sn == gsn_fist)
	    skill = 40 + 5 * ch->level / 2;

	else
	    skill = 0;
    }

//    if (ch->daze > 0) {
//	if (skill_table[sn].spell_fun != spell_null)
//	    skill /= 3;
//	else
//	    skill = 2 * skill / 3;
//    }
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	skill = 9 * skill / 10;

    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SUNWEAK))
	skill = (skill * 9) / 10;

    return URANGE(0, skill, 100);
}
/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool dual )
{
    OBJ_DATA *wield;
    OBJ_DATA *shield;
//	bool wep;
    int victim_ac;
//    int remort_bonus;
    int dam;
    int diceroll;
//	int numb = -3;
//    int ix;
	int sn; /* For weapon checking */
	int skill; /* for Weapon skill calculation... */
	int hitroll;
	int damroll;
	int thac0;
	int thac0_00;
	int thac0_32;
//	char buf[MSL];
    float dam_mod;
//    extern const float hr_damTable[121];
    CHAR_DATA *ppl;    
    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
     
  if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
	
	for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      if   ( ( ppl != ch ) && ( is_same_group( ch, ppl ) ) )
      {
      update_pos ( ppl );
      break;
      }

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_HOLD_HAND_L );
    if ( !IS_WEAPON( wield ) || dual )
      wield = get_eq_char( ch, WEAR_HOLD_HAND_R );
    if ( !IS_WEAPON( wield ) )
      wield = NULL;
    if ( dt == TYPE_UNDEFINED )
    {
      dt = TYPE_HIT;
      if ( wield != NULL && wield->item_type == ITEM_WEAPON )
        dt += wield->value[3];
    }
	sn = get_weapon_sn(ch, dual);
    skill = 10 + get_weapon_skill(ch, sn);
	/* check for martial arts  Taken out for wierd act crash bug with type_martial ZEN
    if ( dt == TYPE_HIT && wield == NULL )
       if ( number_percent() < ( IS_NPC(ch) ? 
          ( IS_SET(ch->skills, MOB_MARTIAL ) ? 75 : 0 ) : ( ch->pcdata->learned[gsn_martial_arts]/4) ) )
       dt = TYPE_MARTIAL;
    
*/
    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
   
    
    if (IS_NPC(ch)) {
	thac0_00 = 20;
	thac0_32 = -4;		/* as good as a thief */
	thac0_32 = -10;
    } else {
	thac0_00 = 20;
	thac0_32 = -10;
    }
    thac0 = interpolate(ch->level, thac0_00, thac0_32);

    if (thac0 < 0)
	thac0 = thac0 / 2;

    if (thac0 < -5)
	thac0 = -5 + (thac0 + 5) / 2;



//slow down hr past 80
	hitroll = GET_HITROLL(ch);
	hitroll = hitroll <= 80 ? hitroll : 80 + ((hitroll - 80) / 5);


    if (!IS_NPC(ch)) {
	if (ch->pcdata->strategy <= 1) {
	    thac0 -= (hitroll - (ch->level / 3)) * skill / 100;
	} else if (ch->pcdata->strategy >= 3) {
	    thac0 -= (hitroll + (ch->level / 3)) * skill / 100;
	} else
	    thac0 -= hitroll * skill / 100;
    } else
	thac0 -= hitroll * skill / 100;

    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab)
	thac0 -= 10 * (100 - get_skill(ch, gsn_backstab));

    if (dt == gsn_circle)
	thac0 -= 10 * (100 - get_skill(ch, gsn_circle));

    victim_ac = GET_AC(victim) / 10;

    if (victim_ac < -15)
	victim_ac = (victim_ac + 15) / 5 - 15;

    if (!can_see(ch, victim)) {
	if (get_skill(ch, gsn_martial_arts) > 0) {
	    if (number_percent() <= get_skill(ch, gsn_martial_arts))
		check_improve(ch, gsn_martial_arts, TRUE, 6);
	    else
		victim_ac -= 4;
	} else
	    victim_ac -= 4;
    }
    if (victim->position < POS_FIGHTING)
	victim_ac += 4;

    if (victim->position <= POS_RESTING)
	victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ((diceroll = number2_bits(5)) >= 20);

    // knock off a bit of the condition for the wielded weapon.
//    if (wield != NULL) {
//	if (number_range(1, 10) == 7) {
//	    if (wield->condition > 0)
//		wield->condition += -1;
//	}
//    }
    if (diceroll == 0 || (diceroll != 19 && diceroll < thac0 - victim_ac)) {
		damage (ch, victim, 0, dt);
//	  sprintf(buf, "{RHitroll{x: %d Diceroll: %d Numb: %d VictAC: %d VictArm: %d\n\r", hitroll, diceroll, numb, victim_ac, victim->armor);
//	  send_to_char(buf, ch);
	  check_improve(ch, sn, FALSE, 1);
	  		if (dt != (
			(gsn_sword) 
	      ||(gsn_mace)
		  ||(gsn_flail)
		  ||(gsn_whip)
		  ||(gsn_fist)
		  ||(gsn_spear)
		  ||(gsn_polearm)
		  ||(gsn_dagger)
		  ||(gsn_axe)
		  ||(TYPE_HIT)
		         )
		    )
	    check_improve(ch, dt, FALSE, 1);
      tail_chain( );
      return;
    }
//    else if (ix <= MAX_IX)
//      dam_mod = hr_damTable[ix];
//    else
     dam_mod = 0.0;
    /*
     * Hit.
     * Calc damage.
     * Tried to make it easy for players to hit mobs... --Stephen
     */
    if ( IS_NPC(ch) )
    {
      if ( wield )
        dam = number_range( wield->value[1], wield->value[2] );
      else
		  /* Mobs do more Damage, :D */
//        dam = number_range( ch->level / 3, ch->level / 2 );
	  	dam = number_range(ch->level / 2, ch->level * 3 / 2);
      if ( IS_SET( ch->act, ACT_SOLO ) )
        dam *= 1.5;

    }
    else
    {
      if ( wield != NULL )
		dam = dice(wield->value[1], wield->value[2]) * skill / 100;
//      dam = number_range( wield->value[1], wield->value[2] );
      else
        dam = UMAX( number_range(2,4), ch->level/4 );
    }

    /*
     * Bonuses.
     */
//    dam += number_range( GET_DAMROLL(ch)* 13/20, GET_DAMROLL( ch ) * 15/20 );
    if ( ( !IS_NPC(ch) && get_skill(ch, gsn_enhanced_damage) > 0 )
           || item_has_apply(ch, ITEM_APPLY_ENHANCED)
           || ( IS_NPC(ch) && IS_SET(ch->skills, MOB_ENHANCED ) ) )
    {
        if (IS_NPC(ch))
            dam *= 2;
        else
            dam += ( get_skill(ch, gsn_enhanced_damage) > 0 ) ?
            dam * get_skill(ch, gsn_enhanced_damage) / 150 :
            dam * .4;
    }

    damroll = GET_DAMROLL(ch);
	damroll = damroll <= 80 ? damroll : 80 + ((damroll - 80) / 5);

	dam += damroll * UMIN(100, skill) / 100;

    if (!IS_NPC(ch)) {
	if (ch->pcdata->strategy <= 1)
	    dam -= (ch->level / 2);
	if (ch->pcdata->strategy >= 3)
	    dam += (ch->level / 5);
    }
    if (dam <= 0)
	dam = 1;
/* Less damage overall, longer battles this way */
//	dam = (dam * 3) / 4;

    if ( !IS_AWAKE(victim) )
	dam *= 1.5;
    /* extra damage from martial arts */
    if ( dt == TYPE_MARTIAL )
       dam = ( dam * 4 ) / 3;
    
    if ( dt == gsn_backstab )
	dam *= 1.4;
    if ( dt == gsn_circle )
       dam *= 1.1;
    if ( IS_AFFECTED( ch, AFF_CLOAK_ADEPT ) )
      dam *= 1.2;
    if ( dam <= 0 )
     dam = 1;
    shield = NULL;
    if ( IS_SHIELD( shield = get_eq_char( victim, WEAR_HOLD_HAND_R ) ) )
    { }
    else if ( IS_SHIELD( shield = get_eq_char( victim, WEAR_HOLD_HAND_L ) ) )
    { }
    else
      shield = NULL;
    {
      int skin_mods;
      if ( !IS_NPC( victim ) )
      {
        skin_mods = race_table[victim->race].race_flags;
      }
      else
      {
        skin_mods = ( victim->race == 0 ?
                       victim->pIndexData->race_mods :
                       race_table[victim->race].race_flags );
      }
      if ( IS_SET( skin_mods, RACE_MOD_TOUGH_SKIN ) )
        dam_mod -= .1;
      else if ( IS_SET( skin_mods, RACE_MOD_STONE_SKIN ) )
        dam_mod -= .2;
      else if ( IS_SET( skin_mods, RACE_MOD_IRON_SKIN ) )
        dam_mod -= .3;
    }
    if (  (  ( IS_NPC( victim ) )
          || (  get_skill(victim, gsn_shield_block) > 0 ) )
       && ( shield ) 
       && ( number_range(1, 4 ) != 3 )
       && ( number_percent() < 
            (  IS_NPC( victim ) ? 
               get_psuedo_level( victim )/ 7  :
                get_skill(victim, gsn_shield_block)/5 )
            + ( IS_NPC( victim ) ? 
                10 :
                ( 1 * ( victim->level - ch->level )  + victim->level/8 ) ) ) )
      /* Shield Block! */
      {
      act( "$n blocks $N's attack with $p", victim, shield, ch, TO_NOTVICT );
      act( "$N blocks your attack with $p", ch, shield, victim, TO_CHAR );
      act( "You block $N's attack with $p", victim, shield, ch, TO_CHAR );
      damage( ch, victim, 0, -1 );
      }
      else
      {
//        dam *= dam_mod;
        if (  ( wield )
           && ( dam > 0 )
           && ( IS_OBJ_STAT( wield, ITEM_LIFESTEALER ) )
           && ( number_range( 0, 99 ) < ( wield->level / 4 )  ) )
        {
          act( "{W$n screams in {ragony{W as an evil {da{Rur{da{W flows from $p!{x", victim, wield, ch, TO_NOTVICT );
          act( "{WYou feel a surge of health as $p{W sucks the life of{x $N{x!!", ch, wield, victim, TO_CHAR );
          act( "{WYou scream in {ragony{W as $p{W shrieks, and shrouds you in an evil {da{Rur{da{x!!", victim, wield, ch, TO_CHAR );
            ch->hit = UMIN( ch->max_hit, ch->hit + number_range( dam * .03, dam * .13 ) );
          ch->alignment = UMAX( -1000, ch->alignment - 50 );
        }
      }

        damage( ch, victim, dam, dt );
//	  sprintf(buf, "{GHitroll{x: %d Diceroll: %d Numb: %d VictAC: %d VictArm: %d\n\r", hitroll, diceroll, numb, victim_ac, victim->armor);
//	  send_to_char(buf, ch);

		check_improve(ch, sn, TRUE, 1); /* Get better at weapon? */

		/* I THINK this will make skills update... Not sure...
		 */

		if (dt != (
			(gsn_sword) 
	      ||(gsn_mace)
		  ||(gsn_flail)
		  ||(gsn_whip)
		  ||(gsn_fist)
		  ||(gsn_spear)
		  ||(gsn_polearm)
		  ||(gsn_dagger)
		  ||(gsn_axe)
		  ||(TYPE_HIT)
		         )
		    )
	    check_improve(ch, dt, TRUE, 1);

    tail_chain( );
    return;
}



/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
     CHAR_DATA       *ppl;
     OBJ_DATA        *sil_weapon;
     int             sn;


/*   char buf[MAX_STRING_LENGTH];   this is unused now -- uni */

    if ( victim->is_free == TRUE )
    {
      bug( "Freed victim in one_hit", 0 );
      return;
    }
    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 3000 )
    {
      char buf[MAX_STRING_LENGTH];
      sprintf( buf, "Combat: %d damage by %s, attacking %s, dt %d", dam,
        IS_NPC(ch) ? ch->short_descr : ch->name,
        victim->name, dt );
      if ( ch->level < 102 )
	  {                         /* stop imms generating warnings!! */
        monitor_chan( buf, MONITOR_COMBAT );
     log_f( buf );
     dam = 3000;
	  }
    }

    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return;
	 if ( victim != ch->fighting )   
         	check_killer( ch, victim );  

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch, FALSE );
	    victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
	    {
		set_fighting( ch, victim, TRUE );
		/* check_killer( ch, victim );*/
            }

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
	{
	    affect_strip( ch, gsn_invis );
	    affect_strip( ch, gsn_mass_invis );
	    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	    act( "$n shimmers into existence.", ch, NULL, NULL, TO_ROOM );
	}

	/*
	 * Damage modifiers.
	 */
	if ( IS_AFFECTED(victim, AFF_SANCTUARY ) 
	     || item_has_apply( victim, ITEM_APPLY_SANC ) )
	    dam /= 2;

	if ( ( IS_AFFECTED(victim, AFF_PROTECT)|| item_has_apply( ch, ITEM_APPLY_PROT ) )
	       && IS_EVIL(ch) )
	    dam -= dam / 4;

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, trip, parry, and dodge.
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( IS_NPC(ch) 
	       && ( number_percent( ) < ch->level / 6 )
	       && IS_SET( ch->skills, MOB_DISARM ) )
		disarm( ch, victim, NULL );
	    
	    if ( IS_NPC(ch) 
	       && ( number_percent( ) < ch->level / 6 )
	       && IS_SET( ch->skills, MOB_TRIP ) )
		trip( ch, victim );
		
	    if ( check_parry( ch, victim ) )
		return;
	    if ( check_dodge( ch, victim ) )
		return;
	    if ( check_skills( ch, victim ) )
	        return;
	}
       if ( dt != -1 ) 
	   dam_message( ch, victim, dam, dt, 0 );
    }

/* for now, can only have one shield up, or alternatively, only the first
   shield does anything		 */

    if (  ( victim->first_shield != NULL )
       && ( ch != victim )  
       && ( dam > 0 ) )
    {
      char	buf1[MSL];
      char	buf2[MSL];
      char	buf3[MSL];
      sprintf( buf1, "%s", victim->first_shield->absorb_message_room );
      if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
        sprintf( buf2, "%s", "$K shield ouch" );
      else
        sprintf( buf2, "%s", victim->first_shield->absorb_message_victim );
      if ( !IS_NPC( victim ) && IS_SET( victim->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
        sprintf( buf3, "%s", "Your shield cool" );
      else
        sprintf( buf3, "%s", victim->first_shield->absorb_message_self );
      victim->first_shield->hits -= dam;   
      dam = dam - dam * ( victim->first_shield->percent / 100 );  
      if ( victim->first_shield->harmfull == TRUE )
      {
        if(!IS_IMMORTAL(ch))
        ch->hit = UMAX( 10, ( ch->hit - victim->first_shield->attack_dam ) ); 
      }
      act( buf1, victim, NULL, ch, TO_NOTVICT );
      act( buf2, ch, NULL, victim, TO_CHAR );
      act( buf3, victim, NULL, ch, TO_CHAR );
      
      if ( victim->first_shield->hits <= 0 )
      {
	AFFECT_DATA * shield_aff;
        
        for ( shield_aff = victim->first_affect; shield_aff != NULL; shield_aff = shield_aff->next )
          if ( shield_aff->type == victim->first_shield->sn )
            break;
        if ( shield_aff != NULL )
          affect_remove( victim, shield_aff );
      }
    }
    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
	if(!IS_IMMORTAL(victim))
    victim->hit -= dam;
     if ( !IS_NPC ( victim ) )
      check_adrenaline( victim , dam );
/*
     if ( !IS_NPC( victim ) && IS_WOLF( victim ) && ( dam > 350 )  )
       do_rage( victim, "FORCE" );
*/

    if (  ( IS_AFFECTED( victim, AFF_CLOAK_FLAMING ) )
       && ( ch != victim )  
     /*  && ( dt == TYPE_HIT ) */  )
    {
      

      act( "{x$n's {Rcloak{x flares and envelops $N in {Rflames{x!!", victim, NULL, ch, TO_NOTVICT );
      if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
        act( "Flame cloak on $K ouch", ch, NULL, victim, TO_CHAR );
      else
        act( "{x$N's {Rcloak{x flares, and envelops you with {Rflame{x!!", ch, NULL, victim, TO_CHAR );
      if ( !IS_NPC( victim ) && IS_SET( victim->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
        act( "Your Flame cloak flares", victim, NULL, ch, TO_CHAR );
      else
        act( "{xYour {Rcloak{x flares, and envelops $N with {Rflame{x!!!", victim, NULL, ch, TO_CHAR );

      if (  ( is_shielded( ch, ICE_SHIELD ) )
         && ( number_range( 1, 100 ) < 30 )  
         && ( ch != victim )   )
      {
        CHAR_DATA	* rch;
        CHAR_DATA	* rch_next;
        OBJ_DATA	* explosion;
        CHAR_DATA	* elemental;

        act( "{R------------------------{x", ch, NULL, NULL, TO_CHAR );
        act( "{B************************{x", ch, NULL, NULL, TO_CHAR );
        act( "{W!!!!!!!!!!!!!!!!!!!!!!!!{x", ch, NULL, NULL, TO_CHAR );
        act( "{xThe elemental forces of {RFire{x and {CIce{x destroy each other!!!", ch, NULL, NULL, TO_CHAR );
        act( "{W!!!!!!!!!!!!!!!!!!!!!!!!{x", ch, NULL, NULL, TO_CHAR );
        act( "{B************************{x", ch, NULL, NULL, TO_CHAR );
        act( "{R------------------------{x", ch, NULL, NULL, TO_CHAR );
        act( "{R------------------------{x", ch, NULL, NULL, TO_ROOM );
        act( "{B************************{x", ch, NULL, NULL, TO_ROOM );
        act( "{W!!!!!!!!!!!!!!!!!!!!!!!!{x", ch, NULL, NULL, TO_ROOM );
        act( "{xThe elemental forces of {RFire{x and {CIce{x destroy each other!!!", ch, NULL, NULL, TO_ROOM );
        act( "{W!!!!!!!!!!!!!!!!!!!!!!!!{x", ch, NULL, NULL, TO_ROOM );
        act( "{B************************{x", ch, NULL, NULL, TO_ROOM );
        act( "{R------------------------{x", ch, NULL, NULL, TO_ROOM );
        act( "{x$N's {Rcloak{x is ripped to shreds!!!{x", ch, NULL, victim, TO_ROOM );
        act( "{xYour {Rcloak{x is ripped to shreds!!!{x", ch, NULL, victim, TO_VICT );


        affect_strip( ch, skill_lookup( "iceshield" ) );
        affect_strip( victim, skill_lookup( "iceshield" ) );
        affect_strip( ch, skill_lookup( "fireshield" ) );
        affect_strip( victim, skill_lookup( "fireshield" ) );
        affect_strip( ch, skill_lookup( "cloak:flaming" ) );
        affect_strip( victim, skill_lookup( "cloak:flaming" ) );
        if ( IS_SET( ch->affected_by, AFF_CLOAK_FLAMING ) )
          REMOVE_BIT( ch->affected_by, AFF_CLOAK_FLAMING );
        if ( IS_SET( victim->affected_by, AFF_CLOAK_FLAMING ) )
          REMOVE_BIT( victim->affected_by, AFF_CLOAK_FLAMING );


        if ( ( explosion = create_object( get_obj_index( OBJ_VNUM_CONFLAGRATION ), 120 ) ) != NULL )
          if ( ( elemental = create_mobile( get_mob_index( MOB_VNUM_COMBAT_ELEMENTAL ) ) ) != NULL )
          {
            char	bufz[MSL];

            explosion->level = 120;
            free_string( explosion->short_descr );
            explosion->short_descr = str_dup( "{mConflagration{x" );
            free_string( explosion->description );
            explosion->description = str_dup( "{x A {RFlaming {xStaff of {CIce{x is supsended in mid air!" );

            elemental->level = 140;
            free_string( elemental->name );
            elemental->name = str_dup( ".hidden" );
            free_string( elemental->short_descr );
            elemental->short_descr = str_dup( "{xThe {GConflict{x of {RFire {xand {CIce{x" );
            free_string( elemental->long_descr );
            elemental->long_descr = str_dup( "{xA {GPillar{x of {RFire {xand {CIce{x immolates itself!" );

            char_to_room( elemental, ch->in_room );
            obj_to_char( explosion, elemental );
            sprintf( bufz, "%s", explosion->name );
            do_wear( elemental, bufz );

            if ( number_range( 0, 99 ) < 40 ) 
              obj_cast_spell( skill_lookup( "Retributive strike" ), 100, elemental, NULL, explosion );
            else
            {
              CREF( rch_next, CHAR_NEXTROOM );
              for ( rch = ch->in_room->first_person; rch != NULL; rch = rch_next )
              {
                rch_next = rch->next_in_room;
                if ( get_psuedo_level( rch ) < 70 )
                  continue;
                send_to_char( "\n\r{xYou are {Cblasted{x by the {Rconflagration{x!\n\r", rch );
                if ( number_range( 0, 99 ) < 50 ) 
                 obj_cast_spell( skill_lookup( "frost breath" ), 120, elemental, rch, explosion );
                else
                  obj_cast_spell( skill_lookup( "lava burst" ), 120, elemental, rch, explosion );
              }
              if (  ( elemental->fighting != NULL )
                 && ( IS_NPC( elemental->fighting ) )  )
              {
                stop_fighting( elemental, TRUE );
                if (  ( number_range( 0,99 ) < 50 )
                   && ( !IS_NPC( ch ) )  )
                {
                  multi_hit( elemental, ch, TYPE_UNDEFINED );
                }
                else
                if ( !IS_NPC( victim )  )
                {
                  multi_hit( elemental, victim, TYPE_UNDEFINED );
                }
              }
              CUREF( rch_next );
            }
          }
      }
      else
      {
        int   flame_damage;
        flame_damage = dam * get_psuedo_level( victim ) / 300;
        if  ( IS_NPC( ch ) )
        {  
          if  ( IS_SET( ch->act, ACT_SOLO ) ) 
            flame_damage = flame_damage / 5;
          else
           flame_damage = flame_damage / 3;
        }
        if ( ( IS_NPC( victim ) ) && ( IS_SET( victim->act, ACT_SOLO ) ) )
          flame_damage = flame_damage * 1.5;
        if(!IS_IMMORTAL(ch))
        ch->hit = UMAX( -5, ch->hit - flame_damage );
      }
 
    }
    if (  ( is_affected( victim, skill_lookup( "cloak:misery" ) ) )
       && ( ch != victim ) 
       && ( number_range( 0, 99 ) < 35 )  
     /*  && ( dt == TYPE_HIT ) */  )
    {
      AFFECT_DATA af;
      act( "{x$n's {dcloak{x flares and shrouds $N in {dmisery{x!!", victim, NULL, ch, TO_NOTVICT );
      if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
        act( "Flame cloak on $K ouch", ch, NULL, victim, TO_CHAR );
      else
        act( "{x$N's {dcloak{x flares, and shrouds you with {dmisery{x!!", ch, NULL, victim, TO_CHAR );
      if ( !IS_NPC( victim ) && IS_SET( victim->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
        act( "Your Death cloak flares", victim, NULL, ch, TO_CHAR );
      else
        act( "{xYour {dcloak{x flares, and shrouds $N with {dmisery{x!!!", victim, NULL, ch, TO_CHAR );
      if ( TRUE )
      {
        af.type      = skill_lookup( "curse" );
        af.duration  = 1;
        af.location  = APPLY_HITROLL;
        af.modifier  = -1;
        af.bitvector = AFF_CURSE;
        affect_join( ch, &af );

        af.location  = APPLY_DAMROLL;
        af.modifier  = -1;
        affect_join( ch, &af );

        af.location  = APPLY_AC;
        af.modifier  = -5;
        affect_join( ch, &af );
      }
    }
    update_pos( victim );

    if ( ( IS_NPC( victim) || !IS_VAMP( victim ) ) &&  !( deathmatch) )
    {
    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is too stunned to do anything!",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are too stunned to do anything!\n\r",
	    victim );
	break;

    case POS_DEAD:
        if ( ( sil_weapon = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL )
          sil_weapon = get_eq_char( ch, WEAR_HOLD_HAND_R );
        if ( IS_WOLF( victim )  && (!IS_NPC( ch ) ) && (get_skill(ch, gsn_decapitate) != 0) 
             && (sil_weapon != NULL) && (IS_SET( sil_weapon->extra_flags, ITEM_SILVER ) ) )
        {
 	    int chance;
           
            chance = IS_NPC( ch ) ? ch->level*2 : get_skill(ch, gsn_decapitate);
            chance += 25;

            if ( ( victim->pcdata->vamp_level * 5 ) > ch->level )
                chance -= ( victim->pcdata->vamp_level * 5 ) - ch->level;
   
            if ( number_percent() < chance )
            {
	        act( "You DECAPITATE $N's head off with one skillful stroke!", ch, NULL, victim, TO_CHAR );
                act( "$n DECAPITATES $N's head off with one skillful stroke!", ch, NULL, victim, TO_NOTVICT );
                send_to_char( "You suddenly feel the world is spinning away from you!", victim );
                send_to_char( "You have been DECAPITATED!!", victim );
                send_to_char( "You feel the rage of the wolf flow from your body.....\n\r", victim );
                send_to_char( "You THINK you are dead!  Ooops....\n\r", victim );
            }

            REMOVE_BIT( victim->pcdata->pflags , PFLAG_WEREWOLF );
            victim->pcdata->vamp_level = 0;
            victim->pcdata->vamp_exp = 0;
            victim->pcdata->bloodlust = 0;
            victim->pcdata->bloodlust_max = 0;
            victim->pcdata->generation = -1;
            victim->pcdata->vamp_bloodline = 0;
            victim->pcdata->recall_vnum = 3001;

	    for (sn = 0; sn <= MAX_SKILL; sn++ )
        	if ( ( skill_table[sn].flag2 == WOLF ) && ( victim->pcdata->learned[sn] > 0 ) )
          	     victim->pcdata->learned[sn] = 0;
        }

        act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	send_to_char( "{YYou have been KILLED!!{x\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "{RThat really did HURT!{x\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "{RYou sure are BLEEDING!{x\n\r", victim );
	break;
    }
    } /* end of if statement */
    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    
    
    update_pos ( ch );
    if ( victim->position == POS_DEAD && ( IS_NPC( victim ) || !IS_VAMP( victim ) || ( deathmatch ) )  )
    {
	    group_gain( ch, victim );
          update_pos ( ch );
	    /* Sort out kill counts..... */
	    if ( !IS_NPC( ch ) )
	    {
	        if ( !IS_NPC( victim ) )
                ch->pcdata->pkills++;
	        else
	            ch->pcdata->mkills++;
	    }
	    if ( !IS_NPC( victim ) )
	    {
	        if ( !IS_NPC( ch ) )
	            victim->pcdata->pkilled++;
	        else
	            victim->pcdata->mkilled++;
	    }
	
	if ( !IS_NPC(victim) || IS_SET(victim->act,ACT_INTELLIGENT))
	{
/*	    if ( !IS_NPC(ch) && !IS_NPC(victim)
	         && IS_SET(ch->pcdata->pflags,PFLAG_PKOK)
	         && IS_SET(victim->pcdata->pflags,PFLAG_PKOK) )
	    {

	     sprintf(buf, "%s kills %s in mortal combat.", ch->name, victim->name);
	     info(buf, 1);
	    }
	    else
	    {
             sprintf( buf, "%s turns %s into a corpse.  Whooops.",
               ( IS_NPC(ch) ? ch->short_descr : ch->name ),
               ( IS_NPC(victim) ? victim->short_descr : victim->name) );
             info( buf, 1 );	   
            }
*/
            update_pos ( ch );
            sprintf( log_buf, "%s killed by %s at %d",
		(IS_NPC(victim) ? victim->short_descr : victim->name),
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string( log_buf );

	notify( log_buf, 82 );

	/* As level gain is no longer automatic, a dead char loses
	 * 1/2 their gained exp.  -S- 
         * Fixed my bug here too, hehe!
	 */

	    if ( victim->exp <= 250 )
			victim->exp = 0;
		else
			victim->exp -=250;
		
	}

        if ( IS_NPC(ch) )
        {
 	   raw_kill( victim, "" );
 	   update_pos ( ch );
 	  }
        else
        {
          char name_buf[MAX_STRING_LENGTH];
          sprintf( name_buf, "%s", ch->name );
          update_pos ( ch );
	    raw_kill( victim, name_buf );
        }

	if ( deathmatch && !IS_NPC( victim ) )
	   do_quit( victim, "" );
	   update_pos ( ch );
	
	if ( IS_NPC(ch) && IS_NPC(victim) && IS_SET( ch->act, ACT_INTELLIGENT ) )
	{
	   do_get( ch, "all corpse" );
	   do_sacrifice( ch, "corpse" );
	   update_pos ( ch );
     check_re_equip( ch );
     check_rewield( ch );
	}
	
	if ( !IS_NPC(ch) && IS_NPC(victim) )
	{
	update_pos ( ch );
	    if ( IS_SET(ch->config, CONFIG_AUTOLOOT) )
	    {	
           do_get( ch, "all corpse" );
           update_pos ( ch );
           	for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      if   ( ( ppl != ch ) && ( is_same_group( ch, ppl ) ) )
      {
      update_pos ( ppl );
      break;
      }

	     } else {
		do_look( ch, "in corpse" );
		update_pos ( ch );
			for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      if   ( ( ppl != ch ) && ( is_same_group( ch, ppl ) ) )
      {
      update_pos ( ppl );
      }

		}
		
	    if ( IS_SET(ch->config, CONFIG_AUTOSAC) )
		do_sacrifice( ch, "corpse" );
		update_pos ( ch );
	}
	return;
    }

    if ( victim == ch )
	return;

    /*
     * Take care of link dead people.
     */
/*
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return;
	}
    }
*/
    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 )
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 1 ) == 0
	&&   victim->hit < victim->max_hit / 2 )
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait == 0 )
	do_flee( victim, "" );

    tail_chain( );
    return;

}



bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( deathmatch )
       return FALSE;    /* Deathmatch? Anything goes!! */
    
    
/*we are going to have safe rooms no attacks at all  SRZ 2 Jul 96
    if ( IS_NPC(ch) || IS_NPC(victim) )
	return FALSE;
*/


    /* No PC vs. PS attacks in safe rooms */
    if ( ( victim->in_room != NULL )
       && IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
       send_to_char( "Not a chance!  This is a safe room.\n\r", ch );
       return TRUE;
    }

    if ( IS_SET( victim->act, PLR_KILLER ) )
	return FALSE;
    if ( IS_SET( victim->act, PLR_THIEF ) )
      return FALSE;	
   /* Vampires are considered PKOK */	
	
    if ( !IS_NPC(ch) && !IS_NPC(victim)
         && (    IS_SET(victim->pcdata->pflags, PFLAG_PKOK)
              || IS_SET(victim->pcdata->pflags, PFLAG_VAMP) )
         && (    IS_SET(ch->pcdata->pflags, PFLAG_PKOK) 
              || IS_SET(ch->pcdata->pflags, PFLAG_VAMP) ) )
        return FALSE;

    if ( ( ( victim->level < 10 ) || ( victim->level + 20 < ch->level ) )
       && (!IS_NPC( victim ) )
       && ( !IS_NPC( ch ) )
       && ( !IS_NPC( ch ) )
       && ( ch->hometown == victim->hometown )
       && ( !IS_IMMORTAL( ch ) )  )
    {
	send_to_char( "The Gods prevent your foul deed.\n\r", ch );
	return TRUE;
    }
    
    return FALSE;
}



struct hunt_mobs_tp {
        int    mob_vnum;
        int    room_vnum;
        int    min_level;
        char*  name;
       } 
         hunt_mobs[]= {
                        { 3062, 3001,         0, "Bug" },
                        { 3561, 3001,        40, "Shadow Dragon"},
                        { 18302, 3001,       80, "Bounty Hunter"},
                        { 18301, 3001,       100, "The Avenger"},
                        { 18306,  3001,      240, "Blayze"},
                        { -1, -1,            -1, " " }
                        };
/* SRZ huntlist!  */                               
/* Scrapped MAX_HUNTERS - can now change table without 
 * full recompile.  Was unused in checking loop. -S-
 **** LEAVE THE LAST ELEMENT of the table in place! ****/


/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     * -S- Mod:  Set sentence according to difference in levels -
     * this is no. times the player must die before the killer flag
     * will be removed.
     */
    
    int diff;
    int a;
    MOB_INDEX_DATA  * pMobIndex;
    ROOM_INDEX_DATA * pRoomIndex;
    CHAR_DATA       * hunter;

    if ( IS_NPC ( ch ) &&(!IS_SET(ch->affected_by, AFF_CHARM) ) )
       return;
  if ( !IS_NPC(ch) && ( ch->hometown == victim->hometown ) && ( !IS_IMMORTAL(ch) ) && (ch != victim) &&(!IS_NPC(victim))  )
    {
/*
	return;
*/
    send_to_char( "{Y*** If you kill this person, you will be a {RWANTED MAN{Y ***{x\n\r", ch );
    save_char_obj( ch );
 }
    
   if ( (ch->fighting == victim ) )
     return;
/*
   if ( ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL ) 
      && ( ch->fighting != victim )  )
	victim = victim->master;   */ 

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim)
    ||   IS_SET(victim->act, PLR_KILLER)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PK ) /* -S- Mod */
    ||   IS_SET(victim->act, PLR_THIEF) 
    ||   IS_VAMP( victim ) 
    ||   IS_WOLF( victim ) /*
    || ( ch->fighting == victim )  */
    || ( ch == victim )  )
	return;
	
    /* if in deathmatch, anything goes */
      if ( deathmatch )
	return; 
    if ( ch->sentence > 5000 )
      return;	

    /* Check to see if ch & victim are in clans, and enemies */
    
    if ( !IS_NPC( ch ) && !IS_NPC( victim ) )
    {
       
	if ( ( ch->pcdata->clan != 0 ) && ( victim->pcdata->clan != 0 )
           && ( politics_data.diplomacy[ch->pcdata->clan][victim->pcdata->clan] < -450 ) ) 
       return;    
       
       if (  IS_SET(ch->pcdata->pflags,PFLAG_PKOK)
          && IS_SET(victim->pcdata->pflags,PFLAG_PKOK) )
          return;
    }
        
    
    
    /*
     * Charm-o-rama.
     */
/*    if ( IS_SET(ch->affected_by, AFF_CHARM) && ( ch->fighting != victim ) )
    {
	if ( ch->master == NULL )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Check_killer: %s bad AFF_CHARM",
		IS_NPC(ch) ? ch->short_descr : ch->name );
	    bug( buf, 0 );
	    affect_strip( ch, gsn_charm_person );
	    REMOVE_BIT( ch->affected_by, AFF_CHARM );
	    return;
	}
	
	ch=ch->master;  */

	/* stop_follower( ch ); Don't think we need to do this */
	
 /*   }    */
    
 /*   if ( IS_SET(victim->affected_by, AFF_CHARM) )
    {
        if ( victim->master == NULL)
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Check_killer: %s bad AFF_CHARM",
		IS_NPC(ch) ? victim->short_descr : victim->name );
	    bug( buf, 0 );
	    affect_strip( ch, gsn_charm_person );
	    REMOVE_BIT( victim->affected_by, AFF_CHARM );
	    return;
	}
	victim=victim->master;
    }
	
*/

    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * BAH.  Imms get flagged too now, unless pkok.
     * And current killers stay as they are.
     */

    

    /* MAG Create a hunter for the person */
    diff= get_psuedo_level( ch );
    
    /* Added if check back... meant to penalize for attacking lower
     * level players -S-
     */
     
    if ( get_psuedo_level( ch ) > get_psuedo_level( victim ) )
     diff+= get_psuedo_level( ch )  - get_psuedo_level( victim );
/*     
    if (diff > MAX_LEVEL)
     diff=MAX_LEVEL;
                       SRZ just set the level of the hunter to the level of the player :) */

    /* Fixed for loop so it can't become infinite -S- */
    for ( a=0; hunt_mobs[a].min_level != -1 ; a++ )
    {
     if (diff <= hunt_mobs[a].min_level)
      break;
    }
    
    if ( hunt_mobs[a].mob_vnum == -1 )
    {
       notify( "No killer hunter found in check_killer!", 83 );
       return;
    }
    
    if ( ( pMobIndex = get_mob_index( hunt_mobs[a].mob_vnum  ) ) == NULL )
    {
	bug("check_killer: No such mob %i.",hunt_mobs[a].mob_vnum);
	return;
    }
    
    if ( ( pRoomIndex = get_room_index( hunt_mobs[a].room_vnum ) ) == NULL )
    {
        bug("check_killer: No such room %i.",hunt_mobs[a].room_vnum);
        return;
    }

    hunter = create_mobile( pMobIndex );
    char_to_room( hunter, pRoomIndex );
   /* do_gossip( hunter, "Well, someone's gonna get it!" ); */
    set_hunt(hunter,NULL,ch,NULL,HUNT_WORLD|HUNT_OPENDOOR|HUNT_PICKDOOR|HUNT_UNLOCKDOOR|HUNT_INFORM,HUNT_CR|HUNT_MERC);

    return;
}



/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance=0;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC( victim ) && !IS_SET( victim->skills, MOB_PARRY ) )
       return FALSE;
    
    if ( IS_NPC(victim) )
    {
	/* Tuan was here.  :) */
	chance  =  get_psuedo_level( victim )/3.2 + get_curr_str( victim ) * 2 /5;
      if ( IS_SET( victim->act, ACT_SOLO ) )
      chance += 15;
    }
    else
    {
      OBJ_DATA * weapon;
      if (  ( weapon = get_eq_char( victim, WEAR_HOLD_HAND_L ) ) == NULL )
        weapon = get_eq_char( victim, WEAR_HOLD_HAND_R );
      if (  ( weapon == NULL )
         || ( !IS_WEAPON( weapon ) )  )
      { 
	    return FALSE;
      }
      if ( !IS_NPC( victim ) && IS_WOLF( victim ) 
         && ( IS_SHIFTED( victim ) || IS_RAGED( victim )  ) )
           return FALSE;

      chance  = (get_skill(victim, gsn_parry) / 3.5 ) + get_curr_str( victim ) * 3 /5;
    }
    if ( IS_AFFECTED( victim, AFF_CLOAK_ADEPT )  )
      chance += 5;

    if ( number_percent( ) < ( chance + ( get_psuedo_level( victim ) - get_psuedo_level( ch ) ) /2 ) )
    {

    act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
    act( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );  
    return TRUE;
    }
  return FALSE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance=0;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC( victim ) && !IS_SET( victim->skills, MOB_DODGE ) )
       return FALSE;
    
    if ( IS_NPC(victim) )
    {
	/* Tuan was here.  :) */
	chance  = get_psuedo_level( victim )/3.1 + get_curr_dex( victim ) *2 /5;
      if ( IS_SET( victim->act, ACT_SOLO ) )
      chance += 15;
    }
    else
    {
      chance  = ( get_skill(victim, gsn_dodge) / 3.5 ) + get_curr_dex( victim ) *3 /5;
    }
    if ( IS_AFFECTED( victim, AFF_CLOAK_ADEPT )  )
      chance += 5;

    if ( !IS_NPC( victim ) && IS_WOLF( victim )
       && ( IS_SHIFTED( victim ) || IS_RAGED( victim )   )  )
      chance += 20;


    if ( number_percent( ) < ( chance + ( get_psuedo_level( victim ) - get_psuedo_level ( ch ) ) /2 ) )
    {

    act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
    act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );  
    return TRUE;
    }
  return FALSE;
}


/*
 * Check_skills : if IS_NPC(ch) then check ch->skills to see if there are
 * any extra attack skills available for use --Stephen
 */

bool check_skills( CHAR_DATA *ch, CHAR_DATA *victim )
{
   int cnt, check;
   
   if ( !IS_NPC( ch ) )
      return FALSE;
   
   if ( number_percent() < 30 + ( ch->level - victim->level ) )
      return FALSE;
      
   /* Count how many of the attack skills are available */
   
   cnt = 0;
   if ( IS_SET( ch->skills, MOB_PUNCH 		) ) cnt++;
   if ( IS_SET( ch->skills, MOB_HEADBUTT	) ) cnt++;
   if ( IS_SET( ch->skills, MOB_KNEE		) ) cnt++;
   if ( IS_SET( ch->skills, MOB_DIRT		) ) cnt++;
   if ( IS_SET( ch->skills, MOB_CHARGE		) ) cnt++;

   if ( cnt == 0 )
      return FALSE;		/* There were no attack skills set */
      
   check = number_range( 1, cnt );
   
   cnt = 0;
   if ( IS_SET( ch->skills, MOB_PUNCH		) && ( ++cnt == check ) )
   {
      do_punch( ch, "" );
      return TRUE;
   }
   if ( IS_SET( ch->skills, MOB_HEADBUTT	) && ( ++cnt == check ) )
   {
      do_headbutt( ch, "" );
      return TRUE;
   }
   if ( IS_SET( ch->skills, MOB_KNEE ) && ( ++cnt == check ) )
   {
      do_knee( ch, "" );
      return TRUE;
   }
   if ( IS_SET( ch->skills, MOB_DIRT ) && ( ++cnt == check ) )
   {
      do_dirt( ch, "" );
      return TRUE;
   }
   if ( IS_SET( ch->skills, MOB_CHARGE ) && ( ++cnt == check ) )
   {
      do_charge( ch, "" );
      return TRUE;
   }
 
   return FALSE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int num, counter;
    CHAR_DATA *ppl;
    /*
    CHAR_DATA *ch;
    */
      
    if ( victim->hit < victim->max_hit  &&  IS_IMMORTAL(victim) )
      {
       victim->hit = victim->max_hit;
      }
    if ( victim->mana < victim->max_mana  &&  IS_IMMORTAL(victim) )
      {
       victim->mana = victim->max_mana;
      }
    if ( victim->move < victim->max_move  && IS_IMMORTAL(victim) )
      {
       victim->move = victim->max_move;
      }
      
     if ( victim->position == POS_FIGHTING && victim->fighting == NULL )
     stop_fighting( victim, TRUE );
     
    if ( IS_SET(victim->config, CONFIG_AUTOASSIST) )
    {
   for ( ppl = victim->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      if   ( ( ppl != victim              )
          && ( is_same_group( victim, ppl ) ) )
             {
/*             
   if ( ppl == NULL )
   {
      send_to_char( "Doesn't look like anyone needs your help right now...\n\r", ch );
      return;
   }
*/
   /* Assisting leader is main priority */
   if ( ( victim->leader != NULL )
     && ( victim->leader->in_room == victim->in_room )
     && ( victim->leader->fighting != NULL )
     && ( victim->fighting == NULL )
     && ( victim->leader != victim ) )
   {
     set_fighting( victim, victim->leader->fighting, TRUE );
     break;                                                          
   }                                                     

   /* Ok, so no leader to help... lets look for other group members 
   for ( ppl = victim->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
   */
      if  ( ( is_same_group( victim, ppl ) )
         && ( ppl != victim )
         && ( ppl->fighting != NULL )
         && ( victim->fighting  == NULL ) )
   {
     set_fighting( victim, ppl->fighting, TRUE );
     break;                                                          
   }                                                       
   }
   }   
      
    if ( victim->hit > 0 )
    {
	if ( IS_SET(victim->affected_by, AFF_VAMP_HEALING ) )
	   REMOVE_BIT(victim->affected_by, AFF_VAMP_HEALING );
	
	if ( victim->position <= POS_STUNNED && victim->stunTimer == 0 )
	{
	    act( "$n stands, and gets to $s feet.", victim, NULL, NULL, TO_ROOM );
	    victim->position = POS_STANDING;
	}
	return;
    }
    if ( IS_VAMP( victim ) 
       && (victim->hit < 1) 
       && !IS_SET( victim->affected_by, AFF_VAMP_HEALING ) 
       && !IS_NPC( victim)
       && !( deathmatch )   )
    {
       CHAR_DATA *check;
    
       if ( !IS_NPC(victim) )
          gain_exp( victim, 0 - ( victim->exp / 4 ) );
                    
       sprintf( buf, "%s (vampire) has been misted!", victim->name );
       monitor_chan( buf, MONITOR_COMBAT );
       
       act( "$n turns to mist and floats away....", victim, NULL, NULL, 
TO_ROOM );
       act( "You turn to mist and float back to the morgue...", victim, NULL, NULL, TO_CHAR );

/* Universe -  need to add junk here */
    if ( IS_VAMP( victim ) && 
        ( ( victim->fighting != NULL ) && !IS_NPC( victim->fighting ) ) )
    {
        counter = number_range( 2, 4 );
 
        while ( counter > 0 )
        {
           num = number_range( 1, UMIN( 1, victim->carry_number ) );
           obj = victim->first_carry;
           if ( obj == NULL )
              break;
    
           for ( obj = victim->first_carry; obj != NULL; 
		obj = obj->next_in_carry_list )
           {
                num = num - 1;
                if ( num == 0 )
                   break;
           }   

           if ( obj == NULL )
	      continue;

           if ( obj != NULL && !IS_SET( obj->extra_flags, ITEM_NOLOOT ) )
           {
                obj_from_char( obj );
                obj_to_room( obj, victim->in_room );
           }
           counter = counter - 1;
        }    
     }
    /* drop stuff if is (WANTED) */
    if (  ( IS_SET( victim->act, PLR_KILLER ) || IS_SET( victim->act, PLR_THIEF ) )
       && (  ( victim->fighting != NULL ) 
          && (  ( !IS_NPC( victim->fighting ) )  
             || ( !str_cmp( rev_spec_lookup( victim->fighting->spec_fun ), "spec_executioner" ) )  )   )    )
    {
        counter = number_range( 2,3 );
    
        while ( counter > 0 )
        {
           num = number_range( 1, UMIN( 1, victim->carry_number ) );
           obj = victim->first_carry;
           if ( obj == NULL )
              break;

           for ( obj =victim->first_carry; obj != NULL; 
		obj = obj->next_in_carry_list )
           {
                num = num - 1;
                if ( num == 0 )
                   break;
           }

           if ( obj == NULL )
	      continue;

           if ( obj != NULL && !IS_SET( obj->extra_flags, ITEM_NOLOOT ) )
           {
                obj_from_char( obj );
                obj_to_room( obj, victim->in_room );
           }
	   counter = counter - 1;
       }
     }

    /* drop stuff if is (WANTED) */
    if (  ( victim->fighting != NULL ) 
       && ( !IS_NPC( victim->fighting ) ) 
       && ( IS_WOLF( victim->fighting ) )  )
    {
        counter = number_range( 4, 8 );
    
        while ( counter > 0 )
        {
           num = number_range( 1, UMIN( 1, victim->carry_number ) );
           obj = victim->first_carry;
           if ( obj == NULL )
              break;

           for ( obj =victim->first_carry; obj != NULL; 
		obj = obj->next_in_carry_list )
           {
                num = num - 1;
                if ( num == 0 )
                   break;
           }

           if ( obj == NULL )
	      continue;

           if ( obj != NULL && !IS_SET( obj->extra_flags, ITEM_NOLOOT ) )
           {
                obj_from_char( obj );
                obj_to_room( obj, victim->in_room );
           }
	   counter = counter - 1;
       }
     }
       char_from_room( victim );
       char_to_room( victim, get_room_index( hometown_table[victim->hometown].death ) );
       act( "A mist floats in, and forms into $n's corpse!", victim, NULL, NULL, TO_ROOM );
            
       SET_BIT( victim->affected_by, AFF_VAMP_HEALING );

       stop_fighting( victim, TRUE );
       victim->hit = -20;    
       for ( check = first_char; check != NULL; check = check->next )
       {
         if ( check->hunting == victim )
           end_hunt( check );
       }
       return;
    }

    if (  IS_VAMP( victim ) 
       && IS_SET( victim->affected_by, AFF_VAMP_HEALING )
       && !( deathmatch ) 
       && !IS_NPC( victim ) )
    {
       
       /* Then they don't really die... just regen slowly...! */
          if ( victim->hit < -20 )
             victim->hit = -20;
          
            if ( victim->hit == -20 )
          {
             act( "$n's body slowly regains it's shape!", victim, NULL, NULL, TO_ROOM );
             send_to_char( "Your body slowly regains it's shape!\n\r", victim );
             victim->hit++;
             victim->position = POS_MORTAL;
          }
       else if ( victim->hit > -20 && victim->hit < -15 )
          victim->hit++;
       else if ( victim->hit == -15 )
          {
             act( "$n's skin grows back into place!", victim, NULL, NULL, TO_ROOM );
             send_to_char( "Your skin grows back into place!\n\r", victim );
             victim->hit++;
          }   
       else if ( victim->hit > -15 && victim->hit < -10 )
          victim->hit++;
       else if ( victim->hit == -10 )
          {
             act( "$n's wounds form scar tissue and vanish!", victim, NULL, NULL, TO_ROOM );
             send_to_char( "Your wounds form scar tissue and vanish!\n\r", victim );
             victim->hit++;
          }   
       else if ( victim->hit >-10 && victim->hit < -5 )
          victim->hit++;
       else if ( victim->hit == -5 )
          {
             act( "$n's looks like $s old self again.", victim, NULL, NULL, TO_ROOM );
             send_to_char( "Your look like your old self again.\n\r", victim );
             victim->hit++;
          }   
       else if ( victim->hit >-5 && victim->hit < -1 )
          victim->hit++;
       else
          {
             REMOVE_BIT( victim->affected_by, AFF_VAMP_HEALING );
             victim->position = POS_STUNNED;
             victim->hit = 5;
          }       
       return;
    }      
    
    if (  ( !IS_NPC( victim ) )
       && ( IS_VAMP(victim ) )
       && !( deathmatch ) )
       return;
    
    if ( IS_NPC(victim) && victim->hit <= 0 )
    {
	victim->position = POS_DEAD;
	return;
    }
         if ( victim->hit <= -10 ) victim->position = POS_DEAD;
    else if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim, bool check )
{
    if ( ch->fighting != NULL )
    {
	 /* bug( "Set_fighting: already fighting", 0 );  */
	return;
    }

    /* Check here for killer flag */
    if ( check )
       check_killer( ch, victim );
    
    
/*    if ( IS_NPC(victim) && IS_SET(victim->act, ACT_HUNTER ) )
       make_hunt( victim, ch );*/		/* fun fun FUN! */
     
    
    if ( IS_AFFECTED(victim, AFF_SLEEP) )
	affect_strip( victim, gsn_sleep );

    if( victim->position == POS_SLEEPING )
        do_stand(victim, "");

    ch->fighting = victim;
    ch->position = POS_FIGHTING;
    
    /* Check if mob has ACT_REMEMBER (ch to attack) SET */
    
    if ( IS_NPC(victim) && !IS_NPC( ch ) && IS_SET( victim->act, ACT_REMEMBER ) )
    {
    /* Then set victim->target to player's name... */
    if ( victim->target != NULL )
      free_string( victim->target );
    if ( ch != NULL && ch->is_free == FALSE )
      victim->target = str_dup( ch->name );
    }
    
    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
  CHAR_DATA *fch;
  CHAR_DATA * fch_next;

  ch->fighting = NULL;
  ch->position = POS_STANDING;
  update_pos(ch);


  if ( !fBoth )
   return;
    
  CREF( fch_next, CHAR_NEXT );
  for ( fch = first_char; fch != NULL; fch = fch_next )
  {
    fch_next = fch->next;
    if ( fch->fighting == ch )
    {
      fch->fighting = NULL;
      fch->position = POS_STANDING;
      update_pos( fch );
    }
  }
   CUREF( fch_next );
   return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CHAR_DATA *target = NULL;
    CHAR_DATA *wch;
    char *name;
    bool leave_corpse = FALSE;
 /*   int counter, num;   */
    extern OBJ_DATA *   quest_object;
    extern int       quest_timer;
    extern CHAR_DATA * quest_target;
    int num;
    
    one_argument ( argument, arg );

    if (deathmatch) 
      return;
       
    /* Vampire bodies crumble to dust when really killed */
    if ( IS_VAMP( ch ) && !IS_NPC( ch ) )
       return;
    
    if ( IS_NPC(ch) )
    {
      if ( ( ch->in_room != NULL )
      && IS_SET( ch->in_room->affected_by, ROOM_BV_SOUL_NET ) )
      {
        ROOM_INDEX_DATA *room;
        ROOM_AFFECT_DATA *raf;
        ROOM_AFFECT_DATA *raf_next;
        corpse = create_object( get_obj_index( OBJ_VNUM_CAPTURED_SOUL ), ch->level );
        corpse->level = ch->level;
        obj_to_room( corpse, ch->in_room );
        OREF( obj_next, OBJ_NEXTCONTENT );
        for ( obj = ch->first_carry; obj != NULL; obj = obj_next )
        {
          obj_next = obj->next_in_carry_list;
          obj_from_char( obj );
          extract_obj( obj );
        }
        OUREF( obj_next );
        act( "{RAs $n's soul attempts to fade from the room, the {dSoul Net{R quickly collapses, entombing the soul into a small figurine!!", 
        ch, NULL, NULL, TO_ROOM );
        room = ch->in_room;
        for ( raf = room->first_room_affect; raf != NULL; raf = raf_next )        
        {
          raf_next = raf->next;
          if ( raf->bitvector == ROOM_BV_SOUL_NET )
          {
            r_affect_remove( room, raf );
          }
        }
        return;
      }
      else  /* still NPC, no soul net */
      {
        int gold = 0;
//      time_t lifetime;
    
        name            = ch->short_descr;
        corpse          = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0 );	 
        corpse->timer   = number_range( 3, 6 );
        corpse->level   = ch->level; /* for animate spell */
	  /* Takes a mob 2 rl hours to gain full gold. */
//        lifetime= current_time - (ch->logon) ;

      for ( num = 0; num < MAX_CURRENCY; num++)
         if( ch->money->cash_unit[num] > 0)
         {
             corpse->money->cash_unit[num] = ch->money->cash_unit[num];
             ch->money->cash_unit[num] = 0;
         }


/*       gold = 5* (ch->level) * ( UMIN(100 , lifetime * 100 / (2*3600)) ) / 100;
       gold = number_range( gold/5 , gold); */
       gold += ch->gold;
	
       if ( gold > 0 )
       {
         join_money( round_money( gold, TRUE ), corpse->money );
       }
       ch->gold = 0;

      }
    }   
    else  /* player */
    {
      name            = ch->name;
      corpse          = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
      corpse->timer   = number_range( 20, 30 );
    
      sprintf( buf, "%s", ch->name );
      free_string( corpse->owner );
      corpse->owner = str_dup( buf );
      for ( num = 0; num < MAX_CURRENCY; num++)
         if( ch->money->cash_unit[num] > 0)
         {
             corpse->money->cash_unit[num] = ch->money->cash_unit[num];
             ch->money->cash_unit[num] = 0;
         }


      if ( ( arg[0] != '\0' ) && ( !IS_NPC( ch ) )  )
      {
        target = NULL;
        for ( wch = first_char; wch != NULL; wch = wch->next )
        {
          if ( !IS_NPC( wch ) &&  is_name( arg, wch->name ) )
          {
            target = wch;
            break;
          }
        }
        if ( (target != NULL ) && !IS_NPC( target ) )
        {
          if (  ( IS_WOLF( ch ) )
             && ( IS_VAMP( target ) || IS_WOLF( target ) )  )
          { 
            leave_corpse = TRUE;
            corpse->value[0] = 1;
          }
          if ( IS_SET(ch->pcdata->pflags, PFLAG_PKOK) )
            corpse->value[0] = 1;
          if ( ch->pcdata->clan > 0 ) 
          {
	    if  ( target->pcdata->clan != ch->pcdata->clan  &&
                ( politics_data.diplomacy[ch->pcdata->clan][target->pcdata->clan] < -450 ) )
            {
              corpse->value[2] = target->pcdata->clan;
            }
          }
          else
            corpse->value[2] = -1;
	  corpse->value[3] = number_range( 2, 5 );
        }
      }
    }  /* end of player only */
    
    
    if (  ( IS_SET( ch->act, PLR_KILLER ) || IS_SET( ch->act, PLR_THIEF ) )
       && (  ( target != NULL ) 
          && (  ( !IS_NPC( target ) )  
             || ( !str_cmp( rev_spec_lookup( target->spec_fun ), "spec_executioner" ) )  )   )    )

    {
      corpse->value[0] = 1;
      corpse->value[3] = number_range( 3, 6 );
    }
    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );
    OREF( obj_next, OBJ_NEXTCONTENT );
    for ( obj = ch->first_carry; obj != NULL; obj = obj_next )
    {
      obj_next = obj->next_in_carry_list;
      obj_from_char( obj );
      if (  ( obj == quest_object )
         && ( ch == quest_target )  )
      {
        obj->value[0] = UMAX( 1, obj->value[0] * quest_timer / 10 );
        obj->value[1] = UMAX( 1, obj->value[1] * quest_timer / 10 );
        obj->value[2] = UMAX( 1, obj->value[2] * quest_timer / 10 );
      }
       if( IS_SET(obj->extra_flags,ITEM_LODGED) )
        REMOVE_BIT(obj->extra_flags,ITEM_LODGED);

      if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
        extract_obj( obj );
      else
        obj_to_obj( obj, corpse );
    }
    OUREF( obj_next );
    
    if ( !IS_NPC( ch ) )
    {
      if (  ( IS_SET(ch->pcdata->pflags,PFLAG_PKOK )  )
         || ( target != NULL && ( target->pcdata->clan != ch->pcdata->clan )
              && ( politics_data.diplomacy[ch->pcdata->clan][target->pcdata->clan] < -450 ) ) 
         || (  ( ch->level > 30 ) 
            && ( IS_SET( ch->act, PLR_KILLER ) || IS_SET( ch->act, PLR_THIEF ) ) ) 
         || ( leave_corpse )   )
        obj_to_room( corpse, ch->in_room);
      else
        obj_to_room( corpse, ch->in_room  );
      {
        CORPSE_DATA * this_corpse;
        GET_FREE( this_corpse, corpse_free );
        this_corpse->next = NULL;
        this_corpse->prev = NULL;
        this_corpse->this_corpse = corpse;
        LINK( this_corpse, first_corpse, last_corpse, next, prev );
      }
      save_corpses( );  
      return;
    }
    else
/*
      if ( IS_SET( ch->act, ACT_INTELLIGENT ) )
      {
        obj_to_room( corpse, ch->in_room );
      }
      else
*/
      {
	  obj_to_room( corpse, ch->in_room );
      }
	return;
    }


/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    /* FIXME:  *ONLY* PKOK victims get a head loaded. */
    /* Make this give 'cry' message, don't load anything */
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;

    

    if ( IS_NPC(ch) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    if (  ( was_in_room  == NULL )
       || ( was_in_room->vnum == 0 )   )
    return;

    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *victim, char *argument )
{
    CHAR_DATA *check;
    char arg[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    stop_fighting( victim, TRUE );
    death_cry( victim );
    mprog_death_trigger( victim );
    if ( victim == quest_target )
     quest_target = NULL;
    if ( victim == quest_mob )
    {
      sprintf( buf, "Oh well, I guess the quest is over, since I am about to {RDIE!!!!!{x" );
      do_crusade( victim, buf );
      
      quest_mob = NULL;
    }
    
    if ( ( IS_NPC( victim ) || !IS_VAMP(victim) )
       && ( victim->is_free == FALSE )
       && ( victim->in_room != NULL )  )
    make_corpse( victim, arg );
    
    for ( check = first_char; check != NULL; check = check->next )
    {
      if ( check->hunting == victim )
        end_hunt(check);
/*        unhunt(check);*/
    }
        

    if ( IS_NPC(victim) && !IS_SET(victim->act,ACT_INTELLIGENT))
    {
	victim->pIndexData->killed++;
	kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
	extract_char( victim, TRUE );
	return;
    
    }

    extract_char( victim, FALSE );
    while ( victim->first_affect )
	affect_remove( victim, victim->first_affect );
    victim->affected_by = 0;
    victim->armor       = 0;
    victim->position    = POS_RESTING;
    victim->hit         = 1;
    victim->mana        = 1;
    victim->move        = 30;
    save_char_obj( victim );
    if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_INTELLIGENT ) )
    {

      OBJ_DATA * my_corpse = NULL;

      for ( my_corpse = victim->in_room->first_content;
            my_corpse;
            my_corpse = my_corpse->next_in_room )
      {
        if (  ( my_corpse->item_type == ITEM_CORPSE_NPC )
     /*      && ( strstr( victim->short_descr, my_corpse->description ) != NULL ) */ )
        {
          break;
        }
      }
      if ( my_corpse == NULL )
      {
        char monbuf[MSL];
        sprintf( monbuf, "%s is looking for their corpse in room %d, but it's not there",
                  victim->name, victim->in_room->vnum );
        monitor_chan( monbuf, MONITOR_MOB );
      }
      else
      {
        OBJ_DATA * obj;
        victim->is_quitting = FALSE;
        do_stand( victim, "" );
        for ( obj = my_corpse->first_content;
              obj;
              obj = obj->next_content )
        {
          obj_from_obj( obj );
          obj_to_char( obj, victim );
        }
        do_get( victim, "all corpse" );
        do_sacrifice( victim, "corpse" );
        do_wear( victim, "all" );
      }
    }
    return;
}
int get_exp( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int level_range;
	int base_exp;
	int xp;

	level_range = victim->level - ch->level;
    switch (level_range) {
    default:
	base_exp = 0;
	break;
    case -9:
	base_exp = 1;
	break;
    case -8:
	base_exp = 2;
	break;
    case -7:
	base_exp = 5;
	break;
    case -6:
	base_exp = 9;
	break;
    case -5:
	base_exp = 11;
	break;
    case -4:
	base_exp = 22;
	break;
    case -3:
	base_exp = 33;
	break;
    case -2:
	base_exp = 50;
	break;
    case -1:
	base_exp = 66;
	break;
    case 0:
	base_exp = 83;
	break;
    case 1:
	base_exp = 99;
	break;
    case 2:
	base_exp = 121;
	break;
    case 3:
	base_exp = 143;
	break;
    case 4:
	base_exp = 165;
	break;
    }

    if (level_range > 4)
	base_exp = 160 + 20 * (level_range - 4);

	xp = base_exp;

    if (ch->level < 6)
	xp = 10 * xp / (ch->level + 4);

    if (ch->level > 50)
	xp = 15 * xp / (ch->level - 25);

	xp = number_range(xp * 3 / 4, xp * 5 / 4);

	return xp;
}

void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
	char buf[MSL];
	int exp = 0;
	int members = 0;
	CHAR_DATA *ppl;

	/* For ever person in the group, add 1 to members.
	 */
	
	for (ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
	{
		if (is_same_group(ppl, ch) )
			members++;
	}

	for (ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
	{
		if (is_same_group(ppl, ch) )
		{
			
	       if ( ppl->leader != NULL && ppl->level - ppl->leader->level >= 20) {
	          send_to_char("You are too high level for this group.\n\r", ppl);
	          continue;
		   }

	       if (ppl->leader != NULL && ppl->level - ppl->leader->level <= -20) {
	          send_to_char("You are too low for this group.\n\r", ppl);
	          continue;
		   }
            
		 exp = get_exp(ppl, victim);

			 if ( members == 2 )
				 exp *= 2;
			 else if ( members == 3 )
				 exp *= 1.7;
			 else if ( members == 4 )
				 exp *= 1.4;
			 else if ( members == 5 )
				 exp -= 50;
			 else if ( members == 6 )
				 exp -= 75;

		if ( exp > 350 )
			exp = 350;
		
		if ( exp < 0 || IS_IMMORTAL(ppl) || (ppl->level >= 50 && IS_NPC(victim) ) )
			exp = 0;
	    gain_exp( ppl, exp );
		sprintf( buf, "You Receive %d Experience Points.\n\r", exp );
        send_to_char( buf, ppl );

		}

	}

}

struct dam_table_str {
     int          min_dam;
     const char * col;
     const char * vs;
     const char * vp;
     const char * str;
     };
struct dam_table_str dam_table[] = 
     {
	{  400, "{x", "{Rdoes UNSPEAKABLE things to{x",        "{Rdoes UNSPEAKABLE things to{x", ""},
	{  350, "{x", "{R<<< ERADICATES >>>{x",        "{R<<< ERADICATES >>>{x",           ""},
	{  300, "{x", "{R>>> ANNIHILATES <<<{x",        "{R>>> ANNIHILATES <<<{x",           ""},
	{  250, "{x", "{R=== OBLITERATES ==={x",        "{R=== OBLITERATES ==={x",           ""},
	{  225, "{x", "{r*** DEVASTATES ***{x",        "{r*** DEVASTATES ***{x",           ""},
	  {  200, "{x", "{r*** DEMOLISHES ***{x",      "{r*** DEMOLISHES ***{x",           ""},
	  {  175, "{x", "{rMANGLES{x",        "{rMANGLES{x",           ""},
      {  150, "{x", "{rMASSACRES{x",        "{rMASSACRES{x",           ""},
      {  125, "{x", "{rDISMEMBERS{x",           "{rDISMEMBERS{x",              ""},
      {  100, "{x", "{rDISEMBOWELS{x",            "{rDISEMBOWELS{x",              ""},
      {   75, "{x", "{rMUTILATES{x",            "{rMUTILATES{x",               ""},
      {   50, "{x", "{ymaims{x",         "{ymaims{x",            ""},
      {   44, "{x", "{ydevastates{x",             "{ydevastates{x",                ""},
      {   38, "{x", "{ydecimates{x",             "{ydecimates{x",                ""},
      {   30, "{x", "{ymauls{x",         "{ymauls{x",               ""},
      {   24, "{x", "{ywounds{x",            "{ywounds{x",               ""},
      {   18, "{x", "{ginjures{x",             "{ginjures{x",                ""},
      {   12, "{x", "{ghits{x",          "{ghits{x",            ""},
      {    8, "{x", "{ggrazes{x",             "{ggrazes{x",                ""},
      {    4, "{x","{gscratchs{x",           "{gscratchs{x",              ""},
      {    0, "{x","{Wmisses{x",      "{Wmisses{x",         ""}
     };
     
void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int sn )
{
 CHAR_DATA *ppl;
 AFFECT_DATA af;
 AFFECT_DATA *paf;

    static char * const attack_table[] =
    {
	"hit",
	"slice",  "stab",  "slash", "whip", "claw",
	"blast",  "pound", "crush", "grip", "bite",
	"pierce", "suction", "tail whip"
    };

    char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH], buf3[MAX_STRING_LENGTH];
    const char *vs;	/* Singular */
    const char *vp;     /* Plural   */
    const char *str;	/* Strength */
    const char *attack;
    const char *col;
    char punct;
    int dam_table_num, check_dt;
	char dbuf[MSL];
    bool dead = FALSE;
    buf1[0] = '\0';
    buf2[0] = '\0';
    buf3[0] = '\0';
	dbuf[0] = '\0';

    if ( dam >= victim->hit )
       dead = TRUE; 
            
    if ( !IS_NPC( ch ) && IS_WOLF( ch )
       && ( IS_SHIFTED( ch ) || IS_RAGED( ch ) ) )
    {
      dt = TYPE_HIT + 5;   /* claw attack */
    }
       sprintf(dbuf, "{m%d{x", dam);
    for (dam_table_num=0; dam_table[dam_table_num].min_dam > dam; dam_table_num++)
    ;
    
    col=dam_table[dam_table_num].col;
    vs=dam_table[dam_table_num].vs;
    vp=dam_table[dam_table_num].vp;
    str=dam_table[dam_table_num].str;
    
   
   
    
    punct   = (dam <= 64) ? '.' : '!';

    if ( dt == TYPE_MAGIC )
    {
         col="{x";
/*              if ( dam ==   0 ) { vs = "{Wmisses{x"; 		vp = "{Wmisses{x"; 	}
         else if ( dam <=  2 ) { col="{x"; vs = "{gscratchs{x"; 	vp = "{gscratchs{x";	}
         else if ( dam <=  5 ) { col="{x"; vs = "{ggrazes{x";	vp = "{ggrazes{x"; }
         else if ( dam <=  7 ) { col="{x"; vs = "{ginjures{x{x";	vp = "{ginjures{x"; }
         else if ( dam <=  10 ) { col="{x"; vs = "{ywounds{x";	vp = "{ywounds{x"; }
         else if ( dam <=  20 ) { col="{x"; vs = "{ymaims{x";	vp = "{ymaims{x"; }
         else if ( dam <=  28 ) { col="{x"; vs = "{ymauls{x";	vp = "{ymauls{x"; }
         else if ( dam <=  50 ) { col="{x"; vs = "{ydevestates{x";	vp = "{ydevestates{x"; }
         else if ( dam <=  100 ) { col="{x"; vs = "{rDEMOLISHES{x";	vp = "{rDEMOLISHES{x"; }
         else if ( dam <=  200 ) { col="{x"; vs = "{rMUTILATES{x";	vp = "{rMUTILATES{x"; }
         else if ( dam <=  250 ) { col ="{x"; vs = "{r***DEVESTATES***{x";	vp = "{r***DEVESTATES***{x"; }
         else if ( dam <=  300 ) { col="{x"; vs = "{R>>>ANIHILATES<<<{x";	vp = "{R>>>ANIHILATES<<<{x";}
         else if ( dam <=  350 ) { col="{x"; vs = "{R<<<ERADICATES>>>{x";		vp = "{R<<<ERADICATES>>>{x";	}
         else if ( dam <=  400 ) { col ="{x"; vs = "{Rdoes UNSPEAKABLE things to{x";	vp = "{Rdoes UNSPEAKABLE things to{x"; }
*/
     if ( sysdata.shownumbers == TRUE )
     {
       sprintf( buf1, "%s$n %s%s $N%c{w {B({d%d{B){x",col, col, vp, punct, dam );
       sprintf( buf2, "%sYouaaaaa %s%s $N%c{w {B({d%d{B){x",col, col,vs, punct, dam );
       sprintf( buf3, "%s$n %s%s you%c{w {B({d%d{B){x",col, col, vp, punct, dam );
     }
     else
     {
       attack = skill_table[sn].name;
     for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      {
       if ( ch == ppl )
       continue;

        if ( victim == ppl )
        continue;
      if ( !IS_NPC( ppl ) && IS_SET( ppl->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
         {
		  sprintf( buf1, "Someone's {W%s{x %s someone! %s%s%s{x\n\r", attack, vp,
			  dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
         } else {
       sprintf( buf1, "%s's {W%s{x %s %s. %s%s%s{x\n\r", PERSON(ppl, ch), attack, vp, PERSON(ppl, victim),
		   	  dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
}
       send_to_char( buf1, ppl);
       }
    if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
    {
      sprintf( buf2, "Your {W%s{x %s someone! %s%s%s{x", attack, vs,
		  	  dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
    } else {
       sprintf( buf2, "Your {W%s{x %s %s. %s%s%s{x", attack, vs, PERSON(ch, victim),
		   	  dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
            }
    if ( !IS_NPC( victim ) && IS_SET( victim->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
    {
     sprintf( buf3, "Someone's {W%s{x %s you! %s%s%s{x", attack, vp,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
    } else {
       sprintf( buf3, "%s's {W%s{x %s you! %s%s%s{x",PERSON(victim, ch), attack, vp,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
            }
   if ( !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) )
      {
        if ( !IS_NPC(ch) && !IS_NPC(victim) )
          {
            if (!IS_AFFECTED(ch, AFF_NOQUIT) )
            {
    af.type      = skill_lookup( "noquit" );
    af.duration  = 4;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_NOQUIT;
    affect_to_char( ch, &af );
            } else
            {
                for ( paf = ch->first_affect; paf != NULL; paf = paf->next )
                {
                    if ( paf->type != skill_lookup( "noquit" ) )
                        continue;
                    paf->duration = 4;
                    break;
                }
            }
          }
      }

   if ( !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) )
      {
        if ( !IS_NPC(ch) && !IS_NPC(victim)  )
          {
            if (!IS_AFFECTED(victim, AFF_NOQUIT) )
            {
    af.type      = skill_lookup( "noquit" );
    af.duration  = 4;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_NOQUIT;
    affect_to_char( victim, &af );
                } else
            {
                for ( paf = victim->first_affect; paf != NULL; paf = paf->next )
                {
                    if ( paf->type != skill_lookup( "noquit" ) )
                        continue;
                    paf->duration = 4;
                    break;
                }
            }
          }
      }

       act( buf2, ch, NULL, victim, TO_CHAR );
       act( buf3, ch, NULL, victim, TO_VICT );

	   if ( !IS_NPC(victim) && victim->pcdata->strategy == 0)
		  do_flee(victim, "");
       return;
     }
   }

    /* Use combat skills, etc to make unarmed combat more fun :) */

    if ( dt == TYPE_MARTIAL )
    {
         
 	   
          col="{m";
         
        switch ( number_range( 0, 13 ) ) 
          {
            case  0: vs = "head punch";	  vp = "head punches"; 	break;
            case  1: vs = "high kick";	  vp = "high kicks";	break;
            case  2: vs = "vital kick";   vp = "vital kick";	break;
            case  3: vs = "head bash";	  vp = "head bashes";	break;
            case  4: vs = "side kick";	  vp = "side kicks";	break;
            case  5: vs = "elbow";	  vp = "elbows";	break;
            case  6: vs = "body punch";	  vp = "body punches";	break;
            case  7: vs = "low kick";	  vp = "low kicks";	break;
            case  8: vs = "graze";	  vp = "grazes";	break;
            case  9: vs = "knee smash";	  vp = "knee smashes";	break;
            case 10: vs = "kidney punch"; vp = "kidney punches";break;
            case 11: vs = "arm twist";	  vp = "arm twists";	break;
            case 12: vs = "uppercut";	  vp = "uppercuts";	break;
            case 13: vs = "rabbit punch"; vp = "rabbit punches";break;
		default : vs = "foot sweep";    vp = "foot sweeps"; break;

          }
    }     
    else if ( dt == TYPE_HIT )
    {
         col="{x";
/*              if ( dam ==   0 ) { vs = "{Wmisses{x"; 		vp = "{Wmisses{x"; 	}
         else if ( dam <=  2 ) { col="{x"; vs = "{gscratchs{x"; 	vp = "{gscratchs{x";	}
         else if ( dam <=  5 ) { col="{x"; vs = "{ggrazes{x";	vp = "{ggrazes{x"; }
         else if ( dam <=  7 ) { col="{x"; vs = "{ginjures{x{x";	vp = "{ginjures{x"; }
         else if ( dam <=  10 ) { col="{x"; vs = "{ywounds{x";	vp = "{ywounds{x"; }
         else if ( dam <=  20 ) { col="{x"; vs = "{ymaims{x";	vp = "{ymaims{x"; }
         else if ( dam <=  28 ) { col="{x"; vs = "{ymauls{x";	vp = "{ymauls{x"; }
         else if ( dam <=  50 ) { col="{x"; vs = "{ydevestates{x";	vp = "{ydevestates{x"; }
         else if ( dam <=  100 ) { col="{x"; vs = "{rDEMOLISHES{x";	vp = "{rDEMOLISHES{x"; }
         else if ( dam <=  200 ) { col="{x"; vs = "{rMUTILATES{x";	vp = "{rMUTILATES{x"; }
         else if ( dam <=  250 ) { col ="{x"; vs = "{r***DEVESTATES***{x";	vp = "{r***DEVESTATES***{x"; }
         else if ( dam <=  300 ) { col="{x"; vs = "{R>>>ANIHILATES<<<{x";	vp = "{R>>>ANIHILATES<<<{x";}
         else if ( dam <=  350 ) { col="{x"; vs = "{R<<<ERADICATES>>>{x";		vp = "{R<<<ERADICATES>>>{x";	}
         else if ( dam <=  400 ) { col ="{x"; vs = "{Rdoes UNSPEAKABLE things to{x";	vp = "{Rdoes UNSPEAKABLE things to{x"; }
*/
     if ( sysdata.shownumbers == TRUE )
     {
       sprintf( buf1, "%s$n %s%s $N%c{w {B({d%d{B){x",col, col, vp, punct, dam );
       sprintf( buf2, "%sYou %s%s $N%c{w {B({d%d{B){x",col, col,vs, punct, dam );
       sprintf( buf3, "%s$n %s%s you%c{w {B({d%d{B){x",col, col, vp, punct, dam );
     }
     else
     { 
     for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      {
       if ( ch == ppl )
       continue;

        if ( victim == ppl )
        continue;
      if ( !IS_NPC( ppl ) && IS_SET( ppl->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
         {      
       sprintf( buf1, "Someone's punch %s Someone! %s%s%s{x\n\r", vp,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
} else {
       sprintf( buf1, "%s's punch %s %s. %s%s%s{x\n\r",PERSON(ppl, ch), vp,PERSON(ppl, victim),
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
}
       send_to_char( buf1, ppl);
}

    if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
    {
       sprintf( buf2, "Your punch %s someone! %s%s%s{x", vs,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "]");
     } else {
       sprintf( buf2, "Your punch %s %s. %s%s%s{x", vs, PERSON(ch, victim),
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
}
    if ( !IS_NPC( victim ) && IS_SET( victim->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
    {
       sprintf( buf3, "Someone's punch %s you! %s%s%s{x", vp,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
} else {
sprintf( buf3, "%s's punch %s you! %s%s%s{x",PERSON(victim, ch), vp,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
}
/*
       act( buf1, ch, NULL, victim, TO_NOTVICT );       
*/
   if ( !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) )
      {
        if ( !IS_NPC(ch) && !IS_NPC(victim) )
          {
            if (!IS_AFFECTED(ch, AFF_NOQUIT) )
            {
    af.type      = skill_lookup( "noquit" );
    af.duration  = 4;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_NOQUIT;
    affect_to_char( ch, &af );
            } else
            {
                for ( paf = ch->first_affect; paf != NULL; paf = paf->next )
                {
                    if ( paf->type != skill_lookup( "noquit" ) )
                        continue;
                    paf->duration = 4;
                    break;
                }
            }
          }
      }

   if ( !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) )
      {
        if ( !IS_NPC(ch) && !IS_NPC(victim)  )
          {
            if (!IS_AFFECTED(victim, AFF_NOQUIT) )
            {
    af.type      = skill_lookup( "noquit" );
    af.duration  = 4;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_NOQUIT;
    affect_to_char( victim, &af );
                } else
            {
                for ( paf = victim->first_affect; paf != NULL; paf = paf->next )
                {
                    if ( paf->type != skill_lookup( "noquit" ) )
                        continue;
                    paf->duration = 4;
                    break;
                }
            }
          }
      }

       act( buf2, ch, NULL, victim, TO_CHAR );
       act( buf3, ch, NULL, victim, TO_VICT );
	   if ( !IS_NPC(victim) && victim->pcdata->strategy == 0)
		  do_flee(victim, "");
       return;

     }

    } else
    {
      if ( dt >= 0 && dt < MAX_SKILL )
  	   attack      = skill_table[dt].noun_damage;
      else if ( dt >= TYPE_HIT
              && dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) && (dt != TYPE_MAGIC) )
      {
        check_dt = UMAX( 0, ( dt - TYPE_HIT ) );
        check_dt = UMIN( check_dt, 13 );
        attack      = attack_table[check_dt];
      }
      else
      {
	      bug( "Dam_message: bad dt %d.", dt );
	      dt  = TYPE_HIT;
	      attack  = attack_table[0];
      }
	
      if ( sysdata.shownumbers == TRUE )
      {
        if ( dt != TYPE_MARTIAL )
        {
          sprintf( buf1, "%s$n %s%s $N%s%s $s %s%c{w {B({d%d{B){x", col, col, vp, col, str, attack, punct, dam );
          sprintf( buf2, "%sYou %s%s $N%s%s your %s%c{w {B({d%d{B){x", col,col, vs,col, str, attack, punct, dam );
          if ( *str == '\'' )
           sprintf( buf3, "%s$n %s%s your%s%s $s %s%c{w {B({d%d{B){x",col,col, vp,col, str+2, attack, punct, dam );
          else
           sprintf( buf3, "%s$n %s%s you%s%s $s %s%c{w {B({d%d{B){x",col, col, vp,col, str, attack, punct, dam );

        }
        else
        {
          sprintf( buf1, "$n pulls a karate move on $N!" );  
          sprintf( buf2, "You pull a karate move on $N!" );
          sprintf( buf3, "$n pulls a karate move on you!" );
        }
      }
      else
      {

        if ( dt != TYPE_MARTIAL )
        {
     for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      {
       if ( ch == ppl )
       continue;

        if ( victim == ppl )
        continue;
      if ( !IS_NPC( ppl ) && IS_SET( ppl->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
         {      
       sprintf( buf1, "Someone's %s %s Someone! %s%s%s{x\n\r", attack, vp,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
         } else {
       sprintf( buf1, "%s's %s %s %s. %s%s%s{x\n\r", PERSON(ppl, ch), attack, vp, PERSON(ppl, victim),
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
}
       send_to_char( buf1, ppl);
}
    if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
    {
      sprintf( buf2, "Your %s %s someone! %s%s%s{x", attack, vs,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
    } else {
       sprintf( buf2, "Your %s %s %s. %s%s%s{x", attack, vs, PERSON(ch, victim),
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
            }
    if ( !IS_NPC( victim ) && IS_SET( victim->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
    {
     sprintf( buf3, "Someone's %s %s you! %s%s%s{x", attack, vp,
		      dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
    } else {
       sprintf( buf3, "%s's %s %s you! %s%s%s{x",PERSON(victim, ch), attack, vp,
	    	  dam <= 0 ? "" : "{d[{x",
			  dam <= 0 ? "" :  dbuf,
			  dam <= 0 ? "" : "{d]{x");
            }
   if ( !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) )
      {
        if ( !IS_NPC(ch) && !IS_NPC(victim) )
          {
            if (!IS_AFFECTED(ch, AFF_NOQUIT) )
            {
    af.type      = skill_lookup( "noquit" );
    af.duration  = 4;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_NOQUIT;
    affect_to_char( ch, &af );
            } else
            {
                for ( paf = ch->first_affect; paf != NULL; paf = paf->next )
                {
                    if ( paf->type != skill_lookup( "noquit" ) )
                        continue;
                    paf->duration = 4;
                    break;
                }
            }
          }
      }

   if ( !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) )
      {
        if ( !IS_NPC(ch) && !IS_NPC(victim)  )
          {
            if (!IS_AFFECTED(victim, AFF_NOQUIT) )
            {
    af.type      = skill_lookup( "noquit" );
    af.duration  = 4;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_NOQUIT;
    affect_to_char( victim, &af );
                } else
            {
                for ( paf = victim->first_affect; paf != NULL; paf = paf->next )
                {
                    if ( paf->type != skill_lookup( "noquit" ) )
                        continue;
                    paf->duration = 4;
                    break;
                }
            }
          }
      }

       act( buf2, ch, NULL, victim, TO_CHAR );
       act( buf3, ch, NULL, victim, TO_VICT );
	   if ( !IS_NPC(victim) && victim->pcdata->strategy == 0)
		  do_flee(victim, "");

       return;

        }
        else
        {
          sprintf( buf1, "$n pulls a karate move on $N!" );  
          sprintf( buf2, "You pull a karate move on $N!" );
          sprintf( buf3, "$n pulls a karate move on you!" );
        }

      }
    }
    act( buf1, ch, NULL, victim, TO_NOTVICT );
    if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
    {
      if ( dam < victim->max_hit/ 30 )
        act( "You glance $K", ch, NULL, victim, TO_CHAR );
      else if ( dam < victim->max_hit/ 20 )
        act( "You hit $K", ch, NULL, victim, TO_CHAR );
      else if ( dam < victim->max_hit / 10 )
        act( "You nail $K", ch, NULL, victim, TO_CHAR );
      else
        act( "You thwack $K", ch, NULL, victim, TO_CHAR );
    }
    else
      act( buf2, ch, NULL, victim, TO_CHAR );
    if ( !IS_NPC( victim ) && IS_SET( victim->pcdata->pflags, PFLAG_BLIND_PLAYER ) )
    {
      if ( dam < victim->max_hit/ 30 )
        act( "$k glances you", ch, NULL, victim, TO_VICT );
      else if ( dam < victim->max_hit/ 20 )
        act( "$k hits you", ch, NULL, victim, TO_VICT );
      else if ( dam < victim->max_hit / 10 )
        act( "$k nails you", ch, NULL, victim, TO_VICT );
      else
        act( "$k thwacks you", ch, NULL, victim, TO_VICT );
    }
    else
      act( buf3, ch, NULL, victim, TO_VICT );

    if ( dead )
    {
       int foo;
       
       foo = sizeof(attack_table)/sizeof(attack_table[0]);
       death_message( ch, victim, ( dt - TYPE_HIT ), foo );
    }
    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim , OBJ_DATA * obj )
{          
    int chance;

    set_fighting( ch, victim, TRUE );
    if ( obj == NULL )
    {
      if (  ( ( obj = get_eq_char( victim, WEAR_HOLD_HAND_L ) ) == NULL )
         || ( obj->item_type != ITEM_WEAPON )  )
         if (  ( ( obj = get_eq_char( victim, WEAR_HOLD_HAND_R ) ) == NULL )
            || ( obj->item_type != ITEM_WEAPON )  )  
      {
        send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
        return;
      }
    }
    if ( IS_SET( obj->extra_flags, ITEM_NODISARM )  )
      return;

    if ( !IS_NPC( victim ) && IS_WOLF( victim )
       && ( IS_SHIFTED( victim ) || IS_RAGED( victim )   )  )
       return;

    
   /*
    * Check to see if victim is warrior, and if they deflect the disarm
    * In part, this idea comes from HiddenWorlds,
    * but it's also pretty common sense really ;)
    * Stephen
    */
    
    chance = IS_NPC( victim ) 
       ? IS_SET( victim->skills, MOB_NODISARM ) ? 90 : 0
       : get_skill(victim, gsn_nodisarm);
    
    if ( number_percent() < chance )
    {
	act( "You dodge $n's disarm attempt!", ch, NULL, victim, TO_VICT    );
	act( "You fail to disarm $N!",  ch, NULL, victim, TO_CHAR    );
	act( "$N dodges $n's disarm attempt!",  ch, NULL, victim, TO_NOTVICT );
	return;
    }
    
    act( "$n DISARMS you!", ch, NULL, victim, TO_VICT    );
    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n DISARMS $N!",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
  /*  if ( IS_NPC(victim) )
	obj_to_char( obj, victim );
      else */
      
	obj_to_room( obj, victim->in_room );

    return;
}



/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->wait == 0 )
    {
	int chance;
	
	chance = IS_NPC( victim ) 
	   ? IS_SET( victim->skills, MOB_NOTRIP ) ? 75 : 0
	   : get_skill(victim, gsn_notrip);
	   
	/* Check for no-trip */
        if ( number_percent() < chance )	
	{
	   act( "You sidestep $n's attempt to trip you!", ch, NULL, victim, TO_VICT );
	   act( "$N sidesteps your attempt to trip $M!", ch, NULL, victim, TO_CHAR );
	   act( "$N sidesteps $n's attempt to trip $m!", ch, NULL, victim, TO_NOTVICT );
	   return;
	}           

	act( "$n trips you and you go down!", ch, NULL, victim, TO_VICT    );
	act( "You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR    );
	act( "$n trips $N and $N goes down!", ch, NULL, victim, TO_NOTVICT );

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
    }

    return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && !( deathmatch) )
/*
    {
	if ( !IS_SET(victim->act, PLR_KILLER)
	&&   !IS_SET(victim->act, PLR_THIEF) )
	{
	    send_to_char( "You must MURDER a player.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	{
	    send_to_char( "You must MURDER a charmed creature.\n\r", ch );
	    return;
	}
    }
*/
    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

void do_target( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    if (  ( !IS_NPC( ch ) )
       && ( get_skill(ch, gsn_target) < 65 )  )
    {
      send_to_char( "You are not trained enough in this skill!!\n\r", ch );
      return;
    }
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Target whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && !( deathmatch) )
    {
	if ( !IS_SET(victim->act, PLR_KILLER)
	&&   !IS_SET(victim->act, PLR_THIEF) )
	{
	    send_to_char( "You must MURDER a player.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	{
	    send_to_char( "You must MURDER a charmed creature.\n\r", ch );
	    return;
	}
    }

    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	one_hit( ch, ch, TYPE_UNDEFINED, FALSE );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "{GTracking, tracking, tracking...{RGOT HIM!!!{x\n\r", ch );
      stop_fighting( ch, FALSE );
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
    return;
}



void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to MURDER, spell it out.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
    {
	send_to_char( "Not here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    sprintf( buf, "%s attacked by %s.\n\r", victim->name, ch->name );
    notify( buf, MAX_LEVEL-2 );
    
    if (   IS_NPC(ch) || IS_NPC(victim)
        || !IS_SET(ch->pcdata->pflags,PFLAG_PKOK)
        || !IS_SET(victim->pcdata->pflags,PFLAG_PKOK))
    {
     /* If not pkok people, do yell. */
     sprintf( buf, "Help! I'M BEING ATTACKED!!! ARRRGGGHHHHHH!" );
     do_yell( victim, buf );
    }
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
//    int cnt;
    int best;
    int level;
    int mult;
    int chance;
    int dam;

    
    /******************************************************************
     * Modified:  'damage' may now be called with sn = -1, in order to *
     * stop damage message being displayed.  Handle ALL the related    *
     * calculations here, including how many hits.  Check for critical *
     * hits too, as well as things like invis and sneak.               * 
      ******************************************************************/ 

    best = -1;
    level = 0;
    
    if ( !IS_NPC( ch ) && IS_WOLF( ch )
       && ( IS_SHIFTED( ch ) || IS_RAGED( ch ) )  )
    {
      send_to_char( "You cannot do that while in this form.\n\r", ch );
      return;
    }


    if ( get_skill(ch, gsn_backstab) <= 0 )
    {
	send_to_char(
	    "You better leave the assassin trade to thieves.\n\r", ch );
	return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (  ( victim == NULL )
       || ( victim->is_free != FALSE )  )
     return;
    if ( IS_NPC(victim) && IS_SET( victim->act, ACT_NO_BODY) )
    {
       act( "$N has no body to backstab!", ch, NULL, victim, TO_CHAR );
       return;
    }
    
    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( ( obj = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL
    ||   obj->value[0] != 2 )
    if ( ( obj = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) == NULL
    ||   obj->value[0] != 2 )
    {
      send_to_char( "You need to wield a dagger.\n\r", ch );
      return;
    }

    if ( victim->fighting != NULL )
    {
	send_to_char( "You can't backstab a fighting person.\n\r", ch );
	return;
    }

   chance = ( IS_NPC(ch) ? 50 : get_skill(ch, gsn_backstab)/2 );
    
    /* Handle Modifiers -- chance will affect thac, etc*/
    
    if ( !IS_AWAKE( victim ) )
       chance += 75;
    
    if (   IS_AFFECTED( victim, AFF_SNEAK ) 
        || item_has_apply( victim, ITEM_APPLY_SNEAK ) )
       chance -= 10;
    
    if (  IS_AFFECTED( ch, AFF_SNEAK ) 
       ||  item_has_apply( ch, ITEM_APPLY_SNEAK ) )
       chance += 20;
    
    if ( IS_AFFECTED( ch, AFF_INVISIBLE ) 
        || item_has_apply( ch, ITEM_APPLY_INV) )
       chance += ( IS_AFFECTED( victim, AFF_DETECT_INVIS ) ? -20 : 20 ); 
    
    if (   IS_AFFECTED( victim, AFF_INVISIBLE )
        || item_has_apply( victim, ITEM_APPLY_INV ) )
       chance -= 10;
       
    if ( get_psuedo_level( ch ) >= get_psuedo_level( victim ) )
       chance += 10;
    else
       chance -= 10;
       
    /* Work out multiplier */
    mult = 1 + ( level >= 20 ) + ( level >= 50 ) + ( level >= 90 ) + ( level >= 120 );
    
    /* Work out damage */
    dam = number_range( obj->value[1], obj->value[2] );
    dam += number_range( level/2, level*2 ) + GET_DAMROLL( ch ) / 2;
    dam *= mult;
          check_killer( ch, victim );    
    if ( chance < number_percent() )
    {
          /* Miss */
          act( "$n tries to backstab $N, but misses!", ch, NULL, victim, TO_NOTVICT );
          act( "You try to backstab $N, but miss!", ch, NULL, victim, TO_CHAR );
          act( "$N tries to backstab you, but misses!", victim, NULL, ch, TO_CHAR );
          damage( ch, victim, 0, -1 );
    }
    else
    {
      if ( sysdata.shownumbers == TRUE )
      {
          /* HIT! */
        char actbuf[MSL];
        sprintf( actbuf, "$n places $p into the back of $N!! {B({W%d{B){x", dam );
        act( actbuf, ch, obj, victim, TO_NOTVICT );
        sprintf( actbuf, "You place $p into the back of $N!! {B({W%d{B){x", dam );
        act( actbuf, ch, obj, victim, TO_CHAR );
        sprintf( actbuf, "$N places $p into your back!! {B({W%d{B){x", dam );
        act( actbuf, victim, obj, ch, TO_CHAR );
      }
      else
      {
        act( "$n places $p into the back of $N!!", ch, obj, victim, TO_NOTVICT );
        act( "You place $p into the back of $N!!", ch, obj, victim, TO_CHAR );
        act( "$N places $p into your back.  OUCH!", victim, obj, ch, TO_CHAR );
      }
      if ( !IS_NPC( ch ) && number_percent() == chance )

      {
        send_to_room( "You hear a large CRACK!\n\r", ch->in_room );
        dam *= 2;
      }

          damage( ch, victim, dam, -1 );
    }       
    
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    int attempt;
    int cost;           /* xp cost for a flee */

    if ( ( victim = ch->fighting ) == NULL )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    /* Check if mob will "allow" ch to flee... */    
    
    if ( IS_SET( victim->act, ACT_NO_FLEE ) && !IS_NPC( ch ) && IS_NPC(victim) )
    {
       send_to_char( "You attempt to flee from battle, but fail!\n\r", ch );
       sprintf( buf, "%s tells you 'No way will you escape ME!!'\n\r",
		victim->short_descr );
	send_to_char( buf, ch );
	return;    
    }
    if ( deathmatch )
    {
       if ( ch->hit  < 200 )
       {
         send_to_char( "{RYou are in too much pain!!{x\n\r", ch );
         return;
       }
       else
       {
         send_to_char( "{RYour enemy nails you one more time!{x\n\r", ch );
         ch->hit -= 150;
       }
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	    ||   pexit->to_room == NULL
	    ||   IS_SET(pexit->exit_info, EX_CLOSED)
	    || ( IS_NPC(ch)
		&& ( IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		    || ( IS_SET(ch->act, ACT_STAY_AREA)
			&& pexit->to_room->area != ch->in_room->area ) ) ) )
	    continue;

	move_char( ch, door );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	if ( !IS_NPC(ch) )
	{
        cost = number_range( ch->exp/15, ch->exp/10);
        if ( ch->adept_level > 0 )
          cost /= 1000;
        cost = UMIN( cost, ch->exp);
        sprintf( buf, "You flee from combat!  You lose %d exps.\n\r", cost );
	    send_to_char( buf, ch );
	    gain_exp( ch, ( 0 - cost ) );
	}
  if (  ( ch->fighting != NULL )
     && ( AI_MOB( ch->fighting ) ) )
  {
    ch->fighting->ngroup->state = GRP_STATE_HUNTING;
    ch->fighting->ngroup->leader->hunting = ch;
  }
	stop_fighting( ch, TRUE );
	/* 75% chance that mobs will hunt fleeing people. -- Alty */
	if ( IS_NPC(victim) &&
	    !IS_SET(victim->act, ACT_SENTINEL) && number_bits(2) > 0 )
	  set_hunt(victim, NULL, ch, NULL, HUNT_WORLD|HUNT_INFORM|HUNT_OPENDOOR, HUNT_MERC|HUNT_CR);
	return;
    }

    cost = get_psuedo_level( ch ) * 3;
    if ( ch->adept_level > 0 )
      cost = 0;
    cost = UMIN( cost, ch->exp);
    sprintf( buf, "You failed!  You lose %d exps.\n\r", cost );
    send_to_char( buf, ch );
    gain_exp( ch, ( 0 - cost ) );
    return;
}


void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;
    int best;
//    int cnt;

    best = -1;

    if ( get_skill(ch, gsn_rescue) <= 0 )
    {
	send_to_char( "You don't know how to rescue!!\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( !IS_NPC(ch) && number_percent( ) > get_skill(ch, gsn_rescue) )
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    set_fighting( ch, fch, TRUE );
    set_fighting( fch, ch, FALSE );
    return;
}


void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;
    int best;
//    int cnt;

    best = -1;

    if ( !IS_NPC( ch ) && IS_WOLF( ch )
       && ( IS_SHIFTED( ch ) || IS_RAGED( ch ) )  )
    {
      send_to_char( "You cannot do that while in this form.\n\r", ch );
      return;
    }

    
    if ( get_skill(ch, gsn_disarm) <= 0 )
    {
       send_to_char( "You don't know how to disarm!\n\r", ch );
       return;
    }
    
    if (  ( !IS_WEAPON(get_eq_char( ch, WEAR_HOLD_HAND_L ) ) )
       && ( !IS_WEAPON(get_eq_char( ch, WEAR_HOLD_HAND_R ) ) ) )
    {
      send_to_char( "You must wield a weapon to disarm.\n\r", ch );
      return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( !IS_NPC( victim ) && IS_WOLF( victim )
       && ( IS_SHIFTED( victim ) || IS_RAGED( victim )   )  )
    {
      send_to_char( "Try slicing off their claws instead!\n\r", ch );
      return;
    }

    if (  ( ( obj = get_eq_char( victim, WEAR_HOLD_HAND_L ) ) == NULL )
       || ( obj->item_type != ITEM_WEAPON )  )
       if (  ( ( obj = get_eq_char( victim, WEAR_HOLD_HAND_R ) ) == NULL )
          || ( obj->item_type != ITEM_WEAPON )  )  
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }
    if ( IS_SET( obj->extra_flags, ITEM_NODISARM )  )
      return;


    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
    percent = number_percent( ) + victim->level - ch->level;
    if ( IS_NPC(ch) || percent < get_skill(ch, gsn_disarm) * 2 / 3 )
	disarm( ch, victim, obj );
    else
	send_to_char( "You failed.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *ppl;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
/*
    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }
*/
    
    if ( IS_IMMORTAL(victim) && !(ch->level == L_GOD) )
    {
       send_to_char( "Not on other Immortal players!\n\r", ch );
       return;
    }
    
    if ( !IS_NPC(victim) && victim->level >= ch->level && ch != victim )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
	      
    act( "You suck the life energy out of $M!",  ch, NULL, victim, TO_CHAR    );
    act( "$n sucks out your life energy!", ch, NULL, victim, TO_VICT    );
    act( "$n sucks out $N's life energy!",  ch, NULL, victim, TO_NOTVICT );
    raw_kill( victim, "" );
    for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
    update_pos ( ppl );
    return;
}

void do_circle( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
//    int cnt;
    int best;
    int level;
    int mult;
    int chance;
    int dam;


    if ( !IS_NPC( ch ) && IS_WOLF( ch )
       && ( IS_SHIFTED( ch ) || IS_RAGED( ch ) )  )
    {
      send_to_char( "You cannot do that while in this form.\n\r", ch );
      return;
    }

    
    /******************************************************************
     * Modified:  'damage' may now be called with sn = -1, in order to *
     * stop damage message being displayed.  Handle ALL the related    *
     * calculations here, including how many hits.  Check for critical *
     * hits too, as well as things like invis and sneak.               * 
      ******************************************************************/ 

    best = -1;
    level = 0;
    
    
    if ( get_skill(ch, gsn_circle) <= 0 )
    {
	send_to_char(
	    "You better leave the assassin trade to thieves.\n\r", ch );
	return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (  ( victim == NULL )
       || ( victim->is_free != FALSE )  )
     return;

    if ( IS_NPC(victim) && IS_SET( victim->act, ACT_NO_BODY) )
    {
       act( "$N has no body to backstab!", ch, NULL, victim, TO_CHAR );
       return;
    }
    
    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( ( obj = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL
       ||   obj->value[0] != 2 )
      if ( ( obj = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) == NULL
         || obj->value[0] != 2 )
    {
      send_to_char( "You need to wield a piercing weapon.\n\r", ch );
      return;
    }

    if ( victim->fighting == NULL )
    {
      send_to_char( "You're victim must be fighting!\n\r", ch );
      return;
    }

   chance = ( IS_NPC(ch) ? 50 : get_skill(ch, gsn_circle)/4.5 );
    
    /* Handle Modifiers -- chance will affect thac, etc*/
    
    if ( !IS_AWAKE( victim ) )
       chance += 75;
    
    if (   IS_AFFECTED( victim, AFF_SNEAK ) 
        || item_has_apply( victim, ITEM_APPLY_SNEAK ) )
       chance -= 10;
    
    if (  IS_AFFECTED( ch, AFF_SNEAK ) 
       ||  item_has_apply( ch, ITEM_APPLY_SNEAK ) )
       chance += 10;
    
    if ( IS_AFFECTED( ch, AFF_INVISIBLE ) 
        || item_has_apply( ch, ITEM_APPLY_INV) )
       chance += ( IS_AFFECTED( victim, AFF_DETECT_INVIS ) ? -20 : 20 ); 
    
    if (   IS_AFFECTED( victim, AFF_INVISIBLE )
        || item_has_apply( victim, ITEM_APPLY_INV ) )
       chance -= 10;
       
    if ( get_psuedo_level( ch ) >= get_psuedo_level( victim ) )
       chance += 10;
    else
       chance -= 10;
       
    /* Work out multiplier */
    mult = 1 + ( level >= 20 ) + ( level >= 50 ) + ( level >= 90 ) + ( level >= 120 );
    
    /* Work out damage */
    dam = number_range( obj->value[1], obj->value[2] );
    dam += number_range( level/2, level*2 ) + GET_DAMROLL( ch ) / 2;
    dam *= mult;
    if ( victim != ch->fighting )
         check_killer( ch, victim );   
    if ( chance < number_percent() )
    {
          /* Miss */
          act( "$n tries to backstab $N, but misses!", ch, NULL, victim, TO_NOTVICT );
          act( "You try to backstab $N, but miss!", ch, NULL, victim, TO_CHAR );
          act( "$N tries to backstab you, but misses!", victim, NULL, ch, TO_CHAR );
          damage( ch, victim, 0, -1 );
    }
    else
    {
          /* HIT! */

      if ( sysdata.shownumbers == TRUE )
      {
          /* HIT! */
        char actbuf[MSL];
        sprintf( actbuf, "$n places $p into the back of $N!! {B({W%d{B){x", dam );
        act( actbuf, ch, obj, victim, TO_NOTVICT );
        sprintf( actbuf, "You place $p into the back of $N!! {B({W%d{B){x", dam );
        act( actbuf, ch, obj, victim, TO_CHAR );
        sprintf( actbuf, "$N places $p into your back!! {B({W%d{B){x", dam );
        act( actbuf, victim, obj, ch, TO_CHAR );
      }
      else
      {
        act( "$n places $p into the back of $N!!", ch, obj, victim, TO_NOTVICT );
        act( "You place $p into the back of $N!!", ch, obj, victim, TO_CHAR );
        act( "$N places $p into your back.  OUCH!", victim, obj, ch, TO_CHAR );
      }

           if ( !IS_NPC( ch ) 
              && number_percent() == chance )

           {
               send_to_room( "You hear a large CRACK!\n\r", ch->in_room );
               dam *= 2;
            }

          damage( ch, victim, dam, -1 );
    }       
       
    WAIT_STATE( ch, skill_table[gsn_circle].beats );
    return;
}



void do_trip( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int best;
/*    int cnt;  */

    best = -1;

    if ( !IS_NPC(ch)  )
    {

      if ( get_skill(ch, gsn_trip) > 75 )
        best = UMAX( 79, get_psuedo_level( ch ) );
    }
    else
       best = ch->level;
    
    if ( best == -1 )
    {
	send_to_char(
	    "You don't know of such a skill.\n\r", ch );
	return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Trip whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "That wouldn't even be funny.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( victim->fighting == NULL )
    {
	send_to_char( "You can't trip someone who isn't fighting.\n\r", ch );
	return;
    }

    check_killer( ch, victim );
    
    if (number_percent( ) < ( IS_NPC( ch ) ? 50 : 2 * get_skill(ch, gsn_trip) )  )
    {
       check_killer( ch, victim );
	trip( ch, victim );        
       WAIT_STATE( ch, skill_table[gsn_trip].beats );

    }

    return;
}

void do_dirt( CHAR_DATA *ch, char *argument )
{
    
    AFFECT_DATA af;
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int best;
 /*   int cnt;   */

    best = -1;

    if ( !IS_NPC(ch) )
    {

      if ( get_skill(ch, gsn_dirt) > 75 )
        best = UMAX( 79, get_psuedo_level( ch ) );  
    }
    else
       best = ch->level;
    
    if ( best == -1 )
    {
	send_to_char( "You don't know of such a skill.\n\r", ch );
	return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
	send_to_char( "Kick dirt at whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL && ch->fighting == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == NULL )
       victim = ch->fighting;

    if ( victim == ch )
    {
	send_to_char( "That wouldn't be too smart, would it??.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( victim->fighting == NULL )
    {
	send_to_char( "Try doing this to them, when fighting...\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_BLIND) )
	return;

    WAIT_STATE( ch, skill_table[gsn_dirt].beats );
    
    if ( number_percent() > (IS_NPC( ch )  ? 50 : get_skill(ch, gsn_dirt) ) )
    {
       act( "You kick dirt at $M but miss!",    ch, NULL, victim, TO_CHAR    );
       act( "$n kicks dirt at you but misses!", ch, NULL, victim, TO_VICT    );
       act( "$n kicks dirt at $N but misses!",  ch, NULL, victim, TO_NOTVICT );
       return;
    }
    else
    {
       act( "You kick dirt at $S eyes!",    ch, NULL, victim, TO_CHAR    );
       act( "$n kicks dirt in your eyes!",  ch, NULL, victim, TO_VICT    );
       act( "$n kicks dirt at $N's eyes!",  ch, NULL, victim, TO_NOTVICT );

       af.type      = skill_lookup( "blindness" );
       af.location  = APPLY_HITROLL;
       af.modifier  = -2;
       af.duration  = 1;
       af.bitvector = AFF_BLIND;
       affect_to_char( victim, &af );
    }   
       return;
}

void do_bash( CHAR_DATA *ch, char *argument )
{
    
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int best;    
/*    int cnt;  */

    best=-1;

    if ( !IS_NPC(ch) )
    {

      if ( get_skill(ch, gsn_bash) > 75 )
        best = UMAX( 79, get_psuedo_level( ch ) );
    }
    else
       best = ch->level;

    if ( best == -1 )
    {
	send_to_char(
	    "You don't know of such a skill.\n\r", ch );
	return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Bash whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Forget it!\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( victim->fighting == NULL )
    {
	send_to_char( "It might help if you were fighting.....\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_bash].beats );
    
    if (   ( IS_NPC(ch) && ( number_percent() > 75+ (ch->level)/2 )  )
        || (!IS_NPC(ch) && ( 2 * number_percent() > get_skill(ch, gsn_bash) ))
       )
    {
       act( "Your bash misses $M, and you fall!",        ch, NULL, victim, TO_CHAR    );
       act( "$n trys to bash you, misses, and falls!",   ch, NULL, victim, TO_VICT    );
       act( "$n trys to bash $N but misses, and falls!", ch, NULL, victim, TO_NOTVICT );
       return;
    }
    else
    {
       act( "Your bash sends $M flying!",         ch, NULL, victim, TO_CHAR    );
       act( "$n bashes you, sending you flying!", ch, NULL, victim, TO_VICT    );
       act( "$n's bash sends $N's flying!",       ch, NULL, victim, TO_NOTVICT );

       /* If victim very weak, set pos to stunned, stop fighting */
       if ( victim->hit < ( victim->max_hit / 50 ) + 1 )    
       {
	  act( "$N stays on the floor.",    ch, NULL, victim, TO_CHAR    );
	  act( "You are unable to get up.", ch, NULL, victim, TO_VICT    );
	  act( "$N stays on the floor.",    ch, NULL, victim, TO_NOTVICT );
	  stop_fighting( victim, TRUE );  /* MAG: might del this? -S- */
	  victim->position = POS_RESTING;
	  update_pos( victim );
       }
       else
       {
	  /* Do some damage instead... */
        damage(  ch, victim, ( best + 12) *2, -1 );
       }

    }   
       
       return;

}


void do_beserk( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int best;    
    int level;
    
    bool prime;
    
    prime = FALSE;
    best=-1;

    
    if ( IS_NPC(ch) )
       return;


    level = ch->level;


    if ( ch->fighting == NULL )
    {
       send_to_char( "You can only go beserk when fighting!\n\r", ch );
       return;
    }

    if ( is_affected( ch, gsn_beserk ) )
    {
       send_to_char( "You are already beserk!!\n\r", ch );
       return;
    }

    if ( get_skill(ch, gsn_beserk) == 0 )
    {
       send_to_char( "You don't know how to use this skill!\n\r", ch );
       return;
    }

    
    
    WAIT_STATE( ch, skill_table[gsn_beserk].beats );
 
    
    if ( number_percent() < ( get_skill(ch, gsn_beserk) / 2 ) )
    {
       /* Failure */
       act( "$n calls on the Dark Powers, but they don't answer!", ch, NULL, NULL, TO_ROOM );
       send_to_char( "You call on the Dark Powers.  They don't answer!", ch );
       return;
    }
    
    
    
    af.type = gsn_beserk;
    af.duration = ( best == 2 ) ? 4 : 2;
    af.location = APPLY_AC;
    af.modifier = ( best == 2 ) ? level : ( level * 2 );
    af.bitvector = 0;
    affect_to_char( ch, &af );
    
    af.location = APPLY_HITROLL;
    af.modifier = ( prime ) 
       ? ( 10 + (level>=20)+(level>=40)+(level>=60)+(level>=80) )
       : ( 5 + (level>=25)+(level>=50) );
    affect_to_char( ch, &af );
    
    af.location = APPLY_DAMROLL;
    af.modifier = ( prime )
       ? ( 12 + (level>=20)+(level>=40)+(level>=60)+(level>=80) )
       : ( 2 + (level>=25)+(level>=50) );
    affect_to_char( ch, &af );
    
    
    af.location = APPLY_SAVING_SPELL;
    af.modifier = ( prime )
       ? ( 10 - (level>=20)-(level>=40)-(level>=60)-(level>=80) )
       : ( 20 - (level>=25)-(level>=50) );
    affect_to_char( ch, &af );
    
    act( "$n calls on the Dark Powers, who answer!!!", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You call on the Dark Powers.  They answer!!!\n\r", ch );
    return;
 }
 
 void do_punch( CHAR_DATA *ch, char *argument )
 {
    
    CHAR_DATA *victim;
    int dam;
    bool prime;
    int chance;
    
    prime = FALSE;
    
    
    if ( !IS_NPC(ch) && get_skill(ch, gsn_punch) == 0 )
    {
       send_to_char( "You are not trained in this skill!\n\r", ch );
       return;
    }
    
    
    
    if ( ( ( victim = get_char_room( ch, argument ) ) == NULL ) 
        && ch->fighting == NULL ) 
    {
       send_to_char( "No such victim!\n\r", ch );
       return;
    }
    
    
    if ( victim == NULL )
       victim = ch->fighting;
              
    if ( IS_NPC(ch) )
       dam = number_range( ch->level/3, ch->level/2 );
    else
       dam = number_range( ch->level/2, ch->level * (prime ? 2 : 1 ) );
    
    chance = ( IS_NPC(ch) ? 50 : get_skill(ch, gsn_punch) / 2 );
    
    chance += ( ch->level - victim->level );
    
    
    WAIT_STATE( ch, skill_table[gsn_punch].beats );
 
       check_killer( ch, victim );    
    if ( number_percent() < chance )
    {

      if ( sysdata.shownumbers == TRUE )
      {
          /* HIT! */
        char actbuf[MSL];
        sprintf( actbuf, "$n punches $N!! {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_NOTVICT );
        sprintf( actbuf, "$N punches you really hard!! {B({W%d{B){x", dam );
        act( actbuf, victim, NULL, ch, TO_CHAR );
        sprintf( actbuf, "You punch $N!! {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_CHAR );
      }
      else
      {
       act( "$n punches $N really hard!", ch, NULL, victim, TO_NOTVICT );
       act( "$N punches you really hard!", victim, NULL, ch, TO_CHAR );
       act( "You punch $N really hard!", ch, NULL, victim, TO_CHAR );
      }
      damage( ch, victim, dam, -1 );
    }
    else
    {
    		/* MISS */
       act( "$n tries to punch $N, but misses!", ch, NULL, victim, TO_NOTVICT );
       act( "$N tries to punch you, but misses!", victim, NULL, ch, TO_CHAR );
       act( "You try to punch $N, but miss!", ch, NULL, victim, TO_CHAR );
       damage( ch, victim, 0, -1 );
    }
    return;
 }
 
 
 void do_headbutt( CHAR_DATA *ch, char *argument )
 {
    
    CHAR_DATA *victim;
    int dam;
    bool prime;
    int chance;
    
    prime = FALSE;
    
    
    if ( !IS_NPC(ch) && get_skill(ch, gsn_headbutt) == 0 )
    {
       send_to_char( "You are not trained in this skill!\n\r", ch );
       return;
    }
    
    
    
    if ( ( ( victim = get_char_room( ch, argument ) ) == NULL ) 
        && ch->fighting == NULL ) 
    {
       send_to_char( "No such victim!\n\r", ch );
       return;
    }
    
    
    if ( victim == NULL )
       victim = ch->fighting;
    
    if ( victim == ch )
    {
       send_to_char( "You can't reach!\n\r", ch );
       return;
    }
       

    if ( IS_NPC(ch) )
       dam = number_range( ch->level/3, ch->level/2 );
    else
       dam = number_range( ch->level/2, ch->level * (prime ? 2 : 1 ) );
    
    chance = ( IS_NPC(ch) ? 50 : get_skill(ch, gsn_headbutt) / 2 );
    
    chance += ( ch->level - victim->level );
    
    
    WAIT_STATE( ch, skill_table[gsn_headbutt].beats );
 
    
    if ( number_percent() < chance )
    {
    		/* HIT */
       check_killer( ch, victim);
      if ( sysdata.shownumbers == TRUE )
      {
          /* HIT! */
        char actbuf[MSL];
        sprintf( actbuf, "$n headbutts $N in the face! {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_NOTVICT );
        sprintf( actbuf, "$N headbutts you in the face! {B({W%d{B){x", dam );
        act( actbuf, victim, NULL, ch, TO_CHAR );
        sprintf( actbuf, "You headbutt $N in the face! {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_CHAR );
      }
      else
      {
       act( "$n headbutts $N in the face!", ch, NULL, victim, TO_NOTVICT );
       act( "$N headbutts you in the face!", victim, NULL, ch, TO_CHAR );
       act( "You headbutt $N in the face!", ch, NULL, victim, TO_CHAR );
      }

       damage( ch, victim, (dam*4)/5, -1 );
    }
    else
    {
    		/* MISS */
       check_killer( ch, victim );
       act( "$n tries to headbutt $N, but misses!", ch, NULL, victim, TO_NOTVICT );
       act( "$N tries to headbutt you, but misses!", victim, NULL, ch, TO_CHAR );
       act( "You try to headbutt $N, but miss!", ch, NULL, victim, TO_CHAR );
       damage( ch, victim, 0, -1 );
    }
    return;
 }
 
 
  void do_charge( CHAR_DATA *ch, char *argument )
 {
    
    CHAR_DATA *victim;
    int dam;
    bool prime;
    int chance;
    
    prime = FALSE;
    
    
    if ( !IS_NPC(ch) && get_skill(ch, gsn_charge) == 0 )
    {
       send_to_char( "You are not trained in this skill!\n\r", ch );
       return;
    }
    
    if (  ( IS_NPC( ch ) )
       && ( get_psuedo_level( ch ) < 80 )   )
      return;
  
    if ( ( ( victim = get_char_room( ch, argument ) ) == NULL ) 
        && ch->fighting == NULL ) 
    {
       send_to_char( "No such victim!\n\r", ch );
       return;
    }
    
    
    if ( victim == NULL )
       victim = ch->fighting;
    
    if ( victim == ch )
    {
       send_to_char( "You can't reach!\n\r", ch );
       return;
    }
       

    
       dam = number_range( get_psuedo_level( ch ) , get_psuedo_level( ch ) * 3 );
       
    if ( !IS_NPC( ch ) )
      chance = get_skill(ch, gsn_charge) / 2;
    else 
      chance = 50;
 
    chance += ( ( get_psuedo_level( ch ) - ( get_psuedo_level( victim ) - 30 ) ) /2 );
    
    
    WAIT_STATE( ch, skill_table[gsn_charge].beats );
 
        check_killer( ch, victim);
   
    if ( number_percent() < chance )
    {
    		/* HIT */

      if ( sysdata.shownumbers == TRUE )
      {
          /* HIT! */
        char actbuf[MSL];
        sprintf( actbuf, "{C$n {Ccharges $N{C, and knocks them over!{x {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_NOTVICT );
        sprintf( actbuf, "{C$N {Ccharges right into you!{x {B({W%d{B){x", dam );
        act( actbuf, victim, NULL, ch, TO_CHAR );
        sprintf( actbuf, "{CYou charge right into $N{C, and knock him over!{x {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_CHAR );
      }
      else
      {
       act( "{C$n {Ccharges $N{C, and knocks them over!{x", ch, NULL, victim, TO_NOTVICT );
       act( "{C$N {Ccharges right into you!{x", victim, NULL, ch, TO_CHAR );
       act( "{CYou charge right into $N{C, and knock him over!{x", ch, NULL, victim, TO_CHAR );
      }

       damage( ch, victim, dam, -1 );
    }
    else
    {
    		/* MISS */
       act( "$n{y charges $N{y, but runs right past!{y", ch, NULL, victim, TO_NOTVICT );
       act( "$N {ycharges you, but runs right past!{x", victim, NULL, ch, TO_CHAR );
       act( "{yYou try to charge $N{y, but run past him. DOH!!{x", ch, NULL, victim, TO_CHAR );
       damage( ch, victim, 0, -1 );
    }
    return;
 }
 
 

 void do_knee( CHAR_DATA *ch, char *argument )
 {
    
    CHAR_DATA *victim;
    int dam;
    bool prime;
    int chance;
    
    prime = FALSE;
    
    
    if ( !IS_NPC(ch) && get_skill(ch, gsn_knee) == 0 )
    {
       send_to_char( "You are not trained in this skill!\n\r", ch );
       return;
    }
    
    
    
    if ( ( ( victim = get_char_room( ch, argument ) ) == NULL ) 
        && ch->fighting == NULL ) 
    {
       send_to_char( "No such victim!\n\r", ch );
       return;
    }
    
    
    if ( victim == NULL )
       victim = ch->fighting;
    
    if ( IS_NPC(victim) && IS_SET( victim->act, ACT_NO_BODY ) )
    {
       act( "$N doesn't have a definable body to knee!", ch, NULL, victim, TO_CHAR );
       return;
    }
    
    
    

    if ( IS_NPC(ch) )
       dam = number_range( ch->level/3, ch->level/2 );
    else
       dam = number_range( ch->level/2, ch->level * (prime ? 2 : 1 ) );
    
    chance = ( IS_NPC(ch) ? 50 : get_skill(ch, gsn_knee) / 2 );
    
    chance += ( ch->level - victim->level );
    
    
    WAIT_STATE( ch, skill_table[gsn_knee].beats );
 
       check_killer( ch, victim );
    
    if ( number_percent() < chance )
    {
    		/* HIT */
      if ( sysdata.shownumbers == TRUE )
      {
          /* HIT! */
        char actbuf[MSL];
        sprintf( actbuf, "$n grabs $N and knees $M in the groin! {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_NOTVICT );
        sprintf( actbuf, "$N grabs you, and knees you in the groin! {B({W%d{B){x", dam );
        act( actbuf, victim, NULL, ch, TO_CHAR );
        sprintf( actbuf, "You grab $M and knee $M in the groin! {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_CHAR );
      }
      else
      {
       act( "$n grabs $N and knees $M in the groin!", ch, NULL, victim, TO_NOTVICT );
       act( "$N grabs you, and knees you in the groin!", victim, NULL, ch, TO_CHAR );
       act( "You grab $M and knee $M in the groin!", ch, NULL, victim, TO_CHAR );
      }

       damage( ch, victim, (dam*3)/5, -1 );
    }
    else
    {
    		/* MISS */
       act( "$n tries to grab $N, but $E twists free!", ch, NULL, victim, TO_NOTVICT );
       act( "$N tries to grab you, but you twist free!", victim, NULL, ch, TO_CHAR );
       act( "You try to grab $N, but $E twists free!", ch, NULL, victim, TO_CHAR );
       damage( ch, victim, 0, -1 );
    }
    return;
 }
 
         
void do_kick( CHAR_DATA *ch, char *argument )
 {
    
    CHAR_DATA *victim;
    int dam;
    bool prime;
    int chance;
    
    prime = FALSE;
    
    
    if ( !IS_NPC(ch) && get_skill(ch, gsn_kick) == 0 )
    {
       send_to_char( "You are not trained in this skill!\n\r", ch );
       return;
    }
    
    if ( ch->in_room->vnum == ROOM_VNUM_JAIL )
    {
      send_to_char( "Isn't jail punishment enough for you?", ch );
      return;
    }
    
    if ( ( ( victim = get_char_room( ch, argument ) ) == NULL ) 
        && ch->fighting == NULL ) 
    {
       send_to_char( "No such victim!\n\r", ch );
       return;
    }
    
    
    if ( victim == NULL )
       victim = ch->fighting;
       

    if ( IS_NPC(ch) )
       dam = number_range( ch->level/3, ch->level/2 );
    else
       dam = number_range( ch->level/2, ch->level * (prime ? 2 : 1 ) );
    
    chance = ( IS_NPC(ch) ? 50 : get_skill(ch, gsn_kick) / 2 );
    
    chance += ( ch->level - (victim->level+5) );
    
    
    WAIT_STATE( ch, skill_table[gsn_kick].beats );
 
       check_killer( ch, victim );    
    if ( number_percent() < chance )
    {
    		/* HIT */

      if ( sysdata.shownumbers == TRUE )
      {
          /* HIT! */
        char actbuf[MSL];
        sprintf( actbuf, "$n kicks $N really hard! {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_NOTVICT );
        sprintf( actbuf, "$N kicks you really hard! {B({W%d{B){x", dam );
        act( actbuf, victim, NULL, ch, TO_CHAR );
        sprintf( actbuf, "You kick $N really hard! {B({W%d{B){x", dam );
        act( actbuf, ch, NULL, victim, TO_CHAR );
      }
      else
      {
       act( "$n kicks $N really hard!", ch, NULL, victim, TO_NOTVICT );
       act( "$N kicks you really hard!", victim, NULL, ch, TO_CHAR );
       act( "You kick $N really hard!", ch, NULL, victim, TO_CHAR );
      }

       damage( ch, victim, dam, -1 );
    }
    else
    {
    		/* MISS */
       act( "$n tries to kick $N, but misses!", ch, NULL, victim, TO_NOTVICT );
       act( "$N tries to kick you, but misses!", victim, NULL, ch, TO_CHAR );
       act( "You try to kick $N, but miss!", ch, NULL, victim, TO_CHAR );
       damage( ch, victim, 0, -1 );
    }
    return;
 }
 
/* -S- Addition: Like damage() but called by objects, no message */
/* WARNING: No killer checks made, etc.  Only use if no keeper for obj */

void obj_damage( OBJ_DATA *obj, CHAR_DATA *victim, int dam )
{
  if (  ( victim->position == POS_DEAD )
     || ( victim->is_free == TRUE )   )
	return;

	/*
	 * Damage modifiers.
	 */
	if ( IS_AFFECTED(victim, AFF_SANCTUARY ) 
	     || item_has_apply( victim, ITEM_APPLY_SANC ) )
	    dam /= 2;

	if ( ( IS_AFFECTED(victim, AFF_PROTECT)|| item_has_apply( victim, ITEM_APPLY_PROT ) )
	       && IS_SET( obj->extra_flags, ITEM_EVIL ) )
	    dam -= dam / 4;

	if ( dam < 0 )
	    dam = 0;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    
    
    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "{RThat really did HURT!{x\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "{RYou sure are BLEEDING!{x\n\r", victim );
	break;
    }

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {

	if ( !IS_NPC(victim) )
	{

            sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		obj->short_descr,
		victim->in_room->vnum );
	    log_string( log_buf );

	notify( log_buf, 82 );

	/* As level gain is no longer automatic, a dead char loses
	 * 1/2 their gained exp.  -S- 
         * Fixed my bug here too, hehe!
	 */

	    if ( victim->exp > 0 )
            {
               gain_exp( victim, ( 0 - ( victim->exp / 2 ) ) );
               victim->exp = UMAX( victim->exp, 0 );
            }
		
	}

	raw_kill( victim, "" );

	if ( deathmatch && !IS_NPC( victim ) )
	   do_quit( victim, "" );
	

	return;
    }


    tail_chain( );
    return;
}


void death_message( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int max_dt )
{
   /* Used to display assorted death messages, based on dt 
    * max_dt == number of entries in attack table in dam_message,
    * so we know if kill was unarmed, armed, or through spells or skills.
    * Stephen
    */
    
   char  buf1[MAX_STRING_LENGTH];
   char  buf2[MAX_STRING_LENGTH];
   char  buf3[MAX_STRING_LENGTH];
   int   vnum;
   
 /*   For debugging purposes  
   sprintf( buf, "dt: %d max_dt: %d\n\r", dt, max_dt );
   notify( buf, 1 );  */
   
   vnum = 0;	/* Used to load up body parts.... eewwwwww! */
   
    if ( dt == 0 )
    {
        if ( !(IS_NPC(victim) && IS_SET( victim->act, ACT_NO_BODY ) ) ) 
            switch( number_range( 0, 9 ) )
            {
            case 0:
                sprintf( buf1, "$n grabs $N's neck, and twists until there is a loud SNAP!" );
                sprintf( buf2, "You grab $N's neck, and twist until there is a loud SNAP!"  );
                sprintf( buf3, "$n grabs your neck, and twists until there is a loud SNAP!" );
                break;
            case 1:
                sprintf( buf1, "$n slams $s fist into $N and crushes $S heart!"     );
                sprintf( buf2, "You slam your fist into $N and crush $S heart!"     );
                sprintf( buf3, "$n slams $s fist into you, and crushes your heart!" );
                break;
            case 2:
                sprintf( buf1, "$n rams $s hand into $N's ribcage, and rips out $S guts!"   );
                sprintf( buf2, "You ram your hand into $N's ribcage, and rip out $S guts!"  );
                sprintf( buf3, "$n rams $s hand into your ribcage, and rips out your guts!" );
                break;
            case 3:
                sprintf( buf1, "$n grabs $N, and rips $S head clean off!" );
                sprintf( buf2, "You grab $N, and rip his head clean off!" );
                sprintf( buf3, "$n grabs you, and rips your head clean off!" );
                break;
            case 4:
                sprintf( buf1, "$n reaches behind $N, and rips $S spine out of $S back!"      );
                sprintf( buf2, "You reach behind $N, and rip $S spine out of $S back!"        );
                sprintf( buf3, "$n reaches behind you, and rips your spine out of your back!" );
                break;
            case 5:
                sprintf( buf1, "$n rips $N's face open via $S eye sockets!"   );
                sprintf( buf2, "You rip $N's face open via $S eye sockets!"   );
                sprintf( buf3, "$n rips your face open via your eye sockets!" );
                break;
            case 6:
                sprintf( buf1, "$n rips off $N's head and vomits down $S throat!"   );
                sprintf( buf2, "You rip off $N's head and vomit down $S throat!"    );
                sprintf( buf3, "$n rips off your head and vomits down your throat!" );
                break;
            case 7:
                sprintf( buf1, "$N splurts blood as $n rips open $S chest with $s teeth!"   );
                sprintf( buf2, "$N splurts blood as you rip open $S chest with your teeth!" );
                sprintf( buf3, "You splurt blood as $n rips open your chest with $s teeth!" );
                break;
            case 8:
                sprintf( buf1, "$n wrenches $N's arms out from their sockets!" );
                sprintf( buf2, "You wrench $N's arms out from their sockets!"  );
                sprintf( buf3, "$n wrenches your arms out of thier sockets!"   );
                break;
            case 9:
                sprintf( buf1, "$n shatters $N's skull with a punch.  Brains leak out." );
                sprintf( buf2, "You shatter $N's skull with a punch.  Brains leak out." );
                sprintf( buf3, "$n shatters your skull with a punch.  Brains leak out." );
                break;
        }     
        else /* Unarmed, mob has no_body */
        {
            sprintf( buf1, "$n shatters $N's skull with a punch.  Brains leak out." );
            sprintf( buf2, "You shatter $N's skull with a punch.  Brains leak out." );
            sprintf( buf3, "$n shatters your skull with a punch.  Brains leak out." );
        }
    }
    
   
   if ( dt <= max_dt && dt > 0 )
   switch( dt )
   {
   case 1:	/* slice */
   case 3:	/* slash */
      switch( number_range( 0, 4 ) )
      {
      case 0:
         sprintf( buf1, "$n slices $N's head clean from $S neck"   );
         sprintf( buf2, "You slice $N's head clean from $S neck"   );
         sprintf( buf3, "$n slices your head clean from your neck" );
         break;
      case 1:
         sprintf( buf1, "$n slashes open $N's chest; $S entrails pour out!"   );
         sprintf( buf2, "You slash open $N's chest; $S entrails pour out!"    );
         sprintf( buf3, "$n slashes open your chest; your entrails pour out!" );
         break;
      case 2:
         sprintf( buf1, "$n slices $N's throat open.  Blood sprays out wildly!" );
         sprintf( buf2, "You slice $N's throat open.  Blood sprays out wildly!" );
         sprintf( buf3, "$n slices your throat open.  Blood sprays out wildly!" );
         break;
      case 3:
         sprintf( buf1, "$n slices $N's legs off, leaving two bloody stumps!" );
         sprintf( buf2, "You slice $N's legs off, leaving two bloody stumps!" );
         sprintf( buf3, "$n slices your legs off, leaving two bloody stumps!" );
         break;
      case 4:
         sprintf( buf1, "$n slashes $N's eyeballs out!" );
         sprintf( buf2, "You slash $N's eyeballs out!"  );
         sprintf( buf3, "$n slashes your eyeballs out!" );
         break;
      }
      break;
   case 2:	/* Stab */
   case 11:	/* Pierce */
      switch( number_range( 0, 4 ) )
      {
      case 0:
         sprintf( buf1, "$n rips a gaping hole down $N's back!" );
         sprintf( buf2, "You rip a gaping hole down $N's back!" );
         sprintf( buf3, "$n rips a gaping hole down your back!" );
         break;
      case 1:
         sprintf( buf1, "$n stabs into $N, and cuts $S heart out!"    );
         sprintf( buf2, "You stab into $N, and cut $S heart out!"     );
         sprintf( buf3, "$n stabs into you, and cuts your heart out!" );
         break;
      case 2:
         sprintf( buf1, "$n stabs into $N's back, and wrenches out $S spine!"   );
         sprintf( buf2, "You stab into $N's back, and wrench out $S spine!"     );
         sprintf( buf3, "$n stabs into your back, and wrenches out your spine!" );
         break;
      case 3:
         sprintf( buf1, "$n plunges $s weapon into $N's head!"   );
         sprintf( buf2, "You plunge your weapon into $N's head!" );
         sprintf( buf3, "$n plunges $s weapon into your head!"   );
         break;
      case 4:
         sprintf( buf1, "$n stabs into $N's chest, skewering $S heart!"   );
         sprintf( buf1, "$n stabs into $N's chest, skewering $S heart!"   );
         sprintf( buf2, "You stab into $N's chest, skewering $S heart!"   );
         sprintf( buf2, "You stab into $N's chest, skewering $S heart!"   );
         sprintf( buf3, "$n stabs into your chest, skewering your heart!" );
         break;
      }
      break;
   case 4:	/* Whip */
      switch( number_range( 0, 3 ) )
      {
      case 0: 
         sprintf( buf1, "$n whips out $N's eyes, spraying blood everywhere!" );
         sprintf( buf2, "You whip out $N's eyes, spraying blood everywhere!" );
         sprintf( buf3, "$n whips out your eyes, spraying blood everywhere!" );
         break;
      case 1:
         sprintf( buf1, "$n's whip catches $N's head, and rips it off!" );
         sprintf( buf2, "Your whip catches $N's head, and rips it off!" );
         sprintf( buf3, "$n's whip catches your head, and rips it off!" );
         break;
      case 2:
         sprintf( buf1, "$n's whip wraps around $N's arms, yanking them off!" );
         sprintf( buf2, "Your whip wraps around $N's arms, yanking them off!" );
         sprintf( buf3, "$n's whip wraps around your arms, yanking them off!" );
         break;
      case 3:
         sprintf( buf1, "$n's whip cuts open $N's main artery, spraying blood!" );
         sprintf( buf2, "Your whip cuts open $N's main artery, spraying blood!" );
         sprintf( buf3, "$n's whip cuts open your main artery, spraying blood!" );
         break;
      }
      break;
   case 5:	/* Claw */ 
      switch( number_range( 0, 4 ) )
      {
      case 0:
         sprintf( buf1, "$n claws out $N's heart!" );
         sprintf( buf2, "You claw out $N's heart!" );
         sprintf( buf3, "$n claws out your heart!" );
         break;
      case 1:
         sprintf( buf1, "$n's claw catches $N's back, slicing it open!" );
         sprintf( buf2, "Your claw catches $N's back, slicing it open!" );
         sprintf( buf3, "$n's claw catches your back, slicing it open!" );
         break;
      case 2:
         sprintf( buf1, "$N screams in agony, as $n's claw removes $S eyes!"   );
         sprintf( buf1, "$N screams in agony, as $n's claw removes $S eyes!"   );
         sprintf( buf2, "$N screams in agony, as your claw removes $S eyes!"   );
         sprintf( buf2, "$N screams in agony, as your claw removes $S eyes!"   );
         sprintf( buf3, "You scream in agony, as $n's claw removes your eyes!" );
         break;
      case 3:
         sprintf( buf1, "$n's claw ruptures $N's ribcage, shredding $S heart!"   );
         sprintf( buf2, "Your claw ruptures $N's ribcage, shredding $S heart!"   );
         sprintf( buf3, "$n's claw ruptures your ribcage, shredding your heart!" );
         break;
      case 4:
         sprintf( buf1, "$n's claw slashes $N's neck, decapitating $M!"  );
         sprintf( buf2, "Your claw slashes $N's neck, decapitating $M!"  );
         sprintf( buf3, "$n's claw slashes your neck, decapitating you!" );
         break;
      }      
      break;
   case 7:	/* Pound */
   case 8:	/* Crush */
      switch ( number_range( 0, 4 ) )
      {
      case 0:
         sprintf( buf1, "$n pounds $N's head; $S brains leak from $S ears!"     );
         sprintf( buf2, "You pound $N's head; $S brains leak from $S ears!"     );
         sprintf( buf3, "$n pounds your head; your brains leak from your ears!" );
         break;
      case 1:
         sprintf( buf1, "$n crushes $N's ribcage, and $S entrails slop out!"   );
         sprintf( buf2, "You crush $N's ribcage, and $S entrails slop out!"    );
         sprintf( buf3, "$n crushes your ribcage, and your entrails slop out!" );
         break;
      case 2:
         sprintf( buf1, "$n pounds $N's spine until you hear it CRACK!" );
         sprintf( buf2, "You pound $N's spine until you hear it CRACK!" );
         sprintf( buf3, "$n pounds your spine until you hear it CRACK!" );
         break;
      case 3:
         sprintf( buf1, "$n pounds $N's face, forcing $S nose into $S brain!"     );
         sprintf( buf2, "You pound $N's face, forcing $S nose into $S brain!"     );
         sprintf( buf3, "$n pounds your face, forcing your nose into your brain!" );
         break;
      case 4:
         sprintf( buf1, "$n crushes $N's head down into $S neck!"   );
         sprintf( buf2, "You crush $N's head down into $S neck!"    );
         sprintf( buf3, "$n crushes your head down into your neck!" );
         break;
      } 
      break;
   case 6:	/* Blast */
      switch( number_range( 0, 4 ) )
      {
      case 0:
         sprintf( buf1, "$n's blast totally obliterates $N's head!" );
         sprintf( buf2, "Your blast totally obliterates $N's head!" );
         sprintf( buf3, "$n's blast totally obliterates your head!" );
         break;
      case 1:
         sprintf( buf1, "$n's blast makes $N's head fly into the air!" );
         sprintf( buf2, "Your blast makes $N's head fly into the air!" );
         sprintf( buf3, "$n's blast makes your head fly into the air!" );
         break;
      case 2:
         sprintf( buf1, "$n blasts $N's stomach open - entrails plop out!" );
         sprintf( buf2, "You blast $N's stomach open - entrails plop out!" );
         sprintf( buf3, "$n blasts your stomach open - entrails plop out!" );
         break;
      case 3:
         sprintf( buf1, "$n's blast removes $N's legs from $S body!"   );
         sprintf( buf2, "Your blast removes $N's legs from $S body!"   );
         sprintf( buf3, "$n's blast removes your legs from your body!" );
         break;
      case 4:
         sprintf( buf1, "$n's blast splits $N's back, and $S spine falls out!"   );
         sprintf( buf2, "Your blast splits $N's back, and $S spine falls out!"   );
         sprintf( buf3, "$n's blast splits your back, and your spine falls out!" );
         break;
      }
      break;
   default:	/* grep, bite, suction */
      switch( number_range( 0, 3 ) )
      {
      case 0:
         sprintf( buf1, "$n pulls $N's heart clean from $S ribcage!"   );
         sprintf( buf2, "You pull $N's heart clean from $S ribcage!"   );
         sprintf( buf3, "$n pulls your heart clean from your ribcage!" );
         break;
      case 1:
         sprintf( buf1, "$n snags $N's spine, and rips it out!" );
         sprintf( buf2, "You snag $N's spine, and rip it out!"  );
         sprintf( buf3, "$n snags your spine, and rips it out!" );
         break;
      case 2:
         sprintf( buf1, "$N's stomach splits open, and $S entrails slip out!"   );
         sprintf( buf2, "$N's stomach splits open, and $S entrails slip out!"   );
         sprintf( buf3, "Your stomach splits open, and your entrails slip out!" );
         break;
      case 3:
         sprintf( buf1, "$n pulls $N's heart from $S chest!"   );
         sprintf( buf2, "You pull $N's heart from $S chest!"   );
         sprintf( buf3, "$n pulls your heart from your chest!" );
         break;
      }
      break;
   }
   act( buf1, ch, NULL, victim, TO_NOTVICT );
   act( buf2, ch, NULL, victim, TO_CHAR );
   act( buf3, ch, NULL, victim, TO_VICT );

    /* Load up object, if needed. */
    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name            = IS_NPC(ch) ? ch->short_descr : ch->name;
	obj             = create_object( get_obj_index( vnum ), 0 );
	obj->timer      = number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	obj_to_room( obj, ch->in_room );
    }



   return;
}	
   	
void do_assist( CHAR_DATA *ch, char *argument )
{
   /* Allow players to join in fight, by typing assist, 
    * or assist <name>.  Will only ever allow players to
    * assist a group member  -- Stephen
    */
    
   CHAR_DATA *assist = NULL;
   CHAR_DATA *ppl;
   char actbuf[MSL];
   
   if ( argument[0] != '\0' )	/* then check for assist <name> */
   {
      if ( ( assist = get_char_room( ch, argument ) ) == NULL )
      {
         send_to_char( "They don't seem to be here right now...\n\r", ch );
         return;
      }
      if ( assist == ch )
      {
         send_to_char( "You sure need help!\n\r", ch );
         return;
      }
      if  ( !is_same_group( ch, assist ) )
      {
         act( "Sorry, $N isn't in your group...", ch, NULL, assist, TO_CHAR );
         return;
      }
      if  ( ( assist->fighting != NULL )         
       && ( ch->fighting == NULL ) )
      { 
         sprintf( actbuf, "$n screams, '%s'", 
           IS_NPC( ch ) ?
             "BANZAI!! $N must be assisted!!" :
             ch->pcdata->assist_msg );
         act( actbuf, ch, NULL, assist, TO_ROOM );
         sprintf( actbuf, "You scream, '%s'",         
           IS_NPC( ch ) ?                             
             "BANZAI!! $N must be assisted!!" :       
             ch->pcdata->assist_msg );                
         act( actbuf, ch, NULL, assist, TO_CHAR );    
         set_fighting( ch, assist->fighting, TRUE );
         return;
      }
      send_to_char( "Doesn't look like anyone needs your help right now...\n\r", ch );
      return;
   }  
   
   /* Check if any group members in room AND fighting */
   
   for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      if   ( ( ppl != ch              )
          && ( is_same_group( ch, ppl ) ) )
             break;
             
   if ( ppl == NULL )
   {
      send_to_char( "Doesn't look like anyone needs your help right now...\n\r", ch );
      return;
   }

   /* Assisting leader is main priority */
   if ( ( ch->leader != NULL )
     && ( ch->leader->in_room == ch->in_room )
     && ( ch->leader->fighting != NULL )
     && ( ch->fighting == NULL )
     && ( ch->leader != ch ) )
   {
     sprintf( actbuf, "$n screams, '%s'",             
       IS_NPC( ch ) ?                                 
        "BANZAI!! $N must be assisted!!" :           
         ch->pcdata->assist_msg );                    
     act( actbuf, ch, NULL, ch->leader, TO_ROOM );        
     sprintf( actbuf, "You scream, '%s'",             
       IS_NPC( ch ) ?                                 
        "BANZAI!! $N must be assisted!!" :           
        ch->pcdata->assist_msg );                    
     act( actbuf, ch, NULL, ch->leader, TO_CHAR );        
     set_fighting( ch, ch->leader->fighting, TRUE );           
     return;                                               
   }                                                      

   /* Ok, so no leader to help... lets look for other group members */
   for ( ppl = ch->in_room->first_person; ppl != NULL; ppl = ppl->next_in_room )
      if  ( ( is_same_group( ch, ppl ) )
         && ( ppl != ch )
         && ( ppl->fighting != NULL )
         && ( ch->fighting  == NULL ) )
   {
     sprintf( actbuf, "$n screams, '%s'",             
       IS_NPC( ch ) ?                                 
        "BANZAI!! $N must be assisted!!" :           
         ch->pcdata->assist_msg );                    
     act( actbuf, ch, NULL, ppl, TO_ROOM );        
     sprintf( actbuf, "You scream, '%s'",             
       IS_NPC( ch ) ?                                 
        "BANZAI!! $N must be assisted!!" :           
        ch->pcdata->assist_msg );                    
     act( actbuf, ch, NULL, ppl, TO_CHAR );        
     set_fighting( ch, ppl->fighting, TRUE );           
     return;                                               
   }                                                      
      

   send_to_char( "Doesn't look like anyone needs your help right now...\n\r", ch );
   return;
}         
          
void do_stake( CHAR_DATA *ch, char *argument )
{
   /* If player has an object of TYPE_STAKE (s)he can try and stake
    * a fallen ( hit<0 ) vampire.  Note that there is still a check
    * for pkilling here... PKOK players can be vamp hunters with
    * no worries! (:  Player must also have learnt the stake skill.
    * A sleeping vamp is also considered vulnerable.
    */
   
   CHAR_DATA *victim;
   OBJ_DATA *stake;
   int chance;
   char arg[MAX_INPUT_LENGTH];
   int sn;
   
   one_argument( argument, arg );
   
   if ( arg[0] == '\0' )
   {
      send_to_char( "Stake who?\n\r", ch );
      return;
   }
   
   victim = get_char_room( ch, arg );
   
   if ( victim == NULL )
   {
      send_to_char( "Couldn't find the target.\n\r", ch );
      return;
   }
   if ( victim == ch )
   {
      send_to_char( "Oh yeah, that would be a really good idea!\n\r", ch );
      return;
   }
   
   if (  ( stake = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL )
     stake = get_eq_char( ch, WEAR_HOLD_HAND_R );
   if ( stake == NULL || stake->item_type != ITEM_STAKE )
   {
      send_to_char( "You have to be holding some sort of stake to do that\n\r", ch );
      return;
   }
   if ( IS_NPC( victim ) )
   {
     send_to_char( "This is only used against PLAYER {dVampyres{x!!\n\r", ch );
     return;
   }
 
   chance = IS_NPC( ch ) ? ch->level*2 : get_skill(ch, gsn_stake);
   if ( !IS_AWAKE( victim ) )
      chance +=25;
   
   /* Make it harder to stake higher level targets */
   if ( ( victim->pcdata->vamp_level * 5 ) > ch->level )
      chance -= ( victim->pcdata->vamp_level * 5 ) - ch->level;
   
   if ( victim->hit > 0 && IS_AWAKE(victim) ) /* i.e. not vulnerable! */
      chance = 0;
 
   if ( !IS_VAMP(victim) )
      chance = 0;
   
   WAIT_STATE( ch, skill_table[gsn_stake].beats );
   check_killer( ch, victim );

   if ( number_percent() < chance )
   {
      /* Success */
      act( "$n raises $p above $s head and plunges it into $N's heart!",  ch, stake, victim, TO_NOTVICT );
      act( "You raise $p above your head and plunge it into $N's heart!", ch, stake, victim, TO_CHAR );
      act( "$n raises $p above his head and plunges it into your heart!", ch, stake, victim, TO_VICT );
      act( "$n screams in agony and withers to black dust!", victim, NULL, NULL, TO_ROOM );
      send_to_char( "You scream in agony and withers to black dust!\n\r", victim );
      send_to_char( "{xYou find yourself back in Moribund....\n\r", victim );
      send_to_char( "{xSuddenly, you realize that your {RKindred{xknowledge\n\r", victim );      
      send_to_char( "{xis gone! So, THAT's what the sun looks like to mortals!\n\r", victim );      
      send_to_char( "{xYou had almost forgotten.....\n\r", victim );  

      
    
      REMOVE_BIT( victim->pcdata->pflags , PFLAG_VAMP );
      victim->pcdata->vamp_level = 0;
      victim->pcdata->vamp_exp = 0;
      victim->pcdata->bloodlust = 0;
      victim->pcdata->bloodlust_max = 0;
      victim->pcdata->generation = -1;
      victim->pcdata->vamp_bloodline = 0;
      victim->pcdata->recall_vnum = 3001;

/* remove vamp skills from dead vamp  */

      for (sn = 0; sn <= MAX_SKILL; sn++ )
        if ( ( skill_table[sn].flag2 == VAMP ) && ( victim->pcdata->learned[sn] > 0 ) )
          victim->pcdata->learned[sn] = 0;

      if ( !IS_NPC( victim ) )
         gain_exp( victim, 0 - ( 3 * victim->exp / 4 ) );
      
      raw_kill( victim, "" ); 
      return;
   }
   else
   {
      act( "$n tries to plunge $p into $N's chest, but misses!", ch, stake, victim, TO_NOTVICT );
      act( "You try to plunge $p into $N's chest, but miss!",    ch, stake, victim, TO_CHAR );
      act( "$n tries to plunge $p into your chest, but misses!", ch, stake, victim, TO_VICT );
      set_fighting( ch, victim, TRUE );
   }
   return;
}      
                             
void do_stun( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   int chance;
   int chance2;

   if ( IS_NPC( ch ) )  /* for now */
     return;

   
   if (  ( ( victim = ch->fighting ) == NULL )
      || ( victim->in_room == NULL )   )
   {
      send_to_char( "You must be fighting someone first!\n\r", ch );
      return;
   }
   if ( !IS_NPC( ch ) && ( get_skill(ch, gsn_stun) == 0 )  )
   {
     send_to_char( "Huh?", ch );
     return;
   }

   chance = 45 - (100 * victim->hit / victim->max_hit);
   /* The lower the victim's hp, the greater the chance */
    
   chance2 = IS_NPC(ch) ? ch->level*2 : get_skill(ch, gsn_stun);
   
   WAIT_STATE( ch, skill_table[gsn_stun].beats );
   
   if ( ( number_percent() + number_percent() ) < ( chance + chance2 ) )
   {
      /* Success */
      act( "You slam into $N, leaving $M stunned.",   ch, NULL, victim, TO_CHAR    );
      act( "$n slams into you, leaving you stunned.", ch, NULL, victim, TO_VICT    );
      act( "$n slams into $N, leaving $M stunned.",   ch, NULL, victim, TO_NOTVICT );

      victim->stunTimer += number_range( 1, get_psuedo_level( ch )/30 );
      if ( ch->level > 40 )
        victim->stunTimer += number_range( 1, 2 );
 

   }
   else
   {
      /* Ooops! */
      act( "You try to slam into $N, but miss and fall onto your face!",    ch, NULL, victim, TO_CHAR    );
      act( "$n tries to slam into you, but misses and falls onto $s face!", ch, NULL, victim, TO_VICT    );
      act( "$n tries to slam into $N, but misses and falls onto $s face!",  ch, NULL, victim, TO_NOTVICT ); 
      damage( ch, ch, number_range( 5, 20), TYPE_UNDEFINED );
      return;
   }
   return;
}

void do_feed( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];
   int chance;
   int bloodgain = 0;
   one_argument( argument, arg );
         
   if ( !IS_VAMP( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   if ( ( victim = ch->fighting ) == NULL )
   {
      send_to_char( "You must be fighting someone first!\n\r", ch );
      return;

   }   
   
   if ( victim == ch )
   {
      send_to_char( "What are you?  A blood donor?\n\r", ch );
      return;
   }
   
   if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
   {
      send_to_char( "The Gods prevent your foul deed.  This is a safe room!\n\r", ch );
      return;
   }
   
   if (    victim->hit > 0 
        && victim->position > POS_FIGHTING )
   {
      /* failure */
      act( "$N is far too alert to let you do that!", ch, NULL, victim, TO_CHAR );
      return;
   }
   
   chance = IS_NPC(ch) ? ch->level : get_skill(ch, gsn_feed);
   WAIT_STATE( ch, skill_table[gsn_feed].beats );
   check_killer( ch, victim );
   if ( number_percent() < chance )
   {
      /* Success! */
      act( "You plunge your fangs into $N's neck, and taste sweet blood!", ch, NULL, victim, TO_CHAR    );
      act( "$n plunges $s fangs into your neck, and sucks your blood!",    ch, NULL, victim, TO_VICT    );
      act( "$n plunges $s fangs into $N's neck and sucks $S blood!",       ch, NULL, victim, TO_NOTVICT );
      if ( ch->pcdata->bloodlust <= -5 )
        {
          ch->pcdata->bloodlust = ch->pcdata->bloodlust_max;
          send_to_char( " You have now entered the ranks of the {dKINDRED{x!!!!\n\r", ch );
        }
      else
        {
          
          bloodgain = ( ( 20 - ch->pcdata->generation ) + ch->pcdata->vamp_level  );
          if ( bloodgain > victim->level )
           bloodgain = victim->level;
        }
 
        if ( ( ch->pcdata->bloodlust + bloodgain ) > ch->pcdata->bloodlust_max )
          ch->pcdata->bloodlust = ch->pcdata->bloodlust_max;
        else
          ch->pcdata->bloodlust += bloodgain;
      
      if ( victim->hit > 0 )
      {
         act( "$N screams in horror at you at jumps to $s feet!",  ch, NULL, victim, TO_CHAR    ); 
         act( "You scream in horror at $n and jump to your feet!", ch, NULL, victim, TO_VICT    );
         act( "$N screams in horror at $n and jumps to $S feet!",  ch, NULL, victim, TO_NOTVICT );
         do_say( victim, "You'll pay for that!" );
         victim->position = POS_STANDING;
         multi_hit( victim, ch, TYPE_UNDEFINED );
      }


        
      if ( IS_VAMP( victim ) )
      {
         return;  /* for now :P SRZ 
         if ( !IS_NPC(ch) && !IS_NPC(victim) )
         {
            ch->pcdata->bloodlust = victim->pcdata->bloodlust;
            victim->pcdata->bloodlust = 0;
         }
         if ( !IS_NPC(ch) && IS_NPC(victim) )
            gain_bloodlust( ch, victim->level*2 );
         if ( IS_NPC(ch) && !IS_NPC(victim) )
            victim->pcdata->bloodlust = 0;     */
      }
        

      
            return;
   }
   else
   {
      /* D'oh! */
      act( "You try to find a spot on $N's neck to suck blood, but fail!",   ch, NULL, victim, TO_CHAR    );
      act( "$n tries to find a spot on your neck to suck blood, but fails!", ch, NULL, victim, TO_VICT    );
      act( "$n tries to find a spot on $N's neck to suck blood, but fails!", ch, NULL, victim, TO_NOTVICT );
      /* Any penalty? */
      return;
   }
   return;     
}


void check_adrenaline( CHAR_DATA *ch, sh_int damage )

{
/*
AFFECT_DATA af;
    

  
   if ( damage > 200 && ch->pcdata->learned[gsn_adrenaline] > 70 )
   {
      
     af.type      = skill_lookup( "adrenaline bonus" );
     af.duration  = 1;
     af.location  = APPLY_DAMROLL;
     af.modifier  = 1;
     af.bitvector = 0;
     affect_join( ch, &af );
     send_to_char( "You feel a surge of {Radrenaline{x!!!\n\r", ch );
   }
*/
  return;
}

void do_frenzy( CHAR_DATA *ch, char *argument ) 
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int moves = 0;
    int damage = 0;

    if ( IS_NPC( ch )  )
      return;	
	
    if ( !IS_NPC(ch) && get_skill(ch, gsn_frenzy) == 0 )
    {
       send_to_char( "You are not trained in this skill!\n\r", ch );
       return;
    }
    
    
    if( !IS_NPC(ch) && ch->move < 100 && ch->position == POS_FIGHTING ) 	
	{	
		send_to_char( "You're too tired to go into a frenzy!\n\r", ch);
        return;
      }

     if ( !IS_NPC( ch ) && number_percent( ) > get_skill(ch, gsn_frenzy)) 
     {
		send_to_char( "You try to go into a frenzy, but nearly cut your leg off!\n\r", ch  );
		return;
     }

	if( ( ( ch->move/10 ) < 75 ) && ch->move >=75 )
		moves = 75;
	else
		moves = ch->move/10;
		
	if( ch->hit > 200 )
		damage = 20;
		
	WAIT_STATE( ch, skill_table[gsn_frenzy].beats );

    if( !IS_NPC( ch ) && ch->position == POS_FIGHTING ) 
    	ch->move -= moves;
    	ch->hit -= damage;
    CREF( vch_next, CHAR_NEXTROOM );
    for ( vch = ch->in_room->first_person; vch != NULL; vch = vch_next ) 
    {
      vch_next = vch->next_in_room;
        if ( vch->in_room == NULL )
			continue;

		if ( vch->in_room == ch->in_room )
		{
		    if ( vch != ch && ( vch->in_room == ch->in_room ) 
		    && ( IS_NPC( ch )? !IS_NPC( vch ) : IS_NPC( vch ) )
                && ( vch->master != ch ) 
                && ( !is_same_group( ch, vch ) )   )
		    {
               	act( "$N is {Wgored{x by your FRENZY!!!\n\r", ch, NULL, 
			vch, TO_CHAR );
               	act( "$n goes into a FRENZY, {Wgoring{x $N!!!", ch, NULL, 
					vch, TO_ROOM );
                  check_killer( ch, vch );
               	one_hit( ch, vch, -1, FALSE );
               	continue;
                }
	}
     }
     CUREF( vch_next );

    return;
}


  

void do_rage( CHAR_DATA *ch, char *argument ) 
{
  char	arg[MAX_INPUT_LENGTH];
  int	chance = 0;


   one_argument( argument, arg );
   


  if ( IS_NPC( ch )
     || !IS_WOLF( ch )  )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if ( IS_RAGED( ch ) )
  {
    send_to_char( "You are already {GRAGING{x!!!\n\r", ch );
    return;
  }

  chance = 60 - ( ch->hit /ch->max_hit * 100 );
  if ( weather_info.moon_loc == MOON_PEAK )
    chance += 15;
  if ( IS_SHIFTED( ch ) )
    chance += 20;
  if ( weather_info.moon_phase == MOON_FULL )
    chance += 30;
  if ( weather_info.moon_phase == MOON_NEW )
    chance -= 30;
  if ( weather_info.moon_loc == MOON_DOWN )
    chance -= 20;
  if ( weather_info.moon_loc == MOON_PEAK )
    chance += 20;

   if ( str_cmp( arg, "FORCE" )  )
   {
     chance += ( ( 5 - ch->pcdata->generation ) * 10 );  
     chance += ch->pcdata->vamp_level;
     if ( ( ch->pcdata->generation < 4 ) || ( ch->pcdata->vamp_level >14 )  )
       chance += 50;
   }
   else
   {
     chance -= ( ( 5 - ch->pcdata->generation ) * 10 );  
     chance -= ch->pcdata->vamp_level;
     if ( ( ch->pcdata->generation < 4 ) || ( ch->pcdata->vamp_level >14 )  )
       chance -= 50;
   }


   if ( number_range( 0, 100 ) < chance )
   {
     AFFECT_DATA af1;
     AFFECT_DATA af2;
     AFFECT_DATA af3;
     AFFECT_DATA af4;

     int	duration;
      
     af1.type      = skill_lookup( "Rage:sharpened claws" );
     af2.type      = skill_lookup( "Rage:wolven strength" );
     af3.type      = skill_lookup( "Rage:disregard for pain" );
     af4.type	   = skill_lookup( "Enraged" );

     if ( !str_cmp( arg, "FORCE" ) )
     {
       duration = number_range( ch->pcdata->generation/4, 4 );

       af4.duration  = duration;
       af4.location  = APPLY_NONE;
       af4.modifier  = 0;
       af4.bitvector = 0;
       affect_join( ch, &af4 );


       af1.duration  = duration;
       af1.location  = APPLY_DAMROLL;
       af1.modifier  = ( 10 - ch->pcdata->generation ) * 4;
       af1.bitvector = 0;
       affect_join( ch, &af1 );

       af2.duration  = duration;
       af2.location  = APPLY_HITROLL;
       af2.modifier  = ( 10 - ch->pcdata->generation ) * 4;
       af2.bitvector = 0;
       affect_join( ch, &af2 );

       af3.duration  = duration;
       af3.location  = APPLY_AC;
       af3.modifier  = ( 5 + ch->pcdata->generation ) * 10;
       af3.bitvector = 0;
       affect_join( ch, &af3 );



     }
     else
     {
       duration = number_range( ch->pcdata->generation/3, 5 );
       af4.duration  = duration;
       af4.location  = 0;
       af4.modifier  = 0;
       af4.bitvector = 0;
       affect_join( ch, &af4 );


       af1.duration  = duration;
       af1.location  = APPLY_DAMROLL;
       af1.modifier  = ( 10 - ch->pcdata->generation ) * 4;
       af1.bitvector = 0;
       affect_join( ch, &af1 );

       af2.duration  = duration;
       af2.location  = APPLY_HITROLL;
       af2.modifier  = ( 10 - ch->pcdata->generation ) * 4;
       af2.bitvector = 0;
       affect_join( ch, &af2 );

       af3.duration  = duration;
       af3.location  = APPLY_AC;
       af3.modifier  = ( 5 + ch->pcdata->generation ) * 15;
       af3.bitvector = 0;
       affect_join( ch, &af3 );


    } 
    send_to_char( "You are {RENRAGED!!!!!!\n\r", ch );
    SET_BIT( ch->pcdata->pflags, PFLAG_RAGED );
    ch->pcdata->bloodlust = ( ch->pcdata->bloodlust_max - number_range( 0, ch->pcdata->generation *3 ) );
  }
  else
    send_to_char( "You fail to become {RENRAGED{x.\n\r", ch );
  return;


};
void do_dislodge( CHAR_DATA *ch, char *argument )
{
        char arg[MSL];
        OBJ_DATA * weapon = NULL;
        CHAR_DATA *victim;
        int dam = 0;

argument = one_argument( argument, arg );        
    if (arg[0] == '\0') /* empty */
    {
        send_to_char ("Dislodge who?\n\r",ch);
        return;
    }   

   if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
        
        if ( get_eq_char(victim, WEAR_LODGED) != NULL)
        {
        weapon = get_eq_char( victim, WEAR_LODGED );
        if ( victim == ch )
{
        act( "With a grunt, you dislodge $p from your body.", ch, weapon, NULL, TO_CHAR );
	  act( "Grunting, $n dislodges $p from $s body.", ch, weapon, NULL, TO_NOTVICT );
} else {
        act( "With a grunt, you dislodge $p from $N's body.", ch, weapon, victim, TO_CHAR );
	  act( "Grunting, $n dislodges $p from YOUR body!", ch, weapon, NULL, TO_VICT );
	  act( "Grunting, $n dislodges $p from $N's body.", ch, weapon, victim, TO_NOTVICT );
}
        unequip_char( victim, weapon );
        REMOVE_BIT(weapon->extra_flags,ITEM_LODGED);
        obj_from_char(weapon);
        obj_to_char(weapon, ch);
        dam      =  dice((2 * weapon->value[1]), (2 * weapon->value[2]));
        damage( victim, victim, dam, ( TYPE_HIT + weapon->value[3] ) );
        return;
        }       

  else
        {
        if (victim == ch )
         {       
        act( "You have nothing lodged in your body!", ch, NULL, NULL, TO_CHAR );
        return;
         } else
       {
        act( "$N has nothing lodged in $S body!", ch, victim, victim, TO_CHAR );
        return;

       }
        }
}      

void do_lodge( CHAR_DATA *ch, char *argument)
{
 char arg[MSL];
 OBJ_DATA  *weapon;
 CHAR_DATA *victim;
 int dam;

argument = one_argument( argument, arg );

  if (IS_NPC(ch))
   return;

    if (arg[0] == '\0')
   {
        send_to_char ("Lodge what in whom?\n\r",ch);
        return;
    }

    if (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  == NULL 
          || ( weapon->item_type != ITEM_WEAPON ) ) 
        {
       send_to_char( "You must be wielding a weapon!\n\r", ch);
       return;
        }

 if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

 if ( is_safe( ch, victim ) )
      return;

 if ( get_eq_char(victim, WEAR_LODGED) != NULL)
 {
  act( "$N already has something lodged in $S body!",  ch, NULL, victim, TO_CHAR    );
  return;
 }

    if ( get_skill(ch, gsn_lodge) > number_percent())
 {
  act( "You lodge your weapon into $N's body!",  ch, NULL, victim, TO_CHAR    );
  act( "$n lodges $s weapon into you!", ch, NULL, victim, TO_VICT    );
  act( "$n lodges $s weapon into $N!",  ch, NULL, victim, TO_NOTVICT );
  dam = dice((3 * weapon->value[1]), (3 * weapon->value[2]));
  obj_from_char(weapon);
        obj_to_char(weapon, victim);
  equip_char( victim, weapon, WEAR_LODGED );
  SET_BIT(weapon->extra_flags,ITEM_LODGED);
  damage( ch, victim, dam, ( TYPE_HIT + weapon->value[3] ) );
  return;
 }
 else
 {
dam = dice(( weapon->value[1]), ( weapon->value[2]));
  act( "You try to lodge your weapon into $N's body and miss!",  ch, NULL, victim, TO_CHAR    );
  act( "$n tries to lodge $s weapon into you and misses!", ch, NULL, victim, TO_VICT    );
  act( "$n tried to lodge $s weapon into $N and misses!",  ch, NULL, victim, TO_NOTVICT );

    damage( ch, victim, dam, ( TYPE_HIT + weapon->value[3] ) );
  return;
 }

}

void do_twist( CHAR_DATA *ch, char *argument)
{
 char arg[MAX_INPUT_LENGTH];
 OBJ_DATA * weapon = NULL;
 CHAR_DATA *victim;
 int dam = 0;

argument = one_argument( argument, arg );
 if (IS_NPC(ch))
  return;

    if (arg[0] == '\0')
    {
        send_to_char ("Twist what in whom?\n\r",ch);
        return;
    }

    if (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_L ) )  != NULL )
       {
        send_to_char( "Your primary hand must be free to twist a weapon!", ch);
        return;
       }

 if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
   if ( is_safe( ch, victim ) )
      return;

 if ( ( get_eq_char(victim, WEAR_LODGED) ) == NULL)
 {
  act( "$N doesn't have anything lodged in $S body!",  ch, NULL, victim, TO_CHAR    );
  return;
 }


 WAIT_STATE( ch, skill_table[gsn_twist].beats );
    if ( get_skill(ch, gsn_twist) > number_percent())
 {
  weapon = get_eq_char( victim, WEAR_LODGED );
  act( "You twist the weapon lodged in $N's body!",  ch, NULL, victim,
TO_CHAR    );
  act( "$n twists the weapon lodged in you!", ch, NULL, victim,
TO_VICT    );
  act( "$n twists the weapon lodged in $N!",  ch, NULL, victim,
TO_NOTVICT );
  dam      =  dice((2 * weapon->value[1]), (2 * weapon->value[2]));
  damage( ch, victim, dam, ( TYPE_HIT + weapon->value[3] ) );
  return;
 }
 else
 {
  dam  =  dice(( weapon->value[1]), (weapon->value[2]));
  act( "You try to twist the weapon lodged in $N's body but can't get a grip!",  ch, NULL, victim,
TO_CHAR    );
  act( "$n tries twist the weapon lodged in you but can't get a grip!", ch, NULL, victim,
TO_VICT    );
  act( "$n tries to twist the weapon lodged in $N but can't get a grip!",  ch, NULL, victim,
TO_NOTVICT );
  damage( ch, victim, dam, ( TYPE_HIT + weapon->value[3] ) );
  return;
 }
}

void check_improve(CHAR_DATA * ch, int sn, bool success, int multiplier)
{
   // int chance;
   // char buf[100];
	return;
	/*
    if (IS_NPC(ch))
	return;

    if (ch->pcdata->learned[sn] == 0
	|| ch->pcdata->learned[sn] >= 100)
	return;			

	if (multiplier <= 0)
		multiplier = 1;

    /* check to see if the character has a chance to learn 
    chance = 20 * int_app[get_curr_int(ch)].learn;
    chance /= (multiplier * 10);
    chance += ch->level;

    if (number_range(1, 1500) > chance)
	return;

    /* now that the character has a CHANCE to learn, see if they really have 

    if (success) {
	chance = URANGE(5, 100 - ch->pcdata->learned[sn], 95);
	if (number_percent() < chance) {
	    sprintf(buf, "{YYou have become better at %s!{x\n\r",
		    skill_table[sn].name);
	    send_to_char(buf, ch);
	    ch->pcdata->learned[sn]++;
	}
    } else {
	chance = URANGE(5, ch->pcdata->learned[sn] / 2, 30);
	if (number_percent() < chance) {
	    sprintf(buf,
		    "You learn from your mistakes, and your %s skill improves.\n\r",
		    skill_table[sn].name);
	    send_to_char(buf, ch);
	    ch->pcdata->learned[sn] += number_range(1, 3);
	    ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn], 100);
	}
    }
	*/
}

void do_strategy(CHAR_DATA * ch, char *argument)
{
    char argone[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

    argument = one_argument(argument, argone);

    if (argone[0] == '\0') {
	send_to_char("Strategy:\n\r", ch);
	send_to_char("Flee, Defend, Engage, Assault \n\r", ch);
	return;
    }
    if (argone[0] == 'f') {
	send_to_char("You will now flee in combat.\n\r", ch);
	ch->pcdata->strategy = 0;
    } else if (argone[0] == 'd') {
	send_to_char("You take up a defensive position.\n\r", ch);
	ch->pcdata->strategy = 1;
    } else if (argone[0] == 'e') {
	send_to_char("You adjust your stance for a balanced fight.\n\r", ch);
	ch->pcdata->strategy = 2;
    } else if (argone[0] == 'a') {
	send_to_char("You commit yourself to aggressive and dangerous attacks.\n\r", ch);
	ch->pcdata->strategy = 3;
    } else {
	send_to_char("Unkown strategy. \n\r", ch);
	return;
    }
}
