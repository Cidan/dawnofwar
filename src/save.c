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
#include <string.h>
#include <time.h>
#include "ack.h"
#include "hash.h"
#include "cJSON.h"

#ifndef DEC_MONEY_H
#include "money.h"
#endif


#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif



/* SAVE_REVISION number defines what has changed:
	 0 -> 1:
		 Went multi-char then lost 3 classes.
		 Need to convert pre-multi-char to Ver.1 multi-char.
		 Need to loose 3 classes off Ver.0 multi-char.        
	 1 -> 2:  
		 Changed exp system - need to void old player's exp 
		 added sentence integer into ch structure - old ver's set to 0
		 (Sentence is unused, btw)
	 2 -> 3:
		 Player-selectable order of class abilities.
		 Need to convert Ver.1- racial class order to player class order.
		 Spells/Skills re-ordered, changed, so Ver.1- lose spells, get pracs.
	 3 -> 4:
		 Needed to fix a bug.  I screwed up.  
	 4 -> 5:
			Arrggghhh.
	 5 -> 6:
			Redid exp.. set all exp to 0.
	 6 -> 7:
			Reduced number of clans.
	 7 ->8:
			Multiple changes for ackmud 4.3
			race wear slots
		 */
		 

#define SAVE_REVISION 15
char *cap_nocol( const char *str )
{
		static char strcap[MAX_STRING_LENGTH];
		int i;

		for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = LOWER(str[i]);
		strcap[i] = '\0';
		strcap[0] = UPPER(strcap[0]);
		return strcap;
}

/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST        100
static  OBJ_DATA *      rgObjNest       [MAX_NEST];


bool                deathmatch;         /* Deathmatch happening?        */


/*
 * Local functions.
 */
void    fwrite_char     args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fwrite_obj      args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
					FILE *fp, int iNest ) );
void    fread_char      args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_obj       args( ( CHAR_DATA *ch,  FILE *fp ) );

void abort_wrapper(void);

/* Courtesy of Yaz of 4th Realm */
char *initial( const char *str )
{
		static char strint[ MAX_STRING_LENGTH ];

		strint[0] = LOWER( str[ 0 ] );
		return strint;

}

void save_char_obj( CHAR_DATA *ch ) {
	if (deathmatch)
		return;
		
	if (IS_NPC(ch) || ch->level < 2)
		return;

	if (!IS_NPC(ch) && ch->desc != NULL && ch->desc->original != NULL)
		ch = ch->desc->original;

	ch->save_time = current_time;
	
	cJSON *root = cJSON_CreateObject();
	generate_char_json(ch, root);
	char *player_json = cJSON_Print(root);
	printf("%s", player_json);
	cJSON_Delete(root);
	return;
}

// Generate character data JSON.
// This is really shitty, and would work better if
// all variables were hashed into a table
// then dynamically generated.
void generate_char_json(CHAR_DATA *ch, cJSON *root ) {
	int cnt = 0;
	AFFECT_DATA *paf;

	cJSON *health = cJSON_CreateObject();
	cJSON *money = cJSON_CreateObject();
	cJSON *player_money = cJSON_CreateArray();
	cJSON *bank_money = cJSON_CreateArray();
	cJSON *pcdata = cJSON_CreateObject();
	cJSON *group = cJSON_CreateArray();
	cJSON *aliases = cJSON_CreateArray();
	cJSON *colors = cJSON_CreateArray();
	cJSON *condition = cJSON_CreateArray();
	cJSON *skills = cJSON_CreateObject();
	cJSON *affects = cJSON_CreateArray();
	cJSON *attributes = cJSON_CreateObject();
	cJSON *attributes_perm = cJSON_CreateObject();
	cJSON *attributes_mod = cJSON_CreateObject();
	cJSON *attributes_max = cJSON_CreateObject();
	cJSON *attributes_learned = cJSON_CreateObject();

	// Construct all of our sub objects first.
	cJSON_AddItemToObject(root, "health", health);
	cJSON_AddItemToObject(root, "affects", affects);
	cJSON_AddItemToObject(root, "money", money);
	cJSON_AddItemToObject(root, "pcdata", pcdata);
	cJSON_AddItemToObject(pcdata, "aliases", aliases);
	cJSON_AddItemToObject(pcdata, "colors", colors);
	cJSON_AddItemToObject(pcdata, "group", group);
	cJSON_AddItemToObject(pcdata, "attributes", attributes);
	cJSON_AddItemToObject(pcdata, "condition", condition);
	cJSON_AddItemToObject(pcdata, "skills", skills);
	cJSON_AddItemToObject(money, "player_money", player_money);
	cJSON_AddItemToObject(money, "bank_money", bank_money);
	// Attributes is a bit complex.
	cJSON_AddItemToObject(attributes, "attributes_perm", attributes_perm);
	cJSON_AddItemToObject(attributes, "attributes_mod", attributes_mod);
	cJSON_AddItemToObject(attributes, "attributes_max", attributes_max);
	cJSON_AddItemToObject(attributes, "attributes_learned", attributes_learned);

	// Construct our JSON object.
	cJSON_AddStringToObject(root, "type", IS_NPC(ch) ? "MOB" : "PLAYER");
	cJSON_AddStringToObject(root, "name", ch->name);
	cJSON_AddStringToObject(root, "short_description", ch->short_descr);
	cJSON_AddStringToObject(root, "long_description", ch->long_descr_orig);
	cJSON_AddStringToObject(root, "description", ch->description);
	cJSON_AddStringToObject(root, "prompt", ch->prompt);
	cJSON_AddNumberToObject(root, "sex", ch->sex);
	cJSON_AddNumberToObject(root, "login_sex", ch->login_sex);
	cJSON_AddNumberToObject(root, "race", ch->race);
	cJSON_AddNumberToObject(root, "level", ch->level);
	cJSON_AddNumberToObject(root, "sentence", ch->sentence);
	cJSON_AddNumberToObject(root, "incog", ch->incog);
	cJSON_AddNumberToObject(root, "hometown", ch->hometown);
	cJSON_AddNumberToObject(root, "adept_level", ch->adept_level);
	cJSON_AddNumberToObject(root, "trust", ch->trust);
	cJSON_AddNumberToObject(root, "wizbit", ch->wizbit);
	cJSON_AddNumberToObject(root, "played", ch->played + (int) (current_time - ch->logon));
	cJSON_AddNumberToObject(root, "last_note", ch->last_note);
	cJSON_AddNumberToObject(root, "room", (
	ch->in_room == get_room_index( ROOM_VNUM_LIMBO ) && ch->was_in_room != NULL )
	? ch->was_in_room->vnum
	: ch->in_room->vnum);

	// Health, Mana, and Moves
	cJSON_AddNumberToObject(health, "current_health", ch->hit);
	cJSON_AddNumberToObject(health, "max_health", ch->max_hit); 
	cJSON_AddNumberToObject(health, "mana", ch->mana); 
	cJSON_AddNumberToObject(health, "max_mana", ch->max_mana); 
	cJSON_AddNumberToObject(health, "move", ch->move); 
	cJSON_AddNumberToObject(health, "max_move", ch->max_move);
	cJSON_AddNumberToObject(root, "exp", ch->exp);
	cJSON_AddNumberToObject(root, "total_exp", ch->total_exp);
	cJSON_AddNumberToObject(root, "act", ch->act);
	cJSON_AddNumberToObject(root, "config", ch->config);
	cJSON_AddNumberToObject(root, "affected_by", ch->affected_by);
	cJSON_AddNumberToObject(root, "position", ch->position == POS_FIGHTING ? POS_STANDING : ch->position);
	cJSON_AddNumberToObject(root, "practice", ch->practice);
	cJSON_AddNumberToObject(root, "train", ch->train);
	cJSON_AddNumberToObject(root, "num_of_spells", ch->num_of_spells);
	cJSON_AddNumberToObject(root, "num_of_skills", ch->num_of_skills);
	cJSON_AddNumberToObject(root, "saving_throw", ch->saving_throw);
	cJSON_AddNumberToObject(root, "alignment", ch->alignment);
	cJSON_AddNumberToObject(root, "hitroll", ch->hitroll);
	cJSON_AddNumberToObject(root, "damroll", ch->damroll);
	cJSON_AddNumberToObject(root, "armor", ch->armor);
	cJSON_AddNumberToObject(root, "wimpy", ch->wimpy);
	cJSON_AddNumberToObject(root, "deaf", ch->deaf);
	// Money
	for (cnt = 0; ch->money->cash_unit[cnt] < MAX_CURRENCY; cnt++) {
		cJSON_AddNumberToArray(player_money, ch->money->cash_unit[cnt]);
		cJSON_AddNumberToArray(bank_money, ch->bank_money->cash_unit[cnt]);
	}
	
	if (IS_NPC(ch))
		cJSON_AddNumberToObject(root, "vnum", ch->pIndexData->vnum);
	else {
		// TODO: pcdata sub object
		cJSON_AddNumberToObject(pcdata, "generation", ch->pcdata->generation);
		cJSON_AddNumberToObject(pcdata, "clan", ch->pcdata->clan);
		cJSON_AddNumberToObject(pcdata, "mkills", ch->pcdata->mkills);
		cJSON_AddNumberToObject(pcdata, "mkilled", ch->pcdata->mkilled);
		cJSON_AddNumberToObject(pcdata, "pkills", ch->pcdata->pkills);
		cJSON_AddNumberToObject(pcdata, "pkilled", ch->pcdata->pkills);
		cJSON_AddStringToObject(pcdata, "password", ch->pcdata->pwd);
		cJSON_AddStringToObject(pcdata, "bamfin", ch->pcdata->bamfin);
		cJSON_AddStringToObject(pcdata, "bamfout", ch->pcdata->bamfout);
		cJSON_AddStringToObject(pcdata, "room_enter", ch->pcdata->room_enter);
		cJSON_AddStringToObject(pcdata, "room_exit", ch->pcdata->room_exit);
		cJSON_AddStringToObject(pcdata, "title", ch->pcdata->title);
		cJSON_AddStringToObject(pcdata, "immskll", ch->pcdata->immskll);
		cJSON_AddStringToObject(pcdata, "who_name", ch->pcdata->who_name);
		cJSON_AddNumberToObject(pcdata, "monitor", ch->pcdata->monitor);
		cJSON_AddStringToObject(pcdata, "host", ch->pcdata->host);
		cJSON_AddNumberToObject(pcdata, "failures", ch->pcdata->failures);
		cJSON_AddStringToObject(pcdata, "last_login", (char *) ctime( &current_time ));
		cJSON_AddNumberToObject(pcdata, "imc_deaf", ch->pcdata->imc_deaf);
		cJSON_AddNumberToObject(pcdata, "imc_allow", ch->pcdata->imc_allow);
		cJSON_AddNumberToObject(pcdata, "imc_deny", ch->pcdata->imc_deny);
		cJSON_AddStringToObject(pcdata, "ice_listen", ch->pcdata->ice_listen);
		//cJSON_AddStringToObject(pcdata, "hicol", ch->pcdata->hicol);
		//cJSON_AddStringToObject(pcdata, "dimcol", ch->pcdata->dimcol);
		cJSON_AddNumberToObject(pcdata, "term_rows", ch->pcdata->term_rows);
		cJSON_AddNumberToObject(pcdata, "term_columns", ch->pcdata->term_columns);
		cJSON_AddStringToObject(pcdata, "email", ch->pcdata->email_address);
		cJSON_AddNumberToObject(pcdata, "valid_email", ch->pcdata->valid_email);
		cJSON_AddStringToObject(pcdata, "assist_msg", ch->pcdata->assist_msg);
		cJSON_AddNumberToObject(pcdata, "points", ch->pcdata->points);
		cJSON_AddNumberToObject(pcdata, "autobuild", ch->pcdata->autobuild);
		cJSON_AddNumberToObject(pcdata, "strategy", ch->pcdata->strategy);
		cJSON_AddNumberToObject(pcdata, "laston", ch->pcdata->laston);
		// Attributes.
		cJSON_AddNumberToObject(attributes_perm, "str", ch->pcdata->perm_str);
		cJSON_AddNumberToObject(attributes_perm, "int", ch->pcdata->perm_int);
		cJSON_AddNumberToObject(attributes_perm, "wis", ch->pcdata->perm_wis);
		cJSON_AddNumberToObject(attributes_perm, "dex", ch->pcdata->perm_dex);
		cJSON_AddNumberToObject(attributes_perm, "con", ch->pcdata->perm_con);
		cJSON_AddNumberToObject(attributes_perm, "lck", ch->pcdata->perm_lck);

		cJSON_AddNumberToObject(attributes_mod, "str", 0);
		cJSON_AddNumberToObject(attributes_mod, "int", 0);
		cJSON_AddNumberToObject(attributes_mod, "wis", 0);
		cJSON_AddNumberToObject(attributes_mod, "dex", 0);
		cJSON_AddNumberToObject(attributes_mod, "con", 0);
		cJSON_AddNumberToObject(attributes_mod, "lck", 0);

		cJSON_AddNumberToObject(attributes_max, "str", ch->pcdata->max_str);
		cJSON_AddNumberToObject(attributes_max, "int", ch->pcdata->max_int);
		cJSON_AddNumberToObject(attributes_max, "wis", ch->pcdata->max_wis);
		cJSON_AddNumberToObject(attributes_max, "dex", ch->pcdata->max_dex);
		cJSON_AddNumberToObject(attributes_max, "con", ch->pcdata->max_con);
		cJSON_AddNumberToObject(attributes_max, "lck", ch->pcdata->max_lck);

		cJSON_AddNumberToObject(attributes_learned, "str", ch->pcdata->learned_str);
		cJSON_AddNumberToObject(attributes_learned, "int", ch->pcdata->learned_int);
		cJSON_AddNumberToObject(attributes_learned, "wis", ch->pcdata->learned_wis);
		cJSON_AddNumberToObject(attributes_learned, "dex", ch->pcdata->learned_dex);
		cJSON_AddNumberToObject(attributes_learned, "con", ch->pcdata->learned_con);
		cJSON_AddNumberToObject(attributes_learned, "lck", ch->pcdata->learned_lck);

		cJSON_AddNumberToObject(pcdata, "bloodlust", ch->pcdata->bloodlust);
		cJSON_AddNumberToObject(pcdata, "bloodlust_max", ch->pcdata->bloodlust_max);
		cJSON_AddNumberToObject(pcdata, "vamp_level", ch->pcdata->vamp_level);
		cJSON_AddNumberToObject(pcdata, "vamp_exp", ch->pcdata->vamp_exp);
		cJSON_AddNumberToObject(pcdata, "vamp_skill_num", ch->pcdata->vamp_skill_num);
		cJSON_AddNumberToObject(pcdata, "vamp_skill_max", ch->pcdata->vamp_skill_max);
		cJSON_AddNumberToObject(pcdata, "vamp_bloodline", ch->pcdata->vamp_bloodline);
		cJSON_AddNumberToObject(pcdata, "vamp_pracs", ch->pcdata->vamp_pracs);
		cJSON_AddNumberToObject(pcdata, "has_exp_fix", ch->pcdata->has_exp_fix);
		cJSON_AddNumberToObject(pcdata, "quest_points", ch->quest_points);
		cJSON_AddNumberToObject(pcdata, "recall_vnum", ch->pcdata->recall_vnum);
		cJSON_AddNumberToObject(pcdata, "gain_mana", ch->pcdata->mana_from_gain);
		cJSON_AddNumberToObject(pcdata, "gain_hp", ch->pcdata->hp_from_gain);
		cJSON_AddNumberToObject(pcdata, "gain_move", ch->pcdata->move_from_gain);
		cJSON_AddNumberToObject(pcdata, "ruler_rank", ch->pcdata->ruler_rank);
		cJSON_AddNumberToObject(pcdata, "pagelen", ch->pcdata->pagelen);
		cJSON_AddNumberToObject(pcdata, "pflags", ch->pcdata->pflags);

		for (cnt = 0; cnt < MAX_ALIASES; cnt ++) {
			cJSON *alias = cJSON_CreateObject();
			cJSON_AddStringToObject(alias, ch->pcdata->alias_name[cnt], ch->pcdata->alias[cnt]);
			cJSON_AddItemToArray(aliases, alias);
		}
		for (cnt = 0; cnt < MAX_color; cnt++)
			cJSON_AddNumberToArray(colors, ch->pcdata->color[cnt]);
		for (cnt = 0; cnt < MAX_GROUP; cnt++)
			cJSON_AddNumberToArray(group, ch->pcdata->group[cnt]);
		for (cnt = 0; cnt < 3; cnt++)
			cJSON_AddNumberToArray(condition, ch->pcdata->condition[cnt]);
		
		for (cnt = 0; cnt < MAX_SKILL; cnt++)
			if (skill_table[cnt].name != NULL && ch->pcdata->learned[cnt] > 0 )
				cJSON_AddNumberToObject(skills, skill_table[cnt].name, ch->pcdata->learned[cnt]);
	}

	for (paf = ch->first_affect; paf != NULL; paf = paf->next) {
		cJSON *affect = cJSON_CreateObject();
		cJSON_AddNumberToObject(affect, "type", paf->type);
		cJSON_AddNumberToObject(affect, "duration", paf->duration);
		cJSON_AddNumberToObject(affect, "modifier", paf->modifier);
		cJSON_AddNumberToObject(affect, "location", paf->location);
		cJSON_AddNumberToObject(affect, "bitvector", paf->bitvector);
		cJSON_AddItemToArray(affects, affect);
	}
	return;
}

