TARGET=loginserver
OBJ=AvailableWorldsHandler.o ChatParticipantServant.o ClientSessionManagerServant.o main.o SessionServant.o ../../shared/SynchronizedTimeHandler.o WorldRegistrationServant.o

CC=g++
INCDIR=-I. -I../../infrastructure/include -I/usr/include -I/usr/include/mysql++ -I/usr/include/mysql -I../../shared
LIBDIR=-L/usr/lib -L../../infrastructure/lib
LIBS=-lIce -lIceStorm -lIceUtil -lGlacier2 -lIcePatch2 -lgenerated -lmysqlpp -lmysqlclient


all: clean depend $(TARGET) install


$(TARGET): $(OBJ)
	@echo "\n         **********creation de l'exe************\n"
	$(CC) $(INCDIR) *.o -o $(TARGET) $(LIBDIR) $(LIBS) 
	@echo "\n"		

.cpp.o:
	@echo "\n         **********creation des .o************\n"
	$(CC) -O2 -D_LBANET_SERVER_SIDE_ -I. $(INCDIR) -c $<
	@echo "\n"

depend: 
	@echo "\n         **********creation des dependances************\n"	
	@$(CC) $(INCDIR) -MM *.cpp  > dep



install:
	@echo "\n         ********** copie des fichiers dans include & bib ************\n"
	cp $(TARGET) ../bin
	@echo "\n"
clean:
	@echo "\n        ********** nettoyage ************\n"
	rm -f $(OBJ) $(TARGET) dep
	@touch dep
	@echo "\n"
		
include dep
