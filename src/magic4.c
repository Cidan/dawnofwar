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
#ifndef DEC_MAGIC_H
#include "magic.h"
#endif

extern   bool    deathmatch;


/*
 * This file should contain:
 *	o Adept Spells
 *	o additional remort spells
 */

bool spell_fireblast( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


    if ( obj==NULL )
    {
      dam         = ( get_psuedo_level( ch ) /2 )  
                    +dice( ( get_psuedo_level( ch )/6 + ADEPT_LEVEL(ch)/2 ), 10 )
                    +dice( ( get_psuedo_level( ch )/6 + ADEPT_LEVEL(ch)/2 ), 10 );
      act( "{wA blast of {Rfire{w flies from $n's hands!{x", ch, NULL, NULL, TO_ROOM );
      send_to_char( "{wA blast of {Rfire{w flies from your hands!{x\n\r", ch );
    }
    else
    {
      dam = dice( level/4, 15 );
      act( "{wA blast of {Rfire{w flies from $p!{x", ch, obj, NULL, TO_ROOM );
      act( "{wA blast of {Rfire{w flies from $p!{x", ch, obj, NULL, TO_CHAR );
    }
    if ( saves_spell( level, victim ) )
  	dam /= 2;     
    act( "{w$n is struck by the blast of {Rfire{w!!{x", victim, NULL, NULL, TO_ROOM );
    send_to_char( "{wYou are struck by the {Rfire {wblast!!{x\n\r", victim );
    sp_damage( obj,  ch, victim, dam, REALM_FIRE, sn, TRUE );
    return TRUE;
}

bool spell_shockstorm( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


    if ( obj==NULL )
    {
      dam         = ( get_psuedo_level( ch ) /2 )  
                    +dice( ( get_psuedo_level( ch )/5 + ADEPT_LEVEL(ch)/2 ), 10 )
                    +dice( ( get_psuedo_level( ch )/5 + ADEPT_LEVEL(ch)/2 ), 10 );
      act( "{wA storm of {Bsparks{w flies from $n's hands!{x", ch, NULL, NULL, TO_ROOM );
      send_to_char( "{wA storm of {Bsparks{w flies from your hands!{x\n\r", ch );
    }
    else
    {
      dam = dice( level/4, 20 );
      act( "{wA storm of {Bsparks{w flies from $p!{x", ch, obj, NULL, TO_ROOM );
      act( "{wA storm of {Bsparks{w flies from $p!{x", ch, obj, NULL, TO_CHAR );
    }
    if ( saves_spell( level, victim ) )
  	dam /= 2;     
    act( "{w$n is struck by the storm of {Bsparks{w!!{x", victim, NULL, NULL, TO_ROOM );
    send_to_char( "{wYou are struck by the storm of {Bsparks{w!!{x\n\r", victim );
    sp_damage( obj,  ch, victim, dam, REALM_SHOCK, sn, TRUE );
    return TRUE;
}

bool spell_cone_cold( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


    if ( obj==NULL )
    {
      dam         = ( get_psuedo_level( ch ) /2 )  
                    +dice( ( get_psuedo_level( ch )/5 + ADEPT_LEVEL(ch)/2 ), 10 )
                    +dice( ( get_psuedo_level( ch )/5 + ADEPT_LEVEL(ch)/2 ), 10 );
      act( "{wA cone of {Ccold{w bursts forth from $n's hands!{x", ch, NULL, NULL, TO_ROOM );
      send_to_char( "{wA cone of {Ccold{w bursts forth from your hands!{x\n\r", ch );
    }
    else
    {
      dam = dice( level/4, 20 );
      act( "{wA cone of {Ccold{w bursts forth from $p!{x", ch, obj, NULL, TO_ROOM );
      act( "{wA cone of {Ccold{w bursts forth from $p!{x", ch, obj, NULL, TO_CHAR );
    }
    if ( saves_spell( level, victim ) )
  	dam /= 2;     
    act( "{w$n is struck by the cone of {Ccold{w!!{x", victim, NULL, NULL, TO_ROOM );
    send_to_char( "{wYou are struck by the cone of {Ccold{w!!{x\n\r", victim );
    sp_damage( obj,  ch, victim, dam, REALM_COLD, sn, TRUE );
    return TRUE;
}

