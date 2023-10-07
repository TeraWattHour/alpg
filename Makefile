all: install

install:
	rm -rf build; \
	mkdir build; \
	cd build; \
  	cp ../src/alpg.control ./; \
	make -f ../src/Makefile install

create: install
	cd ~postgres; \
    sudo -u postgres psql -c "DROP EXTENSION IF EXISTS alpg; CREATE EXTENSION alpg;"