/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
		EXTRA_DESCR_DATA *ed;
		AFFECT_DATA *paf;
		sh_int foo;
		/*
		 * Slick recursion to write lists backwards,
		 *   so loading them will load in forwards order.
		 */
/*		loop_counter++;
		if ( loop_counter > 650 )
		{
			abort_wrapper();
		}*/
		if ( obj->next_in_carry_list != NULL )
	fwrite_obj( ch, obj->next_in_carry_list, fp, iNest );

		/*
		 * Castrate storage characters.
		 */

		 /* Also bypass no-save objects -S- */

		if ( obj->item_type == ITEM_KEY
		||   obj->item_type == ITEM_BEACON
		||   IS_SET( obj->extra_flags, ITEM_NOSAVE ) )
	return;

		fprintf( fp, "#OBJECT\n" );
		fprintf( fp, "Nest         %d\n",   iNest                        );
		fprintf( fp, "Name         %s~\n",  obj->name                    );
		fprintf( fp, "ShortDescr   %s~\n",  obj->short_descr             );
		fprintf( fp, "Description  %s~\n",  obj->description             );
		fprintf( fp, "Vnum         %d\n",   obj->pIndexData->vnum        );
		fprintf( fp, "ExtraFlags   %d\n",   obj->extra_flags             );
		fprintf( fp, "WearFlags    %d\n",   obj->wear_flags              );
		fprintf( fp, "WearLoc      %d\n",   obj->wear_loc                );
		fprintf( fp, "Money %d ", MAX_CURRENCY );
		for ( foo = 0; foo < MAX_CURRENCY; foo++ )
			fprintf( fp, "%d ", obj->money->cash_unit[foo] );
	fprintf( fp, "\n" );
		if ( obj->obj_fun != NULL )
		fprintf( fp, "Objfun       %s~\n", 
			 rev_obj_fun_lookup( obj->obj_fun ) );
		
		fprintf( fp, "ClassFlags   %d\n",   obj->item_apply              );
		/* ClassFlags still used to save fucking with pfiles */
		fprintf( fp, "ItemType     %d\n",   obj->item_type               );
		fprintf( fp, "Weight       %d\n",   obj->weight                  );
		fprintf( fp, "Level        %d\n",   obj->level                   );
		fprintf( fp, "Timer        %d\n",   obj->timer                   );
		fprintf( fp, "Cost         %d\n",   obj->cost                    );
		fprintf( fp, "Values       %d %d %d %d %d %d %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3],
	obj->value[4], obj->value[5], obj->value[6], obj->value[7],
	obj->value[8], obj->value[9] );

		switch ( obj->item_type )
		{
		case ITEM_POTION:
		case ITEM_SCROLL:
	if ( obj->value[1] > 0 )
	{
			fprintf( fp, "Spell 1      '%s'\n", 
		skill_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
			fprintf( fp, "Spell 2      '%s'\n", 
		skill_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
			fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;

		case ITEM_PILL:
		case ITEM_STAFF:
		case ITEM_WAND:
	if ( obj->value[3] > 0 )
	{
			fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;
		}

		for ( paf = obj->first_apply; paf != NULL; paf = paf->next )
		{
	fprintf( fp, "Affect       %d %d %d %d %d\n",
			paf->type,
			paf->duration,
			paf->modifier,
			paf->location,
			paf->bitvector
			);
		}

		for ( ed = obj->first_exdesc; ed != NULL; ed = ed->next )
		{
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
			ed->keyword, ed->description );
		}
	fprintf( fp, "Stored       %i\n", obj->stored);
		fprintf( fp, "End\n\n" );

		if ( obj->first_in_carry_list != NULL )
	fwrite_obj( ch, obj->first_in_carry_list, fp, iNest + 1 );

		return;
}


/* so the stack doesn't get hosed */
void abort_wrapper(void) { abort(); };

hash_table * hash_changed_vnums=NULL;

/* Nasty hack for db.c to get back address of ch */
CHAR_DATA * loaded_mob_addr;

/*
 * Load a char and inventory into a new ch structure.
 */

int cur_revision = 0;

bool load_char_obj( DESCRIPTOR_DATA *d, char *name, bool system_call )
{
		int cnt;
		static PC_DATA pcdata_zero;
		char strsave[MAX_INPUT_LENGTH];
		char tempstrsave[MAX_INPUT_LENGTH];
		char * bufptr,*nmptr;
		CHAR_DATA *ch;
		char buf[MAX_STRING_LENGTH];
		FILE *fp;
		bool found;
		bool is_npc;
		int oldvnum,newvnum;
		int foo;

		if (hash_changed_vnums == NULL)
		{
		 /* Initialise hash table for changed vnums, and read in file. */
		 hash_changed_vnums=create_hash_table(1024);
		 
		 if ((fp=fopen("area_changes.txt","r")) != NULL) /* -- Alty */
		 {
			 while (!feof(fp))
			 {
				if (   str_cmp( fread_word(fp), "Obj:" )
						|| fread_letter(fp) != '['
						|| (oldvnum = fread_number(fp)) == 0
						|| fread_letter(fp) != ']'
						|| str_cmp( fread_word(fp) , "->") 
						|| fread_letter(fp) != '['
						|| (newvnum = fread_number(fp)) == 0
						|| fread_letter(fp) != ']' )
					 fread_to_eol(fp);
				else
				{
					 fread_to_eol(fp);
					 add_hash_entry(hash_changed_vnums,oldvnum,(void *) newvnum);
				}
			 }
			 fclose(fp);
		 }
		}

		if (  ( d == NULL ) /* load npc */
			 && ( !system_call )  )
		 is_npc=TRUE;
		else
		 is_npc=FALSE;

		GET_FREE(ch, char_free);
		clear_char( ch );

		if (!is_npc)
		{
		 GET_FREE(ch->pcdata, pcd_free);
		 *ch->pcdata                         = pcdata_zero;
		
		 d->character                        = ch;

		 ch->pcdata->host			 = str_dup( "Unknown!" );
		 ch->pcdata->lastlogin		 = str_dup( "Unknown!" );
		 ch->pcdata->who_name		 = str_dup( "off" );
		 ch->pcdata->pwd                     = str_dup( "" );
		 ch->pcdata->bamfin                  = str_dup( "" );
		 ch->pcdata->bamfout                 = str_dup( "" );
		 ch->hometown                        = 0;
		 ch->pcdata->room_enter			= str_dup( "" );
		 ch->pcdata->room_exit			= str_dup( "" );
		 ch->pcdata->title                   = str_dup( "" );
		ch->pcdata->immskll			= str_dup( "" );
		ch->pcdata->ice_listen = str_dup( "" );
		 ch->pcdata->perm_str                = 13;
		 ch->pcdata->perm_int                = 13; 
		 ch->pcdata->perm_wis                = 13;
		 ch->pcdata->perm_dex                = 13;
		 ch->pcdata->perm_con                = 13;
	 ch->pcdata->perm_lck                = 13;
		 ch->pcdata->learned_str             = 0;

		 ch->pcdata->learned_int             = 0; 

		 ch->pcdata->learned_wis             = 0;

		 ch->pcdata->learned_dex             = 0;

		 ch->pcdata->learned_con             = 0;

	 ch->pcdata->learned_lck             = 0;

		 ch->pcdata->bloodlust		 = 24;
		 ch->pcdata->condition[COND_THIRST]  = 48;
		 ch->pcdata->pagelen                 = 20;
		 ch->pcdata->condition[COND_FULL]    = 48;
		 ch->pcdata->pkills			= 0;
		 ch->pcdata->pkilled		= 0;
		 ch->pcdata->mkills			= 0;
		 ch->pcdata->mkilled		= 0;
		 ch->pcdata->pflags                 = 0;
		 ch->pcdata->has_exp_fix            = 0;
		 ch->pcdata->recall_vnum			= 3001;
		 ch->pcdata->mana_from_gain = -1;
		 ch->pcdata->hp_from_gain = -1;
		 ch->pcdata->move_from_gain		= -1;
		 ch->pcdata->hicol = 'y';
		 ch->pcdata->dimcol = 'b';
		 ch->pcdata->ruler_rank	= 0;
		 for ( foo = 0; foo < 5; foo++ )
			 ch->pcdata->pedit_string[foo] = str_dup( "none" );
		 ch->pcdata->pedit_state = str_dup( "none" );
		 ch->pcdata->term_rows = 25;
		 ch->pcdata->term_columns = 80;
		 ch->pcdata->valid_email = FALSE;
		 ch->pcdata->email_address = str_dup( "not set" );
		 ch->pcdata->assist_msg = str_dup( "'{RBANZAI!!{x $N must be assisted!!{x'" );
		 ch->quest_points           = 0;
		 ch->adept_level = -1;

		 
		 for ( cnt = 0; cnt < MAX_ALIASES; cnt++ )
		 {

		 free_string(ch->pcdata->alias_name[cnt]);

		 free_string(ch->pcdata->alias[cnt]);
			 ch->pcdata->alias_name[cnt]  = str_dup( "<none>" );
			 ch->pcdata->alias[cnt]	    = str_dup( "<none>" );
		 }
		}
		else
		{ 
		 /* is NPC */
		 ch->pcdata = NULL;
		 loaded_mob_addr=ch;
		}
		
		ch->stunTimer             = 0;
		ch->first_shield          = NULL;
		ch->last_shield	      = NULL;
		ch->switched			= FALSE;
		ch->old_body			= NULL;
		
		ch->deaf				= 0;
		ch->desc                            = d;
		if ( ch->name != NULL )
			free_string( ch->name );
		ch->name                            = str_dup( name );
		ch->prompt = str_dup("");
		ch->old_prompt = str_dup("");
		ch->prompt                          = str_dup( "TYPE HELP PROMPT " );
		ch->last_note                       = 0;
		if (is_npc)
		 ch->act				= ACT_IS_NPC;
		else
		 ch->config                            = CONFIG_BLANK
					| CONFIG_COMBINE
					| CONFIG_PROMPT
					| CONFIG_MAPPER;
		ch->sex = SEX_NEUTRAL;
		ch->login_sex = -1;
		ch->current_brand = NULL;
		ch->carry_weight = 0.0;
		ch->carry_number = 0;
		ch->ngroup = NULL;

		{
			MONEY_TYPE * money;
			GET_FREE( money, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "loading player money, %s", 
			ch->name );
		if ( money->money_key != NULL )
			free_string( money->money_key );
		money->money_key = str_dup( testbuf );
	}
#endif
			for ( cnt = 0; cnt < MAX_CURRENCY; cnt++ )
				money->cash_unit[cnt] = 0;
			ch->money = money;
			GET_FREE( money, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "loading player bank, %s", 
			ch->name );
		if ( money->money_key != NULL )
			free_string( money->money_key );
		money->money_key = str_dup( testbuf );
	}
#endif
			for ( cnt = 0; cnt < MAX_CURRENCY; cnt++ )
				money->cash_unit[cnt] = 0;
			ch->bank_money = money;
		}

		found = FALSE;
		fclose( fpReserve );

		/* parsed player file directories by Yaz of 4th Realm */
		/* decompress if .gz file exists - Thx Alander */
		
#if !defined(machintosh) && !defined(MSDOS)
		 if (is_npc)   /* convert spaces to . */
		 {
			for (nmptr=name,bufptr=buf;*nmptr != 0; nmptr++)
			{
			 if (*nmptr == ' ')
				*(bufptr++)='.';
			 else
				*(bufptr++)=*nmptr;
			}
			*(bufptr)=*nmptr;
		 }
		 else
			strcpy(buf,name);
		sprintf( strsave, "%s%s%s%s", is_npc ? NPC_DIR : PLAYER_DIR,
					 initial( buf ), "/", cap_nocol( buf ) );
#else
		/* Convert npc names to dos compat name.... yuk */
		if (is_npc)
		{
			for (nmptr=ch->name,bufptr=buf;*nmptr != 0; nmptr++)
			{
			 if (*nmptr != ' ' && *nmptr != '.')
				*(bufptr++)=*nmptr;
			 if ( bufptr-buf == 8)
				break;
			}
			*(bufptr)=0;
		}
		else
		 strcpy(buf,name);

		sprintf( strsave, "%s%s", is_npc ? NPC_DIR : PLAYER_DIR,
						 cap_nocol( buf ) );
#endif



#if !defined(macintosh) && !defined(MSDOS)
		sprintf( tempstrsave, "%s%s", strsave, ".gz" );
		if ( ( fp = fopen( tempstrsave, "r" ) ) != NULL )
		{
				char buf[MAX_STRING_LENGTH];
	fclose( fp );
	sprintf( buf, "gzip -dfq %s", tempstrsave );
	system( buf );
		}
#endif

		if ( ( fp = fopen( strsave, "r" ) ) != NULL )
		{
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
			rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
			char letter;
			char *word;

			letter = fread_letter( fp );
			if ( letter == '*' )
			{
		fread_to_eol( fp );
		continue;
			}

			if ( letter != '#' )
			{
		monitor_chan( "Load_char_obj: # not found.",MONITOR_BAD );
		break;
			}

			word = fread_word( fp );
			if      ( !str_cmp( word, "PLAYER" ) ) fread_char ( ch, fp );
			else if ( !str_cmp( word, "MOB"    ) ) fread_char ( ch, fp );
			else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp );
			else if ( !str_cmp( word, "END"    ) ) break;
			else
			{
		monitor_chan( "Load_char_obj: bad section.", MONITOR_BAD );
		break;
			}
	}
	fclose( fp );
		}

		if (!found && is_npc)
		{
		 /* return memory for char back to system. */
		 free_char(ch);
		}
		fpReserve = fopen( NULL_FILE, "r" );
		return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )  if ( !str_cmp( word, literal ) ) { field  = value; fMatch = TRUE;  break;}
