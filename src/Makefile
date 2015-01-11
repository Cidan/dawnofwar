CC      = clang
PROF    = 
NOCRYPT =
XML_C=-I/usr/include/libxml2
XML_L=-L/usr/lib -lxml2 -lz -lpthread -lm
C_FLAGS = -O -g3 -Wall -DACK_43 $(XML_C) $(PROF) $(NOCRYPT) 
L_FLAGS = -O -g -levent $(XML_L) $(PROF)

# Debugging flags possible:  DEBUG_MEM DEBUG_MEM_CORRUPT DEBUG_MEM_DUP_FREE
# DEBUG_MEM is most basic, just checks magic numbers when freeing
# DEBUG_MEM_CORRUPT checks every free block to see if magic numbers ok, every
#                   call to alloc_mem
#		    Also makes calls to void check_free_mem(void) check for
#		    corrupted free memory blocks.
# DEBUG_MEM_DUP_FREE checks to see if freed blocks are overlapping every call
# 		     to mem_alloc.
#


O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o fight.o handler.o interp.o magic.o magic2.o magic3.o magic4.o spell_dam.o \
          mob_commands.o mob_prog.o save.o special.o update.o board.o areasave.o \
          buildtab.o build.o write.o act_clan.o buildare.o hunt.o hash.o areachk.o clutch.o \
          obj_fun.o act_mob.o macros.o trigger.o quest.o lists.o social-edit.o \
          vampyre.o werewolf.o mount.o pdelete.o wizutil.o money.o \
          ssm.o scheck.o rulers.o spendqp.o enchant.o sysdata.o strfuns.o mapper.o email.o \
          apt.o xmlparse.o

dow: $(O_FILES)
	rm -f ../bin/dow
	$(CC) $(L_FLAGS) -o ../bin/dow $(O_FILES) -lcrypt

.c.o: ack.h
	$(CC) -c $(C_FLAGS) $<

scan: scan.o
	rm -f scan
	$(CC) $(L_FLAGS) -o scan scan.o

clean:
	rm -f $(O_FILES) ../bin/dow ../area/core
