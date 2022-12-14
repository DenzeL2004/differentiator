all: mkdirectory build

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

DIR_TREE = src/tree
DIR		 = differentiation

build:  obj/main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/stack.o 			\
		obj/differentiator.o obj/draw_tree.o obj/differentiator_tree.o 									\
		obj/reader.o obj/simplifier.o obj/name_table.o obj/latex_print.o

	g++ obj/main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/stack.o 			\
		obj/differentiator.o  obj/draw_tree.o obj/differentiator_tree.o obj/reader.o obj/simplifier.o 	\
		obj/name_table.o obj/latex_print.o -o differ


obj/main.o: main.cpp
	g++ main.cpp -c -o obj/main.o $(FLAGS)


obj/differentiator.o: $(DIR)/differentiator.cpp $(DIR)/differentiator.h 				\
												$(DIR)/operation_list.h
	g++ $(DIR)/differentiator.cpp -c -o obj/differentiator.o $(FLAGS)



obj/differentiator_tree.o: $(DIR)/differentiator.h  $(DIR)/differentiator_tree/differentiator_tree.cpp  	\
						   $(DIR)/operation_list.h  $(DIR)/differentiator_tree/differentiator_tree.h		
	g++ $(DIR)/differentiator_tree/differentiator_tree.cpp -c -o obj/differentiator_tree.o $(FLAGS)


obj/draw_tree.o:  $(DIR)/differentiator_tree/differentiator_tree.cpp $(DIR)/differentiator_tree/draw_tree.cpp 	\
				  $(DIR)/differentiator_tree/differentiator_tree.h   $(DIR)/differentiator_tree/draw_tree.h	  	
	g++ $(DIR)/differentiator_tree/draw_tree.cpp -c -o obj/draw_tree.o $(FLAGS)




obj/reader.o:  $(DIR)/differentiator_reader/reader.cpp $(DIR)/differentiator_reader/reader.h
	g++ $(DIR)/differentiator_reader/reader.cpp -c -o obj/reader.o $(FLAGS)


obj/simplifier.o:  $(DIR)/differentiator_simplifier/simplifier.cpp $(DIR)/differentiator_simplifier/simplifier.h
	g++ $(DIR)/differentiator_simplifier/simplifier.cpp -c -o obj/simplifier.o $(FLAGS)


obj/name_table.o:  $(DIR)/name_table/name_table.cpp $(DIR)/name_table/name_table.h
	g++ $(DIR)/name_table/name_table.cpp -c -o obj/name_table.o $(FLAGS)

obj/latex_print.o:  $(DIR)/latex_print/latex_print.cpp $(DIR)/latex_print/latex_print.h
	g++ $(DIR)/latex_print/latex_print.cpp -c -o obj/latex_print.o $(FLAGS)




obj/tree.o: $(DIR_TREE)/tree.cpp $(DIR_TREE)/tree.h 
	g++ $(DIR_TREE)/tree.cpp -c -o obj/tree.o $(FLAGS)


obj/log_errors.o: src/log_info/log_errors.h src/log_info/log_errors.cpp
	g++ src\log_info\log_errors.cpp -c -o obj/log_errors.o $(FLAGS)

obj/generals.o: src\Generals_func\generals.cpp
	g++ src\Generals_func\generals.cpp -c -o obj/generals.o $(FLAGS)

obj/process_text.o: src\process_text\process_text.cpp src\process_text\process_text.h
	g++ src\process_text\process_text.cpp -c -o obj/process_text.o $(FLAGS)

obj/stack.o: src\stack\stack.cpp src\stack\stack.h src\stack\config.h
	g++  src\stack\stack.cpp -c -o obj/stack.o $(FLAGS)

.PHONY: cleanup mkdirectory

mkdirectory:
	mkdir -p obj;

cleanup:
	rm obj/*.o