#define SKEY( literal, field, value )  if ( !str_cmp( word, literal ) ) { if (field!=NULL) free_string(field);field  = value; fMatch = TRUE;  break;}

void fread_char( CHAR_DATA *ch, FILE *fp )
{
		char buf[MAX_STRING_LENGTH];
		char *word;
		bool fMatch;
		int cnt;
		int g;
	 /* Save revision control: */


	 /* Ugly fix for pfiles with no balance value */    
	 ch->balance = 0;
	 /* Another fix for m/c levels.. this is getting to be a habit... */
				 

	 for ( ; ; )
		{
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;

	case 'A':
			KEY( "Act",         ch->act,                fread_number( fp ) );
			KEY( "AffectedBy",  ch->affected_by,        fread_number( fp ) );
			KEY( "Alignment",   ch->alignment,          fread_number( fp ) );
			KEY( "Armor",       ch->armor,              fread_number( fp ) );
			KEY( "Adeptlevel",  ch->adept_level,		fread_number( fp ) );
				KEY( "AutoBuild",   ch->pcdata->autobuild,  fread_number( fp ) );
		SKEY( "AssistMsg", ch->pcdata->assist_msg, fread_string( fp ) );
			if (!IS_NPC(ch))
			{
			 SKEY( "Alias_Name0", 
								ch->pcdata->alias_name[0],	fread_string( fp ) );
								
						 SKEY( "Alias_Name1", 
								ch->pcdata->alias_name[1],	fread_string( fp ) );
		 
			 SKEY( "Alias_Name2", 
								ch->pcdata->alias_name[2],	fread_string( fp ) );
			
						 SKEY( "Alias_Name3", 
								ch->pcdata->alias_name[3],	fread_string( fp ) );
			
						 SKEY( "Alias_Name4", 
								ch->pcdata->alias_name[4],	fread_string( fp ) );
			
			 SKEY( "Alias_Name5", 
								ch->pcdata->alias_name[5],	fread_string( fp ) );
						

			 SKEY( "Alias0", 
								ch->pcdata->alias[0],	fread_string( fp ) );
								
						 SKEY( "Alias1", 
								ch->pcdata->alias[1],	fread_string( fp ) );
		 
			 SKEY( "Alias2", 
								ch->pcdata->alias[2],	fread_string( fp ) );
			
						 SKEY( "Alias3", 
								ch->pcdata->alias[3],	fread_string( fp ) );
			
						 SKEY( "Alias4", 
								ch->pcdata->alias[4],	fread_string( fp ) );
			
			 SKEY( "Alias5", 
								ch->pcdata->alias[5],	fread_string( fp ) );
			}


			if ( !str_cmp( word, "Affect" ) )
			{  
		AFFECT_DATA *paf;

		GET_FREE(paf, affect_free);
		paf->type       = fread_number( fp );
		paf->duration   = fread_number( fp );
		paf->modifier   = fread_number( fp );
		paf->location   = fread_number( fp );
		paf->bitvector  = fread_number( fp );
		paf->caster	= NULL;
		if ( paf->type != -1 )
			LINK( paf, ch->first_saved_aff, ch->last_saved_aff, next, prev);
		else
			PUT_FREE(paf, affect_free);
		fMatch = TRUE;    
		break;
			}

			if ( !IS_NPC(ch))
			{
			 if ( !str_cmp( word, "AttrMod"  ) )
			 {
		ch->pcdata->mod_str  = fread_number( fp );
		ch->pcdata->mod_int  = fread_number( fp );
		ch->pcdata->mod_wis  = fread_number( fp );
		ch->pcdata->mod_dex  = fread_number( fp );
		ch->pcdata->mod_con  = fread_number( fp );
		ch->pcdata->mod_lck  = fread_number( fp );
		fMatch = TRUE;
		break;
			 }

			 if ( !str_cmp( word, "AttrMax" ) )
			 {
		ch->pcdata->max_str = fread_number( fp );
		ch->pcdata->max_int = fread_number( fp );
		ch->pcdata->max_wis = fread_number( fp );
		ch->pcdata->max_dex = fread_number( fp );
		ch->pcdata->max_con = fread_number( fp );
		ch->pcdata->max_lck = fread_number( fp );
		fMatch = TRUE;
		break;
			 }

			 if ( !str_cmp( word, "AttrPerm" ) )
			 {
		ch->pcdata->perm_str = fread_number( fp );
		ch->pcdata->perm_int = fread_number( fp );
		ch->pcdata->perm_wis = fread_number( fp );
		ch->pcdata->perm_dex = fread_number( fp );
		ch->pcdata->perm_con = fread_number( fp );
		ch->pcdata->perm_lck = fread_number( fp );
		fMatch = TRUE;
		break;
			 }

		 if ( !str_cmp( word, "AttrLearned" ) )

			 {

		ch->pcdata->learned_str = fread_number( fp );

		ch->pcdata->learned_int = fread_number( fp );

		ch->pcdata->learned_wis = fread_number( fp );

		ch->pcdata->learned_dex = fread_number( fp );

		ch->pcdata->learned_con = fread_number( fp );

		ch->pcdata->learned_lck = fread_number( fp );

		fMatch = TRUE;

		break;

			 }


			}
			break;

	case 'B':
/*	    KEY( "Balance",     ch->balance,            fread_number( fp ) );  */
			if ( !str_cmp( word, "Balance" ) )
			{
				join_money( round_money( fread_number( fp ), TRUE ), ch->bank_money );
				fMatch = TRUE;
				break;
			}
			KEY( "Bloodlust",     ch->pcdata->bloodlust,            fread_number( fp ) );
			KEY( "Bloodlustmax",     ch->pcdata->bloodlust_max,            fread_number( fp ) );
			if (!IS_NPC(ch))
			{
			 SKEY( "Bamfin",      ch->pcdata->bamfin,     fread_string( fp ) );
			 SKEY( "Bamfout",     ch->pcdata->bamfout,    fread_string( fp ) );
			}
			if ( !str_cmp( word, "BankMoney" ) )
			{
				MONEY_TYPE * transfer;
				int num_coins;
				GET_FREE( transfer, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "reading player money, %s", 
			ch->name );
		transfer->money_key = str_dup( testbuf );
	}
#endif
				num_coins = fread_number( fp );
				for (cnt=0; cnt < num_coins; cnt++)
				 transfer->cash_unit[( cnt < MAX_CURRENCY ? cnt : MAX_CURRENCY -1)] = fread_number( fp );
				join_money( transfer, ch->bank_money );
				fMatch = TRUE;
				break;
			}
			break;

	case 'C':
			if (!IS_NPC(ch))
			{
			 KEY( "Clan",        ch->pcdata->clan,       fread_number( fp ) );
			}
			KEY( "Config",      ch->config,             fread_number( fp ) );
			if ( !str_cmp( word, "colors" ) && !IS_NPC(ch))
			{
				 int foo;
				 for ( foo = 0; foo < MAX_color; foo++ )
						ch->pcdata->color[foo] = fread_number( fp );
				 fMatch = TRUE;
				 break;
			} 
			
			if ( !str_cmp( word, "Condition" ) && !IS_NPC(ch))
			{
		ch->pcdata->condition[0] = fread_number( fp );
		ch->pcdata->condition[1] = fread_number( fp );
		ch->pcdata->condition[2] = fread_number( fp );
		fMatch = TRUE;
		break;
			}
			break;

	case 'D':
			KEY( "Damroll",     ch->damroll,            fread_number( fp ) );
			KEY( "Deaf",        ch->deaf,               fread_number( fp ) );
			SKEY( "Description", ch->description,        fread_string( fp ) );

						if ( !str_cmp( word, "DimCol" ) )
						{
							char * temp;
							temp =  fread_string( fp );
							ch->pcdata->dimcol = temp[0];
/*              fread_to_eol( fp );   */
							free_string( temp );
							fMatch = TRUE;
							break;
						}


			break;

	case 'E':
			if ( !str_cmp( word, "End" ) )
			{
				if ( ch->login_sex < 0 )
						ch->login_sex = ch->sex;
			 return;
			}
			KEY( "Exp",         ch->exp,                fread_number( fp ) );
			KEY( "EmailValid", ch->pcdata->valid_email, fread_number( fp ) );
			SKEY( "Email", ch->pcdata->email_address, fread_string( fp ) );
			break;

				case 'F':
						if (!IS_NPC(ch))
						{
						 KEY( "Failures",	ch->pcdata->failures,	fread_number( fp ) );
						}
						break;

	case 'G':
						KEY( "GainMana",	ch->pcdata->mana_from_gain, fread_number( fp ) );
						KEY( "GainHp",	ch->pcdata->hp_from_gain, fread_number( fp ) );
			KEY( "GainMove",	ch->pcdata->move_from_gain, fread_number( fp ) );
/*	    KEY( "Gold",        ch->gold,               fread_number( fp ) );  */

			if ( !str_cmp( word, "Gold" ) )
			{
				join_money( round_money( fread_number( fp ), TRUE ), ch->money );
				fMatch = TRUE;
				break;
			}
			KEY( "Generation",  ch->pcdata->generation, fread_number( fp ) );

		if ( !str_cmp( word, "Group" ) && !IS_NPC(ch) )
						{ 
							 for ( g = 0; g < MAX_GROUP; g++ )
							 {
									ch->pcdata->group[g] = fread_number( fp );
							 }
				 fMatch = TRUE;
						}   	

			break;

	case 'H':
			KEY( "Hitroll",     ch->hitroll,            fread_number( fp ) );
/*	    if (!IS_NPC(ch))
			{  */
						 SKEY( "Host",	ch->pcdata->host,	fread_string( fp ) );
/*            }  */
						KEY( "Hasexpfix",   ch->pcdata->has_exp_fix,  fread_number( fp ) );


						if ( !str_cmp( word, "HiCol" ) )
						{
							char * temp;
							temp =  fread_string( fp );
							ch->pcdata->hicol = temp[0];
/*              fread_to_eol( fp );   */
							free_string( temp );
							fMatch = TRUE;
							break;
						}

			if ( !str_cmp( word, "HpManaMove" ) )
			{
		ch->hit         = fread_number( fp );
		ch->max_hit     = fread_number( fp );
		ch->mana        = fread_number( fp );
		ch->max_mana    = fread_number( fp );
		ch->move        = fread_number( fp );
		ch->max_move    = fread_number( fp );
		fMatch = TRUE;
		break;
			}
						KEY( "Hometown",    ch->hometown,           fread_number( fp ) );
			break;

	case 'I':
			SKEY ( "Immskll",    ch->pcdata->immskll,    fread_string( fp ) );
			KEY ( "IMC",	ch->pcdata->imc_deaf,	fread_number( fp ) );
			KEY ( "IMCAllow",	ch->pcdata->imc_allow,	fread_number( fp ) );
			KEY ( "IMCDeny",	ch->pcdata->imc_deny,	fread_number( fp ) );
			SKEY( "ICEListen", ch->pcdata->ice_listen, fread_string( fp ) );
			KEY( "Incog", ch->incog,		fread_number( fp ) );
			KEY( "Invis",	ch->invis,		fread_number( fp ) );
			break;
	
	
	
	case 'L':
			
			KEY( "Level",       ch->level,              fread_number( fp ) );
		KEY( "Laston",      ch->pcdata->laston,      fread_number( fp ) );
			SKEY( "LongDescr",   ch->long_descr,         fread_string( fp ) );
						KEY( "LoginSex",	ch->login_sex,		fread_number( fp ) );

			if (!IS_NPC(ch))
			{
			 SKEY( "LastLogin",   ch->pcdata->lastlogin,  fread_string( fp ) );
			}
			break;

	case 'M': 
			if (!IS_NPC(ch))
			{
			 KEY( "Mkills",	ch->pcdata->mkills,	fread_number( fp ) );
			 KEY( "Mkilled",	ch->pcdata->mkilled,	fread_number( fp ) );
			 KEY( "Monitor",	ch->pcdata->monitor,    fread_number( fp ) );
			}
			if ( !str_cmp( word, "Money" ) )
			{
				MONEY_TYPE * transfer;
				int num_coins;
				GET_FREE( transfer, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "reading player money, %s", 
			ch->name );
		transfer->money_key = str_dup( testbuf );
	}
#endif
				num_coins = fread_number( fp );
				for (cnt=0; cnt < num_coins; cnt++)
				 transfer->cash_unit[( cnt < MAX_CURRENCY ? cnt : MAX_CURRENCY -1)] = fread_number( fp );
				join_money( transfer, ch->money );
				ch->carry_weight += money_weight( transfer );
				fMatch = TRUE;
				break;
			}

			break;
	
	case 'N':
			if ( !str_cmp( word, "Name" ) )
			{
		/*
		 * Name already set externally.
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
			}
			KEY( "Note",        ch->last_note,          fread_number( fp ) );
			KEY( "NumOfSpells", ch->num_of_spells,      fread_number( fp ) );
			KEY( "NumOfSkills", ch->num_of_skills,      fread_number( fp ) );
			break;

				case 'O':
			break;
	
	case 'P':
			if (!IS_NPC(ch))
			{
			 KEY( "Pagelen",     ch->pcdata->pagelen,    fread_number( fp ) );
			 SKEY( "Password",    ch->pcdata->pwd,        fread_string( fp ) );
			 KEY( "Pkills",	ch->pcdata->pkills,	fread_number( fp ) );
			 KEY( "Pkilled",	ch->pcdata->pkilled,	fread_number( fp ) );
			 KEY( "Pflags",      ch->pcdata->pflags,     fread_number( fp ) );
			}
			KEY( "Played",      ch->played,             fread_number( fp ) );
					KEY( "Points",      ch->pcdata->points,     fread_number( fp ) );
			KEY( "Position",    ch->position,           fread_number( fp ) );
			KEY( "Practice",    ch->practice,           fread_number( fp ) );
			SKEY( "Prompt",      ch->prompt,             fread_string( fp ) );
			break;

			case 'Q':
					KEY( "Questpoints", ch->quest_points,   fread_number( fp ) );
					break;

	case 'R':
			KEY( "Race",      ch->race,               fread_number( fp ) );
			KEY( "Revision",  cur_revision,           fread_number( fp ) );
					SKEY( "Roomenter",	ch->pcdata->room_enter, fread_string( fp ) );
					SKEY( "Roomexit",	ch->pcdata->room_exit,  fread_string( fp ) );
			KEY( "RulerRank",   ch->pcdata->ruler_rank, fread_number( fp ) );

			
			if ( !str_cmp( word, "Room" ) )
			{
		ch->in_room = get_room_index( fread_number( fp ) );
		if ( ch->in_room == NULL )
				ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
		fMatch = TRUE;
		break;
			}
					KEY( "RecallVnum",	ch->pcdata->recall_vnum,	fread_number( fp ) );

			break;

	

	case 'S':
			KEY( "SavingThrow", ch->saving_throw,       fread_number( fp ) );
			KEY( "Sentence",    ch->sentence,           fread_number( fp ) );
			KEY( "Sex",         ch->sex,                fread_number( fp ) );
			SKEY( "ShortDescr",  ch->short_descr,        fread_string( fp ) );
				KEY( "Strategy",    ch->pcdata->strategy,   fread_number( fp ) );
			if ( !str_cmp( word, "Skill" ) && !IS_NPC(ch))
			{
				int sn;
				int value;
				char * skill_word;
				value = fread_number( fp );
				skill_word = fread_word( fp );
				sn    = skill_lookup( skill_word );
				if ( sn < 0 )
				{
					sprintf( log_buf, "Loading pfile %s, unknown skill %s.",
						ch->name, skill_word );
					monitor_chan( log_buf, MONITOR_BAD );
				}
		else
				ch->pcdata->learned[sn] = value;
		fMatch = TRUE;
			}

			break;

	case 'T':
			KEY( "TotalExp",    ch->total_exp,          fread_number( fp ) );
		KEY( "Trust",       ch->trust,              fread_number( fp ) );
			KEY( "Train",       ch->train,              fread_number( fp ) );
		KEY( "TermRows", ch->pcdata->term_rows,     fread_number( fp ) );
			KEY( "TermColumns", ch->pcdata->term_columns, fread_number( fp ) );
			if ( !str_cmp( word, "Title" ) && !IS_NPC(ch))
			{
								if ( ch->pcdata->title != NULL )
									free_string( ch->pcdata->title );
		ch->pcdata->title = fread_string( fp );
		if ( isalpha(ch->pcdata->title[0])
		||   isdigit(ch->pcdata->title[0]) )
		{
				sprintf( buf, " %s", ch->pcdata->title );
				free_string( ch->pcdata->title );
				ch->pcdata->title = str_dup( buf );
		}
		fMatch = TRUE;
		break;
			}
			break;

	case 'V':
			if ( !str_cmp( word, "Vnum" ) && IS_NPC(ch))
			{
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
			}

					KEY( "Vamplevel",       ch->pcdata->vamp_level,              fread_number( fp ) );
			KEY( "Vampexp",         ch->pcdata->vamp_exp,                fread_number( fp ) );    
					KEY( "Vampbloodline",   ch->pcdata->vamp_bloodline,          fread_number( fp ) );
					KEY( "Vampskillnum",    ch->pcdata->vamp_skill_num,           fread_number( fp ) );
			KEY( "Vampskillmax",    ch->pcdata->vamp_skill_max,          fread_number( fp ) ); 
					KEY( "Vamppracs",       ch->pcdata->vamp_pracs,              fread_number( fp ) );
	 

			break;

	case 'W':
			KEY( "Wimpy",       ch->wimpy,              fread_number( fp ) );
			KEY( "Wizbit",      ch->wizbit,             fread_number( fp ) );
			if ( !str_cmp( word, "Whoname" ) )
			{
							 if ( ch->pcdata->who_name != NULL )
								 free_string( ch->pcdata->who_name );
							 ch->pcdata->who_name = fread_string( fp );
							 sprintf( buf, "%s", ch->pcdata->who_name+1 ); 

							 free_string( ch->pcdata->who_name );  
							 ch->pcdata->who_name = str_dup( buf );
							 fMatch = TRUE;
							 break;
						}   	    
			
			break;
	}

	/* Make sure old chars have this field - Kahn */
	if (!IS_NPC(ch))
	{
	 if ( !ch->pcdata->pagelen )
			ch->pcdata->pagelen = 20;
	 if ( !ch->prompt || *ch->prompt == '\0' )
			ch->prompt = str_dup("<%h %m %mv> ");
	}

	/* Why this: ?? */
	/* if ( ch->exp > 2000 )
			 ch->exp = 2000;      */
			ch->long_descr_orig = str_dup( ch->long_descr );	
	if ( !fMatch )
	{
		 sprintf( log_buf, "Loading in pfile :%s, no match for ( %s ).",
			ch->name, word );
			monitor_chan( log_buf, MONITOR_BAD );
			fread_to_eol( fp );
	}
		}
		
			
}


