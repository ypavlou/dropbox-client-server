#
# In order to execute this "Makefile" just type "make"
#

OBJS    = dropbox_server.o clients_list.o select_functions.o
SOURCE  = dropbox_server.c clients_list.c select_functions.c
HEADER  = dropbox_server.h clients_list.h select_functions.h
OUT     = dropbox

CC      = gcc
FLAGS   = -g -c 
# -g option enables debugging mode
# -c flag generates object code for separate files

all: 	$(OBJS)
		$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<<
dropbox_server..o: dropbox_server.c
	$(CC) $(FLAGS) dropbox_server.c
clients_list..o: clients_list.c
	$(CC) $(FLAGS) clients_list.c
select_functions..o: select_functions.c
	$(CC) $(FLAGS) select_functions.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
