#
#by TigreFeng
#fwj@yealink.com
#
TARGET_OBJ=$(CC_BUILTIN)

#file need to be CC
SRC_FILES = $(wildcard *.c)
SRC_DIRS  = 

C_FILES		=$(SRC_FILES)
O_FILES		=$(addprefix $(CC_O_DIR)/, $(C_FILES:.c=.o))
D_FILES		=$(addprefix $(CC_D_DIR)/, $(C_FILES:.c=.d))

DIRS_BUILD  =$(SRC_DIRS:=_all)
DIRS_CLEAN  =$(SRC_DIRS:=_clean)
DIRS_BULITIN=$(SRC_DIRS:=/$(CC_BUILTIN))

all : $(DIRS_BUILD) $(TARGET_OBJ)

clean: $(DIRS_CLEAN) 
	@-rm -rf  *.o
	@-rm -rf  $(CC_O_DIR)
	@-rm -rf  $(TARGET_OBJ)
	
$(DIRS_BUILD) :
	@echo "BUILD        $(@:_all=) ..."
	@(cd  $(@:_all=)  ; $(MAKE) all)

$(DIRS_CLEAN) :
	@echo "CLEAN        $(@:_clean=) ..."
	@(cd $(@:_clean=) ; $(MAKE) clean)

#
# dep objs
#	
$(D_FILES) :$(CC_D_DIR)/%.d : %.c
	@mkdir -p $(dir $@)
	@echo -n $(patsubst $(CC_D_DIR)%,$(CC_O_DIR)%,$(dir $@)) > $@ 
	@$(PAGING_CC) -MM $(CC_FLAGS)   $(patsubst $(CC_D_DIR)/%,%,$(@:.d=.c)) | grep ^\s*# -v >> $@ 
	@echo "	@mkdir -p \$$(dir \$$@)" >> $@
	@echo "	@echo \"CC      $<\"" >> $@
	@echo "	$(CC_ECHO)\$$(PAGING_CC) -c \$$(CC_FLAGS) \$$(patsubst \$$(CC_O_DIR)/%,%,\$$(@:.o=.c)) -o \$$@" >> $@

#
# cc objs
#	
-include $(D_FILES)

#
# target
#	
$(TARGET_OBJ) : $(DIRS_BULITIN) $(O_FILES)
	@echo "LD      $(TARGET_OBJ)"
	$(CC_ECHO)$(PAGING_LD) -r -o $@ $(DIRS_BULITIN) $(O_FILES) 

