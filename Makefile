
JSHUND_NAME=jshund

PREFIX?=/usr/local
INSTALL_BIN= $(PREFIX)/bin
INSTALL=cp -pf
JSHUND_CXXFLAGS=-Wall -Wextra $(CXXFLAGS)

.PHONY: all

all: 
	mkdir -p bin || false
	cd src && $(CXX) $(JSHUND_CXXFLAGS) -o ../bin/$(JSHUND_NAME) $(JSHUND_NAME).cpp 


.PHONY: clean

clean:
	rm -rf bin/$(JSHUND_NAME) 


distclean: clean

.PHONY: install

install: all
	mkdir -p $(INSTALL_BIN)
	$(INSTALL) bin/$(JSHUND_NAME) $(INSTALL_BIN)
