
TARGET   = routercli
CXXFLAGS = -g -Wall -Werror -pedantic-errors -fmessage-length=0 -pthread
OBJPATH = bin
OBJPATHS = $(OBJPATH)/Controller.o $(OBJPATH)/ExecBuilder.o $(OBJPATH)/Lock.o $(OBJPATH)/Network.o $(OBJPATH)/Parser.o
COBJ = Controller.o ExecBuilder.o Lock.o Network.o Parser.o

all: $(COBJ)
	g++ $(CXXFLAGS) src/$(TARGET).cpp -o $(TARGET) $(OBJPATHS)

%.o: src/%.cpp
	g++ $(CXXFLAGS) -c -o $(OBJPATH)/$@ $<

clean:
	@rm -f $(TARGET) $(OBJPATH)/*.o

init:
	@mkdir -p $(OBJPATH)

install:
	./scripts/installPackages.sh
	@rm /usr/bin/$(TARGET) -f
	@make clean
	make
	@chmod 755 ./$(TARGET)
	cp ./$(TARGET) /usr/bin/

test:
	@bash ./tests/test_cli.sh
