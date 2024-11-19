#ifndef SCORE_H
#define SCORE_H

#define SCORE_PATH "score.txt"

struct store {
    int highscore;
    int seed;
};

void mount_sd();
void save_highscore(int highscore);
int get_highscore();
void save_seed();
int get_seed();

#endif