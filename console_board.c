#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifdef WIN32
void clear() { system("cls"); }
#else
void clear() { system("clear"); }
#endif

char capitalize(char car)
{
    if (!((int)'a'<=(int)car && (int)car<=(int)'z'))
        return car;
    int e = car;
    e -= abs((int)'A' - (int)'a');/*zan ar ry zokybe hajaina*/
    return (char)e;
}
bool is_car(char car) { return (((int)'A'<=(int)car && (int)car<=(int)'E')); }
bool is_num(char car) { return ((int)'0'<=(int)car && (int)car<=(int)'9'); }

enum { MENU, GAME, HELP };
typedef enum { N, NE, E, SE, S, SW, W, NW, UNDEFINED } Direction;

typedef struct
{
    int state;
    char tour;
    char input[10];
    bool running;
    char *move_error;
}State;

typedef char** Board;

void clear_buf()
{
    int c;
    for (c = 0; c!='\n' && c!=EOF; c=getchar());
}
int len(char* string)
{
    if (!string) return 0;

    int ret;
    for (ret=0; string[ret] != '\0'; ++ret);
    return ret;
}
bool valid(char*);
char** init(int, int);
void clean(Board*);
void print(Board);
void draw(Board, State*);
bool move_pawn(Board, State*, int, int, int, int);
void handle_state(State*, Board);
void get_input(State*);
Direction define_dir(int, int, int, int);

int main()
{
    Board board = init(5, 9);
    State state = {MENU, 'B', "", true, NULL};
    draw(board, &state);
    while (state.running)
    {
        get_input(&state);
        handle_state(&state, board);
        clear();
        draw(board, &state);
    }
    clean(&board);
    return 0;
}

char** init(int w, int h)
{
    char **board = (char**)malloc(w*sizeof (char*));
    int i, j;
    for (i=0; i<w; ++i)
    {
        board[i] = (char*)malloc(h*sizeof (char));
        for (j=0; j<9; ++j)
            board[i][j] = (i<2)?'N':((i>2)?'B':((j<4)?((j%2)?'B':'N'):((j>4)?((j%2)?'N':'B'):' ')));
    }
    return board;
}

void clean(Board* board)
{
    if (!(*board)) return;
    int i;
    for (i=0; i<5; ++i)
        free((*board)[i]);
    free(*board);
}

void print(Board board)
{
    char lignes[] = "ABCDE";
    printf ("        1     2     3     4     5     6     7     8     9\n");
    printf ("   _____________________________________________________________\n");
    printf ("  |                                                             |\n");
    int i;
    for (i=0; i<5; ++i)
    {
        printf ("%c |\t", lignes[i]);
        int j;
        for (j=0; j<8; ++j)
            printf("%c_____", board[i][j]);
        printf("%c\t| %c\n", board[i][8], lignes[i]);
        if (i == 4) continue;
        if (i%2)
        {
            printf ("  |	|    /|\\    |    /|\\    |    /|\\    |    /|\\    |	|\n");
            printf ("  |	|   / | \\   |   / | \\   |   / | \\   |   / | \\   |	|\n");
            printf ("  |	|  /  |  \\  |  /  |  \\  |  /  |  \\  |  /  |  \\  |	|\n");
            printf ("  |	| /   |   \\ | /   |   \\ | /   |   \\ | /   |   \\ |	|\n");
        }
        else
        {
            printf ("  |	|\\    |    /|\\    |    /|\\    |    /|\\    |    /|	|\n");
            printf ("  |	| \\   |   / | \\   |   / | \\   |   / | \\   |   / |	|\n");
            printf ("  |	|  \\  |  /  |  \\  |  /  |  \\  |  /  |  \\  |  /  |	|\n");
            printf ("  |	|   \\ | /   |   \\ | /   |   \\ | /   |   \\ | /   |	|\n");
        }
    }
    printf ("  |                                                             |\n");
    printf ("  |_____________________________________________________________|\n");
    printf ("        1     2     3     4     5     6     7     8     9\n");
}

