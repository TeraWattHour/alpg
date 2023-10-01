all: install

install:
	mkdir build; \
	cd build; \
	cmake ..; \
	sudo make install;

dev: install
	cd ~postgres; \
    sudo -u postgres psql -c "DROP EXTENSION IF EXISTS alpg; CREATE EXTENSION alpg;"