#define TEMP_VNUM 3090

extern int top_obj_index;

void fread_obj( CHAR_DATA *ch, FILE *fp )
{
		static OBJ_DATA obj_zero;
		OBJ_DATA *obj;
		char *word;
		int iNest;
		bool fMatch;
		bool fNest;
		bool fVnum;
		int Temp_Obj=0,OldVnum=0;
		extern int cur_revision;

		GET_FREE(obj, obj_free);
		*obj                = obj_zero;
		obj->name           = str_dup( "" );
		obj->short_descr    = str_dup( "" );
		obj->description    = str_dup( "" );
		{
			MONEY_TYPE * money;
			sh_int cnt;
			GET_FREE( money, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "loading obj money, %s", 
			obj->name );
		money->money_key = str_dup( testbuf );
	}
#endif
			for ( cnt = 0; cnt < MAX_CURRENCY; cnt++ )
				money->cash_unit[cnt] = 0;
			obj->money = money;
		}
		fNest               = FALSE;
		fVnum               = TRUE;
		iNest               = 0;

		for ( ; ; )
		{
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;

	case 'A':
			if ( !str_cmp( word, "Affect" ) )
			{
		AFFECT_DATA *paf;

		GET_FREE(paf, affect_free);
		paf->type       = fread_number( fp );
		paf->duration   = fread_number( fp );
		paf->modifier   = fread_number( fp );
		paf->location   = fread_number( fp );
		paf->bitvector  = fread_number( fp );
		LINK(paf, obj->first_apply, obj->last_apply, next, prev);
		fMatch          = TRUE;
		break;
			}
			break;

	case 'C':
			KEY( "Cost",        obj->cost,              fread_number( fp ) );
			KEY( "ClassFlags",  obj->item_apply,        fread_number( fp ) );
			break;

	case 'D':
			SKEY( "Description", obj->description,       fread_string( fp ) );
			break;

	case 'E':
			KEY( "ExtraFlags",  obj->extra_flags,       fread_number( fp ) );

			if ( !str_cmp( word, "ExtraDescr" ) )
			{
		EXTRA_DESCR_DATA *ed;

		GET_FREE(ed, exdesc_free);
		ed->keyword             = fread_string( fp );
		ed->description         = fread_string( fp );
		LINK(ed, obj->first_exdesc, obj->last_exdesc, next, prev);
		fMatch = TRUE;
			}

			if ( !str_cmp( word, "End" ) )
			{
		if ( !fNest || !fVnum )
		{
				AFFECT_DATA *paf;
				EXTRA_DESCR_DATA *ed;
				
				monitor_chan( "Fread_obj: incomplete object.", MONITOR_BAD );
				while ( (paf = obj->first_apply) != NULL )
				{
					obj->first_apply = paf->next;
					PUT_FREE(paf, affect_free);
				}
				while ( (ed = obj->first_exdesc) != NULL )
				{
					obj->first_exdesc = ed->next;
					PUT_FREE(ed, exdesc_free);
				}
				PUT_FREE(obj, obj_free);
				return;
		}
		else
		{
				LINK(obj, first_obj, last_obj, next, prev);
				obj->pIndexData->count++;
				
				if (Temp_Obj)
				{
				 int newvnum;
				 OBJ_INDEX_DATA *pObjIndex;
				 int nMatch=0;
				 int vnum;
				
				 /* One of three things:
							Obj Vnum was deleted
							Obj Vnum was moved
							Obj Vnum was previously deleted */
				 newvnum=TEMP_VNUM;     
				 
				 if (OldVnum != TEMP_VNUM)
				 {
					/* Check on move table */
					if ( (newvnum=(int) get_hash_entry(hash_changed_vnums,OldVnum)) != 0)
					{
					 obj->pIndexData=get_obj_index(newvnum);
					 if (obj->pIndexData == NULL)
					 {
						obj->pIndexData=get_obj_index(TEMP_VNUM);
						newvnum=TEMP_VNUM;
					 }
					}
				 }
				 
				 if (newvnum==TEMP_VNUM)
				 {
					/* Scan through objects, trying to find a matching description */
											for ( vnum = 0; nMatch < top_obj_index; vnum++ )
											{
										if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
									{
													nMatch++;
											if ( !str_cmp( obj->short_descr, pObjIndex->short_descr ) )
											{
											obj->pIndexData=pObjIndex;
											break;
													}
										}
					}
				 }
				 
				} 
				
				if ( iNest == 0 || rgObjNest[iNest] == NULL )
			obj_to_char( obj, ch );
				else 
			obj_to_obj( obj, rgObjNest[iNest-1] );
				return;
		}
			}
			break;

	case 'I':
			KEY( "ItemType",    obj->item_type,         fread_number( fp ) );
			break;

	case 'L':
			KEY( "Level",       obj->level,             fread_number( fp ) );
			break;
	case 'M':
			if ( !str_cmp( word, "Money" ) )
			{
				MONEY_TYPE * transfer;
				int num_coins, cnt;
				GET_FREE( transfer, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "loading obj money, %s", 
			obj->name );
		transfer->money_key = str_dup( testbuf );
	}
#endif
				num_coins = fread_number( fp );
				for (cnt=0; cnt < num_coins; cnt++)
				 transfer->cash_unit[( cnt < MAX_CURRENCY ? cnt : MAX_CURRENCY -1)] = fread_number( fp );
				join_money( transfer, obj->money );
				fMatch = TRUE;
				break;
			}
	case 'N':
			SKEY( "Name",        obj->name,              fread_string( fp ) );

			if ( !str_cmp( word, "Nest" ) )
			{
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
				monitor_chan( "Fread_obj: bad nest.", MONITOR_BAD );
		}
		else
		{
				rgObjNest[iNest] = obj;
				fNest = TRUE;
		}
		fMatch = TRUE;
			}
			break;

	case 'O':
