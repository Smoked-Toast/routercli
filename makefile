
TARGET   = routercli
CXXFLAGS = -g -Wall -Werror -pedantic-errors -fmessage-length=0
OBJPATH = bin
COBJ = $(OBJPATH)/Controller.o $(OBJPATH)/ExecBuilder.o $(OBJPATH)/Lock.o $(OBJPATH)/Network.o $(OBJPATH)/Parser.o

all: init Controller ExecBuilder Lock Network Parser
	g++ $(CXXFLAGS) src/$(TARGET).cpp -o $(TARGET) $(COBJ)

Controller:
	g++ $(CXXFLAGS) src/Controller.cpp -c -o $(OBJPATH)/Controller.o

ExecBuilder:
	g++ $(CXXFLAGS) src/ExecBuilder.cpp -c -o $(OBJPATH)/ExecBuilder.o

Lock:
	g++ $(CXXFLAGS) src/Lock.cpp -c -o $(OBJPATH)/Lock.o

Network:
	g++ $(CXXFLAGS) src/Network.cpp -c -o $(OBJPATH)/Network.o

Parser:
	g++ $(CXXFLAGS) src/Parser.cpp -c -o $(OBJPATH)/Parser.o

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
