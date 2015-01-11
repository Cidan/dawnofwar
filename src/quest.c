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
#include "ack.h" 

/**** Local Functions ****/
CHAR_DATA *get_quest_target 	args( ( int min_level, int max_level ) );
CHAR_DATA *get_quest_giver	args( ( int min_level, int max_level ) );
OBJ_DATA  *load_quest_object    args( ( CHAR_DATA *target ) );
void	   clear_quest		args( ( void ) );


/* 17 messages, organised by blocks for each personality 
   indented messages are for when the target mob gets killed  */
struct	qmessage_type
{
	char * const message1;
	char * const message2;
};


 const struct qmessage_type  qmessages[4][17] =

 {
   {
      {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""},
	{"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""},
	{"", ""}
   },
 {     
	{ "{COh my! My %s {Chas been stolen from me, and I am too young to find it!",	 	"" },
	{ "{CWould someone please search for my %s{C? I'm sure that it will be much too hard for me to find.", 							"" },
	{ "{CWhat will I do without my %s{C?",								"" },
	{ "{CCan't anybody find my %s {Cfor me?",  							"" },
	{ "{CHelp me! My %s {Cis still missing!", 							"" },
	{ "{CKeep searching for my %s{C, and i'll ask my mom if she knows who stole it!",	"" },
	{ "{CDoes anyone know who stole my %s{C?", 							"" },
	{ "{CMy mom says %s{C stole my %s{C. I know it is too hard for me to get back. Oh my, what will I do?",
		"{CYeay!! It looks like whoever stole my %s {Cis now dead!! Thank you very much!" 					},
	{ "{CPlease, can you go kill %s{C? I need my %s{C!! I can't sleep without it!!",
		"{CWell, thank you for killing the nasty thief, but can you please return my %s {Cto me?" 				},
	{ "{CMy mom says %s{C stole my %s{C. I know it is too hard for me to get back. Oh my, what will I do?",
		"{CYeay!! It looks like whoever stole my %s {Cis now dead!! Thank you very much!" 					},
	{ "{CIt's time for my nap now, and %s still has my %s{C!! Can anyone please get it back for me?",
		"{CPlease, time is running out! Return my %s {Cto me!" 				},
	{ "{C%s{C is a real meanie for stealing my %s{C! Can you pretty pretty pretty please get it back for me?",
		"{CPlease, time is running out! Return my %s {Cto me!" 				},
	{ "{CIt's time for my nap now, and %s still has my %s{C!! Can anyone please get it back for me?",
		"{CPlease, time is running out! Return my %s {Cto me!" 				},
	{ "{COh my, I'll never get up to watch cartoons tomorrow now!  %s {Cis still holding my %s{C ransom, and I need it for my nap!",
		"{CHow can I sleep without my %s{C?"					},
	{ "{CI give up! %s {Ccan keep my %s {Cfor all I care! I didn't want to take a nap, anyway!", 
		"{CI give up! I never want to see my %s {Cagain!"					},
	{ "{CMommy, can I watch cartoons now, instead of taking a nap?? PLEASE??" , 				"" },
	{ "{COh THANK YOU, %s{C!! Now that I have my %s {Cagain, I can go sleepy sleep!",	"" }

    },


   {
	{ "Hmm, I seem to have lost my %s{B. Oh well, didn't really need it much, anyway.", 	"" },
	{ "I wonder where I could find another %s{B? Anyone have any ideas?", 							"" },
	{ "Where can my %s {Bhave gone?", 									"" },
	{ "I guess no one really cares, anyway, but I think I might need a %s {Blater.",  							"" },
	{ "I guess I should try and find my %s{B, but I really don't feel like it.", 						"" },
	{ "If anyone has an extra %s{B, I might be willing to reward them for it.", 			"" },
	{ "Doesn't anyone out there know where to find a %s{B?", 					"" },
	{ "Hmm, maybe %s{B knew something I didn't, and thought it was a good idea to steal my %s{B. Maybe he could use it, I know I can't.",
		"I guess my %s {Bdidn't help him much, since he is now dead!  I do miss it though." 					},
	{ "Hmm, maybe it IS worth something.  Could someone go kill %s{B and get my %s{B back for me?",
		"I guess my %s {Bdidn't help him much, since he is now dead!  I do miss it though." 					},
	{ "I would pay a lot if someone would kill %s {Band get my %s{B back. I don't really know where it went.",
		"Even though it's not worth very much, my %s {Bis kind of important to me. Oh, well, guess I will never see it again." 				},
	{ "Hmm, maybe it IS worth something.  Could someone go kill %s{B and get my %s{B back for me?",
		"I guess my %s {Bdidn't help him much, since he is now dead!  I do miss it though." 					},
	{ "I would pay a lot if someone would kill %s {Band get my %s{B back. I don't really know where it went.",
		"Even though it's not worth very much, my %s {Bis kind of important to me. Oh, well, guess I will never see it again." 				},
	{ "Oh well, since no one will help me, I guess %s {Bcan keep my %s{B.",
		"It must be cursed, since everyone who has it is dead. I don't think I want my %s {Bafter all!" 					},
	{ "Oh well, since no one will help me, I guess %s {Bcan keep my %s{B.",
		"It must be cursed, since everyone who has it is dead. I don't think I want my %s {Bafter all!" 					},
	{ "I give up! %s {Bcan keep my %s {Bfor all I care!", 
		"I give up! I never want to see my %s {Bagain!"					},
	{ "Well, I will stop asking now, but don't ever ask ME for any favors, ok?" , 				"" },
	{ "Well, looks like %s {Bhas recovered my %s {Bfor me. Not sure I want it anymore, but thanks anyway.", 						"" }

    },

/*
{
	{ "BANZAI! My %s {Bhas been stolen from me!  Will someone brave recover it?", 	"" },
	{ "Oh! Has no one found my %s {Bfor me yet?", 							"" },
	{ "Where can my %s {Bhave gone?", 									"" },
	{ "Can't anybody find my %s {Bfor me?",  							"" },
	{ "Help me! My %s {Bhas not yet been recovered!", 						"" },
	{ "Keep searching for my %s{B, and i'll find out who stole it!", 			"" },
	{ "Were there no witnesses to the theft of my %s?{B", 					"" },
	{ "It was %s {Bwho stole my %s {Bfrom me!  Someone help me!",
		"It looks like whoever stole my %s {Bis now dead!!" 					},
	{ "Please, time is running out! Recover my %s {Bfrom %s {Bfor me NOW!",
		"Please, time is running out! Return my %s {Bto me!" 				},
	{ "Please, time is running out! Recover my %s {Bfrom %s {Bfor me NOW!",
		"Please, time is running out! Return my %s {Bto me!" 				},
	{ "Please, time is running out! Recover my %s {Bfrom %s {Bfor me NOW!",
		"Please, time is running out! Return my %s {Bto me!" 				},
	{ "Please, time is running out! Recover my %s {Bfrom %s {Bfor me NOW!",
		"Please, time is running out! Return my %s {Bto me!" 				},
	{ "I give up! %s {Bcan keep my %s {Bfor all I care!",
		"I give up! I never want to see my %s {Bagain!" 					},
	{ "I give up! %s {Bcan keep my %s {Bfor all I care!",
		"I give up! I never want to see my %s {Bagain!"					},
	{ "I give up! %s {Bcan keep my %s {Bfor all I care!", 
		"I give up! I never want to see my %s {Bagain!"					},
	{ "Shoot! Just forget about recovering ANYTHING for me, ok?" , 				"" },
	{ "At Last! %s {Bhas recovered %s {Bfor me!", 						"" }

    },  */  

{
	{ "{RMuuaahhhaaahaaaa! Some puny mortal has stolen my %s{R!  I shall seek revenge!!", 	"" },
	{ "{RI shall send many minions to seek my %s{R! All that steal from me shall die!!", 							"" },
	{ "{RSO, you have defeated my servants.  I shall still regain my %s{R!!", 									"" },
	{ "{RI am prepared to reward well anyone that aids the return of my %s{R. Are any of you puny mortals willing to attempt my challenge?",  							"" },
	{ "{RIf you are worthy, I will grant many favors upon anyone that returns my %s{R.", 						"" },
	{ "{mMethlok{R, By the dark powers, I command you to seek my %s{R! Now, if any of you worthless mortals wish to attempt to return it, I shall grant you many powers!", 			"" },
	{ "{RI sense that {mMethlok{R is nearing the witless thief who stole my %s{R. Now, my vengence shall be sweet!", 					"" },
	{ "{RAhhh, my servant has returned, and informs me that %s {Rstole my %s{R. They shall be incinerated by the powers that I command!!!",
		"{mMethlok{R has informed me that the weakling that stole my %s {Bhas met his maker!!" 					},
	{ "{RAre none of you powerful enough to kill %s {Rand regain my %s{R? Bah!! Mortals are useless, except as side dishes!!",
		"{RThough my taste for blood has been satiated, my %s {Rstill evades my grasp!" 				},
	{ "{RAre none of you powerful enough to kill %s {R and regain my %s {R? Bah!! Mortals are useless, except as side dishes!!",
		"{RThough my taste for blood has been satiated, my %s {Rstill evades my grasp!" 				},
	{ "{RAre none of you powerful enough to kill %s {R and regain my %s {R? Bah!! Mortals are useless, except as side dishes!!",
		"{RThough my taste for blood has been satiated, my %s {Rstill evades my grasp!" 				},
	{ "{RI should have known that a powerless, puny mortal could never be the servant of my vengence against %s{R, or regain my %s{R!!",
		"{RI shall rain death upon all of you for refusing to return my %s{R!!!" 				},
	{ "{RI should have known that a powerless, puny mortal could never be the servant of my vengence against %s{R, or regain my %s{R!!",
		"{RI shall rain death upon all of you for refusing to return my %s{R!!!" 				},
	{ "{RI shall slay your brothers and poison your fields for refusing to seek %s {Rand return my %s{R!!!!",
		"{RThough my vengeance has been served, I shall drink your souls for your failure to return my %s{R!!!"					},
	{ "{RI shall slay your brothers and poison your fields for refusing to seek %s {Rand return my %s{R!!!!",
		"{RThough my vengeance has been served, I shall drink your souls for your failure to return my %s{R!!!"					},
	{ "{RDeath and great suffering shall be your punishment for failing me!!!?" , 				"" },
	{ "{RWell done.  It seems that %s {Rat least has a modicum of strength, unlike you worthless idiots who failed to return my %s{R! My curse shall lie upon you for the rest of your short days!", "" }

    }




  };


void do_quest( CHAR_DATA *ch, char *argument )
{
   extern bool quest;
   extern bool auto_quest;
   extern CHAR_DATA *quest_mob;
   extern CHAR_DATA *quest_target;
   extern OBJ_DATA *quest_object;
   extern int quest_timer;
   extern int quest_wait;
  
   char buf[MAX_STRING_LENGTH];
   char new_long_desc[MAX_STRING_LENGTH];
   
   if ( argument[0] == '\0' )	/* Display status */
   {
      if ( !quest )
      {
         send_to_char( "There is no quest currently running.\n\r", ch );
         if ( auto_quest )
            send_to_char( "Quests are currently running automatically.\n\r", ch );
         if ( quest_wait > 0 )
         {
            sprintf( buf, "The next quest may occur in %d minutes.\n\r", quest_wait );
            send_to_char( buf, ch );
         }
         return;
      }
      else
         send_to_char( "There is currently a quest running ", ch );
      
      if ( auto_quest )
         send_to_char( "(Automatically)", ch );
      
      send_to_char( "\n\rQuest Details:\n\r\n\r", ch );
      if ( quest_mob )
      {

        sprintf( buf, "The questing mobile is: %s [In Room %d]\n\r", 
           quest_mob->short_descr, quest_mob->in_room->vnum );
        send_to_char( buf, ch );
      }
      else
      {
        send_to_char( "The questing mobile is dead!\n\r", ch );
      }
      if ( quest_target )
      {
        sprintf( buf, "Target Mobile is: %s [In Room %d]\n\r",
           quest_target->short_descr, quest_target->in_room->vnum );
        send_to_char( buf, ch );
      }
      else
        send_to_char( "The target mobile is dead!\n\r", ch );

      sprintf( buf, "Target Object is: %s.\n\r", quest_object->short_descr );
      send_to_char( buf, ch );

      sprintf( buf, "Quest Object is worth: %d QP, %d Prac, %d GP\n\r",
         quest_object->value[0], quest_object->value[1],
         quest_object->value[2]);
      send_to_char( buf, ch );

      
      sprintf( buf, "The Quest has been running for %d/15 minutes.\n\r", quest_timer );
      send_to_char( buf, ch );
      
      return;
   }
   if ( !strcmp( argument, "stop" ) )
   {
     if ( quest )
     {
       sprintf( buf, "{xThe quest has been stopped by an {mImmortal{x. Please speak up if you have already gotten the item.\n\r" );
       do_echo( ch, buf );
       clear_quest();
     }
     return;
   }

   if ( !strcmp( argument, "start" ) )
   {
      DESCRIPTOR_DATA *d;
      int a = 80;
      int b = 0;
      sh_int player_count = 0, average_level= 0, total_levels = 0;
      
      /* generate a new quest! */
      if ( quest )
      {
         send_to_char( "There is already a quest running...\n\r", ch );
         return;
      }
      
      if ( auto_quest )
      {
         auto_quest = FALSE;
         send_to_char( "Automatic Quests now OFF.\n\r", ch );
      }
      
      /* Work out levels of currently playing folks */
      for ( d = first_desc; d; d = d->next )
      {
         if (  ( d->connected != CON_PLAYING )
            || ( IS_IMMORTAL( d->character )  ) )
            continue;
         player_count += 1;
         total_levels += d->character->level;
      }
      average_level = ( ( ( total_levels == 0 ) ? 30 : total_levels ) /
                        ( ( player_count == 0 ) ? 1 : player_count ) );
      a = average_level - 20;
      b = average_level + 20;
      quest_mob    = get_quest_giver(a, b);
      if ( quest_mob == NULL )
      {
         send_to_char( "Failed to find a quest mob\n\r", ch );
         return;
      }
      b = UMIN( b, 110 );
      a = UMAX( 20, a );

      quest_target = get_quest_target( a, b );
      if (  ( quest_target == NULL )
         || ( quest_target == quest_mob )  )
      {
         send_to_char( "Failed to find a quest target\n\r", ch );
         return;
      }
      quest_object = load_quest_object( quest_target );
      if ( quest_object == NULL )
      {
         send_to_char( "An invalid quest object was encountered.  Check log files.\n\r", ch );
         quest = FALSE;
         return;
      }
      /* Set values on quest item for Qp, Pracs, Exp, Gold */
      quest_object->value[0] = UMAX( 1, ( quest_target->level / 30 ) );
      quest_object->value[1] = UMAX( 1, ( quest_target->level / 25 ) );
      quest_object->value[2] = ( quest_target->level * 20 );

      if ( number_percent() < 10 )
      {
         quest_object->value[0] += 2;
         quest_object->value[1] += 3;
         quest_object->value[2] *= 2;

      }
      
      quest_timer = 0;
      quest = TRUE;
      new_long_desc[0] = '\0';
      if ( quest_mob->long_descr_orig != NULL )
        free_string( quest_mob->long_descr_orig );
      quest_mob->long_descr_orig = str_dup(quest_mob->long_descr );
      sprintf( new_long_desc, "%s {xsays have you found my %s ?\n\r",
         quest_mob->short_descr, quest_object->short_descr );
      if ( quest_mob->long_descr != NULL )      
        free_string( quest_mob->long_descr );    
      quest_mob->long_descr = str_dup( new_long_desc );
      SET_BIT( quest_mob->act, PLR_NOSUMMON );
      SET_BIT( quest_mob->act, PLR_NOVISIT );
      SET_BIT( quest_mob->act, PLR_NOBLOOD );

      new_long_desc[0] = '\0';
      if ( quest_target->long_descr_orig != NULL )
        free_string( quest_target->long_descr_orig );
      quest_target->long_descr_orig = str_dup( quest_target->long_descr );
      sprintf( new_long_desc, "%s {xsays I stole the %s !!!\n\r",
        quest_target->short_descr, quest_object->short_descr );
      if ( quest_target->long_descr != NULL )     
        free_string( quest_target->long_descr );   
      quest_target->long_descr = str_dup( new_long_desc );

      SET_BIT( quest_target->act, PLR_NOSUMMON );
      SET_BIT( quest_target->act, PLR_NOVISIT );
      SET_BIT( quest_target->act, PLR_NOBLOOD );

      send_to_char( "QUEST STARTED!\n\r\n\r", ch );
            
      sprintf( buf, "The questing mobile is: %s [In Room %d]\n\r", 
         quest_mob->short_descr, quest_mob->in_room->vnum );
      send_to_char( buf, ch );
      
      sprintf( buf, "Target Mobile is: %s [In Room %d]\n\r",
         quest_target->short_descr, quest_target->in_room->vnum );
      send_to_char( buf, ch );
      
      sprintf( buf, "Target Object is: %s.\n\r", quest_object->short_descr );
      send_to_char( buf, ch );
      
      sprintf( buf, "Quest Object is worth: %d QP, %d Prac, %d GP\n\r",
         quest_object->value[0], quest_object->value[1],
         quest_object->value[2] );
      send_to_char( buf, ch );
      
      return;
   }
   if ( !str_cmp( argument, "auto" ) )
   {
      send_to_char( "AutoQuest now initiated!\n\r", ch );
      auto_quest = TRUE;
      return;
   }
   
   
   return;
}      
      
            

/*
 * get_quest_target : This attempts to pick a random mobile to hold the quest
 * item for the player (questor).  Various checks are made to ensure that the
 * questor has a chance of killing the mobile, etc.
 * Returns NULL if it didn't get a mobile this time.
 */

CHAR_DATA *get_quest_target( int min_level, int max_level )
{
   CHAR_DATA *target;
   int min_index = 0;  /* the minimum number of times to go through the list */



/*   int min_distance = 50; unused */
/*   char *dirs = NULL; unused */


    if ( max_level > 140 )
      max_level = 140;
   min_index = number_range(1, 1000);

   for ( target = first_char; target != NULL; target = target->next )
   {
      if ( !IS_NPC( target ) )
         continue;
      min_index -= 1;

      if ( min_index > 0 )
         continue;

         
      /*
       * Check if mobile is suitable for the quest -
       * Ignore mobs that are likely to be in well known places, such
       * as train/prac mobs, healers, etc
       */
      
     if (    ( target->level < min_level                 	  )
          || ( target->level > max_level                 	  )
          || ( IS_VAMP( target )					  )
          || ( IS_SET( target->in_room->area->flags, AREA_NOSHOW ) )   
          || ( IS_SET( target->act, ACT_SENTINEL ) )  
          || ( IS_SET( target->act, ACT_PET ) ) 
          || ( !str_cmp( rev_spec_lookup( target->spec_fun ) , "spec_stephen" ) )
          || ( !str_cmp( rev_spec_lookup( target->spec_fun ) , "spec_tax_man" )  ) )
             continue;
       
     /* Lastly, some random choice */
     if ( number_percent() < 2 )
        break;
   } 
   
   return target;
}       

/*
 * load_quest_object : This takes a pointer to OBJ_INDEX_DATA and places the
 * object onto the target.
 */

OBJ_DATA *load_quest_object( CHAR_DATA *target )
{
   OBJ_INDEX_DATA *pObj;
   OBJ_DATA *object;
   int foo;
   
   foo = number_range( OBJ_VNUM_QUEST_MIN, OBJ_VNUM_QUEST_MAX );
   
   pObj = get_obj_index( foo );
   
   if ( pObj == NULL )
   {
      bug( "load_quest_object : Invalid object vnum %d.", foo );
      return NULL;
   }
      
   object = create_object( pObj, 1 );
   obj_to_char( object, target );
   
   return object;
}
   
CHAR_DATA *get_quest_giver( int min_level, int max_level )
{
   CHAR_DATA *target;
   int    min_index = 0;



/*   int max_distance = 20; unused */
/*   char *dirs = NULL; unused */
   min_index = number_range(0, 1000);

   for ( target = first_char; target != NULL; target = target->next )
   {
      if ( !IS_NPC( target ) )
         continue;
      min_index -= 1;

      if ( min_index > 0 )
        continue;

         
      /*
       * Check if mobile is suitable for the quest -
       * Ignore mobs that are likely to be in well known places, such
       * as train/prac mobs, healers, etc
       */
      
     if (    ( target->level < min_level                 	  )
          || ( target->level > max_level                 	  )
          || ( IS_VAMP( target )					  )
          || ( IS_SET( target->in_room->area->flags, AREA_NOSHOW ) )   
          || ( IS_SET( target->act, ACT_SENTINEL ) ) 
          || ( IS_SET( target->act, ACT_PET ) )
          || ( !str_cmp( rev_spec_lookup( target->spec_fun ) , "spec_stephen" ) )
          || ( !str_cmp( rev_spec_lookup( target->spec_fun ) , "spec_tax_man" )  ) )

             continue;
     {
       if ( number_percent() < 2 )
        break;
     }
      
   }  

   return target;
}       

/*
 * quest_inform : Makes the questing mobile give out information to the
 * players on the mud.  Starts off real simple, and gets more helpful as
 * time runs out :P
 */

void quest_inform( void )
{
   char buf[MAX_STRING_LENGTH];
   extern CHAR_DATA *quest_mob;
   extern CHAR_DATA *quest_target;
   extern OBJ_DATA *quest_object;
   extern int quest_timer;
   extern sh_int quest_personality;
   extern const struct qmessage_type  qmessages[4][17]; 



   /* Work out what the mob should tell the players.... */
   /* Add random element to each case so quests look different each time? */
   if ( quest_timer < 7 )
   {
    sprintf( buf, qmessages[quest_personality][quest_timer].message1, quest_object->short_descr );
   }
   else
   { 
   if (quest_target)
	sprintf ( buf, qmessages[quest_personality][quest_timer].message1,
			 quest_target->short_descr,quest_object->short_descr );
   else
      sprintf( buf, qmessages[quest_personality][quest_timer].message2,
		 quest_object->short_descr );
   }



   quest_timer++;
   if ( quest_mob && quest_timer < 16 )
        do_crusade( quest_mob, buf );
   if ( quest_timer == 1 )
   {
     sprintf( buf, " %s is crusading for %s ", NAME( quest_mob ), quest_object->short_descr);
     info( buf, 5 );
   }
   if ( !quest_mob )
   {
     clear_quest();
   }    
   return;
}

void quest_complete( CHAR_DATA *ch )
{
   extern CHAR_DATA *quest_mob;
   extern OBJ_DATA *quest_object;
   extern sh_int quest_personality;
   extern const struct qmessage_type  qmessages[4][17]; 


   char buf[MAX_STRING_LENGTH];

   sprintf( buf, qmessages[quest_personality][16].message1, NAME(ch), quest_object->short_descr );
   do_crusade( quest_mob, buf );
   clear_quest();
   return;
}   
   

void quest_cancel()
{
   extern CHAR_DATA *quest_mob;
   
    if ( quest_mob )
       do_crusade( quest_mob, "Shoot! Just forget about recovering ANYTHING for me, ok?" );
    
    clear_quest();
    return;   
}   

void clear_quest()
{
   extern bool quest;
   extern CHAR_DATA *quest_mob;
   extern CHAR_DATA *quest_target;
   extern OBJ_DATA *quest_object;
   extern int quest_timer;
   extern int quest_wait;
   extern sh_int quest_personality;

   /* Clear ALL values, ready for next quest */
   
   quest = FALSE;
   extract_obj ( quest_object );
   if ( quest_mob )
   {
     free_string( quest_mob->long_descr );
     quest_mob->long_descr =  str_dup( quest_mob->long_descr_orig );
     free_string( quest_mob->long_descr_orig );
     quest_mob->long_descr_orig = NULL;
   }
   if ( quest_target )
   {
     free_string( quest_target->long_descr );
     quest_target->long_descr =  str_dup( quest_target->long_descr_orig );
     free_string( quest_target->long_descr_orig );
     quest_target->long_descr_orig = NULL;
   };


   quest_mob = NULL;
   quest_target = NULL;
   quest_object = NULL;
   quest_timer = 0;
   quest_wait = 2 + number_range( 1, 4);
   quest_personality = 0;
   
   return;
}
   
   
void generate_auto_quest()
{

  extern bool quest;
   
  extern CHAR_DATA *quest_mob;
  extern CHAR_DATA *quest_target;
  extern OBJ_DATA *quest_object;
  extern int quest_timer;
  extern int quest_wait;
  extern sh_int quest_personality;
  int  hunt_flags = 0;
  char new_long_desc[MAX_STRING_LENGTH];
  sh_int       loop_counter = 0; 

  int a = 140;
  int b = 0;
  sh_int  average_level= 0;
      
  /* generate a new quest! */
  if ( quest )
  {
    return;
  }
      
#if 0
      /* Work out levels of currently playing folks */
  if ( first_desc && first_desc->connected == CON_PLAYING )
  {
  for ( d = first_desc; d; d = d->next )
  {
    if (    d->connected != CON_PLAYING )
      continue;
      player_count += 1;
      total_levels += d->character->level;
  }
  player_count = UMAX( 1, player_count );
  average_level = ( total_levels / player_count );
#endif

  quest_mob = NULL;
  quest_target = NULL;

  average_level = number_range( 0, 99 );
  if ( average_level < 20 )
  {
    a = number_range( 5, 25 );
    b = number_range( 30, 45 );
    hunt_flags = HUNT_WORLD | HUNT_OPENDOOR ;
    quest_personality = 1;
  }
  else   if ( average_level < 65 )
  {
    a = number_range( 40, 55 );
    b = number_range( 60, 84 );
    hunt_flags = HUNT_WORLD | HUNT_OPENDOOR | HUNT_PICKDOOR;
    quest_personality = 2;
  }
  else
  {
    a = number_range( 100, 110 );
    b = number_range( 115, 140 );
    hunt_flags = HUNT_WORLD | HUNT_OPENDOOR | HUNT_PICKDOOR | HUNT_UNLOCKDOOR;
    quest_personality = 3;
  }

  while (  ( quest_mob == NULL )
        && ( loop_counter < 500 )  )
  {
    loop_counter++;
    quest_mob    = get_quest_giver(a, b);
    if (  ( quest_mob == NULL )
       || ( ( h_find_dir( get_room_index( ROOM_VNUM_TEMPLE ), quest_mob->in_room, hunt_flags ) < 0 ) ) ) 
      quest_mob = NULL;
  }

  if ( quest_mob == NULL )
  {
    quest = FALSE;
    quest_wait = number_range( 1, 3 );
    return;
  }
  loop_counter = 0;
  while (  ( quest_target == NULL )
        && ( loop_counter < 500 )  )
  {
    loop_counter++;
    quest_target = get_quest_target( a, b );
    if (  ( quest_target == NULL )
       || ( ( h_find_dir( get_room_index( ROOM_VNUM_TEMPLE ), quest_target->in_room, hunt_flags ) < 0 ) )
       || ( quest_target == quest_mob )  ) 
      quest_target = NULL;
  }



  if ( quest_target == NULL )
  {
    quest = FALSE;
    quest_wait = number_range( 1, 3 );
    return;
  }
  quest_object = load_quest_object( quest_target );
  if ( quest_object == NULL )
  {
    quest = FALSE;
    quest_wait = number_range( 1, 3 );
    return;
  }
      /* Set values on quest item for Qp, Pracs, Exp, Gold */
  quest_object->value[0] = UMAX( 1, ( quest_target->level / 20 ) );
  quest_object->value[1] = UMAX( 1, ( quest_target->level / 18 ) );
  quest_object->value[2] = ( quest_target->level * 20 );
  quest_object->value[3] = average_level;

  if ( number_percent() < 10 )
  {
    quest_object->value[0] += 2;
    quest_object->value[1] += 3;
    quest_object->value[2] *= 2;

  }
      
  quest_timer = 0;
  quest = TRUE;
  new_long_desc[0] = '\0';
  if ( quest_mob->long_descr_orig != NULL )
    free_string( quest_mob->long_descr_orig );
  quest_mob->long_descr_orig = str_dup(quest_mob->long_descr );
  sprintf( new_long_desc, "%s {xsays have you found my %s ?\n\r",
         quest_mob->short_descr, quest_object->short_descr );
  if ( quest_mob->long_descr != NULL )      
    free_string( quest_mob->long_descr );    
  quest_mob->long_descr = str_dup( new_long_desc );
  SET_BIT( quest_mob->act, PLR_NOSUMMON );
  SET_BIT( quest_mob->act, PLR_NOVISIT );
  SET_BIT( quest_mob->act, PLR_NOBLOOD );


  new_long_desc[0] = '\0';
  if ( quest_target->long_descr_orig != NULL )
    free_string( quest_target->long_descr_orig );
  quest_target->long_descr_orig = str_dup( quest_target->long_descr );
  sprintf( new_long_desc, "%s {xsays I stole the %s !!!\n\r",
        quest_target->short_descr, quest_object->short_descr );
  if ( quest_target->long_descr != NULL )     
        free_string( quest_target->long_descr );   
  quest_target->long_descr = str_dup( new_long_desc );

  SET_BIT( quest_target->act, PLR_NOSUMMON );
  SET_BIT( quest_target->act, PLR_NOVISIT );
  SET_BIT( quest_target->act, PLR_NOBLOOD );
  
  return;

}
