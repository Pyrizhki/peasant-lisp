//for debuging
#include <iostream>

#include "lexer.h"

/*******************/
/* Lexer Functions */
/*******************/

void Lexer::eat_white_space() {
  while (pos < source.size() &&
	 (source[pos] == ' ' || source[pos] == '\n' || source[pos] == '\t')) {
    if (source[pos] == '\n') {
      line++;
      character = 1;
    } else {
      character++;
    }
    pos++;    
  }
}

Token Lexer::next_token() {

  if (current_token+1 < tokens.size()) {
    current_token++;
    return tokens[current_token];
  }
  
  current_token++;
  eat_white_space();
  Token t;
  if (pos >= source.size()) {
    t = Token{TOKEN_END_OF_FILE, line, character, line, character};
    tokens.push_back(t);
    return t;
  }
  
  char c = source[pos];

  if (c == ';') {
    current_token--;
    while (pos < source.size() && source[pos] != '\n') {
      ++pos;
    }
    return next_token();
    
  } else if (c == '"') {
    ++pos;
    ++character;
    t = read_string();
    
  } else if (c == '(') {
    pos++;
    character++;
    t = Token{TOKEN_L_PAREN, line, character-1, line, character-1};
    
  } else if (c == ')') {
    pos++;
    character++;
    t = Token{TOKEN_R_PAREN, line, character-1, line, character-1};

  } else if (c == '\'') {    
    pos++;
    character++;
    t = Token{TOKEN_QUOTE, line, character-1, line, character-1};

  } else if (c == '`') {    
    pos++;
    character++;
    t = Token{TOKEN_BACKTICK, line, character-1, line, character-1};
    
  } else if (c == ',') {    
    pos++;
    character++;
    if (pos < source.size() && source[pos] == '@') {
      pos++;
      t = Token{TOKEN_COMMA_AT, line, character-1, line, character-1};
    } else {
      t = Token{TOKEN_COMMA, line, character-1, line, character-1};
    }    
    
  } else if (isdigit(c)) {
    t = read_number();

  } else {

    t = read_identifier();
    // fprintf(stderr, "failed to read token at %s:%d:%d\n", filename, line, character);
    // exit(1);
  }
  
  tokens.push_back(t);

  return t;
}

Token Lexer::peek_next_token() {
  if (current_token+1 < tokens.size()) {
    return tokens[current_token+1];
  }
  next_token();
  return tokens[current_token--];
}

Token Lexer::read_number() {
  Token t;
  t.type = TOKEN_INTEGER;
  t.start_line = line;
  t.start_char = character;

  char c = source[pos];
  while (isdigit(c) || c == '.') {
    t.name.push_back(c);
    if (c == '.') t.type = TOKEN_FLOAT;
    pos++;
    character++;
    c = source[pos];
  }

  t.stop_line = line;
  t.stop_char = character;
  return t;
}

Token Lexer::read_identifier() {
  Token t;
  t.name.resize(0);  
  t.type = TOKEN_IDENTIFIER;
  t.start_line = line;
  t.start_char = character;
  
  int source_size = source.size();
  char c = source[pos];
  int count = 0;
  while (c != ' ' && c != '\n' && c != '\t' && c != '(' && c != ')') {
    t.name.push_back(c);
    ++count;
    pos++;
    character++;
    if (pos < source_size) {
      c = source[pos];
    } else {
      break;
    }
  }

  t.stop_line = line;
  t.stop_char = character;
  return t;
}

Token Lexer::read_string() {
  Token t;
  t.type = TOKEN_STRING;
  t.start_line = line;
  t.start_char = character;

  char c = source[pos];
  
  while (c != '"') {
    t.name.push_back(c);
    if (c == '\n') {
      ++line;
      character = 1;
    } else {
      ++character;
    }
    pos++;
    c = source[pos];
  }
  if (c == '"') {
    ++character;
    ++pos;
  }

  t.stop_line = line;
  t.stop_char = character;
  return t;
}

void Lexer::feed(std::string input) {
  pos = 0;
  line = 1;
  character = 1;
  source = input;
}

/*******************/
/* Token Functions */
/*******************/

void Token::print_position() {
  fprintf(stderr, "%d:%d", start_line, start_char);
}

void Token::print_token() {
  std::cout << "Token: \"" << name << "\" of type ";
  print_token_type(type);
}

std::string get_whole_file(const char *filename) {
  std::FILE *fp = std::fopen(filename, "rb");
  if (fp) {
    std::string contents;
    std::fseek(fp, 0, SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&contents[0], 1, contents.size(), fp);
    std::fclose(fp);
    return(contents);    
  }
  fprintf(stderr, "Faile to open file %s", filename);
  throw(errno);
}

void print_token_type(Token_Type t) {
  switch (t) {
  case TOKEN_L_PAREN:
    printf("TOKEN_L_PAREN");
    break;
  case TOKEN_R_PAREN:
    printf("TOKEN_R_PAREN");
    break;
  case TOKEN_IDENTIFIER:
    printf("TOKEN_IDENTIFIER");
    break;
  case TOKEN_INTEGER:
    printf("TOKEN_INTEGER");
    break;
  case TOKEN_FLOAT:
    printf("TOKEN_FLOAT");
    break;
  case TOKEN_END_OF_FILE:
    printf("TOKEN_END_OF_FILE");
    break;
  }
}
