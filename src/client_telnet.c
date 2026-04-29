#include "client_telnet.h"
#include "game.h"
#include "key.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>


void client_telnet_render(Client *client) {

}


typedef struct {
	Client *client;
	size_t head;
	size_t handled_size;
} TelnetLexer;


typedef enum {
	RESULT_OK,
	RESULT_ERR,
	RESULT_UNFINISHED,
} LexerResult;


static inline uint8_t curr(TelnetLexer *lexer) {
	if (lexer->head >= lexer->client->in_len)
		return '\0';
	return lexer->client->in_buffer[lexer->head];
}

static inline uint8_t next(TelnetLexer *lexer) {
	return lexer->client->in_buffer[lexer->head++];
}

static inline bool lexer_has_next(TelnetLexer *lexer) {
	return lexer->head < lexer->client->in_len;
}

#define TELNET_IAC	255
#define TELNET_SB	250
#define TELNET_NAWS	31
#define TELNET_SE	240

#define expect(lexer, expected_char)		\
	if (!lexer_has_next(lexer))		\
		return RESULT_UNFINISHED;	\
	if (next(lexer) != expected_char)	\
		return RESULT_ERR

#define try_next(lexer, variable)		\
	if (!lexer_has_next(lexer))		\
		return RESULT_UNFINISHED;	\
	variable = next(lexer)


static inline LexerResult parse_telnet_command(TelnetLexer *lexer) {
	expect(lexer, TELNET_IAC);
	expect(lexer, TELNET_SB);
	expect(lexer, TELNET_NAWS);

	try_next(lexer, uint16_t width_high);
	try_next(lexer, uint16_t width_low);
	try_next(lexer, uint16_t height_high);
	try_next(lexer, uint16_t height_low);

	expect(lexer, TELNET_IAC);
	expect(lexer, TELNET_SE);

	lexer->client->terminal.width = (width_high << 8) | width_low;
	lexer->client->terminal.height = (height_high << 8) | height_low;
	lexer->client->terminal.resized = true;

	printf(
		"Terminal dim: %d;%d\n",
		lexer->client->terminal.width,
		lexer->client->terminal.height
	);
	return RESULT_OK;
}

#define ESCAPE_CHAR 27
#define ESCAPE_CHAR_CSI '['
#define ESCAPE_CHAR_SS3 'O'

static inline void press_key(TelnetLexer *lexer, KeyStroke key) {
	game_input(lexer->client->player_index, key);
}

static inline LexerResult parse_number(TelnetLexer *lexer, int *number) {
	int n = 0;
	while (1) {
		if (!lexer_has_next(lexer))
			return RESULT_UNFINISHED;
		uint8_t c = curr(lexer);
		if (c >= '0' && c <= '9') {
			n *= 10;
			n += c - '0';
			next(lexer);
			continue;
		}
		break;
	}
	*number = n;
	return RESULT_OK;
}
#define try_parse_number(lexer, num) \
	do {							\
		LexerResult _result = parse_number(lexer, num);	\
		if (_result != RESULT_OK)			\
			return _result;				\
	} while (0);


static inline LexerResult parse_modifier(TelnetLexer *lexer, KeyStroke *key) {
	int m;
	try_parse_number(lexer, &m);
	m -= 1;
	key->shift = m & 0b001;
	key->alt   = m & 0b010;
	key->ctrl  = m & 0b100;
	return RESULT_OK;
}

#define try_parse_modifier(lexer, key) \
	do {								\
		LexerResult _result = parse_modifier(lexer, key);	\
		if (_result != RESULT_OK)				\
			return _result;					\
	} while (0);
	


static inline LexerResult parse_tilde_terminated_char(TelnetLexer *lexer, uint8_t number) {
	KeyStroke key = {0};
	switch (number) {
		case 2: key.key = KEY_INSERT; break;
		case 3: key.key = KEY_DELETE; break;
		case 4: key.key = KEY_END; break;
		case 5: key.key = KEY_PAGE_UP; break;
		case 6: key.key = KEY_PAGE_DOWN; break;

		case 15: key.key = KEY_F5; break;
		case 17: key.key = KEY_F6; break;
		case 18: key.key = KEY_F7; break;
		case 19: key.key = KEY_F8; break;

		case 20: key.key = KEY_F9; break;
		case 21: key.key = KEY_F10; break;
		case 23: key.key = KEY_F11; break;
		case 24: key.key = KEY_F12; break;
	}

	try_next(lexer, uint8_t c);
	if (c == ';') {
		try_parse_modifier(lexer, &key);
		try_next(lexer, c);
	}
	if (c != '~')
		return RESULT_ERR;
	press_key(lexer, key);
	return RESULT_OK;
}

