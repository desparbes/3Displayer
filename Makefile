all: debug

debug: 
	cd scripts; ./install.sh

clean:
	$(RM) -r build/ 3Displayer/
