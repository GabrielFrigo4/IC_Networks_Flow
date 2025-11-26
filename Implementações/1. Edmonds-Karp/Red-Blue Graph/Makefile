CC := g++
EXT := cpp
TARGET := main
INPUT := "./input.txt"

UNIX_FLAGS := -Wl,-z,stack-size=268435456 -std=c++23 -O2
UNIX_EXE := "./$(TARGET)"
UNIX_CLIP := wl-copy

WIN_FLAGS := -Wl,--stack=268435456 -std=c++23 -O2
WIN_EXE := "./$(TARGET).exe"
WIN_CLIP := win32yank -i

.PHONY: unix
unix:
	g++ $(UNIX_FLAGS) $(TARGET).$(EXT) -o $(TARGET)
	cat $(INPUT) | $(UNIX_EXE)

.PHONY: urun
urun:
	cat $(INPUT) | $(UNIX_EXE)

.PHONY: urm
urm:
	rm $(UNIX_EXE)

.PHONY: uclip
uclip:
	cat $(TARGET).$(EXT) | $(UNIX_CLIP)

.PHONY: win
win:
	$(CC) $(WIN_FLAGS) $(TARGET).$(EXT) -o $(TARGET)
	cat $(INPUT) | $(WIN_EXE)

.PHONY: wrun
wrun:
	cat $(INPUT) | $(WIN_EXE)

.PHONY: wrm
wrm:
	rm $(WIN_EXE)

.PHONY: wclip
wclip:
	cat $(TARGET).$(EXT) | $(WIN_CLIP)