/*	   KEY( "Objfun", obj->obj_fun,  obj_fun_lookup( fread_string( fp ) ) );  */
					 if ( !str_cmp( word, "Objfun" ) )
					 {
						 char	* dumpme;
						 dumpme = fread_string(fp );
						 obj->obj_fun = obj_fun_lookup( dumpme );
						 free_string( dumpme );
						 fMatch = TRUE;
					 }
		 break;   
	
	case 'S':
			SKEY( "ShortDescr",  obj->short_descr,       fread_string( fp ) );
			KEY(  "Stored",      obj->stored,            fread_number( fp ) );
			if ( !str_cmp( word, "Spell" ) )
			{
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 3 )
		{
				monitor_chan( "Fread_obj: bad iValue ", MONITOR_BAD );
		}
		else if ( sn < 0 )
		{
				monitor_chan( "Fread_obj: unknown skill.", MONITOR_BAD );
		}
		else
		{
				obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
			}

			break;

	case 'T':
			KEY( "Timer",       obj->timer,             fread_number( fp ) );
			break;

	case 'V':
			if ( !str_cmp( word, "Values" ) )
			{
				int looper;

				if ( cur_revision < UPGRADE_REVISION )
				{
					obj->value[0]   = fread_number( fp );
					obj->value[1]   = fread_number( fp );
					obj->value[2]   = fread_number( fp );
					obj->value[3]   = fread_number( fp );
					for ( looper = 4; 
								looper < 10; 
								obj->value[looper] = 0,looper++ );
				}
				else
				{
					for( looper = 0; 
							 looper < 10; 
							 obj->value[looper] = fread_number( fp ),looper++ );
				}
				fMatch          = TRUE;
				break;
			}

			if ( !str_cmp( word, "Vnum" ) )
			{
		int vnum;

		vnum = fread_number( fp );
		
		
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL 
				 || vnum == TEMP_VNUM)
		{
			 /* Set flag saying that object is temporary */
			 Temp_Obj=1;
			 OldVnum=vnum;
			 vnum = TEMP_VNUM;
			 obj->pIndexData = get_obj_index(vnum);
		}
				/* bug( "Fread_obj: bad vnum %d.", vnum ); This killed it. */
		else
				fVnum = TRUE;
		fMatch = TRUE;
		break;
			}
			break;

	case 'W':
			if ( !str_cmp( word, "WearLoc" ) )
			{
				if ( cur_revision < UPGRADE_REVISION )
				{
					int temp_loc;
					temp_loc = fread_number( fp );
					obj->wear_loc = -1;
					fMatch = TRUE;
					break;
				}
				else
				{
					KEY( "WearLoc",     obj->wear_loc,          fread_number( fp ) );
				}
			}
			if ( !str_cmp( word, "WearFlags" ) )
			{
				if ( cur_revision < UPGRADE_REVISION )
				{
					int temp_flags, index, new_flags = 0;
					extern const int convert_wearflags[];
					temp_flags = fread_number( fp );
					for ( index = 0; index < 32; index++ )
					{
						if (  IS_SET( temp_flags, ( 1 << index )  ) )
						{
							SET_BIT( new_flags, convert_wearflags[ index ]);
						}
					}
					obj->wear_flags = new_flags;
					fMatch = TRUE;
					break;
				}
				else
				{
					KEY( "WearFlags",     obj->wear_flags,          fread_number( fp ) );
				}
			}
			KEY( "Weight",      obj->weight,            fread_number( fp ) );
			break;

	}

	if ( !fMatch )
	{
			monitor_chan( "Fread_obj: no match.", MONITOR_BAD );
			fread_to_eol( fp );
	}
		}
}

