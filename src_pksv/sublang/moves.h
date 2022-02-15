#ifndef PKSV_LANG_MOVES_H
#define PKSV_LANG_MOVES_H 1

struct move_data {
	char const* name;
	int value;
};

const struct move_data *
lookup_gsc_move (register const char *str, register size_t len);
const char* lookup_gsc_move_reverse (int value);

#endif
