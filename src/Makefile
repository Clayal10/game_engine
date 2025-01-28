OBJS = main.o stb_image.o helpers.o tiny_obj_loader.o baseclass.o old_objects.o geometric_objects.o hud.o
CXXFLAGS = -g -Wall 
LDFLAGS = -lGL -lglfw -lGLEW -pthread -g 

# For FreeBSD (doesn't seem to hurt anything on Linux)
CXXFLAGS += -I /usr/local/include
LDFLAGS += -L /usr/local/lib

all: ${OBJS}
	${CXX} ${OBJS}  ${LDFLAGS} -o play

clean:
	rm -f play ${OBJS}

test:	all
	./play

${OBJS} : game.h
main.o : projectiles.h activation_area.h turret.h tile_floor.h old_objects.h moving_platform.h geometric_objects.h hud.h bot.h pathway.h