bool spell_holy_wrath( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


    if ( obj==NULL )
    {
      dam         = ( get_psuedo_level( ch ) /2 )  
                    +dice( ( get_psuedo_level( ch )/4 + ADEPT_LEVEL(ch) ), 12 )
                    +dice( ( get_psuedo_level( ch )/4 + ADEPT_LEVEL(ch) ), 12 );
      act( "{wA coruscating sphere of {Ylight{w bursts forth from $n's hands!{x", ch, NULL, NULL, TO_ROOM );
      send_to_char( "{wA coruscating sphere of {Ylight{w bursts forth from your hands!{x\n\r", ch );
    }
    else
    {
      dam = dice( level/5, 20 );
      act( "{wA coruscating sphere of {Ylight{w bursts forth from $p!{x", ch, obj, NULL, TO_ROOM );
      act( "{wA coruscating sphere of {Ylight{w bursts forth from $p!{x", ch, obj, NULL, TO_CHAR );
    }
    if ( saves_spell( level, victim ) )
  	dam /= 2;     
    act( "{w$n is struck by the coruscating sphere of {Ylight{w!!{x", victim, NULL, NULL, TO_ROOM );
    send_to_char( "{wYou are struck by the coruscating sphere of {Ylight{w!!{x\n\r", victim );
    sp_damage( obj,  ch, victim, dam, REALM_HOLY, sn, TRUE );
    return TRUE;
}

bool spell_wraith_touch( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    float drain_mod = 1.0;

    if ( obj==NULL )
    {
      dam         = ( get_psuedo_level( ch ) /3 )  
                    +dice( ( get_psuedo_level( ch )/6 + ADEPT_LEVEL(ch) ), 8 )
                    + dice( ( get_psuedo_level( ch )/6 + ADEPT_LEVEL(ch) ), 8 );
      act( "{rA {dwraithlike hand {rleaps forth from $n!{x", ch, NULL, NULL, TO_ROOM );
      send_to_char( "{rA {dwraithlike hand {rleaps forth from your hands!{x\n\r", ch );
    }
    else
    {
      dam = dice( level/5, 20 );
      act( "{rA {dwraithlike hand {rleaps forth from $p!{x", ch, obj, NULL, TO_ROOM );
      act( "{rA {dwraithlike hand {rleaps forth from $p!{x", ch, obj, NULL, TO_CHAR );
    }
    if ( saves_spell( level, victim ) )
  	dam /= 2;     
    act( "{r$n is struck by the {dwraithlike hand {r!!{x", victim, NULL, NULL, TO_ROOM );
    send_to_char( "{rYou are struck by a {dwraithlike hand {r!!{x\n\r", victim );
    drain_mod = get_psuedo_level( ch )  * dam / 130;
    if ( sp_damage( obj,  ch, victim, dam, REALM_DRAIN | NO_REFLECT | NO_ABSORB, sn, TRUE ) )
      ch->hit = UMIN( ch->max_hit, (ch->hit +  drain_mod ) );
    return TRUE;
}

bool spell_thought_vise( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


    if ( obj==NULL )
    {
      dam         = ( get_psuedo_level( ch ) /2 )  
                    +dice( ( get_psuedo_level( ch )/3 + ADEPT_LEVEL(ch) ), 10 )
                   +dice( ( get_psuedo_level( ch )/3 + ADEPT_LEVEL(ch) ), 10 );
      act( "{GA crushing weight brushes your mind, then is gone.{x", ch, NULL, NULL, TO_ROOM );
      send_to_char( "{GYou clench your mind, crushing the thoughts of your foe.\n\r{x", ch );
    }
    else
    {
      dam = dice( level/5, 20 );
      act( "{GA crushing weight fills your mind.{x", ch, obj, NULL, TO_CHAR );
    }
    if ( saves_spell( level, victim ) )
  	dam /= 2;     
    act( "{GA crushing weight brushes your mind, then is gone.{x", victim, NULL, NULL, TO_ROOM );
    send_to_char( "{GA crushing weight fills your mind.{x", victim );
    sp_damage( obj,  ch, victim, dam, REALM_MIND | NO_REFLECT | NO_ABSORB, sn, FALSE );
    return TRUE;
}

