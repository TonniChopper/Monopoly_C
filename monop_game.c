#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "monopoly.h" // NEMENIT !!!

void default_vypis(int kocka, int turn_num, int player_id) {
    printf("R: %d\nTurn: %d\nPlayer on turn: P%d\n\n",kocka , turn_num, player_id);
}
int get_property_location(PLAYER player, PROPERTY* property, SPACE game_board[], int num_spaces) {
    for (int i = 0; i <= num_spaces; i++) {
        if (game_board[i].type == Property && game_board[i].property == property) {
            for (int j = 0; j < player.num_properties; j++) {
                if (player.owned_properties[j] == property) {
                    return i;
                }
            }
        }
    }
    return -1;
}
void p_vypis(PLAYER players[], int num_players, SPACE game_board[]){
    printf("Players:\n");
    for (int i = 0; i < num_players; ++i) {
        printf("%d. S: %d, C: %d, JP: %d, IJ: %s\n", players[i].number, players[i].space_number, players[i].cash,
               players[i].num_jail_pass, players[i].is_in_jail ? "YES" : "NO");
        if (players[i].num_properties > 0)
            for (int j = 0; j < players[i].num_properties; ++j) {
                printf("\t%-16s %3d %-7s S%d\n", players[i].owned_properties[j]->name, players[i].owned_properties[j]->price,
                       property_colors[players[i].owned_properties[j]->color], 1+get_property_location(players[i], players[i].owned_properties[j], game_board, NUM_SPACES));

            }
    }
}
void s_vypis(PLAYER players[], SPACE game_board[], int num_players){
    printf("Game board:\n");
    for (int i = 0; i < NUM_SPACES; ++i) {
        if (game_board[i].type == Property) {
            int property_printed = 0;
            for (int k = 0; k < num_players; ++k) {
                for (int j = 0; j < players[k].num_properties; ++j) {
                    if (players[k].owned_properties[j] == game_board[i].property) {
                        for (int d = 0; d < players[k].num_properties; ++d) {
                            if (players[k].owned_properties[d]->color == game_board[i].property->color &&
                                players[k].owned_properties[d] != game_board[i].property) {
                                printf("%d. %-16s %3d %-8s P%d %s\n", i+1, game_board[i].property->name, game_board[i].property->price,
                                       property_colors[game_board[i].property->color], players[k].number, "yes");
                                property_printed = 1;
                                break;
                            }
                        }
                        if (!property_printed) {
                            printf("%d. %-16s %3d %-8s P%d %s\n", i+1, game_board[i].property->name, game_board[i].property->price,
                                   property_colors[game_board[i].property->color], players[k].number, "no");
                            property_printed = 1;
                        }
                        break;
                    }
                }
                if (property_printed) {
                    break;
                }
            }
            if (!property_printed) {
                printf("%d. %-16s %3d %-8s\n", i+1, game_board[i].property->name, game_board[i].property->price,
                       property_colors[game_board[i].property->color]);
            }
        }else {
            printf("%d. %-10s\n", i+1, space_types[game_board[i].type]);
        }
    }
}
void vypis(PLAYER players[], int num_players, SPACE game_board[], int s_style, int p_style, int g_style){
    if (g_style == 1) {
        p_vypis(players, num_players, game_board);
        s_vypis(players, game_board, num_players);
        printf("WINNER: -\n");
    }
    if (p_style == 1) {
        p_vypis(players, num_players, game_board);
    }
    if (s_style == 1) {
        s_vypis(players, game_board, num_players);
    }
}
int check_winner(PLAYER players[], int num_players){
    int max_wealth = 0, max_wealth_p = 0;;
    int winner_index = -1;
    int check_prop = 0;
    int draw = 0;

    // Find the player with the highest wealth
    for (int i = 0; i < num_players; ++i) {
        int total_wealth = players[i].cash;
        if (total_wealth > max_wealth) {
            max_wealth = total_wealth;
            winner_index = i;
            check_prop = 0;
        } else if (total_wealth == max_wealth) {
            check_prop = 1;
        }
    }
    if (check_prop == 1) {
        for (int i = 0; i < num_players; ++i) {
            if (players[i].cash == max_wealth) {
                int total_wealth_p = players[i].cash;
                for (int j = 0; j < players[i].num_properties; ++j) {
                    total_wealth_p += players[i].owned_properties[j]->price;
                }
                if (total_wealth_p > max_wealth_p) {
                    max_wealth_p = total_wealth_p;
                    winner_index = i;
                    draw = 0;
                } else if (total_wealth_p == max_wealth_p) {
                    draw = 1;
                }
            }
        }
    }else {
        return winner_index + 1;
    }
    // Check for a draw
    if (draw) {
        return -1;
    } else {
        return winner_index + 1;
    }
}
int is_property_owned(PLAYER player, int num_players, PROPERTY* property_to_check) {
    for (int i = 0; i < num_players; ++i) {
        for (int j = 0; j < player.num_properties; ++j) {
            if (player.owned_properties[j] == property_to_check) {
                return 1;
            }
        }
    }
    return 0;
}
int main(int argc, char *argv[]) {
    int opt;
    int s_style=0, p_style=0, g_style=0, kocka = 0, turn_num=0, winner = 0, remiza = 0,stop = 0, result = 0, next = 0, already_owned = 0, result_a = 0;
    int num_players = 2;
    PLAYER players[MAX_PLAYERS];
    while ((opt = getopt(argc, argv, "n:spg")) != -1) {
        switch (opt) {
            case 'n':
                num_players = atoi(optarg);
                break;
            case 's':
                s_style = 1;
                break;
            case 'p':
                p_style = 1;
                break;
            case 'g':
                g_style = 1;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-n num_players] [-s] [-p] [-g]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < num_players; ++i) {
        players[i].number = i + 1;
        players[i].space_number = 1;
        if (num_players == 2){
            players[i].cash = 20;
        }else if (num_players == 3) {
            players[i].cash = 18;
        }else if (num_players == 4) {
            players[i].cash = 16;
        }
        players[i].num_jail_pass = 0;
        players[i].is_in_jail = 0;
        players[i].num_properties = 0;
    }
    p_vypis(players, num_players, game_board);
    s_vypis(players, game_board, num_players);
    printf("Winner: -\n");
    while (winner!= 1 || remiza != 1) {
        for (int i = 0; i < num_players; ++i) {
            turn_num += 1;
            scanf("%d", &kocka);
            default_vypis(kocka, turn_num, players[i].number);
            players[i].space_number += kocka;
            if (players[i].space_number > NUM_SPACES) {
                players[i].space_number -= NUM_SPACES;
                players[i].cash += 2;
            }
            if (players[i].is_in_jail == 1) {
                if (players[i].cash >= 1) {
                    players[i].cash -= 1;
                    players[i].is_in_jail = 0;
                }else {
                    stop = 1;
                    break;
                }
            }
            for (int j = 0; j < num_players; ++j) {
                if (players[j].cash < 0) {
                    stop = 1;
                    break;
                }
            }
            if (game_board[players[i].space_number - 1].type == Property) {
                already_owned = is_property_owned(players[i], num_players, game_board[players[i].space_number - 1].property);
                if (already_owned) {
                    vypis(players, num_players, game_board, s_style, p_style, g_style);
                    continue;
                }
                for (int j = 0; j < num_players; ++j) {
                    if (i != j) {
                        int property_location = get_property_location(players[j], game_board[players[i].space_number - 1].property, game_board, NUM_SPACES);
                        if (property_location != -1) {
                            int fine = game_board[players[i].space_number - 1].property->price;
                            for (int k = 0; k < players[j].num_properties; ++k) {
                                if (players[j].owned_properties[k]->color == game_board[players[i].space_number - 1].property->color &&
                                    players[j].owned_properties[k] != game_board[players[i].space_number - 1].property) {
                                    fine *= 2;
                                    break;
                                }
                            }if (players[i].cash >= fine) {
                                players[i].cash -= fine;
                                players[j].cash += fine;
                                next = 1;
                            } else {
                                result_a = j + 1;
                                stop = 1;
                                break;
                            }
                        }
                    }
                }
                if (next == 1) {
                    next = 0;
                    vypis(players, num_players, game_board, s_style, p_style, g_style);
                    continue;
                }
                if (stop == 1) {
                    break;
                }
                if (game_board[players[i].space_number - 1].property->price <= players[i].cash) {
                    players[i].cash -= game_board[players[i].space_number - 1].property->price;
                    players[i].owned_properties[players[i].num_properties] = game_board[players[i].space_number - 1].property;
                    players[i].num_properties += 1;
                } else {
                    stop = 1;
                    break;
                }
            }else if (game_board[players[i].space_number - 1].type == Go_to_jail) {
                if (players[i].num_jail_pass > 0) {
                    players[i].num_jail_pass -= 1;
                }else {
                    players[i].is_in_jail = 1;
                    players[i].space_number = 7;
                }
            }else if (game_board[players[i].space_number - 1].type == Jail_pass) {
                players[i].num_jail_pass += 1;
            } else if (game_board[players[i].space_number - 1].type == In_jail) {
                vypis(players, num_players, game_board, s_style, p_style, g_style);
                continue;
            } else if (game_board[players[i].space_number - 1].type == Free_parking) {
                vypis(players, num_players, game_board, s_style, p_style, g_style);
                continue;
            }
            if (stop != 1)
                vypis(players, num_players, game_board, s_style, p_style, g_style);
        }
        if (stop == 1) {
            if(num_players == 2) {
                if (result_a == 0) {
                    result = check_winner(players, num_players);
                    if (result == -1) {
                        winner = 0;
                        remiza = 1;
                        p_vypis(players, num_players, game_board);
                        s_vypis(players, game_board, num_players);
                        printf("Winner: ?\n");
                        break;
                    }
                    p_vypis(players, num_players, game_board);
                    s_vypis(players, game_board, num_players);
                    printf("Winner: P%d\n", result);
                    break;
                } else {
                    p_vypis(players, num_players, game_board);
                    s_vypis(players, game_board, num_players);
                    printf("Winner: P%d\n", result_a);
                    break;
                }
            } else {
                result = check_winner(players, num_players);
                if (result == -1) {
                    winner = 0;
                    remiza = 1;
                    p_vypis(players, num_players, game_board);
                    s_vypis(players, game_board, num_players);
                    printf("Winner: ?\n");
                    break;
                }
                p_vypis(players, num_players, game_board);
                s_vypis(players, game_board, num_players);
                printf("Winner: P%d\n", result);
                break;
            }
        }
    }
    return 0;
}