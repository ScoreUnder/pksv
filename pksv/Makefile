check:
	bunzip2 -fkq tests/fakerom.gba.bz2
	./pksv -r tests/test.pks tests/fakerom.gba
	./pksv tests/fakerom.gba 6B09F8

.PHONY: check
