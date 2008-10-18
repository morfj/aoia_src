#ifndef PARSER_H
#define PARSER_H


class Parser
{
public:
    Parser(char* memory, unsigned int size);
    ~Parser();

    // Number of bytes remaining to be parsed
    unsigned int remaining() const;

    // Moves the current position the number of bytes ahead
    void skip(unsigned int count);

    // Get the next byte
    unsigned char popChar() const;

    // Get the next 2 bytes 
    unsigned short popShort() const;

    // Get the next 4 bytes
    unsigned int popInteger() const;

    // Get the string at current position. <len><char[]><NULL>
    std::string popString() const;

    char* start() const { return m_start; }
    char* end() const { return m_end; }
    char* pos() const { return m_pos; }

private:
    char* m_start;
    char* m_end;
    mutable char* m_pos;
};


#endif // PARSER_H
