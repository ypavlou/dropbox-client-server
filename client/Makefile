#
# In order to execute this "Makefile" just type "make"
#

OBJS    = dropbox_client.o server_connect.o client_list.o client_server.o files_list.o pool.o worker_threads.o
SOURCE  = dropbox_client.c server_connect.c client_list.c client_server.c files_list.c pool.c worker_threads.c
HEADER  =server_connect.h client_list.h client_server.h files_list.h pool.h worker_threads.h
OUT     = dropbox_client

CC      = gcc
FLAGS   = -g -c 
# -g option enables debugging mode
# -c flag generates object code for separate files

all: 	$(OBJS)
		$(CC) -g $(OBJS) -pthread -o $(OUT)

# create/compile the individual files >>separately<<
dropbox_client..o: dropbox_client.c
	$(CC) $(FLAGS) dropbox_client.c
server_connect..o: server_connect.c
	$(CC) $(FLAGS) server_connect.c
client_list..o: client_list.c
	$(CC) $(FLAGS) client_list.c
client_server..o: client_server.c
	$(CC) $(FLAGS) client_server.c
files_list..o: files_list.c
	$(CC) $(FLAGS) files_list.c
pool..o: pool.c
	$(CC) $(FLAGS) pool.c
worker_threads..o: worker_threads.c
	$(CC) $(FLAGS) worker_threads.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
