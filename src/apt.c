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


/* Max attribute */
#define MAX_ATT 25
#define MAX_SKILL_LEVEL 50

int get_skill_apt(CHAR_DATA *ch, int sn)
{

	int prime = -1;
	int apt;
	float x;
	float y;
	float z;

	switch(skill_table[sn].prime_att)
	{
	case STAT_STR :
		prime = ch->pcdata->perm_str;
		break;
	case STAT_INT :
		prime = ch->pcdata->perm_int;
		break;
	case STAT_WIS :
		prime = ch->pcdata->perm_wis;
		break;
	case STAT_DEX :
		prime = ch->pcdata->perm_dex;
		break;
	case STAT_CON :
		prime = ch->pcdata->perm_con;
		break;
	case STAT_LCK :
		prime = ch->pcdata->perm_lck;
		break;
	default :
		prime = -1;
		break;
	}

	if (prime == -1)
		return -1;

	x = ((prime * prime) / MAX_ATT);
	y = (MAX_ATT * MAX_SKILL_LEVEL);
	z = 100 / y;
	apt = x * (ch->level * z);

	return apt;
}



void update_scale(CHAR_DATA *ch, int sn, bool down)
{

	switch(skill_table[sn].prime_att)
	{
	case STAT_STR :
		if (down)
		  ch->pcdata->learned_str -= 1;
		else
		  ch->pcdata->learned_str += 1;
		break;
	case STAT_INT :
		if (down)
		  ch->pcdata->learned_int -= 1;
		else
		  ch->pcdata->learned_int += 1;
		break;
	case STAT_WIS :
		if (down)
		  ch->pcdata->learned_wis -= 1;
		else
		  ch->pcdata->learned_wis += 1;
		break;
	case STAT_DEX :
		if (down)
		  ch->pcdata->learned_dex -= 1;
		else
		  ch->pcdata->learned_dex += 1;
		break;
	case STAT_CON :
		if (down)
		  ch->pcdata->learned_con -= 1;
		else
		  ch->pcdata->learned_con += 1;
		break;
	case STAT_LCK :
		if (down)
		  ch->pcdata->learned_lck -= 1;
		else
		  ch->pcdata->learned_lck += 1;
		break;
	}
	return;
}


bool can_gain_skill(CHAR_DATA *ch, int sn)
{
	int num = 0;

	if(ch->pcdata->learned[sn] > 0)
		return FALSE;

	if (skill_table[sn].apt_needed > get_skill_apt(ch, sn))
		return FALSE;

	switch(skill_table[sn].prime_att)
	{
	case STAT_STR :
		num = ch->pcdata->learned_str;
		break;
	case STAT_INT :
		num = ch->pcdata->learned_int;
		break;
	case STAT_WIS :
		num = ch->pcdata->learned_wis;
		break;
	case STAT_DEX :
		num = ch->pcdata->learned_dex;
		break;
	case STAT_CON :
		num = ch->pcdata->learned_con;
		break;
	case STAT_LCK :
		num = ch->pcdata->learned_lck;
		break;
	default :
		num = -1;
		break;
	}

	if (num < skill_table[sn].famil)
		return FALSE;
	return TRUE;
}
