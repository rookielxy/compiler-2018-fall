# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CXX = g++
FLEX = flex
BISON = bison
CPPFLAGS = -g -std=c++11 -Iinclude

# 编译目标：src目录下的所有.cpp文件
CFILES = $(shell find src -name "*.cpp")
OBJS = $(CFILES:.cpp=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO),$(OBJS))
	$(CXX)  $(CPPFLAGS)  -o parser $(YFO) $(filter-out $(LFO),$(OBJS)) -ll -ly
	cp parser ../parser

syntax: lexical syntax-c
	$(CXX)  $(CPPFLAGS)  -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v -t $(YFILE)

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: clean run
run:
	../parser ../Test/test1.cmm > ../Out/test1.ir
	../parser ../Test/test2.cmm > ../Out/test2.ir
	../parser ../Test/test3.cmm > ../Out/test3.ir
	../parser ../Test/test4.cmm > ../Out/test4.ir

clean:
	rm -f parser lex.yy.c $(filter-out $(YFILE), $(shell find ./ -name "syntax.*"))
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
