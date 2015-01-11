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
#include "ack.h"
#include "magic.h"

extern   bool    deathmatch;

/*
 * This file should contain:
 *	o Adept Spells
 *	o Remort Spells
 *	o Room-affect Spells
 */


bool spell_seal_room( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   ROOM_INDEX_DATA *room;
   ROOM_AFFECT_DATA raf;
   
   room = ch->in_room;
   
   if ( room == NULL )
      return FALSE;
      
   if ( IS_SET( room->affected_by, ROOM_BV_ENCAPS ) )
   {
      send_to_char( "This room is already sealed!\n\r", ch );
      return FALSE;
   }
   if ( IS_SET( room->area->flags, AREA_NO_ROOM_AFF ) )
   {
     send_to_char( "A strange {RDeamon{x appears, waggles it's finger at you, and shakes its(?) head!\n\r", ch );
     return FALSE;
   }
   if ( deathmatch )
   {
     if ( ch->hit < 150 )
     {
       send_to_char( "You don't have enough life force remaining!\n\r", ch);
       return FALSE;
     }
     else
     {
       send_to_char( "{RYou feel some of your life force rip from your soul!{x\n\r", ch );
       ch->hit -= 100;
     }
    }
   act( "$n spreads his hands into the air and ejects a web of energy!", ch, NULL, NULL, TO_ROOM );
   send_to_char( "You spread your hands into the air and eject a web of energy!\n\r", ch );
   
   raf.type      = sn;
   raf.level     = level;
   raf.duration  = ( level / 20 ) + 2;
   raf.bitvector = ROOM_BV_ENCAPS;
   raf.caster    = ch;
   affect_to_room( room, &raf );
   return TRUE;
}


bool spell_deflect_weapon( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return FALSE;
    af.type      = sn;
    af.duration  = 16;
    af.modifier  = -40 - get_psuedo_level( ch )/5;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel a mind shield form around you.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return TRUE;
}

bool spell_black_hand( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *victim = (CHAR_DATA *)vo;
   
   
   AFFECT_DATA af;
   
   
   
   
   act( "You summon a hand of death, and it chokes $N!",   ch, NULL, victim, TO_CHAR    );
   act( "A Black Hand grows from the shadows, and begins to choke $N!",  ch, NULL, victim, TO_NOTVICT );
   act( "$n summons a Black Hand from the shadows, which begins to choke you!", ch, NULL, victim, TO_VICT    );
   
   if ( saves_spell( level, victim ) || IS_VAMP(victim) || is_affected( victim, sn ) )
   {
      send_to_char( "The Black Hand dissolves back into the shadows!\n\r", victim );
      send_to_char( "The Black Hand dissolves back into the shadows!\n\r", ch );
      act( "The Black Hand dissolves back into the shadows!", ch, NULL, victim,  TO_NOTVICT );
      return TRUE;
   }
   
   af.type 	= sn;
   af.duration  = 3;
   af.location	= APPLY_HIT;
   af.modifier  = level;
   af.bitvector = 0;
   af.caster    = ch;
   
   affect_to_char( victim, &af );
   
   act( "The Black Hand surrounds $N and begins to choke!", ch, NULL, victim, TO_ROOM );
   
   return TRUE;
}

bool spell_throw_needle( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *victim = (CHAR_DATA *)vo;
   
   AFFECT_DATA af;

   if ( !sp_damage( obj, ch, (CHAR_DATA *) vo, dice(2, level), REALM_POISON,  sn, TRUE ) )
     return TRUE;
    
   if ( !saves_spell( level, victim ) )
   {
    af.type      = sn;
    af.duration  = 12 + (level/10);
    af.location  = APPLY_STR;
    af.modifier  = -3;
    af.bitvector = AFF_POISON;
    affect_join( victim, &af );
    send_to_char( "You feel very sick.\n\r", victim );
    act( "$n looks very sick.", victim, NULL, NULL, TO_ROOM );
   }

    return TRUE;


    
    
}
bool spell_morale( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

    for ( gch = ch->in_room->first_person; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_affected( gch, sn ) || !is_same_group( ch, gch ) )
	      
	    continue;
	act( "$n seems much more willing to fight.", gch, NULL, NULL, TO_ROOM );
	send_to_char( "You are inspired to fight better!\n\r", gch );
	af.type      = sn;
	af.duration  = 4 + ( level / 5 );
	af.location  = APPLY_DAMROLL;
	af.modifier  = get_psuedo_level( ch )/10;
	af.bitvector = 0;
	affect_to_char( gch, &af );
    }
    send_to_char( "You inspire the troops!\n\r", ch );

    return TRUE;
}

bool spell_leadership( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

    for ( gch = ch->in_room->first_person; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_affected( gch, sn ) || !is_same_group( ch, gch ) )
	      
	    continue;
	act( "$n looks more courageous!", gch, NULL, NULL, TO_ROOM );
	send_to_char( "You fell courage wash over you!\n\r", gch );
	af.type      = sn;
	af.duration  = 4 + ( level / 5 );
	af.location  = APPLY_HITROLL;
	af.modifier  = get_psuedo_level( ch )/10;
	af.bitvector = 0;
	affect_to_char( gch, &af );
    }
    send_to_char( "You inspire the troops!\n\r", ch );

    return TRUE;
}

