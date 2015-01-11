
/* This file contains all the functions to write out the map matrix.
 * Maps are now logical, and can't overlap. Each room has it's xyz
 * coordinates. Room 3001 is ground zero (x0, y0, z0).
 * When a door is named, ie, 'enter tree', the room it leads to starts
 * a new gz for that area, since you're actually 'in' something. This will
 * allow flexability when creating areas.
 *
 */


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

bool conn_checkz(ROOM_INDEX_DATA *rooms)
{
    /* Checks if a room is connected to another room that has already been checked.
     */
    int num;
    ROOM_INDEX_DATA *next;

    for( num = 0; num < 6; num++)
    {
        if (rooms->exit[num] == NULL)
            continue;
        next = rooms->exit[num]->to_room;
        if (next->checked == TRUE)
            return TRUE;
    }
    return FALSE;
}
bool has_exits(ROOM_INDEX_DATA *room)
{
    /* Checks if a room has an exit
     */
    int num;

    for (num = 0; num < 6; num++)
        if (room->exit[num] != NULL)
            return TRUE;
        return FALSE;
}

void give_me_co(ROOM_INDEX_DATA *rooms, ROOM_INDEX_DATA *next, int num)
{
    extern unsigned long int top_pco;
    COOR_DATA * this_coor;
    rooms->xco = 0;
    rooms->yco = 0;
    rooms->zco = 0;
    rooms->pco = 0;
    GET_FREE(this_coor, coor_free);
    /* When you 'enter' a door by typing 'enter <keyword>', it starts a new gz at 0 0 0 0
     */

    if (rooms->exit[num]->is_named == TRUE && rooms->exit[num]->seperate_plane == TRUE)
    {
        rooms->pco = (++top_pco);
        rooms->checked = TRUE;
        return;
    }

    switch ( num )
    {
    case 0 :
        rooms->yco = (next->yco - 1);
        rooms->xco = next->xco;
        rooms->zco = next->zco;
        rooms->pco = next->pco;
        this_coor->xco = rooms->xco;
        this_coor->yco = rooms->yco;
        this_coor->zco = rooms->zco;
        this_coor->pco = rooms->pco;
        rooms->checked = TRUE;
        break;
    case 1 :
        rooms->xco = (next->xco - 1);
        rooms->yco = next->yco;
        rooms->zco = next->zco;
        rooms->pco = next->pco;
        this_coor->xco = rooms->xco;
        this_coor->yco = rooms->yco;
        this_coor->zco = rooms->zco;
        this_coor->pco = rooms->pco;
        rooms->checked = TRUE;
        break;
    case 2 :
        rooms->xco = next->xco;
        rooms->yco = (next->yco + 1);
        rooms->zco = next->zco;
        rooms->pco = next->pco;
        this_coor->xco = rooms->xco;
        this_coor->yco = rooms->yco;
        this_coor->zco = rooms->zco;
        this_coor->pco = rooms->pco;
        rooms->checked = TRUE;
        break;
    case 3 :
        rooms->xco = (next->xco + 1);
        rooms->yco = next->yco;
        rooms->zco = next->zco;
        rooms->pco = next->pco;
        this_coor->xco = rooms->xco;
        this_coor->yco = rooms->yco;
        this_coor->zco = rooms->zco;
        this_coor->pco = rooms->pco;
        rooms->checked = TRUE;
        break;
    case 4 :
        rooms->zco = (next->zco - 1);
        rooms->yco = next->yco;
        rooms->zco = next->zco;
        rooms->pco = next->pco;
        this_coor->xco = rooms->xco;
        this_coor->yco = rooms->yco;
        this_coor->zco = rooms->zco;
        this_coor->pco = rooms->pco;
        rooms->checked = TRUE;
        break;
    case 5 :
        rooms->zco = (next->zco + 1);
        rooms->xco = next->xco;
        rooms->yco = next->yco;
        rooms->pco = next->pco;
        this_coor->xco = rooms->xco;
        this_coor->yco = rooms->yco;
        this_coor->zco = rooms->zco;
        this_coor->pco = rooms->pco;
        rooms->checked = TRUE;
        break;
    }
    rooms->is_linked_main = TRUE;
    LINK(this_coor, first_coor, last_coor, next, prev);
    area_modified(rooms->area);
    return;
}

bool is_touching_center(ROOM_INDEX_DATA *rooms)
{
    ROOM_INDEX_DATA *center;
    int dir;
    int flags = (HUNT_OPENDOOR|HUNT_PICKDOOR|HUNT_UNLOCKDOOR|HUNT_WORLD);

    if ((center = get_room_index(ROOM_VNUM_CENTEROW) ) == NULL )
    {
        log_f("BUGBUGBUG!, Center of the world isn't there! Check config.h!");
        return FALSE;
    }

    if (rooms == center)
        return TRUE;
     if( ( dir = h_find_dir(rooms, center, flags)) < 0 )
         return FALSE;
     return TRUE;
}

