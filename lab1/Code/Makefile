# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CXX = g++
FLEX = flex
BISON = bison
CPPFLAGS = -w -std=c++11

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
	$(CXX)  $(CPPFLAGS)  -o parser $(YFO) $(filter-out $(LFO),$(OBJS)) -ll -ly

syntax: lexical syntax-c
	$(CXX)  $(CPPFLAGS)  -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v -t $(YFILE)

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

clean:
	rm -f parser lex.yy.c $(filter-out $(YFILE), $(shell find ./ -name "syntax.*"))
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