bool spell_ice_bolt( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam         = 150 + dice( level/4, 12 );
    if ( saves_spell( level, victim ) )
      dam /= 1.2;
    sp_damage( obj, ch, victim, dam, REALM_COLD, sn, TRUE );
    return TRUE;
}
bool spell_waterelem( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the elemental forces of {Bwater{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the elemental forces of {Bwater{x.", ch, obj, NULL, TO_CHAR );
   
   act( "A waterfall appears, and an elemental steps from the pool!!", ch, obj, NULL, TO_ROOM );
   act( "A waterfall appears, and an elemental steps from the pool!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_WATERELEM ) );
   char_to_room( summoned, ch->in_room );
   act( "$n floods from the pool, drawing all the water into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
    add_follower( summoned, ch );
  return TRUE;
}



bool spell_skeleton( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the {dNegative Plane{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the {dNegative Plane{x.", ch, obj, NULL, TO_CHAR );
   
   act( "The ground opens beneath you, and a skeleton crawls out of the graveyard filth!!", ch, obj, NULL, TO_ROOM );
   act( "The ground opens beneath you, and a skeleton crawls out of the graveyard filth!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_SKELETON ) );
   char_to_room( summoned, ch->in_room );
   act( "$n Stands erect, and bow's towards it's master.", summoned, NULL, NULL, TO_ROOM );
   
  /* 
   summoned->level    = 40;
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   */
   
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_poison_weapon( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    OBJ_DATA *ob = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    /* Quick way to stop imms (Bash?) enchanting weapons for players */
    if ( IS_IMMORTAL( ch ) && ch->level != L_GOD )
    {
       send_to_char( "Nothing Happens.\n\r", ch );
       return FALSE;
    }
    
    if ( ob->item_type != ITEM_WEAPON )
    {
      send_to_char( "That is not a weapon!.\n\r", ch );
      return FALSE;
    }
   
   /* This is gonna fuck up a lot.. paf->type isnt saved for objects in
      pfiles.. -- Alty */
   for ( paf = ob->first_apply; paf != NULL; paf = paf->next )
    {
	if ( paf->type == sn )
	{
	    send_to_char( "That weapon is already poisoned!.\n\r", ch );
       return FALSE;
	}
    }

    GET_FREE(paf, affect_free);
    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_DAMROLL;
    paf->modifier       = UMIN( ( level / 30 ) + 1, ob->level );
    paf->bitvector      = 0;
    paf->caster		= NULL;
    LINK(paf, ob->first_apply, ob->last_apply, next, prev);
    
    return TRUE;
}



 void do_disguise( CHAR_DATA *ch, char *argument )
 {
    char arg[MAX_STRING_LENGTH]; 
    

    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_disguise] == 0 )
    {
       send_to_char( "You are not trained in this skill!\n\r", ch );
       return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
      send_to_char( " Enter the disguise name, or reset to reset your description to normal.\n\r", ch );
      return;
    }

    if ( !str_cmp( arg, "reset" ) )
     {
       free_string( ch->long_descr );
       ch->long_descr = str_dup( ch->long_descr_orig );
       return;
     }
     else
     {
       
	free_string( ch->long_descr );
      safe_strcat( MAX_STRING_LENGTH, argument, "\n\r" );
	ch->long_descr = str_dup( argument );
      send_to_char( "You are now Disguised!!!\n\r", ch );
      return;
     }

      
    
    return;
 }
 
bool spell_fireshield( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
  MAGIC_SHIELD * shield;
  AFFECT_DATA af;
  bool	char_login = FALSE;

  if (  ( !IS_NPC( ch ) )
     && ( ch->desc != NULL )
     && ( ch->desc->connected == CON_SETTING_STATS )  )
     char_login = TRUE;
		
/* check to see if victim is already flaming 
  if ( is_shielded( victim, FLAME_SHIELD ) )	
    return FALSE;
  
    for now, we are gonna allow only one shield up at a time  */	
  if ( ch->first_shield != NULL )
  {
    return FALSE;
  }

    af.type      = sn;
    af.duration  = get_psuedo_level( ch ) /10;
    if ( char_login )
      af.duration /= 2;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

  GET_FREE( shield, shield_free );
  shield->name = str_dup( "{RFIRE{x" );		
  shield->type = FLAME_SHIELD;	
  shield->harmfull = TRUE;
  shield->attack_dam = number_range( ( level * 3 ), ( level * 5 ) );
  shield->percent = 10 + ( get_psuedo_level( ch ) / 10 );
  shield->hits = 5000 + ( get_psuedo_level( ch ) * 10 );
  shield->sn	= sn;
  if ( char_login )
    shield->hits /= ( number_range( 2, 10 ) );

  shield->absorb_message_room = str_dup( "{x$n's {Rshield{x flares and envelops $N in {Rflames{x!!" );
  shield->absorb_message_victim = str_dup( "{x$N's {Rshield{x flares, and envelops you with {Rflame{x!!" );
  shield->absorb_message_self = str_dup( "{xYour {Rshield{x flares, and envelops $N with {Rflame{x!!!" );
  shield->wearoff_room = str_dup( "{x$n's {Rshield{x {YFLAMES OUT{x!!!!!" );
  shield->wearoff_self = str_dup( "{xYour {Rshield{x {YFLAMES OUT{x!!!!!" );
  
  LINK( shield, ch->first_shield, ch->last_shield, next, prev );

  send_to_char( "{xYou {Rburst{x into {rflames{x!\n\r", ch );
  act( "{x$n {Rbursts{x into {rflames{x!",  ch, NULL, NULL, TO_ROOM );
  
  return TRUE;
}


bool spell_iceshield( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
  MAGIC_SHIELD * shield;
  AFFECT_DATA af;
  bool	char_login = FALSE;

  if (  ( !IS_NPC( ch ) )
     && ( ch->desc != NULL )
     && ( ch->desc->connected == CON_SETTING_STATS )  )
     char_login = TRUE;
		
/* check to see if victim is already flaming 
  if ( is_shielded( victim, ICE_SHIELD ) )	
    return FALSE;
  
    for now, we are gonna allow only one shield up at a time  */	
  if ( ch->first_shield != NULL )
  {
    return FALSE;
  }

    af.type      = sn;
    af.duration  = get_psuedo_level( ch ) /15;
    if ( char_login )
      af.duration /= 2;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

  GET_FREE( shield, shield_free );
  shield->name = str_dup( "{CICE{x" );		
  shield->type = ICE_SHIELD;	
  shield->harmfull = FALSE;
  shield->attack_dam = 0;
  shield->percent = 50 + ( get_psuedo_level( ch ) / 6 );
  shield->hits = 15000 + ( get_psuedo_level( ch ) * 50);
  shield->sn	= sn;
  if ( char_login )
    shield->hits /= ( number_range( 2, 10 ) );

  shield->absorb_message_room = str_dup( "{x$n's {Cshield{x absorbs the blow{x!!" );
  shield->absorb_message_victim = str_dup( "{x$N's {Cshield{x absorbs the blow{x!!" );
  shield->absorb_message_self = str_dup( "{xYour {Cshield{x absorbs the blow{x!!!" );
  shield->wearoff_room = str_dup( "{x$n's {Cshield {xis {GSHATTERED{x!!!" );
  shield->wearoff_self = str_dup( "{xYour {Cshield{x is {GSHATTERED{x!!!" );

  
  LINK( shield, ch->first_shield, ch->last_shield, next, prev );

  send_to_char( "{xYou are encased in {CIce{x!\n\r", ch );
  act( "{x$n is encased in {CIce{x!!",  ch, NULL, NULL, TO_ROOM );
  
  return TRUE;
}

bool spell_shockshield( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
  MAGIC_SHIELD * shield;
  AFFECT_DATA af;
  bool	char_login = FALSE;

  if (  ( !IS_NPC( ch ) )
     && ( ch->desc != NULL )
     && ( ch->desc->connected == CON_SETTING_STATS )  )
     char_login = TRUE;
		
/* check to see if victim is already flaming 
  if ( is_shielded( victim, SHOCK_SHIELD ) )	
    return FALSE;
  
    for now, we are gonna allow only one shield up at a time  */	
  if ( ch->first_shield != NULL )
  {
    return FALSE;
  }

    af.type      = sn;
    af.duration  = get_psuedo_level( ch ) /7;
    if ( char_login )
      af.duration /= 2;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

  GET_FREE( shield, shield_free );
  shield->name = str_dup( "{BSHOCK{x" );		
  shield->type = SHOCK_SHIELD;	
  shield->harmfull = TRUE;
  shield->attack_dam = number_range( ( level * 2 ), ( level * 4 ) );
  shield->percent = 25 + get_psuedo_level( ch ) / 9;
  shield->hits = 7000 + get_psuedo_level( ch ) * 20;
  shield->sn	= sn;
  if ( char_login )
    shield->hits /= ( number_range( 2, 10 ) );

  shield->absorb_message_room = str_dup( "{x$n's {Bshield{x {Ysparks,{x and zaps $N{x!!" );
  shield->absorb_message_victim = str_dup( "{x$N's {Bshield{x {Ysparks{x, and zaps you{x!!" );
  shield->absorb_message_self = str_dup( "{xYour {Bshield{x {Ysparks{x, and zaps $N{x!!!" );
  shield->wearoff_room = str_dup( "{x$n's {Bshield{x {CDISSAPATES{x!!!!!" );
  shield->wearoff_self = str_dup( "{xYour {Bshield{x {CDISSAPATES{x!!!!!" );
 
  
  LINK( shield, ch->first_shield, ch->last_shield, next, prev );

  send_to_char( "{xYou are surrounded by an {Belectric field{x!!\n\r", ch );
  act( "{x$n is surrounded in an {Belectric field{x!!",  ch, NULL, NULL, TO_ROOM );
  
  return TRUE;
}

bool spell_shadowshield( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
  MAGIC_SHIELD * shield;
  AFFECT_DATA af;
  bool	char_login = FALSE;

  if (  ( !IS_NPC( ch ) )
     && ( ch->desc != NULL )
     && ( ch->desc->connected == CON_SETTING_STATS )  )
     char_login = TRUE;
		
/* check to see if victim is already flaming 
  if ( is_shielded( victim, FLAME_SHIELD ) )	
    return FALSE;
  
    for now, we are gonna allow only one shield up at a time  */	
  if ( ch->first_shield != NULL )
  {
    return FALSE;
  }

    af.type      = sn;
    af.duration  = get_psuedo_level( ch ) /10;
    if ( char_login )
      af.duration /= 2;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

  GET_FREE( shield, shield_free );
  shield->name = str_dup( "{dSHADOW{x" );		
  shield->type = FLAME_SHIELD;	
  shield->harmfull = TRUE;
  shield->attack_dam = number_range( ( level * 2 ), ( level * 4.5 ) );
  shield->percent = 20;
  shield->hits = 3000;
  shield->sn	= sn;
  if ( char_login )
    shield->hits /= ( number_range( 2, 10 ) );

  shield->absorb_message_room = str_dup( "{x$n's {Rshield{x flares and envelops $N in {Rflames{x!!" );
  shield->absorb_message_victim = str_dup( "{x$N's {Rshield{x flares, and envelops you with {Rflame{x!!" );
  shield->absorb_message_self = str_dup( "{xYour {Rshield{x flares, and envelops $N with {Rflame{x!!!" );
  shield->wearoff_room = str_dup( "{x$n's {dshield{x {dDISSAPATES{x!!!!!" );
  shield->wearoff_self = str_dup( "{xYour {dshield{x {dDISSAPATES{x!!!!!" );
 
  
  LINK( shield, ch->first_shield, ch->last_shield, next, prev );

  send_to_char( "You burst into flames!\n\r", ch );
  act( "$n bursts into flames!",  ch, NULL, NULL, TO_ROOM );
  
  return TRUE;
}

bool spell_thoughtshield( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
  MAGIC_SHIELD * shield;
  AFFECT_DATA af;
  bool	char_login = FALSE;

  if (  ( !IS_NPC( ch ) )
     && ( ch->desc != NULL )
     && ( ch->desc->connected == CON_SETTING_STATS )  )
     char_login = TRUE;
		
/* check to see if victim is already flaming 
  if ( is_shielded( victim, FLAME_SHIELD ) )	
    return FALSE;
  
    for now, we are gonna allow only one shield up at a time  */	
  if ( ch->first_shield != NULL )
  {
    return FALSE;
  }

    af.type      = sn;
    af.duration  = get_psuedo_level( ch ) /10;
    if ( char_login )
      af.duration /= 2;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

  GET_FREE( shield, shield_free );
  shield->name = str_dup( "{mTHOUGHT{x" );		
  shield->type = FLAME_SHIELD;	
  shield->harmfull = TRUE;
  shield->attack_dam = number_range( ( level * 2 ), ( level * 4.5 ) );
  shield->percent = 20;
  shield->hits = 3000;
  shield->sn	= sn;
  if ( char_login )
    shield->hits /= ( number_range( 2, 10 ) );

  shield->absorb_message_room = str_dup( "{x$n's {Rshield{x flares and envelops $N in {Rflames{x!!" );
  shield->absorb_message_victim = str_dup( "{x$N's {Rshield{x flares, and envelops you with {Rflame{x!!" );
  shield->absorb_message_self = str_dup( "{xYour {Rshield{x flares, and envelops $N with {Rflame{x!!!" );
  shield->wearoff_room = str_dup( "{x$n's {dshield{x {dDISSAPATES{x!!!!!" );
  shield->wearoff_self = str_dup( "{xYour {dshield{x {dDISSAPATES{x!!!!!" );
  
  LINK( shield, ch->first_shield, ch->last_shield, next, prev );

  send_to_char( "You burst into flames!\n\r", ch );
  act( "$n bursts into flames!",  ch, NULL, NULL, TO_ROOM );
  
  return TRUE;
}




bool spell_ethereal( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    /* Ethereal travel :)
     * 
     * Zenithar
     */
    
    
     
    if ( deathmatch )
    {
      send_to_char( "Not during a {RDeath Match{x!!\n\r", ch );
      return FALSE;
    }
    if (  ( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) )
       && ( !IS_NPC( ch ) && ( !IS_SET( ch->act, ACT_INTELLIGENT ) ) )   )
    {
	send_to_char( "You failed.\n\r", ch );
	return FALSE;
    }

    
    /* Check is ch screws up, and ends up in limbo... <grin> */

    if (  ( number_percent() < 15 ) 
       && ( !IS_NPC( ch ) )   )       /* 15% chance */
    {
       send_to_char( "{xYou get distracted, and appear in the middle of {dnowhere{x!\n\r", ch );   
       act( "{x$n begins to disperse into {Bghostly {xparticles, then disappears!!", ch, NULL, NULL, TO_ROOM );
       char_from_room( ch );
       char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
       if ( ch->riding != NULL )
       {
         char_from_room( ch->riding );
         char_to_room( ch->riding, get_room_index( ROOM_VNUM_LIMBO ) );
       }

       act( "{x$n coelesces from a {Bghostly{x image into the room.", ch, NULL, NULL, TO_ROOM );
       do_look( ch, "auto" );
       return TRUE;
    }
    
    act( "{x$n begins to disperse into {Bghostly {xparticles, then disappears!!", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, get_room_index( ROOM_VNUM_ETHEREAL_PLANE ) );
    if ( ch->riding != NULL )
    {
      char_from_room( ch->riding );
      char_to_room( ch->riding, get_room_index( ROOM_VNUM_ETHEREAL_PLANE ) );
    }

    act( "{x$n coelesces from a {Bghostly{x image into the room.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou enter the {BEthereal Plane{x!\n\r", ch );
    do_look( ch, "auto" );
    return TRUE;
}

bool spell_fireelem( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the elemental forces of {Rfire{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the elemental forces of {Rfire{x.", ch, obj, NULL, TO_CHAR );
   
   act( "{xA {Rbonfire{x appears, and an elemental steps from the flames!!", ch, obj, NULL, TO_ROOM );
   act( "{xA {Rbonfire{x appears, and an elemental steps from the flames!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_FIREELEM ) );
   char_to_room( summoned, ch->in_room );
   act( "$n leaps from the bonfire, drawing all the {Rflames{x into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_rune_fire( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_FIRE_RUNE ) )
    {
       send_to_char( "{xThere is already a {RFire{x Rune operating here!\n\r", ch );
       return FALSE;
    }
    
   if ( IS_SET( room->area->flags, AREA_NO_ROOM_AFF ) )
   {
     send_to_char( "A strange {RDeamon{x appears, waggles it's finger at you, and shakes its(?) head!\n\r", ch );
     return FALSE;
   }
    
    
    act( "$n draws a mystical {RFire{x Rune in the air.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou draw a {RFire{x Rune in the air.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level ) + number_range(2,20);
    raf.level     = level;
    raf.bitvector = ROOM_BV_FIRE_RUNE;
    raf.caster    = ch;
    raf.modifier  = (level * 3) - number_range(10, 50);
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 
bool spell_rune_poison( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_POISON_RUNE ) )
    {
       send_to_char( "{xThere is already a {dPoison{x Rune operating here!\n\r", ch );
       return FALSE;
    }
    
   if ( IS_SET( room->area->flags, AREA_NO_ROOM_AFF ) )
   {
     send_to_char( "A strange {RDeamon{x appears, waggles it's finger at you, and shakes its(?) head!\n\r", ch );
     return FALSE;
   }
    
    
    act( "$n draws a mystical {dPoison{x Rune in the air.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou draw a {dPoison{x Rune in the air.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level ) + number_range(2,20);
    raf.level     = level;
    raf.bitvector = ROOM_BV_POISON_RUNE;
    raf.caster    = ch;
    raf.modifier  = 0;
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 
bool spell_rune_shock( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_SHOCK_RUNE ) )
    {
       send_to_char( "{xThere is already a {BShock{x Rune operating here!\n\r", ch );
       return FALSE;
    }
   if ( IS_SET( room->area->flags, AREA_NO_ROOM_AFF ) )
   {
     send_to_char( "A strange {RDeamon{x appears, waggles it's finger at you, and shakes its(?) head!\n\r", ch );
     return FALSE;
   }
    
    
    
    act( "$n draws a mystical {BShock{x Rune in the air.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou draw a {BShock{x Rune in the air.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level ) + number_range(2,20);
    raf.level     = level;
    raf.bitvector = ROOM_BV_SHOCK_RUNE;
    raf.caster    = ch;
    raf.modifier  = (level * 3) - number_range(10, 50);
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 

bool spell_healing_light( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_HEAL_REGEN ) )
    {
       send_to_char( "{xThere is already a {mHealing Light{x operating here!\n\r", ch );
       return FALSE;
    }
    
    
    
    act( "{xA majestic {mHealing Light{x fills the room.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou fill the room with a majestic {mHealing Light{x.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level/8 ) + number_range(2,10);
    raf.level     = level;
    raf.bitvector = ROOM_BV_HEAL_REGEN;
    raf.caster    = ch;
    raf.modifier  = 0;
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 
bool spell_wither_shadow( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_HEAL_STEAL ) )
    {
       send_to_char( "{xThere is already a {dWithering Shadow{x operating here!\n\r", ch );
       return FALSE;
    }
    
    if ( IS_SET( room->area->flags, AREA_NO_ROOM_AFF ) )
   {
     send_to_char( "A strange {RDeamon{x appears, waggles it's finger at you, and shakes its(?) head!\n\r", ch );
     return FALSE;
   }
   
    
    act( "{xA deadly {dWithering Shadow{x fills the room.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou fill the room with a deadly {dWithering Shadow{x.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level/8 ) + number_range(2,10);
    raf.level     = level;
    raf.bitvector = ROOM_BV_HEAL_STEAL;
    raf.caster    = ch;
    raf.modifier  = 0;
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 

bool spell_mana_flare( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_MANA_REGEN ) )
    {
       send_to_char( "{xThere is already a {RMana Flare{x operating here!\n\r", ch );
       return FALSE;
    }
    
    
    
    act( "{xA powerful {RMana Flare{x encompasses the room.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou fill the room with a powerful {RMana Flare{x.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level/8 ) + number_range(2,10);
    raf.level     = level;
    raf.bitvector = ROOM_BV_MANA_REGEN;
    raf.caster    = ch;
    raf.modifier  = 0;
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 

bool spell_mana_drain( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_MANA_STEAL ) )
    {
       send_to_char( "{xThere is already a {dMana Drain{x operating here!\n\r", ch );
       return FALSE;
    }
   if ( IS_SET( room->area->flags, AREA_NO_ROOM_AFF ) )
   {
     send_to_char( "A strange {RDeamon{x appears, waggles it's finger at you, and shakes its(?) head!\n\r", ch );
     return FALSE;
   }
    
    
    
    act( "{xA mind-sapping {dMana Drain{x fills the room.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou fill the room with a mind-sapping {dMana Drain{x.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level/8 ) + number_range(2,10);
    raf.level     = level;
    raf.bitvector = ROOM_BV_MANA_STEAL;
    raf.caster    = ch;
    raf.modifier  = 0;
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 
bool spell_cage( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_HOLD ) )
    {
       send_to_char( "{xThere is already a {GCage{x operating here!\n\r", ch );
       return FALSE;
    }
    
    
    
    act( "{xA paralysing {GCage{x surrounds the room.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou surround the room with a paralyzing {GCage{x.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level/20 ) + number_range(2,10);
    raf.level     = level;
    raf.bitvector = ROOM_BV_HOLD;
    raf.caster    = ch;
    raf.modifier  = 0;
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 
bool spell_cloak_flaming( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    AFFECT_DATA af;
 

    if ( is_affected( ch, sn ) )
     return FALSE;


   
    af.type      = sn;
    af.duration  = ch->level / 8;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CLOAK_FLAMING;
    affect_to_char( ch, &af );

    return TRUE;
}

 
bool spell_cloak_reflect( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    AFFECT_DATA af;
  


    if ( is_affected( ch, sn ) )
     return FALSE;

  
    af.type      = sn;
    af.duration  = ch->level / 8;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CLOAK_REFLECTION;
    affect_to_char( ch, &af );

    return TRUE;
}

bool spell_cloak_absorb( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    AFFECT_DATA af;


    if ( is_affected( ch, sn ) )
     return FALSE;


    
    af.type      = sn;
    af.duration  = ch->level / 8;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CLOAK_ABSORPTION;
    affect_to_char( ch, &af );

    return TRUE;
}

bool spell_cloak_adept( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    AFFECT_DATA af;


    if ( is_affected( ch, sn ) )
     return FALSE;

    if ( IS_NPC( ch ) )
      return FALSE;

    
    af.type      = sn;
    af.duration  = get_psuedo_level( ch ) / 5;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CLOAK_ADEPT;
    affect_to_char( ch, &af );

    return TRUE;
}

bool spell_cloak_regen( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    AFFECT_DATA af;


    if ( is_affected( ch, sn ) )
     return FALSE;


    
    af.type      = sn;
    af.duration  = get_psuedo_level( ch ) / 10;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CLOAK_REGEN;
    affect_to_char( ch, &af );

    return TRUE;
}


bool spell_room_dispel( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA *raf;
    ROOM_AFFECT_DATA *raf_next;
    int  		chance = 0;


    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_INTELLIGENT ) )
      chance = 1000;
     else chance = ch->level + 20;
    
    
    act( "$n gestures demonically at the magical spells around the room.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou gesture demonically!\n\r", ch );
    
    
      for ( raf = room->first_room_affect; raf != NULL; raf = raf_next )        
      {
	 raf_next = raf->next;
	 if ( number_percent() < chance )
	 {
	    r_affect_remove( room, raf );
	    chance = ( 2 * chance ) / 3; 
	 }
	 else
	    break;
      }     
    
    return TRUE;
 }
 

bool spell_throw_star( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *victim = (CHAR_DATA *)vo;
   
   AFFECT_DATA af;

   if ( !sp_damage( obj, ch, (CHAR_DATA *) vo,
        dice( 5, level/2 ) + dice(5, level/2) + level, REALM_POISON,  sn, TRUE ) )
     return TRUE;
    
   if ( !saves_spell( level, victim ) )
   {
    af.type      = sn;
    af.duration  = 12 + (level/10);
    af.location  = APPLY_DAMROLL;
    af.modifier  = -level/3;
    af.bitvector = AFF_POISON;
    affect_join( victim, &af );
    send_to_char( "You feel very sick.\n\r", victim );
    act( "$n looks very sick.", victim, NULL, NULL, TO_ROOM );
   }

    return TRUE;


    
    
}
bool spell_soul_net( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *room;
    ROOM_AFFECT_DATA raf;
    
    room = ch->in_room;
    
    if ( room == NULL )
       return FALSE;
       
    if ( IS_SET( room->affected_by, ROOM_BV_SOUL_NET ) )
    {
       send_to_char( "{xThere is already a {dSoul Net{x operating here!\n\r", ch );
       return FALSE;
    }
    
    
    
    act( "{xA demonic {dSoul Net{x enshrouds the room.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou enshroud the room with a demonic {dSoul Net{x.\n\r", ch );
    
    raf.type      = sn;
    raf.duration  = ( level/8 ) ;
    raf.level     = level;
    raf.bitvector = ROOM_BV_SOUL_NET;
    raf.caster    = ch;
    raf.modifier  = 0;
    
    affect_to_room( room, &raf );
    return TRUE;
 }
 
bool spell_condense_soul( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    OBJ_DATA *ob = (OBJ_DATA *) vo;
   
    if ( IS_NPC( ch ) )
      return FALSE;

    if ( ob->item_type != ITEM_SOUL )
    {
      send_to_char( "That is not a soul!.\n\r", ch );
      return FALSE;
    }
   
    if ( ob->level < 80 )
    {
      send_to_char( "Bah!! That soul is to weak to use!!\n\r", ch );
      return FALSE;
    }
    extract_obj(  ob );
    ob = create_object( get_obj_index( OBJ_VNUM_SOUL_POTION ), level );
    obj_to_char( ob, ch );
 
    act( "{x$n gestures diabolically, and his captured soul condenses into a{dSoul Potion{x.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "{xYou condense the soul and some of your life force into a {dSoul potion{x.\n\r", ch );
    ch->max_hit -= 75;
    ch->pcdata->hp_from_gain -= 75;
   
    return TRUE;
}


bool spell_restoration( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
  char buf[MAX_STRING_LENGTH];
  if ( IS_NPC( ch ) ) 
   return FALSE;

  ch->hit = ch->max_hit;
  ch->mana = ch->max_mana;
  ch->move = ch->max_move;
  if ( !IS_NPC( ch ) )
    ch->pcdata->bloodlust = ch->pcdata->bloodlust_max;

  send_to_char( "{RTHe life force of tha captured soul restores you!\n\r", ch );
  sprintf( buf, " %s has used a restoration spell.\n\r", ch->name );
  monitor_chan( buf, MONITOR_BAD );
  return TRUE;
}

bool spell_infuse( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    OBJ_DATA *ob = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    OBJ_DATA *obj_soul;
    if ( IS_NPC( ch ) )
      return FALSE;

    /* Quick way to stop imms (Bash?) enchanting weapons for players */
    if ( IS_IMMORTAL( ch ) && ch->level != 85 )
    {
       send_to_char( "Nothing Happens.\n\r", ch );
       return FALSE;
    }
    if ( IS_NPC( ch ) )
      return FALSE;
    if ( ( obj_soul = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL )
      if ( ( obj_soul = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) == NULL )
    {
      send_to_char( "{xYou must be holding a {RSoul{x to cast this spell!!\n\r", ch );
      return FALSE;
    }
    if ( obj_soul->item_type != ITEM_SOUL )
    {
      send_to_char( "{xYou are not holding a {RSoul{x!!\n\r", ch );
      return FALSE;
    }

    if ( ob->item_type != ITEM_WEAPON )
    {
      send_to_char( "That is not a weapon!.\n\r", ch );
      return FALSE;
    }
   
   for ( paf = ob->first_apply; paf != NULL; paf = paf->next )
   {
      if ( paf->type == sn )
      {
        send_to_char( "That weapon is already infused with a soul!.\n\r", ch );
        return FALSE;
      }
   }
   if ( ch->pcdata->hp_from_gain < 100 )
   {
     send_to_char( "You have exhausted your life force, and are unable to" \
        " control the necromantic forces necessary for this act.\n\r", ch );
     return FALSE;
   }
    GET_FREE(paf, affect_free);
    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_DAMROLL;
    paf->modifier       = ( obj_soul->level/8 );
    paf->bitvector      = 0;
    paf->caster		= NULL;
    LINK(paf, ob->first_apply, ob->last_apply, next, prev);

    GET_FREE(paf, affect_free);
    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_AC;
    paf->modifier       = -(obj_soul->level/2);
    paf->bitvector      = 0;
    paf->caster		= NULL;
    LINK(paf, ob->first_apply, ob->last_apply, next, prev);
    
    GET_FREE(paf, affect_free);
    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_MANA;
    paf->modifier       = obj_soul->level/3;
    paf->bitvector      = 0;
    paf->caster		= NULL;
    LINK(paf, ob->first_apply, ob->last_apply, next, prev);


    GET_FREE(paf, affect_free);
    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_HITROLL;
    paf->modifier       = ( obj_soul->level/8 );
    paf->bitvector      = 0;
    paf->caster		= NULL;
    LINK(paf, ob->first_apply, ob->last_apply, next, prev);

    SET_BIT( ob->extra_flags, ITEM_NODISARM );
    SET_BIT( ob->extra_flags, ITEM_NODROP );
    SET_BIT( ob->extra_flags, ITEM_NO_AUCTION );
    SET_BIT( ob->extra_flags, ITEM_NOSAC );
    SET_BIT( ob->extra_flags, ITEM_NOLOOT );
    SET_BIT( ob->extra_flags, ITEM_ANTI_GOOD );
    SET_BIT( ob->extra_flags, ITEM_NOLOOT );

    ob->obj_fun = obj_fun_lookup( "objfun_infused_soul" );
    extract_obj(  obj_soul );
 
    act( "{x$n gestures diabolically, and his captured {Rsoul{x is infused into his $P.", ch, NULL, ob, TO_ROOM );
    send_to_char( "{xYou condense the soul and some of your life force into your weapon.\n\r", ch );
    ch->max_hit -= 100;  
    ch->pcdata->hp_from_gain -= 100;
    do_save( ch, "" ); 
        return TRUE;
}

bool spell_holy_light( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    CHAR_DATA *gch;

    for ( gch = ch->in_room->first_person; gch != NULL; gch = gch->next_in_room )
    {
	if ( !is_same_group( ch, gch ) )
	      
	    continue;
	act( "$N is invigorated by the light shining off of $n!", ch, NULL, gch, TO_NOTVICT );
	send_to_char( "You feel a warm light invigorate you!\n\r", gch );
	gch->hit = UMIN( gch->max_hit, ( gch->hit + number_range( level *2 , level * 5 ) )   );
    }
    send_to_char( "You invigorate the troops!\n\r", ch );

    return TRUE;
}
bool spell_holy_armor( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return FALSE;
    af.type      = sn;
    af.duration  = 12;
    af.modifier  = -80 -get_psuedo_level( ch )/5;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "Your armor is now blessed!.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return TRUE;
}

bool spell_divine_intervention( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    heal = UMAX( 600, number_range( level * 4, level * 10 ) );
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );
    send_to_char( "You feel the hand of your God invigorate your soul!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return TRUE;
}

bool spell_earthelem( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the elemental forces of {yearth{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the elemental forces of {yearth{x.", ch, obj, NULL, TO_CHAR );
   
   act( "{xA huge mound of {yearth{x appears, and an elemental steps from the boulders!!", ch, obj, NULL, TO_ROOM );
   act( "{xA huge mound of {yearth{x appears, and an elemental steps from the boulders!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_EARTHELEM ) );
   char_to_room( summoned, ch->in_room );
   act( "{x$n emerges from the mound, assuming the {yearth{x into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_iron_golem( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "{x$n calls upon the {Calchemical{x forces of {dmetal{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the {Calchemical{x forces of {dmetal{x.", ch, obj, NULL, TO_CHAR );
   
   act( "{xA large slab of {diron{x appears, and a golem steps from the mass!!", ch, obj, NULL, TO_ROOM );
   act( "{xA large slab of {diron{x appears, and a golem steps from the mass!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_IRON_GOLEM ) );
   char_to_room( summoned, ch->in_room );
   act( "{x$n mutates from the {dslab of iron{x, drawing all the metal into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_soul_thief( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;


   act( "$n calls upon the {dNegative Plane{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the {dNegative Plane{x.", ch, obj, NULL, TO_CHAR );
   
   act( "{xThe ground opens beneath you, and a {BSoul {RThief{x crawls out of the graveyard filth!!", ch, obj, NULL, TO_ROOM );
   act( "{xThe ground opens beneath you, and a {BSoul {RThief{x crawls out of the graveyard filth!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_SOUL_THIEF ) );
   char_to_room( summoned, ch->in_room );
   act( "$n Stands erect, and bow's towards it's master.", summoned, NULL, NULL, TO_ROOM );
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_holy_avenger( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the holy forces of {Wlight{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the holy forces of {Wlight{x.", ch, obj, NULL, TO_CHAR );
   
   act( "{xA shimmering halo appears, and a {YHoly {WAvenger{x steps from the light!!", ch, obj, NULL, TO_ROOM );
   act( "{xA shimmering halo appears, and a {YHoly {WAvenger{x steps from the light!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_HOLY_AVENGER ) );
   char_to_room( summoned, ch->in_room );
   act( "$n steps from the light, drawing all the power into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_diamond_golem( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;
   CHAR_DATA *och;
   CHAR_DATA *och_next;

    sh_int num_followers = 0, max_orders = 0;
    max_orders = ( get_curr_int( ch ) / 9 );
    for ( och = ch->in_room->first_person; och != NULL; och = och_next )
    {
	    och_next = och->next_in_room;
      if (  ( och->is_free == FALSE )
         && ( IS_NPC( och ) )
         && ( IS_AFFECTED( och, AFF_CHARM ) )
         && ( och->master == ch )  )
      {  
        num_followers++;

        if ( num_followers <= max_orders )
        {
         continue;
        }
        else
        {
          send_to_char( "{RYou can't control anymore followers!!{x", ch );
          return 0;
        }
     }
   }

   act( "{x$n calls upon the {Calchemical{x forces of {Ylight{x!", ch, obj, NULL, TO_ROOM );
   act( "{xYou call upon the {Calchemical{x forces of {Ylight{x.", ch, obj, NULL, TO_CHAR );
   
   act( "A huge gemstone appears, and a golem steps from the diamond!!", ch, obj, NULL, TO_ROOM );
   act( "A huge gemstone appears, and a golem steps from the diamond!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_DIAMOND_GOLEM ) );
   char_to_room( summoned, ch->in_room );
   act( "$n floods from the pool, drawing all the water into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}
bool spell_summon_pegasus( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the holy forces of {Wlight{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the holy forces of {Wlight{x.", ch, obj, NULL, TO_CHAR );
   
   act( "{xA shimmering pyramid appears, and a {mMagestic {WPegasus{x steps from the light!!", ch, obj, NULL, TO_ROOM );
   act( "{xA shimmering pyramid appears, and a {mMagestic {WPagsus{x steps from the light!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_PEGASUS ) );
   char_to_room( summoned, ch->in_room );
   act( "$n steps from the light, drawing all the power into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_summon_nightmare( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the unholy forces of {REvil{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the unholy forces of {REvil{x.", ch, obj, NULL, TO_CHAR );
   
   act( "{xA flaming pit appears, and a {RDemonic {dNightmare{x steps from the heat!!", ch, obj, NULL, TO_ROOM );
   act( "{xA flaming pit appears, and a {RDemonic {dNightmare{x steps from the heat!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_NIGHTMARE ) );
   char_to_room( summoned, ch->in_room );
   act( "$n steps from the flames, drawing all the power into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_summon_beast( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the elemental forces of {BAir{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the elemental forces of @@'Air{x.", ch, obj, NULL, TO_CHAR );
   
   act( "{xA whirlwind appears, and an {BElemental {CBeast{x steps from the winds!!", ch, obj, NULL, TO_ROOM );
   act( "{xA whirlwind appears, and an {BElemental {CBeast{x steps from the winds!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_ELEM_BEAST ) );
   char_to_room( summoned, ch->in_room );
   act( "$n steps from the whirlwind, drawing all the power into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_summon_devourer( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the psychic forces of the {GMind{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the psychic forces of the {GMind{x!", ch, obj, NULL, TO_CHAR );
   
   act( "{xA vortex appears, and an {GIntellect {yDevourer{x steps from the circle!!", ch, obj, NULL, TO_ROOM );
   act( "{xA vortex appears, and an {GIntellect {yDevourer{x steps from the circle!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_INT_DEVOURER ) );
   char_to_room( summoned, ch->in_room );
   act( "$n steps from the vortex, drawing all the power into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}
bool spell_summon_shadow( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *summoned;

   act( "$n calls upon the shadowy forces of {dDarkness{x!", ch, obj, NULL, TO_ROOM );
   act( "You call upon the shadowy forces of {dDarkness{x!", ch, obj, NULL, TO_CHAR );
   
   act( "{xA tunnel of shadow appears, and a {dShadow {rHound{x steps from the depths!!", ch, obj, NULL, TO_ROOM );
   act( "{xA tunnel of shadow appears, and a {dShadow {rHound{x steps from the depths!!", ch, obj, NULL, TO_CHAR );
   
   summoned = create_mobile( get_mob_index( MOB_VNUM_SHADOW_HOUND ) );
   char_to_room( summoned, ch->in_room );
   act( "$n steps from the tunnel, drawing all the power into it's body.", summoned, NULL, NULL, TO_ROOM );
   
 /* don't think we need these  
   summoned->level    = 40;	
   summoned->max_hit  = dice( 8, 40 );
   summoned->hit      = summoned->max_hit;
   summoned->max_move = dice( 10, 40 );
   summoned->move     = summoned->max_move;  
   
   */
   SET_BIT( summoned->act, ACT_PET );
   SET_BIT( summoned->affected_by, AFF_CHARM );
   summoned->extract_timer = get_psuedo_level( ch )/3;
   add_follower( summoned, ch );
   return TRUE;
}

bool spell_lava_burst( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *victim = (CHAR_DATA *)vo;
   float   save_mod = 1.0;   

   if ( saves_spell( level, victim ) )
	save_mod = .75;


   
    if ( number_range( 0, 100 ) < 50 ) 
    {
      OBJ_DATA * heated_item = NULL;
      OBJ_DATA * prev_carried = NULL;

      for ( heated_item = victim->first_carry; heated_item!= NULL; heated_item = heated_item->next_in_carry_list )
      {
        if ( heated_item->wear_loc == WEAR_NONE )
          continue;
        prev_carried = heated_item;
        if ( number_range( 0, 100 ) < 25 )
          continue;
        else
          break;

      }
      if ( heated_item == NULL )
      {
        if ( prev_carried != NULL )
        {
          SET_BIT( prev_carried->item_apply, ITEM_APPLY_HEATED );
          return TRUE;
        }
        else
        {
          return TRUE;
        }
      }
      else
      {
        SET_BIT( heated_item->item_apply, ITEM_APPLY_HEATED );
        return TRUE;
      }

    }

   sp_damage(obj,  ch, (CHAR_DATA *) vo,
      number_range( get_psuedo_level( ch ) * 2, get_psuedo_level( ch ) * 4 )  * save_mod,
      REALM_FIRE, sn, TRUE );

    return TRUE;

  
    
}

bool spell_heat_armor( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *victim = (CHAR_DATA *)vo;
   
   
    if ( saves_spell( level, victim ) && ( number_range( 0, 99 ) < 50 ) )
      return TRUE;
    {
      OBJ_DATA * heated_item = NULL;
      OBJ_DATA * prev_carried = NULL;

      for ( heated_item = victim->first_carry; heated_item!= NULL; heated_item = heated_item->next_in_carry_list )
      {
        if ( heated_item->wear_loc == WEAR_NONE )
          continue;
        prev_carried = heated_item;
        if ( number_range( 0, 100 ) < 25 )
          continue;
        else
          break;

      }
      if ( heated_item == NULL )
      {
        if ( prev_carried != NULL )
        {
          if (  ( !IS_NPC( victim ) && IS_WOLF( victim ) )
             && ( !IS_RAGED( victim ) && !IS_SHIFTED( victim ) )  )

          SET_BIT( prev_carried->item_apply, ITEM_APPLY_HEATED );
          return TRUE;
        }
        else
        {
          return TRUE;
        }
      }
      else
      {
          if (  ( !IS_NPC( victim ) && IS_WOLF( victim ) )
             && ( !IS_RAGED( victim ) && !IS_SHIFTED( victim ) )  )

        SET_BIT( heated_item->item_apply, ITEM_APPLY_HEATED );
        return TRUE;
      }

    }


    return TRUE;

  
    
}

bool spell_retri_strike( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA * staff_obj = NULL;

    if ( ( staff_obj = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) == NULL )
      if ( ( staff_obj = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL )
    {
      send_to_char( "You must be holding a {Gstaff{x for this spell!\n\r", ch );
      return FALSE;
    }

    if ( staff_obj->item_type != ITEM_STAFF )
    {
      send_to_char( "That is not a staff you are holding, you fool!!\n\r", ch );
      return FALSE;
    }


    if ( obj == NULL )
    {
       send_to_char( "The earth trembles beneath your feet!\n\r", ch );
       act( "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
       act( "$p vibrates violently, making the earth tremble!", ch, obj, NULL, TO_CHAR );      
       act( "$p vibrates violenty, making the earth around $n tremble!", ch, obj, NULL, TO_ROOM );
    }
    CREF( vch_next, CHAR_NEXT );

    for ( vch = first_char; vch != NULL; vch = vch_next )
    {
      vch_next        = vch->next;
      if ( vch->in_room == NULL )
        continue;
      if ( vch->in_room == ch->in_room )
      {
        if ( vch != ch )
        {
          OBJ_DATA * heated_item = NULL;
          OBJ_DATA * prev_carried = NULL;

          act( "{W$n {Wis {Rimmolated{x by the fury released from the $p{W, and falls to the ground!", vch, staff_obj, NULL, TO_ROOM );
          act( "{WYou are {Rimmolated{x by the fury released from the $p{W, and fall to the ground!", vch, staff_obj, NULL, TO_CHAR );
          if ( !sp_damage( obj, ch, vch, number_range( staff_obj->level * 6, staff_obj->level * 10 ),
           REALM_LIGHT, sn, FALSE ) )
            continue;
          for ( heated_item = vch->first_carry; heated_item != NULL; heated_item = heated_item->next_in_carry_list )
          {
            if ( heated_item->wear_loc == WEAR_NONE )
              continue;
            prev_carried = heated_item;
            if ( number_range( 0, 100 ) < 25 )
              continue;
            else
              break;

          }
          if ( heated_item == NULL )
          {
            if ( prev_carried != NULL )
            {
              if (  ( !IS_NPC( vch ) && IS_WOLF( vch ) )
                 && ( !IS_RAGED( vch ) && !IS_SHIFTED( vch ) )  )
                SET_BIT( prev_carried->item_apply, ITEM_APPLY_HEATED );
         
            }
        
          }
          else
          {
              if (  ( !IS_NPC( vch ) && IS_WOLF( vch ) )
                 && ( !IS_RAGED( vch ) && !IS_SHIFTED( vch ) )  )
                SET_BIT( heated_item->item_apply, ITEM_APPLY_HEATED );
          }
        }
        continue;
	    }
	    else
	    {
	       act( "{W$n {Wis protected by the fury of the {W$p{W.", vch, staff_obj, NULL, TO_ROOM );
         continue;
	    }
      if ( vch->in_room->area == ch->in_room->area )
        send_to_char( "{WSuddenly, a {Ybright flash{W sears your eyes, then is gone.{x\n\r", vch );
    }

    CUREF( vch_next );
    extract_obj( staff_obj );
    return TRUE;
}



bool spell_creature_bond( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *victim = (CHAR_DATA *)vo;
   CHAR_DATA	* master;
   if ( IS_NPC( ch ) )
   {
     send_to_char( "Not for Npcs.\n\r", ch );
     return FALSE;
   }

   if (  ( !IS_NPC( victim ) )
      || ( !IS_SET( victim->affected_by, AFF_CHARM ) )  )
   {
     send_to_char( "That is not a charmed creature!\n\r", ch );
     return FALSE;
   }
   master = victim->master ? victim->master : victim;
   if (  ( master != NULL )
      && ( get_psuedo_level( ch ) < ( get_psuedo_level( master ) - 20 ) )  )
   {
     send_to_char( "The current bond is too strong for you to overcome.\n\r", ch );
     return FALSE;
   }

   if ( number_range( 0, 105 ) < ( level + ( get_psuedo_level( ch ) - get_psuedo_level( master ) )  ) )
   {
     if ( saves_spell( level, victim ) )
     {
       do_say( victim, "How dare you!, I LOVE my master!" );
       multi_hit( victim, ch, TYPE_UNDEFINED );
       return TRUE;
     }
     stop_follower( victim );
     SET_BIT( victim->act, ACT_PET );
     SET_BIT( victim->affected_by, AFF_CHARM );
     victim->extract_timer = get_psuedo_level( ch )/3;
     add_follower( victim, ch );
   }
   else
   {
     do_say( victim, "How dare you!, I LOVE my master!" );
     multi_hit( victim, ch, TYPE_UNDEFINED );
     return TRUE;
   }

   return TRUE;
}

bool spell_corrupt_bond( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
   CHAR_DATA *victim = (CHAR_DATA *)vo;
   CHAR_DATA *master = NULL;
   
   if ( IS_NPC( ch ) )
   {
     send_to_char( "Not for Npcs.\n\r", ch );
     return FALSE;
   }

   if (  ( !IS_NPC( victim ) )
      || ( !IS_SET( victim->affected_by, AFF_CHARM ) )  )
   {
     send_to_char( "That is not a charmed creature!\n\r", ch );
     return FALSE;
   }
   master = victim->master ? victim->master : victim;
   if (  ( master != NULL )
      && ( get_psuedo_level( ch ) < ( get_psuedo_level( master ) - 20 ) )  )
   {
     send_to_char( "The current bond is too strong for you to corrupt.\n\r", ch );
     return FALSE;
   }

   if ( number_range( 0, 105 ) < ( level + ( get_psuedo_level( ch ) - get_psuedo_level( master ) )  ) )
   {
     if ( saves_spell( level, victim ) )
     {
       do_say( victim, "How dare you!, I LOVE my master!" );
       multi_hit( victim, ch, TYPE_UNDEFINED );
       return TRUE;
     }
     
     stop_follower( victim );

     if ( victim->in_room == master->in_room )
     {
       do_say( victim, "Now I shall have my revenge for being charmed!!!" );
       multi_hit( victim, master, TYPE_UNDEFINED );
       return TRUE;
     }
     else
     {
       do_say( victim, "AARRGH!  I HATE being charmed! Now I shall have my revenge!" );
       victim->hunt_flags = HUNT_WORLD|HUNT_OPENDOOR|HUNT_PICKDOOR|HUNT_UNLOCKDOOR|HUNT_INFORM;
       set_hunt(victim,NULL,master,NULL,HUNT_WORLD|HUNT_OPENDOOR|HUNT_PICKDOOR|HUNT_UNLOCKDOOR|HUNT_INFORM,HUNT_MERC|HUNT_CR);
       return TRUE;
     }
   }
   else
   {
     do_say( victim, "How dare you!, I LOVE my master!" );
     multi_hit( victim, ch, TYPE_UNDEFINED );
     return TRUE;
   }

   return TRUE;
}
