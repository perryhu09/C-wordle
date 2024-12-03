/* Author: perryhu09 */
/* Date: Nov-27-2024 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#define ResultGreen     1
#define ResultYellow    2
#define ResultRed       4
#define max             14855
#define ValOk           0
#define ValBadInput     1
#define ValNoSuchWord   2

#define ClrGreen        "\033[0;32m"
#define ClrYellow       "\033[0;33m"
#define ClrRed          "\033[0;31m"
#define ClrStop         "\033[0m"

typedef char ValResult;
typedef char Result;
static char words[max][5];
bool continuation;
int rounds;
bool corrects[5];
bool win;

ValResult validator(char*);
void seed(void);
char *readline(void);
bool isin(char, char*);
Result checkcolor(char, int, char*);
Result *checkword(char*, char*);
int readfile(char*);
char *randomword(int);
void prompt(char*);
void print_results(Result*, char*, char*);
void gameloop(char*);
int main(int, char**);

ValResult validator(char *word) {
    int n, i;
    bool ret;

    bool strcmp_(char *str1, char *str2) {
        int s, i;

        s = 0;
        for(i = 0; i < 5; i++) {
            if (str1[i] == str2[i]) {
                s++;
            }
        }
        return (s == 5)? true : false;
    };

    n = strlen(word);
    if(n != 5) return ValBadInput;

    ret = false;
    n = 0;
    for (i = 0; i < max; i++) {
        if(strcmp_(words[i], word)) {
            ret = true;
            break;
        }
    }

    if (ret) {
        return ValOk;
    } else {
        return ValNoSuchWord;
    }
}

void seed() {
    int x;

    x = getpid();
    srand(x);

    return;
}

char *readline() {
    static char buf[8];
    int size;

    memset(buf, 0, 8);
    fgets(buf, 7, stdin);
    size = strlen(buf);
    assert(size > 0);
    size--;
    buf[size] = 0;
    
    return buf;
}

bool isin(char c, char *str) {
    bool ret; 
    int size;

    ret = false;
    size = strlen(str);
    for(int i = 0; i < size; i++) {
        if(c == str[i]) {
            ret = true;
            break;
        }
    }

    return ret;
}

Result checkcolor(char guess, int idx, char *word) {
    char correct;

    correct = (word[idx]);
    if (guess == correct) {
        corrects[idx] = true;
        return ResultGreen;
    } else if (isin(guess, word)){
        return ResultYellow;
    }
    return ResultRed;
}

Result *checkword(char *guess, char *word) {
    static Result res[5];
    
    for(int i = 0; i < 5; i++) {
        res[i] = checkcolor(guess[i], i, word);
    }

    return res;
}

char *randomword(int m) {
    int x;
    static char ret[8];

    x = rand() % m;

    ret[0] = words[x][0];
    ret[1] = words[x][1];
    ret[2] = words[x][2];
    ret[3] = words[x][3];
    ret[4] = words[x][4];
    ret[5] = 0;

    return ret;
}

int readfile(char *filename) {
    char buf[8];
    int i, size;
    FILE *fd;

    fd = fopen(filename, "r");
    if (!fd) {
        perror("fopen");
        return -1;
    }

    i = 0;
    memset(buf, 0, 8);

    while (fgets(buf, 7, fd)) {
        size = strlen(buf); 
        if (size < 1) {
            memset(buf, 0, 8);
            continue;
        }
        size--;
        buf[size] = 0;

        if(size != 5) {
            memset(buf, 0, 8);
            continue;
        }

        words[i][0] = buf[0];
        words[i][1] = buf[1];
        words[i][2] = buf[2];
        words[i][3] = buf[3];
        words[i][4] = buf[4];

        memset(buf, 0, 8);
        i++;

        if (max <= i) {
            break;
        }
    }
    fclose(fd);

    return i;
}

void prompt(char *correctword) {
    int i;

    for(i = 0; i < 5; i++) {
        switch (corrects[i]) {
            case true:
                printf("%c", correctword[i]);
                break;
            case false:
                printf("-");
                break;
        }
    }

    printf("\n\n%d> ", (6 - rounds));
    fflush(stdout);

    return;
}

void print_results(Result *res, char *guess, char *correct) {
    int i;

    for(i = 0; i < 5; i++) {
        switch (res[i]) {
            case ResultGreen:
                printf("%s%c%s", ClrGreen, guess[i], ClrStop);
                break;
            case ResultYellow:
                printf("%s%c%s", ClrYellow, guess[i], ClrStop);
                break;
            case ResultRed:
                printf("%s%c%s", ClrRed, guess[i], ClrStop);
                break;
            default:
                printf("%s%c%s", ClrStop, guess[i], ClrStop);
                break;
        }
    }
    printf("\n");

    return;
}

void gameloop(char *correct) {
    char *input;
    Result *res;
    ValResult valres;
    int i, c;

    prompt(correct);
    input = readline();
    valres = validator(input);
    switch (valres) {
        case ValBadInput:
            printf("%s\n", "Bad input");
            return;
        case ValNoSuchWord:
            printf("%s\n", "No such word - only 5 letter English words are permitted.");
            //rounds++;
            return;
        default:
            break;
    }

    res = checkword(input, correct);
    for (i = c = 0; i < 5; i++) {
        if (corrects[i]) {
            c++;
        }
    }

    print_results(res, input, correct);

    if (c == 5) {
        win = true;
        continuation = false;
        return;
    }

    rounds++;
    if (rounds > 5) {
        win = false;
        continuation = false;
        return;
    }

    return;
}

int main(int argc, char *argv[]) {
    int n;
    char *p; 

    corrects[0] = false;
    corrects[1] = false;
    corrects[2] = false;
    corrects[3] = false;
    corrects[4] = false;

    rounds = 0;
    win = false;
    seed();
    n = readfile("wordlist.txt");
    assert(!(n < 0));
    p = randomword(n);
    fflush(stdout);

    printf("Welcome to Wordle. You have 6 attempts to guess a 5 letter English Word. Good Luck!\n");
    continuation = true;
    while (continuation) {
        gameloop(p);
    }
    printf("The correct word was: '%s'\n", p);
    if (win) {
        printf("Congratulations, you won the game!\n");
    } else {
        printf("You lost. Game Over.\n");
    }
    
    return 0;
}