void fread_corpse( FILE *fp )
{
		static OBJ_DATA obj_zero;
		OBJ_DATA *obj;
		char *word;
		int iNest;
		bool fMatch;
		bool fNest;
		bool fVnum;
		int Temp_Obj=0,OldVnum=0;
		int this_room_vnum;

		GET_FREE(obj, obj_free);
		*obj                = obj_zero;
		obj->name           = str_dup( "" );
		obj->short_descr    = str_dup( "" );
		obj->description    = str_dup( "" );
		{
			MONEY_TYPE * money;
			sh_int cnt;
			GET_FREE( money, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "loading obj money, %s", 
			obj->name );
		money->money_key = str_dup( testbuf );
	}
#endif
			for ( cnt = 0; cnt < MAX_CURRENCY; cnt++ )
				money->cash_unit[cnt] = 0;
			obj->money = money;
		}
		fNest               = FALSE;
		fVnum               = TRUE;
		iNest               = 0;
		this_room_vnum      = 0;

		for ( ; ; )
		{
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;

	case 'A':
			if ( !str_cmp( word, "Affect" ) )
			{
		AFFECT_DATA *paf;

		GET_FREE(paf, affect_free);
		paf->type       = fread_number( fp );
		paf->duration   = fread_number( fp );
		paf->modifier   = fread_number( fp );
		paf->location   = fread_number( fp );
		paf->bitvector  = fread_number( fp );
		LINK(paf, obj->first_apply, obj->last_apply, next, prev);
		fMatch          = TRUE;
		break;
			}
			break;

	case 'C':
			KEY( "Cost",        obj->cost,              fread_number( fp ) );
			KEY( "ClassFlags",  obj->item_apply,        fread_number( fp ) );
			break;

	case 'D':
			SKEY( "Description", obj->description,       fread_string( fp ) );
			break;

	case 'E':
			KEY( "ExtraFlags",  obj->extra_flags,       fread_number( fp ) );

			if ( !str_cmp( word, "ExtraDescr" ) )
			{
		EXTRA_DESCR_DATA *ed;

		GET_FREE(ed, exdesc_free);
		ed->keyword             = fread_string( fp );
		ed->description         = fread_string( fp );
		LINK(ed, obj->first_exdesc, obj->last_exdesc, next, prev);
		fMatch = TRUE;
			}

			if ( !str_cmp( word, "End" ) )
			{
		if ( !fNest || !fVnum )
		{
				AFFECT_DATA *paf;
				EXTRA_DESCR_DATA *ed;
				
				monitor_chan( "Fread_obj: incomplete object.", MONITOR_BAD );
				while ( (paf = obj->first_apply) != NULL )
				{
					obj->first_apply = paf->next;
					PUT_FREE(paf, affect_free);
				}
				while ( (ed = obj->first_exdesc) != NULL )
				{
					obj->first_exdesc = ed->next;
					PUT_FREE(ed, exdesc_free);
				}
				PUT_FREE(obj, obj_free);
				return;
		}
		else
		{
				LINK(obj, first_obj, last_obj, next, prev);
				obj->pIndexData->count++;
				
				if (Temp_Obj)
				{
				 int newvnum;
				 OBJ_INDEX_DATA *pObjIndex;
				 int nMatch=0;
				 int vnum;
				
				 /* One of three things:
							Obj Vnum was deleted
							Obj Vnum was moved
							Obj Vnum was previously deleted */
				 newvnum=TEMP_VNUM;     
				 
				 if (OldVnum != TEMP_VNUM)
				 {
					/* Check on move table */
					if ( (newvnum=(int) get_hash_entry(hash_changed_vnums,OldVnum)) != 0)
					{
					 obj->pIndexData=get_obj_index(newvnum);
					 if (obj->pIndexData == NULL)
					 {
						obj->pIndexData=get_obj_index(TEMP_VNUM);
						newvnum=TEMP_VNUM;
					 }
					}
				 }
				 
				 if (newvnum==TEMP_VNUM)
				 {
					/* Scan through objects, trying to find a matching description */
											for ( vnum = 0; nMatch < top_obj_index; vnum++ )
											{
										if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
									{
													nMatch++;
											if ( !str_cmp( obj->short_descr, pObjIndex->short_descr ) )
											{
											obj->pIndexData=pObjIndex;
											break;
													}
										}
					}
				 }
				 
				} 
				
				if ( iNest == 0 || rgObjNest[iNest] == NULL )
			obj_to_room( obj, get_room_index( this_room_vnum ) );
				else
			obj_to_obj( obj, rgObjNest[iNest-1] );
				return;
		}
			}
			break;

	case 'I':
			KEY( "ItemType",    obj->item_type,         fread_number( fp ) );
			break;

	case 'L':
			KEY( "Level",       obj->level,             fread_number( fp ) );
			break;
	case 'M':
			if ( !str_cmp( word, "Money" ) )
			{
				MONEY_TYPE * transfer;
				int num_coins, cnt;
				GET_FREE( transfer, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "loading obj money, %s", 
			obj->name );
		transfer->money_key = str_dup( testbuf );
	}
#endif
				num_coins = fread_number( fp );
				for (cnt=0; cnt < num_coins; cnt++)
				 transfer->cash_unit[( cnt < MAX_CURRENCY ? cnt : MAX_CURRENCY -1)] = fread_number( fp );
				join_money( transfer, obj->money );
				fMatch = TRUE;
				break;
			}
	case 'N':
			KEY( "Name",        obj->name,              fread_string( fp ) );

			if ( !str_cmp( word, "Nest" ) )
			{
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
				monitor_chan( "Fread_obj: bad nest.", MONITOR_BAD );
		}
		else
		{
				rgObjNest[iNest] = obj;
				fNest = TRUE;
		}
		fMatch = TRUE;
			}
			break;

	case 'O':
/*	   KEY( "Objfun", obj->obj_fun,  obj_fun_lookup( fread_string( fp ) ) );  */
					 if ( !str_cmp( word, "Objfun" ) )
					 {
						 char	* dumpme;
						 dumpme = fread_string(fp );
						 obj->obj_fun = obj_fun_lookup( dumpme );
						 free_string( dumpme );
						 fMatch = TRUE;
					 }
		 break;   
	
	case 'S':
			SKEY( "ShortDescr",  obj->short_descr,       fread_string( fp ) );
			KEY( "Stored",       obj->stored,            fread_number( fp ) );
			if ( !str_cmp( word, "Spell" ) )
			{
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 3 )
		{
				monitor_chan( "Fread_obj: bad iValue ", MONITOR_BAD );
		}
		else if ( sn < 0 )
		{
				monitor_chan( "Fread_obj: unknown skill.", MONITOR_BAD );
		}
		else
		{
				obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
			}

			break;

	case 'T':
			KEY( "Timer",       obj->timer,             fread_number( fp ) );
			break;

	case 'V':
			if ( !str_cmp( word, "Values" ) )
			{
		obj->value[0]   = fread_number( fp );
		obj->value[1]   = fread_number( fp );
		obj->value[2]   = fread_number( fp );
		obj->value[3]   = fread_number( fp );
		fMatch          = TRUE;
		break;
			}

			if ( !str_cmp( word, "Vnum" ) )
			{
		int vnum;

		vnum = fread_number( fp );
		
		
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL 
				 || vnum == TEMP_VNUM)
		{
			 /* Set flag saying that object is temporary */
			 Temp_Obj=1;
			 OldVnum=vnum;
			 vnum = TEMP_VNUM;
			 obj->pIndexData = get_obj_index(vnum);
		}
				/* bug( "Fread_obj: bad vnum %d.", vnum ); This killed it. */
		else
				fVnum = TRUE;
		fMatch = TRUE;
		break;
			}
			break;

	case 'W':
			KEY( "WearFlags",   obj->wear_flags,        fread_number( fp ) );
			KEY( "WearLoc",     obj->wear_loc,          fread_number( fp ) );
			KEY( "Weight",      obj->weight,            fread_number( fp ) );
					KEY( "WhereVnum",   this_room_vnum,         fread_number( fp ) );
			break;

	}

	if ( !fMatch )
	{
			monitor_chan( "Fread_obj: no match.", MONITOR_BAD );
			fread_to_eol( fp );
	}
		}
}
/* Start fread clans */
void fread_clan( FILE *fp )
{
		char *word;
	int ceq;
		bool fMatch;
	bool endoq;
	CLAN_DATA *pClan;
	extern int top_clan;

	GET_FREE(pClan, clan_free);
	pClan->clan_name = str_dup("");
	pClan->clan_abbr = str_dup("");
	pClan->clan_room = 0;
	pClan->donat_room = 0;
	pClan->enemies =   str_dup("");
		pClan->members =   str_dup("");
	pClan->leader =    str_dup("");
	for ( ceq = 0; ceq < MAX_CLAN_EQ; ceq++ );
	pClan->eq[ceq] = 0;
	pClan->clan_numb = 0;
	endoq = FALSE;

		for ( ; ; )
		{
	word   = feof( fp ) ? "End" : fread_word( fp ); 
	fMatch = FALSE;

	switch ( LOWER(word[0]) )
	{
	case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;

	case 'c':
			SKEY( "clan_name", pClan->clan_name,       fread_string( fp ) );
			SKEY( "clan_abbr", pClan->clan_abbr,       fread_string( fp ) );
			 KEY( "clan_room", pClan->clan_room,       fread_number( fp ) );
		 if (!str_cmp(word, "clan_numb") )
		 {
			endoq = TRUE;
			KEY( "clan_numb", pClan->clan_numb,       fread_number( fp ) );
			break;
		 }
			break;

	case 'd':
			KEY( "donat_room", pClan->donat_room,       fread_number( fp ) );
			break;

	case 'e':
				SKEY( "enemies", pClan->enemies,       fread_string( fp ) );
		if ( !str_cmp( word, "end" ) )
		{
			if(endoq)
			{
						LINK(pClan, first_clan, last_clan, next, prev);
			log_f("Clan Linked in Load");
			top_clan++;
			return;
			} else {
			PUT_FREE(pClan, clan_free);
			log_f("Clan put free in load");
			return;
			}
			return;
		}

		if ( !str_cmp( word, "eq" ) )
			{
		pClan->eq[0]     =  fread_number( fp );
		pClan->eq[1]     =  fread_number( fp );
		pClan->eq[2]     =  fread_number( fp );
		pClan->eq[3]     =  fread_number( fp );
		pClan->eq[4]     =  fread_number( fp );
		pClan->eq[5]     =  fread_number( fp );
		fMatch          = TRUE;
		break;
			}

		break;
	case 'l':
		SKEY( "leader", pClan->leader,       fread_string( fp ) );
			break;
	case 'm' :
		SKEY( "members", pClan->members,     fread_string( fp ) );
		break;
	}

	if ( !fMatch )
	{
			monitor_chan( "Fread_obj: no match.", MONITOR_BAD );
			fread_to_eol( fp );
	}
	}
}
/* Ending Clan Read */

