#include "../../include/common.h"
#include "../../include/client.h"


void read_command(char *command, int size)
{
    struct termios new_term;

    tcgetattr(STDIN_FILENO, &old_term);

    new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    int pos = 0;
    int history_index = history_count;

    command[0] = '\0';

    while (1)
    {
        char ch;

        if (read(STDIN_FILENO, &ch, 1) <= 0)
            continue;

        /* Enter key */
        if (ch == '\n' || ch == '\r')
        {
            command[pos] = '\0';
            printf("\n");
            break;
        }

        /* Arrow keys */
        /* Up Arrow generates sequence   : ESC + [ + A  */
        /* Down Arrow generates sequence : ESC + [ + B  */
        /* ASCII value of ESC : 27 */
        else if (ch == 27)
        {
            char seq[2];

            if (read(STDIN_FILENO, &seq[0], 1) <= 0)
                continue;

            if (read(STDIN_FILENO, &seq[1], 1) <= 0)
                continue;

            /* UP arrow */
            if (seq[1] == 'A' && history_index > 0)
            {
                history_index--;

                strcpy(command, history[history_index]);
                pos = strlen(command);
            }

            /* DOWN arrow */
            else if (seq[1] == 'B' && history_index < history_count)
            {
                history_index++;

                if (history_index == history_count)
                    command[0] = '\0';
                else
                    strcpy(command, history[history_index]);

                pos = strlen(command);
            }

            /* Refresh terminal display */
            printf("\r\033[2KClient]%s", command);
            fflush(stdout);
        }
        /* Backspace */
        else if (ch == 127 || ch == '\b')
        {
            if (pos > 0)
            {
                pos--;
                command[pos] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        }
        /* Normal characters */
        else
        {
            if (pos < size - 1)
            {
                command[pos++] = ch;
                command[pos] = '\0';

                putchar(ch);
                fflush(stdout);
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
}


void store_history(const char *command)
{
    if (strlen(command) == 0)
        return;

    if (history_count == MAX_HISTORY)
    {
        memmove(history, history + 1, (MAX_HISTORY - 1) * MAX_COMMAND_SIZE);

        history_count--;
    }

    strcpy(history[history_count], command);
    history_count++;
}   