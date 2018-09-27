# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CC = g++
FLEX = flex
BISON = bison
CFLAGS = -std=c99

# 编译目标：src目录下的所有.cpp文件
CFILES = $(shell find ./ -name "*.cpp")
OBJS = $(CFILES:.cpp=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO),$(OBJS))
	$(CC) -o parser $(filter-out $(LFO),$(OBJS) -ll -ly

syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v $(YFILE)

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: clean scanner run debug
scanner:
	flex lexical.l
	g++ main.c lex.yy.cc -lfl -o scanner

run:
	./parser ../Test/test1.cmm
	./parser ../Test/test2.cmm
	./parser ../Test/test3.cmm
	./parser ../Test/test4.cmm
	./parser ../Test/test5.cmm
	./parser ../Test/test6.cmm
	./parser ../Test/test7.cmm
	./parser ../Test/test8.cmm
	./parser ../Test/test9.cmm
	./parser ../Test/test13.cmm


debug:
	flex lexical.l
	bison -d -v -t syntax.y
	g++ syntax.tab.c ast.c main.c -ly -o parser -D YY_DEBUG

fake:
	flex -o lex.yy.c lexical.l
	bison -o syntax.tab.c -d -v syntax.y
	g++ -c syntax.tab.c -o syntax.tab.o
	g++ -c -o main.o main.cpp
	g++ -c -o ast.o ast.cpp
	g++ -o parser main.o ast.o syntax.tab.o -ll -ly

clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
