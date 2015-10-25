all: debug

test:
	cd build/debug; make test

launch:
	cd 3Displayer; ./3Displayer

debug: 
	cd scripts; ./install.sh

clean:
	$(RM) -r build/ 3Displayer/
