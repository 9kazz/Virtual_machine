TARGET = program

OBJECTS     = main.o utils.o assembler.o
SOURCE_DIR  = .\Source\\
HEADERS_DIR = .\Headers\

.PHONY: all clean


all: $(TARGET)
all: FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE


$(TARGET): $(OBJECTS)
	g++ $(FLAGS) $^ -o $@
	
%.o: $(SOURCE_DIR)%.cpp
	g++ $(FLAGS) -I$(HEADERS_DIR) -c $< -o $@


clean:
	del -f *.o $(TARGET)