static inline LexerResult parse_csi_char(TelnetLexer *lexer) {
	expect(lexer, ESCAPE_CHAR_CSI);

	KeyStroke key = {0};

	int n = 0;
	try_parse_number(lexer, &n);
	switch (n) {
	case 1:
		try_next(lexer, uint8_t sep);
		// NOTE: `ESC[1~` is home
		if (sep == '~') {
			key.key = KEY_HOME;
			press_key(lexer, key);
			return RESULT_OK;
		}
		// NOTE: Now it must be followed by a separator
		if (sep != ';') return RESULT_ERR;
		try_parse_modifier(lexer, &key);
	break;
	// Not a number or modifier, must be letter
	case 0: break;
	// Any other number is a tilde_terminated one
	default: return parse_tilde_terminated_char(lexer, n);
	}
	
	try_next(lexer, uint8_t c);
	switch (c) {
	case 'A': key.key = KEY_UP; break;
	case 'B': key.key = KEY_DOWN; break;
	case 'C': key.key = KEY_RIGHT; break;
	case 'D': key.key = KEY_LEFT; break;
	case 'P': key.key = KEY_F1; break;
	case 'Q': key.key = KEY_F2; break;
	case 'R': key.key = KEY_F3; break;
	case 'S': key.key = KEY_F4; break;
	case 'H': key.key = KEY_HOME; break;
	case 'F': key.key = KEY_END; break;
	default: return RESULT_ERR;
	}
	press_key(lexer, key);
	return RESULT_OK;
}

static inline LexerResult parse_ss3_char(TelnetLexer *lexer) {
	expect(lexer, ESCAPE_CHAR_SS3);
	try_next(lexer, uint8_t c);
	KeyStroke key = {0};
	switch (c) {
	case 'P': key.key = KEY_F1; break;
	case 'Q': key.key = KEY_F2; break;
	case 'R': key.key = KEY_F3; break;
	case 'S': key.key = KEY_F4; break;
	default: return RESULT_ERR;
	}
	press_key(lexer, key);
	return RESULT_OK;
}


static inline LexerResult parse_escaped_character(TelnetLexer *lexer) {
	expect(lexer, ESCAPE_CHAR);

	switch (curr(lexer)) {
	case ESCAPE_CHAR_CSI: return parse_csi_char(lexer);
	case ESCAPE_CHAR_SS3: return parse_ss3_char(lexer);
	
	// NOTE: Just assuming that it was an escape if it's none of the above
	default:
		press_key(lexer, (KeyStroke){.key = KEY_ESCAPE}); 
		return RESULT_OK;
	}
}

static inline LexerResult parse_simple_character(TelnetLexer *lexer, uint8_t c) {
	KeyStroke key = {0};
	if (c == 9) {
		key.key = KEY_TAB;
	} else if (c >= 1 && c <= 26) {
		key.ctrl = true;
		key.key = 'a' + (c - 1);
	} else if (c >= 'A' && c <= 'Z') {
		key.shift = true;
		key.key = c - 'A' + 'a';
	} else {
		key.key = c;
	}
	press_key(lexer, key);
	next(lexer);
	return RESULT_OK;
}


static inline LexerResult parse_message(TelnetLexer *lexer) {
	uint8_t c = curr(lexer);
	switch (c) {
	case TELNET_IAC: return parse_telnet_command(lexer);
	case ESCAPE_CHAR: return parse_escaped_character(lexer);
	default: return parse_simple_character(lexer, c);
	}
}


void client_telnet_receive(Client *client) {
	TelnetLexer lexer = {0};
	lexer.client = client;

	bool should_continue = true;
	while (lexer_has_next(&lexer) && should_continue) {
		switch (parse_message(&lexer)) {
		case RESULT_OK: lexer.handled_size = lexer.head; break;
		case RESULT_ERR: lexer.handled_size = lexer.head; break;
		case RESULT_UNFINISHED: should_continue = false; break;
		};
	}

	if (lexer_has_next(&lexer)) {
		// NOTE: We partially read the buffer, shift it all to the left
		memmove(
			client->in_buffer,
			client->in_buffer + lexer.handled_size,
			client->in_len - lexer.handled_size
		);
		client->in_len -= lexer.handled_size;
	} else {
		// NOTE: We read the whole input buffer
		client->in_len = 0;
	}
}