void fread_hobj( FILE *fp )
{
		static OBJ_DATA obj_zero;
		OBJ_DATA *obj;
		char *word;
		int iNest;
		bool fMatch;
		bool fNest;
		bool fVnum;
		int Temp_Obj=0,OldVnum=0;
		int this_room_vnum;

		GET_FREE(obj, obj_free);
		*obj                = obj_zero;
		obj->name           = str_dup( "" );
		obj->short_descr    = str_dup( "" );
		obj->description    = str_dup( "" );
		{
			MONEY_TYPE * money;
			sh_int cnt;
			GET_FREE( money, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "loading obj money, %s", 
			obj->name );
		money->money_key = str_dup( testbuf );
	}
#endif
			for ( cnt = 0; cnt < MAX_CURRENCY; cnt++ )
				money->cash_unit[cnt] = 0;
			obj->money = money;
		}
		fNest               = FALSE;
		fVnum               = TRUE;
		iNest               = 0;
		this_room_vnum      = 0;

		for ( ; ; )
		{
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;

	case 'A':
			if ( !str_cmp( word, "Affect" ) )
			{
		AFFECT_DATA *paf;

		GET_FREE(paf, affect_free);
		paf->type       = fread_number( fp );
		paf->duration   = fread_number( fp );
		paf->modifier   = fread_number( fp );
		paf->location   = fread_number( fp );
		paf->bitvector  = fread_number( fp );
		LINK(paf, obj->first_apply, obj->last_apply, next, prev);
		fMatch          = TRUE;
		break;
			}
			break;

	case 'C':
			KEY( "Cost",        obj->cost,              fread_number( fp ) );
			KEY( "ClassFlags",  obj->item_apply,        fread_number( fp ) );
			break;

	case 'D':
			SKEY( "Description", obj->description,       fread_string( fp ) );
			break;

	case 'E':
			KEY( "ExtraFlags",  obj->extra_flags,       fread_number( fp ) );

			if ( !str_cmp( word, "ExtraDescr" ) )
			{
		EXTRA_DESCR_DATA *ed;

		GET_FREE(ed, exdesc_free);
		ed->keyword             = fread_string( fp );
		ed->description         = fread_string( fp );
		LINK(ed, obj->first_exdesc, obj->last_exdesc, next, prev);
		fMatch = TRUE;
			}

			if ( !str_cmp( word, "End" ) )
			{
		if ( !fNest || !fVnum )
		{
				AFFECT_DATA *paf;
				EXTRA_DESCR_DATA *ed;
				
				monitor_chan( "Fread_obj: incomplete object.", MONITOR_BAD );
				while ( (paf = obj->first_apply) != NULL )
				{
					obj->first_apply = paf->next;
					PUT_FREE(paf, affect_free);
				}
				while ( (ed = obj->first_exdesc) != NULL )
				{
					obj->first_exdesc = ed->next;
					PUT_FREE(ed, exdesc_free);
				}
				PUT_FREE(obj, obj_free);
				return;
		}
		else
		{
				LINK(obj, first_obj, last_obj, next, prev);
				obj->pIndexData->count++;
				
				if (Temp_Obj)
				{
				 int newvnum;
				 OBJ_INDEX_DATA *pObjIndex;
				 int nMatch=0;
				 int vnum;
				
				 /* One of three things:
							Obj Vnum was deleted
							Obj Vnum was moved
							Obj Vnum was previously deleted */
				 newvnum=TEMP_VNUM;     
				 
				 if (OldVnum != TEMP_VNUM)
				 {
					/* Check on move table */
					if ( (newvnum=(int) get_hash_entry(hash_changed_vnums,OldVnum)) != 0)
					{
					 obj->pIndexData=get_obj_index(newvnum);
					 if (obj->pIndexData == NULL)
					 {
						obj->pIndexData=get_obj_index(TEMP_VNUM);
						newvnum=TEMP_VNUM;
					 }
					}
				 }
				 
				 if (newvnum==TEMP_VNUM)
				 {
					/* Scan through objects, trying to find a matching description */
											for ( vnum = 0; nMatch < top_obj_index; vnum++ )
											{
										if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
									{
													nMatch++;
											if ( !str_cmp( obj->short_descr, pObjIndex->short_descr ) )
											{
											obj->pIndexData=pObjIndex;
											break;
													}
										}
					}
				 }
				 
				} 
				
				if ( iNest == 0 || rgObjNest[iNest] == NULL )
			obj_to_room( obj, get_room_index( this_room_vnum ) );
				else
			obj_to_obj( obj, rgObjNest[iNest-1] );
				return;
		}
			}
			break;

	case 'I':
			KEY( "ItemType",    obj->item_type,         fread_number( fp ) );
			break;

	case 'L':
			KEY( "Level",       obj->level,             fread_number( fp ) );
			break;
	case 'M':
			if ( !str_cmp( word, "Money" ) )
			{
				MONEY_TYPE * transfer;
				int num_coins, cnt;
				GET_FREE( transfer, money_type_free );
#ifdef DEBUG_MONEY
	{
		char testbuf[MSL];
		sprintf( testbuf, "loading obj money, %s", 
			obj->name );
		transfer->money_key = str_dup( testbuf );
	}
#endif
				num_coins = fread_number( fp );
				for (cnt=0; cnt < num_coins; cnt++)
				 transfer->cash_unit[( cnt < MAX_CURRENCY ? cnt : MAX_CURRENCY -1)] = fread_number( fp );
				join_money( transfer, obj->money );
				fMatch = TRUE;
				break;
			}
	case 'N':
			KEY( "Name",        obj->name,              fread_string( fp ) );

			if ( !str_cmp( word, "Nest" ) )
			{
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
				monitor_chan( "Fread_obj: bad nest.", MONITOR_BAD );
		}
		else
		{
				rgObjNest[iNest] = obj;
				fNest = TRUE;
		}
		fMatch = TRUE;
			}
			break;

	case 'O':
/*	   KEY( "Objfun", obj->obj_fun,  obj_fun_lookup( fread_string( fp ) ) );  */
					 if ( !str_cmp( word, "Objfun" ) )
					 {
						 char	* dumpme;
						 dumpme = fread_string(fp );
						 obj->obj_fun = obj_fun_lookup( dumpme );
						 free_string( dumpme );
						 fMatch = TRUE;
					 }
		 break;   
	
	case 'S':
			SKEY( "ShortDescr",  obj->short_descr,       fread_string( fp ) );
			KEY( "Stored",       obj->stored,            fread_number( fp ) );
			if ( !str_cmp( word, "Spell" ) )
			{
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 3 )
		{
				monitor_chan( "Fread_obj: bad iValue ", MONITOR_BAD );
		}
		else if ( sn < 0 )
		{
				monitor_chan( "Fread_obj: unknown skill.", MONITOR_BAD );
		}
		else
		{
				obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
			}

			break;

	case 'T':
			KEY( "Timer",       obj->timer,             fread_number( fp ) );
			break;

	case 'V':
			if ( !str_cmp( word, "Values" ) )
			{
		obj->value[0]   = fread_number( fp );
		obj->value[1]   = fread_number( fp );
		obj->value[2]   = fread_number( fp );
		obj->value[3]   = fread_number( fp );
		fMatch          = TRUE;
		break;
			}

			if ( !str_cmp( word, "Vnum" ) )
			{
		int vnum;

		vnum = fread_number( fp );
		
		
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL 
				 || vnum == TEMP_VNUM)
		{
			 /* Set flag saying that object is temporary */
			 Temp_Obj=1;
			 OldVnum=vnum;
			 vnum = TEMP_VNUM;
			 obj->pIndexData = get_obj_index(vnum);
		}
				/* bug( "Fread_obj: bad vnum %d.", vnum ); This killed it. */
		else
				fVnum = TRUE;
		fMatch = TRUE;
		break;
			}
			break;

	case 'W':
			KEY( "WearFlags",   obj->wear_flags,        fread_number( fp ) );
			KEY( "WearLoc",     obj->wear_loc,          fread_number( fp ) );
			KEY( "Weight",      obj->weight,            fread_number( fp ) );
					KEY( "WhereVnum",   this_room_vnum,         fread_number( fp ) );
			break;

	}

	if ( !fMatch )
	{
			monitor_chan( "Fread_obj: no match.", MONITOR_BAD );
			fread_to_eol( fp );
	}
		}
}