void draw(Board board, State* s)
{
    int state = s->state;
    printf ("\t*****FANORONTSIVY*****\n");
    char **h_board = NULL;
    int i, j;
    switch (state)
    {
    case MENU:
        printf (" 1 -Hilalao\n");
        printf (" 2 -Hanohy lalao\n");
        printf (" 3 -Mahakasika ny fikisahan'ny vatom-panorona\n");
        printf (" Q -Hiala\n");
        break;
    case GAME:
        print(board);
        printf (" Anjaran'ny %s izao no milalao!\n", (s->tour == 'B')?"fotsy('B')":"mainty('N')");
        if (s->move_error)
        {
            printf (" Tsy ara-dalana ny fihetsehana natao!\n");
            printf (" %s\n", s->move_error);
            free(s->move_error);
            s->move_error = NULL;
        }
        printf (" Toromarika : ");
        break;
    case HELP:
        h_board = init(5, 9);
        for (i=0; i<5; ++i)
            for (j=0; j<9; ++j)
            {
                if (i==3 && j==5)
                    h_board[i][j] = 'B';
                else if (i==2 && j==4)
                    h_board[i][j] = 'X';
                else
                    h_board[i][j] = ' ';
            }
        print(h_board);
        printf ("\n Raha hanisaka ny vato(D6) ho eo hafovoan'ny Akalana");
        printf (" dia tahaka izao no toromarika homena : D6-C5\n");
        printf (" Anjaran'ny solosaina no mamaritra raha ara-dalàna na tsia");
        printf (" ny fihetsehana nokasaina natao.\n");
        printf (" Tsindrio 'O' raha hanohy : ");
        break;
    default: ;
    }
    clean(&h_board);
}

bool choose(Board board, int x1, int y1, int x2, int y2)
{
    State state;
    int c_x = -1, c_y = -1;
    while ((c_x!=x1 || c_y!=y1)&&(c_x!=x2 || c_y!=y2))
    {
        clear();
        printf ("\t*****FANORONTSIVY*****\n");
        print (board);
        char lignes[] = "ABCDE";
        printf ("%c%d sa %c%d ?", lignes[x1], y1+1, lignes[x2], y2+1);
        printf(" Safidio ny vato tiana ho alàna : ");
        get_input(&state);
        if (is_car(state.input[0]) && is_num(state.input[1]))
        {
            c_x = (int)state.input[0]-(int)'A';
            c_y = strtol(&state.input[1], NULL, 10)-1;
        }
    }
    return (c_x == x2 && c_y == y2);
}
void handle_state(State* s, Board board)
{
    int state = s->state;
    char input = s->input[0];
    switch(state)
    {
    case MENU:
        if (input == '1')
            s->state = GAME;
        else if (input == '2');
        else if (input == '3')
            s->state = HELP;
        else if (input == 'Q')
            s->running = false;
        break;
    case HELP:
        if (input == 'O')
            s->state = MENU;
        break;
    case GAME:
        if (input == 'Q')
            s->state = MENU;

        // Gestion des commandes
        else if (valid(s->input))
        {
            int p_x, p_y, d_x, d_y;
            p_x = (int)s->input[0]-(int)'A';
            p_y = strtol(&s->input[1], NULL, 10)-1;
            d_x = (int)s->input[3]-(int)'A';
            d_y = strtol(&s->input[4], NULL, 10)-1;

            // Gestion des captures
            if (move_pawn(board, s, p_x, p_y, d_x, d_y))
            {
                int x = d_x-p_x ,y = d_y-p_y;
                int i=d_x+x, j=d_y+y;
                int a = p_x-x, b = p_y-y;
                int s_x = i, s_y = j;
                if ((a>=0 && a<5) && (b>=0 && b<9))
                    if (board[a][b] == s->tour)
                    {
                        if ((i>=0 && i<5) && (j>=0 && j<9))
                        {
                            if (board[i][j] == s->tour)
                            {
                                if (choose(board, i, j, a, b))
                                {
                                    s_x = a;
                                    s_y = b;
                                    x *= -1;
                                    y *= -1;
                                }
                            }
                            else
                            {
                                s_x = a;
                                s_y = b;
                                x *= -1;
                                y *= -1;
                            }
                        }
                        else
                        {
                            s_x = a;
                            s_y = b;
                            x *= -1;
                            y *= -1;
                        }
                    }
                for (i=s_x, j=s_y; (i>=0 && i<5) && (j>=0 && j<9); i+=x, j+=y)
                {
                    if (board[i][j] != s->tour)
                        break;
                    board[i][j] = ' ';
                }
            }
        }
        break;
    default: ;
    }
}

bool valid(char* command)
{
    if (len(command) != 5)
        return false;
    return (
            (is_car(command[0]) && is_car(command[3])) &&
            command[2] == '-' &&
            (is_num(command[1]) && is_num(command[4]))
            );
}

