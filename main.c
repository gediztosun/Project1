#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

const char *keywords[18] = {
        "break", "case", "char",
        "const", "continue", "do",
        "else", "enum", "float",
        "for", "goto", "if",
        "int", "long", "record",
        "return", "static", "while",
};

int main() {
    FILE *in, *out;
    in = fopen("code.psi", "r");
    out = fopen("code.lex", "w");

    int nextState = 0;
    int identifierLength = 0;
    int ch = fgetc(in);

    char identifier[20];

    int iLength = 0;

    while (1) {
        switch (nextState) {
            case 0:
                if (isalpha(ch)) {
                    // fprintf(out, "Identifier(");
                    // fputc(ch, out);
                    identifier[identifierLength] = (char)toupper(ch);
                    identifierLength++;
                    nextState = 1;
                    continue;
                } else if (isdigit(ch)) {

                    fprintf(out, "Integer Constant(");
                    fputc(ch, out);

                    iLength++;

                    nextState = 6;
                    continue;
                }
                switch (ch) {
                    case EOF:
                        goto end;
                    case '"':
                        fprintf(out, "String Constant(\"");
                        nextState = 2;
                        break;
                    case '(':
                        nextState = 3;
                        break;
                    case '+':
                        nextState = 7;
                        break;
                    case '-':
                        nextState = 8;
                        break;
                    case '*':
                        fprintf(out, "Operator(*)\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case '/':
                        fprintf(out, "Operator(/)\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case ':':
                        nextState = 9;
                        break;
                    case ')':
                        fprintf(out, "Right Parenthesis\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case '[':
                        fprintf(out, "Left Square Bracket\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case ']':
                        fprintf(out, "Right Square Bracket\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case '{':
                        fprintf(out, "Left Curly Brace\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case '}':
                        fprintf(out, "Right Curly Brace\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case ';':
                        fprintf(out, "End of line\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    default:
                        ch = fgetc(in);
                        break;
                }
                break;
            case 1:
                ch = fgetc(in);

                if (identifierLength > 20) {
                    freopen("code.lex", "w", out);

                    printf("Identifier names cannot be longer than 20 chars.\n");
                    exit(1);
                }

                if (isalnum(ch) || ch == '_') {
                    // fputc(ch, out);
                    identifier[identifierLength] = (char)toupper(ch);
                    identifierLength++;
                } else {
                    // fprintf(out, ")\n");
                    int isKeyword = 0;
                    int i;
                    for (i = 0; i < 18; i++) {
                        if (strcasecmp(keywords[i], identifier) == 0) {
                            isKeyword = 1;
                            break;
                        }
                    }

                    if (isKeyword) {
                        fprintf(out, "Keyword(%s)\n", keywords[i]);
                    } else {
                        fprintf(out, "Identifier(%s)\n", identifier);
                    }

                    for (i = 0; i < 20; i++) {
                        identifier[i] = '\0';
                    }

                    identifierLength = 0;
                    nextState = 0;
                }
                break;
            case 2:
                ch = fgetc(in);
                switch (ch) {
                    case '"':
                        fprintf(out, "\")\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case EOF:
                        freopen("code.lex", "w", out);

                        printf("Close the string before file ends.\n");
                        exit(1);
                    default:
                        fputc(ch, out);
                        nextState = 2;
                        break;
                }
                break;
            case 3:
                ch = fgetc(in);
                switch (ch) {
                    case '(':
                        fprintf(out, "Left Parenthesis\n");
                        nextState = 3;
                        break;
                    case '*':
                        nextState = 4;
                        break;
                    default:
                        fprintf(out, "Left Parenthesis\n");
                        nextState = 0;
                        break;
                }
                break;
            case 4:
                ch = fgetc(in);
                switch (ch) {
                    case '*':
                        nextState = 5;
                        break;
                    case EOF:
                        freopen("code.lex", "w", out);

                        printf("Close the comment before file ends.\n");
                        exit(1);
                    default:
                        nextState = 4;
                        break;
                }
                break;

            case 5:
                ch = fgetc(in);
                switch (ch) {
                    case ')':
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    case '*':
                        nextState = 5;
                        break;
                    default:
                        nextState = 4;
                        break;
                }
                break;
            case 6:
                ch = fgetc(in);

                if (iLength > 10) {
                    freopen("code.lex", "w", out);

                    printf("Integer constants cannot be longer than 10 digits.\n");
                    exit(1);
                }

                if (isdigit(ch)) {
                    fputc(ch, out);

                    iLength++;
                    nextState = 6;
                } else {
                    fprintf(out, ")\n");
                    iLength = 0;
                    nextState = 0;
                }
                break;
            case 7:
                ch = fgetc(in);
                switch (ch) {
                    case '+':
                        fprintf(out, "Operator(++)\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    default:
                        fprintf(out, "Operator(+)\n");
                        nextState = 0;
                        break;
                }
                break;
            case 8:
                ch = fgetc(in);
                switch (ch) {
                    case '-':
                        fprintf(out, "Operator(--)\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    default:
                        fprintf(out, "Operator(-)\n");
                        nextState = 0;
                        break;
                }
                break;
            case 9:
                ch = fgetc(in);
                switch (ch) {
                    case '=':
                        fprintf(out, "Operator(:=)\n");
                        ch = fgetc(in);
                        nextState = 0;
                        break;
                    default:
                        nextState = 0;
                        break;
                }
                break;
            default:
                break;
        }
    }

    end:
    fclose(in), fclose(out);

    return 0;
}
