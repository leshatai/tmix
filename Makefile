CXX=c++
RM=rm -f
CPPFLAGS=-std=c++14 -Wall -g
LDLIBS=-L/usr/local/lib -lncurses
PRG_NAME=tmix

tmix: mixer_device.o mixer_manager.o mixer_window.o mixer_panel.o main.o
	$(CXX) -o $(PRG_NAME) mixer_device.o mixer_manager.o mixer_window.o mixer_panel.o main.o $(LDLIBS)

mixer_panel.o:
	$(CXX) $(CPPFLAGS) -c mixer_panel.cxx

mixer_window.o:
	$(CXX) $(CPPFLAGS) -c mixer_window.cxx

mixer_device.o:
	$(CXX) $(CPPFLAGS) -c mixer_device.cxx

mixer_manager.o:
	$(CXX) $(CPPFLAGS) -c mixer_manager.cxx

main.o:
	$(CXX) $(CPPFLAGS) -c main.cxx

clean:
	$(RM) *.o

dist-clean:
	$(RM) *.o tmix