void fwrite_corpse( OBJ_DATA *obj, FILE *fp, int iNest )
{
		EXTRA_DESCR_DATA *ed;
		AFFECT_DATA *paf;
		int   where_vnum = 3300;
		sh_int foo;
		/*
		 * Slick recursion to write lists backwards,
		 *   so loading them will load in forwards order.
		 */
		if ( obj->next_in_carry_list != NULL )
	fwrite_corpse( obj->next_in_carry_list, fp, iNest );

		if ( obj->in_obj != NULL )
			where_vnum = 3300;
		if ( obj->in_room != NULL )
			where_vnum = obj->in_room->vnum;

		if ( obj->in_room == NULL && obj->in_obj == NULL )
			obj->in_room = get_room_index( ROOM_VNUM_LIMBO );


		fprintf( fp, "#OBJECT\n" );
		fprintf( fp, "WhereVnum    %d\n",   where_vnum           );

		fprintf( fp, "Nest         %d\n",   iNest                        );
		fprintf( fp, "Name         %s~\n",  obj->name                    );
		fprintf( fp, "ShortDescr   %s~\n",  obj->short_descr             );
		fprintf( fp, "Description  %s~\n",  obj->description             );
		fprintf( fp, "Vnum         %d\n",   obj->pIndexData->vnum        );
		fprintf( fp, "ExtraFlags   %d\n",   obj->extra_flags             );
		fprintf( fp, "WearFlags    %d\n",   obj->wear_flags              );
		fprintf( fp, "WearLoc      %d\n",   obj->wear_loc                );
		fprintf( fp, "Money %d ", MAX_CURRENCY );
		for ( foo = 0; foo < MAX_CURRENCY; foo++ )
			fprintf( fp, "%d ", obj->money->cash_unit[foo] );
		if ( obj->obj_fun != NULL )
		fprintf( fp, "Objfun       %s~\n", 
			 rev_obj_fun_lookup( obj->obj_fun ) );
		
		fprintf( fp, "ClassFlags   %d\n",   obj->item_apply              );
		/* ClassFlags still used to save fucking with pfiles */
		fprintf( fp, "ItemType     %d\n",   obj->item_type               );
		fprintf( fp, "Weight       %d\n",   obj->weight                  );
		fprintf( fp, "Level        %d\n",   obj->level                   );
		fprintf( fp, "Timer        %d\n",   obj->timer                   );
		fprintf( fp, "Cost         %d\n",   obj->cost                    );
		fprintf( fp, "Values       %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3]           );

		switch ( obj->item_type )
		{
		case ITEM_POTION:
		case ITEM_SCROLL:
	if ( obj->value[1] > 0 )
	{
			fprintf( fp, "Spell 1      '%s'\n", 
		skill_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
			fprintf( fp, "Spell 2      '%s'\n", 
		skill_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
			fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;

		case ITEM_PILL:
		case ITEM_STAFF:
		case ITEM_WAND:
	if ( obj->value[3] > 0 )
	{
			fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;
		}

		for ( paf = obj->first_apply; paf != NULL; paf = paf->next )
		{
	fprintf( fp, "Affect       %d %d %d %d %d\n",
			paf->type,
			paf->duration,
			paf->modifier,
			paf->location,
			paf->bitvector
			);
		}

		for ( ed = obj->first_exdesc; ed != NULL; ed = ed->next )
		{
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
			ed->keyword, ed->description );
		}

		fprintf( fp, "End\n\n" );

		if ( obj->first_in_carry_list != NULL )
	fwrite_corpse( obj->first_in_carry_list, fp, iNest + 1 );

		return;
}

void fwrite_clan( CLAN_DATA *pClan, FILE *fp)
{
	int ceq;

		fprintf( fp, "#CLAN\n" );
		fprintf( fp, "clan_name    %s~\n",  pClan->clan_name             );
		fprintf( fp, "clan_abbr    %s~\n",  pClan->clan_abbr             );
		fprintf( fp, "donat_room   %d\n",   pClan->donat_room            );
		fprintf( fp, "clan_room    %d\n" ,  pClan->clan_room             );
		fprintf( fp, "leader       %s~\n",  pClan->leader                );
		fprintf( fp, "enemies      %s~\n",  pClan->enemies               );
	fprintf( fp, "members      %s~\n",  pClan->members               );
	fprintf( fp, "eq           " );
	for ( ceq = 0; ceq < MAX_CLAN_EQ; ceq++ )
	fprintf( fp,              "%d ",    pClan->eq[ceq]               );
	fprintf( fp, "\n" );
	fprintf( fp, "clan_numb    %d\n",   pClan->clan_numb             );
		fprintf( fp, "end\n\n" );
		return;
}


void fwrite_hobj( OBJ_DATA *obj, FILE *fp, int iNest )
{
		EXTRA_DESCR_DATA *ed;
		AFFECT_DATA *paf;
		int   where_vnum = 3300;
		sh_int foo;
		/*
		 * Slick recursion to write lists backwards,
		 *   so loading them will load in forwards order.
		 */
		if ( obj->next_in_carry_list != NULL )
	fwrite_hobj( obj->next_in_carry_list, fp, iNest );

		if ( obj->in_obj != NULL )
			where_vnum = 3300;
		if ( obj->in_room != NULL )
			where_vnum = obj->in_room->vnum;

		if ( obj->in_room == NULL && obj->in_obj == NULL )
			obj->in_room = get_room_index( ROOM_VNUM_LIMBO );


		fprintf( fp, "#OBJECT\n" );
		fprintf( fp, "WhereVnum    %d\n",   where_vnum           );

		fprintf( fp, "Nest         %d\n",   iNest                        );
		fprintf( fp, "Name         %s~\n",  obj->name                    );
		fprintf( fp, "ShortDescr   %s~\n",  obj->short_descr             );
		fprintf( fp, "Description  %s~\n",  obj->description             );
		fprintf( fp, "Vnum         %d\n",   obj->pIndexData->vnum        );
		fprintf( fp, "ExtraFlags   %d\n",   obj->extra_flags             );
		fprintf( fp, "WearFlags    %d\n",   obj->wear_flags              );
		fprintf( fp, "WearLoc      %d\n",   obj->wear_loc                );
		fprintf( fp, "Money %d ", MAX_CURRENCY );
		for ( foo = 0; foo < MAX_CURRENCY; foo++ )
			fprintf( fp, "%d ", obj->money->cash_unit[foo] );
		if ( obj->obj_fun != NULL )
		fprintf( fp, "Objfun       %s~\n", 
			 rev_obj_fun_lookup( obj->obj_fun ) );
		
		fprintf( fp, "ClassFlags   %d\n",   obj->item_apply              );
		/* ClassFlags still used to save fucking with pfiles */
		fprintf( fp, "ItemType     %d\n",   obj->item_type               );
		fprintf( fp, "Weight       %d\n",   obj->weight                  );
		fprintf( fp, "Level        %d\n",   obj->level                   );
		fprintf( fp, "Timer        %d\n",   obj->timer                   );
		fprintf( fp, "Cost         %d\n",   obj->cost                    );
		fprintf( fp, "Values       %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3]           );

		switch ( obj->item_type )
		{
		case ITEM_POTION:
		case ITEM_SCROLL:
	if ( obj->value[1] > 0 )
	{
			fprintf( fp, "Spell 1      '%s'\n", 
		skill_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
			fprintf( fp, "Spell 2      '%s'\n", 
		skill_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
			fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;

		case ITEM_PILL:
		case ITEM_STAFF:
		case ITEM_WAND:
	if ( obj->value[3] > 0 )
	{
			fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;
		}

		for ( paf = obj->first_apply; paf != NULL; paf = paf->next )
		{
	fprintf( fp, "Affect       %d %d %d %d %d\n",
			paf->type,
			paf->duration,
			paf->modifier,
			paf->location,
			paf->bitvector
			);
		}

		for ( ed = obj->first_exdesc; ed != NULL; ed = ed->next )
		{
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
			ed->keyword, ed->description );
		}

		fprintf( fp, "End\n\n" );

		if ( obj->first_in_carry_list != NULL )
	fwrite_hobj( obj->first_in_carry_list, fp, iNest + 1 );
		return;
}

void save_corpses( )
{

	FILE * fp;
	char corpse_file_name[MAX_STRING_LENGTH];
	CORPSE_DATA * this_corpse;

	fclose( fpReserve );
	sprintf( corpse_file_name, "%s", CORPSE_FILE );

 if ( ( fp = fopen( corpse_file_name, "w" ) ) == NULL )
		{
	bug( "Save Corpses: fopen", 0 );
	perror( "failed open of corpse_file in save_corpses" );
		}
		else
		{
			for (this_corpse = first_corpse; this_corpse != NULL; this_corpse = this_corpse->next )
			{
				fwrite_corpse(  this_corpse->this_corpse, fp, 0 );
			}
			fprintf( fp, "#END\n\n" );
			 
			fflush( fp );
			fclose( fp ); 
		}
	fpReserve = fopen( NULL_FILE, "r" );
	return;

}

void save_clans( )
{

	FILE * fp;
	char clan_file_name[MAX_STRING_LENGTH];
	CLAN_DATA *pClan;

	fclose( fpReserve );
	sprintf( clan_file_name, "%s", NEW_CLAN_FILE );

 if ( ( fp = fopen( clan_file_name, "w" ) ) == NULL )
		{
	bug( "Save clans: fopen", 0 );
	perror( "failed open of clans.txt in save_clans" );
		}
		else
		{
			for (pClan = first_clan; pClan != NULL; pClan = pClan->next )
			{
				fwrite_clan(  pClan, fp );
			}
			fprintf( fp, "#END\n\n" );
			 
			fflush( fp );
			fclose( fp ); 
		}
	fpReserve = fopen( NULL_FILE, "r" );
	return;

}

void save_hobj( )
{

	FILE * fp;
	char corpse_file_name[MAX_STRING_LENGTH];
	OBJ_DATA * obj;

	fclose( fpReserve );
	sprintf( corpse_file_name, "%s", HOBJ_FILE );

 if ( ( fp = fopen( corpse_file_name, "w" ) ) == NULL )
		{
	bug( "Save Corpses: fopen", 0 );
	perror( "failed open of corpse_file in save_corpses" );
		}
		else
		{
			for (obj = first_obj; obj != NULL; obj = obj->next )
			{
			if( IS_SET(obj->extra_flags,ITEM_HIDDEN) )
				fwrite_hobj( obj, fp, 0 );
			}
			fprintf( fp, "#END\n\n" );
			 
			fflush( fp );
			fclose( fp ); 
		}
	fpReserve = fopen( NULL_FILE, "r" );
	return;

}

void save_marks( )
{

	FILE * fp;
	char mark_file_name[MAX_STRING_LENGTH];
	MARK_LIST_MEMBER	*mark_list;



	fclose( fpReserve );
	sprintf( mark_file_name, "%s", MARKS_FILE );

 if ( ( fp = fopen( mark_file_name, "w" ) ) == NULL )
 {
	 bug( "Save Mark list: fopen", 0 );
	 perror( "failed open of roommarks.lst in save_marks" );
 }
 else
 {
	 for ( mark_list = first_mark_list; mark_list != NULL; mark_list = mark_list->next )
	 {
		 fprintf( fp, "#MARK~\n" );
		 fprintf( fp, "%d\n", mark_list->mark->room_vnum );
		 fprintf( fp, "%s~\n", mark_list->mark->message );
		 fprintf( fp, "%s~\n", mark_list->mark->author );
		 fprintf( fp, "%d\n", mark_list->mark->duration );
		 fprintf( fp, "%d\n", mark_list->mark->type );
	 }
	 fprintf( fp, "#END~\n\n" );
 }

			 
 fflush( fp );
 fclose( fp ); 
		
 
 fpReserve = fopen( NULL_FILE, "r" );
 return;

}

void save_bans( )
{

	FILE * fp;
	char ban_file_name[MAX_STRING_LENGTH];
	BAN_DATA	*pban;



	fclose( fpReserve );
	sprintf( ban_file_name, "%s", BANS_FILE );

 if ( ( fp = fopen( ban_file_name, "w" ) ) == NULL )
 {
	 bug( "Save ban list: fopen", 0 );
	 perror( "failed open of bans.lst in save_ban" );
 }
 else
 {
	 for ( pban = first_ban; pban != NULL; pban = pban->next )
	 {
		 fprintf( fp, "#BAN~\n" );
		 fprintf( fp, "%d\n", ( pban->newbie ? 1 : 0 ) );
		 fprintf( fp, "%s~\n", pban->name );
		 fprintf( fp, "%s~\n", pban->banned_by );
	 }
	 fprintf( fp, "#END~\n\n" );
 }

			 
 fflush( fp );
 fclose( fp ); 
		
 
 fpReserve = fopen( NULL_FILE, "r" );
 return;

}
