#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "monopoly.h" // Nemenit !!!
// Nevkladat monopoly.c !!!

#define MAX_PROPERTY_NAME_LENGTH 100
#define MAX_NAME 100

PROPERTY default_properties[NUM_PROPERTIES] = {
        {.name = "FOOD TRUCK", .price = 1, .color = Brown},
        {.name = "PIZZA RESTAURANT", .price = 1, .color = Brown},
        {.name = "DOUGHNUT SHOP", .price = 1, .color = Skyblue},
        {.name = "ICE CREAM SHOP", .price = 1, .color = Skyblue},
        {.name = "MUSEUM", .price = 2, .color = Purple},
        {.name = "LIBRARY", .price = 2, .color = Purple},
        {.name = "THE PARK", .price = 2, .color = Orange},
        {.name = "THE BEACH", .price = 2, .color = Orange},
        {.name = "POST OFFICE", .price = 3, .color = Red},
        {.name = "TRAIN STATION", .price = 3, .color = Red},
        {.name = "COMMUNITY GARDEN", .price = 3, .color = Yellow},
        {.name = "PET RESCUE", .price = 3, .color = Yellow},
        {.name = "AQUARIUM", .price = 4, .color = Green},
        {.name = "THE ZOO", .price = 4, .color = Green},
        {.name = "WATER PARK", .price = 5, .color = Blue},
        {.name = "AMUSEMENT PARK", .price = 5, .color = Blue}
};
char *property_colors_[NUM_COLORS] = {
        "Brown",
        "Skyblue",
        "Purple",
        "Orange",
        "Red",
        "Yellow",
        "Green",
        "Blue"
};
char *space_types_[NUM_SPACE_TYPES] = {
        "PROPERTY",
        "START",
        "FREE PARKING",
        "IN JAIL",
        "GO TO JAIL",
        "JAIL PASS"
};

const char* color_to_string(COLOR color) {
    switch (color) {
        case Brown: return "Brown";
        case Skyblue: return "Skyblue";
        case Purple: return "Purple";
        case Orange: return "Orange";
        case Red: return "Red";
        case Yellow: return "Yellow";
        case Green: return "Green";
        case Blue: return "Blue";
        default: return "Unknown";
    }
}


PROPERTY *load_properties(const char *filename, int *num_properties) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("E1");
        return 0;
    }

    PROPERTY *properties_ = NULL;
    *num_properties = 0;

    char line[MAX_NAME + 20];
    COLOR color = Brown;
    while (fgets(line, sizeof(line), file)) {

        if (strcmp(line, "\n") == 0) {
            continue;
        }

        if (line[0] == '[') {
            switch (line[1]) {
                case 'B':
                    if (line[2] == 'l') // Ak nasleduje "l", ide o Blue, inak ide o Brown
                        color = Blue;
                    else
                        color = Brown;
                    break;
                case 'G': color = Green; break;
                case 'P': color = Purple; break;
                case 'O': color = Orange; break;
                case 'R': color = Red; break;
                case 'Y': color = Yellow; break;
                case 'S': color = Skyblue; break;
                default: color = Brown; break; // Default farba
            }
            continue;
        }

        char name[MAX_NAME];
        int price;
        sscanf(line, "\"%[^\"]\":%d", name, &price);

        properties_ = realloc(properties_, (*num_properties + 1) * sizeof(PROPERTY));
        if (properties_ == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            fclose(file);
            free(properties_);
            return NULL;
        }

        strncpy(properties_[*num_properties].name, name, MAX_NAME);
        properties_[*num_properties].price = price;
        properties_[*num_properties].color = color;

        (*num_properties)++;
    }

    fclose(file);

    return properties_;
}
void strip_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
void printProperties(PROPERTY *properties_, int count) {
    for (int i = 0; i < count; i++) {
        printf("%-16s\t%-3d\t", properties_[i].name, properties_[i].price);
        switch (properties_[i].color) {
            case Brown: printf("Brown\n"); break;
            case Skyblue: printf("Skyblue\n"); break;
            case Purple: printf("Purple\n"); break;
            case Orange: printf("Orange\n"); break;
            case Red: printf("Red\n"); break;
            case Yellow: printf("Yellow\n"); break;
            case Green: printf("Green\n"); break;
            case Blue: printf("Blue\n"); break;
            default: printf("Unknown\n"); break; // NeznÃ¡ma farba
        }
    }
}
SPACE *read_game_board(const char *game_board_file, int *num_spaces) {
    FILE *file = fopen(game_board_file, "r");
    if (file == NULL) {
        printf("E1");
        return 0;
    }
    char str[3];

    int line_count;
    fgets(str , 3 , file);
    line_count = atoi(str);

    SPACE *game_boardi = malloc(line_count * sizeof(SPACE));
    if (game_boardi == NULL) {
        fclose(file);
        return NULL;
    }


    for (int i = 0; i < line_count; i++) {
        char line[MAX_NAME + 1];
        if (fgets(line, sizeof(line), file)) {
            strip_newline(line);
            if (strcmp(line, "START") == 0) {
                game_boardi[i].type = Start;
            } else if (strcmp(line, "PROPERTY") == 0) {
                game_boardi[i].type = Property;
            } else if (strcmp(line, "FREE PARKING") == 0) {
                game_boardi[i].type = Free_parking;
            } else if (strcmp(line, "IN JAIL") == 0) {
                game_boardi[i].type = In_jail;
            } else if (strcmp(line, "GO TO JAIL") == 0) {
                game_boardi[i].type = Go_to_jail;
            } else if (strcmp(line, "JAIL PASS") == 0) {
                game_boardi[i].type = Jail_pass;
            } else {
                i = i - 1;
                continue;
            }

            // Inicializujeme property na NULL
            game_boardi[i].property = NULL;
        } else {
            free(game_boardi);                                           // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            fclose(file);
            return NULL;
        }
    }

    //
    fclose(file);

    //
    *num_spaces = line_count;
    return game_boardi;
}
void print_game_board( SPACE *game_boardi, int num_spaces) {
    for (int i = 0; i < num_spaces; i++) {
        printf("%s\n", space_types_[game_boardi[i].type]);
    }
}
void s_vypis(SPACE* game_boardi, int board_size){
    printf("Game board:\n");
    for (int i = 0; i < board_size; ++i) {
        printf("%d. ", i+1 );
        if (game_boardi[i].type == Property) {
            if (game_boardi[i].property != NULL) {
                printf("%-20s %d  %s\n", game_boardi[i].property->name,
                       game_boardi[i].property->price,
                       property_colors_[game_boardi[i].property->color]);
            }
        } else {
            printf("%s\n", space_types_[game_boardi[i].type]);
        }
    }
}