bool spell_black_curse( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_CURSE) )
    {
      send_to_char( "They are already weakened!\n\r", ch );
      return FALSE;
    }
    if ( saves_spell( level, victim ) )
	    return TRUE;
    if ( saves_spell( level, victim ) )
	    return TRUE;
    af.type      = sn;
    af.duration  = 2 * (level / 8);
    af.location  = APPLY_HITROLL;
    af.modifier  = -1 * get_psuedo_level(ch)/12 * get_psuedo_level( victim )/10;
    af.bitvector = AFF_CURSE;
    affect_to_char( victim, &af );

    af.location  = APPLY_AC;
    af.modifier  = 7 * get_psuedo_level(ch)/12 * get_psuedo_level( victim )/10;
    affect_to_char( victim, &af );

    send_to_char( "{rA Cloud of {dDespair{r washes over you.{x\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


bool spell_cloak_misery( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    AFFECT_DATA af;
 

    if ( is_affected( ch, sn ) )
     return FALSE;


   
    af.type      = sn;
    af.duration  = get_psuedo_level( ch )/8;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    return TRUE;
}


bool spell_poison_quinine( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected(victim, sn) )
    {
      send_to_char( "They are already poisoned with quinine!\n\r", ch );
      return FALSE;
    }
    if ( saves_spell( level, victim ) )
      if ( saves_spell( level, victim ) )
        return TRUE;
    af.type      = sn;
    af.duration  = 2 * (level / 8);
    af.location  = APPLY_HITROLL;
    af.modifier  = -1 * get_psuedo_level(ch)/20 * get_psuedo_level( victim )/20;
    af.bitvector = AFF_POISON;
    affect_to_char( victim, &af );

    af.location  = APPLY_AC;
    af.modifier  = 7 * get_psuedo_level(ch)/20 * get_psuedo_level( victim )/20;
    affect_to_char( victim, &af );

    send_to_char( "@@You feel a dart pierce your neck.{x\n\r", victim );
    if ( ch != victim )
      send_to_char( "Ok.\n\r", ch );
    return TRUE;
}

bool spell_poison_arsenic( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected(victim, sn) )
    {
      send_to_char( "They are already poisoned with arsenic!\n\r", ch );
      return FALSE;
    }
    if ( saves_spell( level, victim ) )
      if ( saves_spell( level, victim ) )
        return TRUE;
    af.type      = sn;
    af.duration  = 2 * (level / 8);
    af.location  = APPLY_DAMROLL;
    af.modifier  = -1 * get_psuedo_level(ch)/20 * get_psuedo_level( victim )/20;
    af.bitvector = AFF_POISON;
    affect_to_char( victim, &af );

    af.location  = APPLY_AC;
    af.modifier  = 5 * get_psuedo_level(ch)/20 * get_psuedo_level( victim )/20;
    affect_to_char( victim, &af );

    send_to_char( "@@You feel a dart pierce your neck.{x\n\r", victim );
    if ( ch != victim )
      send_to_char( "Ok.\n\r", ch );
    return TRUE;
}

bool spell_sonic_blast( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam;
    dam = dice( get_psuedo_level( ch )/2, 3 ) + dice( get_psuedo_level( ch )/2, 3 );
    if ( saves_spell( level, victim ) )
      dam = dam*2/3;
    if ( sp_damage( obj, ch, victim, dam, REALM_SOUND | NO_REFLECT | NO_ABSORB,
         sn, TRUE ) )
    {
      if ( is_affected( ch, sn ) )
        return TRUE;
       af.type      = sn;
      af.duration  =  (level / 20);
      af.location  = APPLY_DAMROLL;
      af.modifier  = -1 * get_psuedo_level(ch)/15 * get_psuedo_level( victim )/15;
      af.bitvector = AFF_BLASTED;
      affect_to_char( victim, &af );

      af.type      = sn;
      af.duration  =  (level / 20);
      af.location  = APPLY_HITROLL;
      af.modifier  = -1 * get_psuedo_level(ch)/15 * get_psuedo_level( victim )/15;
      af.bitvector = AFF_BLASTED;
      affect_to_char( victim, &af );

    }
      return TRUE;
}
bool spell_mystical_focus( int sn, int level, CHAR_DATA *ch, void *vo, OBJ_DATA *obj )
{
    
    AFFECT_DATA af;
 

    if ( is_affected( ch, sn ) )
     return FALSE;


   
    af.type      = sn;
    af.duration  = get_psuedo_level( ch )/7;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    return TRUE;
}