void recheck_count(ROOM_INDEX_DATA *center)
{
    ROOM_INDEX_DATA *rooms;
    ROOM_INDEX_DATA *next;
    int count = 0;
    int num;
    int curvnum;
    int min;
    int max;
    int total;
    min = center->area->min_vnum;
    max = center->area->max_vnum;
    total = max - min + 1;
        for (curvnum = min; curvnum <= max; curvnum++ )
    {
        if ( ( rooms = get_room_index(curvnum) ) == NULL
            && curvnum == max)
        {
            ++count;
            if (count < total)
            {
            recheck_count(center);
            return;
            }
        }

        if ( ( rooms = get_room_index(curvnum) ) == NULL )
        {
            ++count;
            continue;
        }
        if (rooms->is_linked_main == FALSE)
            rooms->checked = TRUE;

        if (rooms->checked == TRUE && rooms->vnum == max)
        {
            ++count;
            rooms->checked = TRUE;
            if (count < total)
            {
            recheck_count(center);
            return;
            }
        } 
        if (rooms->checked == TRUE )
        {
            ++count;
            rooms->checked = TRUE;
            continue;
        }
          for (num = 0; num < 6; num++)
          {
              if (rooms->exit[num] == NULL)
                  continue;
              next = get_room_index(rooms->exit[num]->to_room->vnum);

                if (next->checked == TRUE && rooms->checked == FALSE)
                {
                    give_me_co(rooms, next, num);
                    ++count;
                }

          }
          if (count < total && curvnum == max)
          {
              recheck_count(center);
              return;
          }

    }
    return;
}

void mapout_all()
{
    /* Main bugger. Gets the center of the world, defined in
     * config.h as ROOM_VNUM_CENTEROW, and starts mapping out the world from there.
     * Only called at boot or area to area linking. If a room overlaps on link, all
     * the affected rooms are logged, and then unlinked.
     */
    ROOM_INDEX_DATA *center;
    ROOM_INDEX_DATA *rooms;
    ROOM_INDEX_DATA *next;
    COOR_DATA *this_coor;
    COOR_DATA *next_coor;
    int num;
    int count = 0;
    int curvnum;
    int min;
    int max;
    int total;
    if ((center = get_room_index(ROOM_VNUM_CENTEROW) ) == NULL )
    {
        log_f("BUGBUGBUG!, Center of the world isn't there! Check config.h!");
        return;
    }
    /* Initialize center as GZ */
    center->xco = 0;
    center->yco = 0;
    center->zco = 0;
    center->pco = 0;
    center->checked = TRUE;
    center->is_linked_main = TRUE;
    center->area->linked_main = TRUE;
    min = center->area->min_vnum;
    max = center->area->max_vnum;
    total = max - min + 1;
    for (this_coor = first_coor; this_coor != NULL; this_coor = next_coor)
    {
        next_coor = this_coor->next;
        if (this_coor != NULL)
        {
          UNLINK(this_coor, first_coor, last_coor, next, prev);
          PUT_FREE(this_coor, coor_free);
        }
    }

    for (curvnum = min; curvnum < max; curvnum++ )
    {
       if ( ( rooms = get_room_index(curvnum) ) == NULL )
       continue;
      rooms->xco = 0;
      rooms->yco = 0;
      rooms->zco = 0;
      if (rooms != center)
      {
      rooms->checked = FALSE;
      if (!is_touching_center(rooms))
      rooms->is_linked_main = FALSE;
      else
      rooms->is_linked_main = TRUE;
      }
    }

    for (curvnum = min; curvnum <= max; curvnum++ )
    {
        if ( ( rooms = get_room_index(curvnum) ) == NULL
            && curvnum == max)
        {
            ++count;
            if (count < total)
            {
            recheck_count(center);
            return;
            }
        }

        if ( ( rooms = get_room_index(curvnum) ) == NULL)
        {
            count++;
            continue;
        }
        
        if (rooms->is_linked_main == FALSE)
            rooms->checked = TRUE;

        if (rooms->checked == TRUE && curvnum == max)
        {
            ++count;
            if (count < total)
            {
            recheck_count(center);
            return;
            }
        }

        if (rooms->checked == TRUE)
        {
            count++;
            continue;
        }
          for (num = 0; num < 6; num++)
          {
              if (rooms->exit[num] == NULL)
                  continue;
              next = get_room_index(rooms->exit[num]->to_room->vnum);

                if (next->checked == TRUE && rooms->checked == FALSE)
                {
                    give_me_co(rooms, next, num);
                    count++;
                }

                area_modified(next->area);
          }
          if (count < total && curvnum == max)
          {
              recheck_count(center);
              return;
          }
    }
    return;
}