int main(int argc, char *argv[]) {
    char *properties_file = NULL;
    char *game_board_file = NULL;
    SPACE *game_boardi =NULL;
    int opt, rezim_t = 0, rezim_b = 0;
    int board_size = 0, counter = 0, prop_in_board = 0;

    while ((opt = getopt(argc, argv, "t:b:")) != -1) {
        switch (opt) {
            case 't':
                rezim_t = 1;
                properties_file = optarg;
                break;
            case 'b':
                rezim_b = 1;
                game_board_file = optarg;
                break;
            case '?':
                printf("E1\n");
            default:
                return 0;
        }
    }
    if (rezim_t == 1 && rezim_b == 0){
        int num_properties;
        PROPERTY *properties_ = load_properties(properties_file, &num_properties);
        if (properties_ == NULL) {
            //free(properties_);
            return 1;
        }
        printProperties(properties_, num_properties);
        free(properties_);
        return 0;
    }
    if (rezim_b == 1) {
//        int board_size;

        game_boardi = read_game_board(game_board_file, &board_size);
        if (game_boardi != NULL) {
            if (rezim_t == 1){
                int num_properties;
                PROPERTY *properties_ = load_properties(properties_file, &num_properties);
                if (properties_ == NULL) {
                    free(game_boardi);
                    free(properties_);
                    return 1;
                }
                for (int i = 0; i < board_size; ++i) {
                    if(game_boardi[i].type == Property){
                        prop_in_board++;
                    }
                }
                for (int i = 0; i < board_size; ++i) {
//                    strip_newline(game_boardi[i].type);
                    if (game_boardi[i].type == Property) {
                        game_boardi[i].property = &properties_[counter];
                        counter++;
                        if(counter > num_properties){
                            printf("E2\n");
                            free(game_boardi);
                            free(properties_);
                            return 0;
                        }
                    }
                }
                if (properties_ != NULL) {
                    free(properties_);
                }
            } else{
                for (int i = 0; i < board_size; ++i) {
//                    strip_newline(game_boardi[i].type);
                    if (game_boardi[i].type == Property) {
                        game_boardi[i].property = &default_properties[counter];
                        counter++;
                        if(counter > 16){
                            printf("E2\n");
                            free(game_boardi);
                            return 0;
                        }
                    }
                }
            }
            s_vypis(game_boardi, board_size);

        }
    }
    free(game_boardi);
    return 0;
}