#include "../include/score.h"
#include <stdio.h>

void loadHighscore(int* highscore){
    FILE* f = fopen("../assets/highscores.txt", "r");
    if (!f){
        *highscore = 0;
        return;
    }
    fscanf(f, "%d", highscore);
    fclose(f);
}

void saveHighscore(int score, int* highscore){

        *highscore = score;
        printf("Ukládám highscore: %d\n", score);

        FILE* f = fopen("../assets/highscores.txt", "w");
        if (!f) {
            perror("Chyba při otevírání highscores.txt");
            return;
        }
        if (!f) return;

        fprintf(f, "%d\n", *highscore);
        fclose(f);

}