void get_input(State* s)
{
    if (!fgets(s->input, 9, stdin)) return clear_buf();
    char* ret = strchr(s->input, '\n');
    if (ret) *ret = '\0';
    else clear_buf();
    int i;
    for (i=0; i<9; ++i)
        s->input[i] = capitalize(s->input[i]);
}

void suggest_move(Board, State*, int, int);
bool _move_pawn(Board board, State *state, int p_x, int p_y, int d_x, int d_y, bool try_move)
{
    if (!try_move)
    {
        state->move_error = (char*)malloc(200*sizeof (char));
        state->move_error[0] = '\0';
    }
    if ((p_x<0 || p_y<0) || (p_x>=5 || p_y>=9) ||
        (d_x<0 || d_y<0) || (d_x>=5 || d_y>=9))
    {
        if (!try_move)
            sprintf(state->move_error, " Havoaka ny Akalana angaha ny vatom-panoronao?");
        return false;
    }

    float dist = sqrt((d_x-p_x)*(d_x-p_x)+(d_y-p_y)*(d_y-p_y));
    if ((p_x+p_y)%2)
    {
        if (dist!=1)
        {
            if (!try_move)
                suggest_move(board, state, p_x, p_y);
            return false;
        }
    }
    else
        if (1>dist || dist>sqrt(2))
        {
            if (!try_move)
                suggest_move(board, state, p_x, p_y);
            return false;
        }

    char *p = &board[p_x][p_y], *d = &board[d_x][d_y];
    if (p[0] != state->tour || p[0] == ' ')
    {
        if (!try_move)
        {
            sprintf(state->move_error, " %s(%c) no vatom-panoronao!\n", (state->tour=='N')?"mainty":"fotsy", state->tour);
            suggest_move(board, state, p_x, p_y);
        }
        return false;
    }
    if (d[0] != ' ')
    {
        if (!try_move)
        {
            sprintf(state->move_error, " Efa misy vatom-panorona eo!\n");
            suggest_move(board, state, p_x, p_y);
        }
        return false;
    }
    if (!try_move)
    {
        d[0] = p[0];
        p[0] = ' ';
        state->tour = (state->tour == 'B')?'N':'B';
        free(state->move_error);
        state->move_error = NULL;
    }
    return true;
}

void suggest_move(Board board, State* state, int p_x, int p_y)
{
    int i, j;
    char lignes[] = "ABCDE";
    int sgn[2] = {-1, 1};

    if ((p_x+p_y)%2)
    {
        for (i=0; i<2; ++i)
        {
            if (_move_pawn(board, state, p_x, p_y, p_x+sgn[i], p_y, true))
            {
                sprintf(state->move_error, " Andramo : %c%d-%c%d\n", lignes[p_x], p_y+1, lignes[p_x+sgn[i]], p_y+1);
                break;
            }
            else if (_move_pawn(board, state, p_x, p_y, p_x, p_y+sgn[i], true))
            {
                sprintf(state->move_error, " Andramo : %c%d-%c%d\n", lignes[p_x], p_y+1, lignes[p_x], p_y+sgn[i]+1);
                break;
            }
        }
    }
    else
    {
        for (i=0; i<2; ++i)
            for (j=0; j<2; ++j)
                if (_move_pawn(board, state, p_x, p_y, p_x+sgn[i], p_y+sgn[j], true))
                {
                    sprintf(state->move_error, " Andramo : %c%d-%c%d\n", lignes[p_x], p_y+1, lignes[p_x+sgn[i]], p_y+sgn[j]+1);
                    break;
                }
    }
}
bool move_pawn(Board board, State* state, int p_x, int p_y, int d_x, int d_y)
{
    return _move_pawn(board, state, p_x, p_y, d_x, d_y ,false);
}

Direction define_dir(int p_x, int p_y, int d_x, int d_y)
{
    int v_x = d_x-p_x, v_y = d_y-p_y;
    if (v_x)
    {
        if (v_y)
        {
            if (v_x>0)
            {
                if (v_y>0)
                    return SE;
                else
                    return NE;
            }
            else
            {
                if (v_y>0)
                    return SW;
                else
                    return NW;
            }
        }
        else
        {
            if (v_x>0)
                return E;
            else
                return W;
        }
    }
    else
    {
        if (v_y<0)
            return N;
        else
            return S;
    }
    return UNDEFINED;
}
