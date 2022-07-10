/*
It can be more useful to use some JSON library for a real application instead of this solution
*/
#include <stdio.h>
#include <stdlib.h>
#include "rt_mb_buffer.h"

void save_to_json(Trajectory_list* tl){
    FILE *fptr;

    fptr = fopen("/home/all/Documents/code/data/trips.json", "w");
    if(fptr == NULL){
        printf("FILE ERROR\n");   
        exit(1);             
    }

    int LINEMAXLENGTH = 150;
    char line_to_json[LINEMAXLENGTH];
    char concatenation_tmp[LINEMAXLENGTH];
    struct tm tm;
    time_t epoch;


    strncpy(line_to_json,"[\n",LINEMAXLENGTH);
    fputs(line_to_json, fptr);

    for (int t = 0; t < tl->current_number_trajectory; t++) {
        strncpy(line_to_json,"  {\n",LINEMAXLENGTH);
        fputs(line_to_json, fptr);
        memset(line_to_json, 0, LINEMAXLENGTH);
        strncpy(line_to_json,"    \"vehicleID\":",LINEMAXLENGTH);
        memset(concatenation_tmp, 0, LINEMAXLENGTH);
        sprintf(concatenation_tmp, "%s", tl->trajectory_array[t]->vehicle_id);
        strcat(line_to_json, concatenation_tmp);
        memset(concatenation_tmp, 0, LINEMAXLENGTH);
        strcat(line_to_json, ",\n");
        fputs(line_to_json, fptr);
        memset(line_to_json, 0, LINEMAXLENGTH);

        strncpy(line_to_json,"    \"path\": [\n",LINEMAXLENGTH);
        fputs(line_to_json, fptr);
        memset(line_to_json, 0, LINEMAXLENGTH);
        for (int p = 0; p < tl->trajectory_array[t]->current_number_position; p++) {
            memset(concatenation_tmp, 0, LINEMAXLENGTH);
            memset(line_to_json, 0, LINEMAXLENGTH);
            sprintf(concatenation_tmp, "          [%s",  tl->trajectory_array[t]->position_array[p]->latitude);
            strcat(line_to_json, concatenation_tmp);
            memset(concatenation_tmp, 0, LINEMAXLENGTH);
            strcat(line_to_json, ", ");

            sprintf(concatenation_tmp, "%s",  tl->trajectory_array[t]->position_array[p]->longitude);
            strcat(line_to_json, concatenation_tmp);

            if(p < tl->trajectory_array[t]->current_number_position-1){
            strcat(line_to_json, "],\n");
            }
            else{
            strcat(line_to_json, "]\n");
            }
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);
        }
        memset(line_to_json, 0, LINEMAXLENGTH);
        strcat(line_to_json, "            ],\n");    
        fputs(line_to_json, fptr);
        memset(line_to_json, 0, LINEMAXLENGTH);
        strncpy(line_to_json,"    \"timestamps\": [\n",LINEMAXLENGTH);
        fputs(line_to_json, fptr);
        memset(line_to_json, 0, LINEMAXLENGTH);
        for (int p = 0; p < tl->trajectory_array[t]->current_number_position; p++) {
              
            if ( strptime(tl->trajectory_array[t]->position_array[p]->timestamp, "%Y-%m-%d %H:%M:%S", &tm) != NULL ){
            epoch = mktime(&tm);
            }

            sprintf(concatenation_tmp, "          %ld",  epoch);
            strcat(line_to_json, concatenation_tmp);
            memset(concatenation_tmp, 0, LINEMAXLENGTH);
            if(p < tl->trajectory_array[t]->current_number_position-1){
            strcat(line_to_json, ",\n");
            }
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);
        }
        strcat(line_to_json, "\n            ]\n");
        fputs(line_to_json, fptr);
        memset(line_to_json, 0, LINEMAXLENGTH);
        if(t < tl->current_number_trajectory-1){
            strncpy(line_to_json,"  },\n",LINEMAXLENGTH);
        }
        else{
            strncpy(line_to_json,"  }\n",LINEMAXLENGTH);
        }
        fputs(line_to_json, fptr);
        memset(line_to_json, 0, LINEMAXLENGTH);
    }
    memset(line_to_json, 0, LINEMAXLENGTH);
    strncpy(line_to_json,"]\n",LINEMAXLENGTH);
    fputs(line_to_json, fptr);
    memset(line_to_json, 0, LINEMAXLENGTH);

    fclose(fptr);

    /*
    [
    {
    "vendor": 1,
    "path": [
      [-73.97265, 40.63433],
      [-73.82531, 40.66386],
      [-73.82502, 40.66386]],
    "timestamps": [1649384546, 1649384547]
    },
    {
    "vendor": 0,
    "path": [

    struct tm tm;  
    */
}



void save_to_json_last_five_positions(Trajectory_list* tl){
    FILE *fptr;
    
    fptr = fopen("/home/all/Documents/code/data/trips_last.json", "w");

    if(fptr == NULL){
        printf("FILE ERROR\n");   
        exit(1);             
    }

    int LINEMAXLENGTH = 150;
    char line_to_json[LINEMAXLENGTH];
    char concatenation_tmp[LINEMAXLENGTH];
    struct tm tm;
    time_t epoch;


    strncpy(line_to_json,"[\n",LINEMAXLENGTH);
    fputs(line_to_json, fptr);
    int trajectory_saved_to_json = 0;
    for (int t = 0; t < tl->current_number_trajectory; t++) {
        trajectory_saved_to_json = 0;
        if (tl->trajectory_array[t]->current_number_position - 5 >= 0){
            trajectory_saved_to_json = 1;
            strncpy(line_to_json,"  {\n",LINEMAXLENGTH);
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);
            strncpy(line_to_json,"    \"vehicleID\":",LINEMAXLENGTH);
            memset(concatenation_tmp, 0, LINEMAXLENGTH);
            sprintf(concatenation_tmp, "%s", tl->trajectory_array[t]->vehicle_id);
            strcat(line_to_json, concatenation_tmp);
            memset(concatenation_tmp, 0, LINEMAXLENGTH);
            strcat(line_to_json, ",\n");
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);

            strncpy(line_to_json,"    \"path\": [\n",LINEMAXLENGTH);
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);

            for (int p = tl->trajectory_array[t]->current_number_position - 5; p < tl->trajectory_array[t]->current_number_position; p++) {
                memset(concatenation_tmp, 0, LINEMAXLENGTH);
                memset(line_to_json, 0, LINEMAXLENGTH);
                sprintf(concatenation_tmp, "          [%s",  tl->trajectory_array[t]->position_array[p]->latitude);
                strcat(line_to_json, concatenation_tmp);
                memset(concatenation_tmp, 0, LINEMAXLENGTH);
                strcat(line_to_json, ", ");

                sprintf(concatenation_tmp, "%s",  tl->trajectory_array[t]->position_array[p]->longitude);
                strcat(line_to_json, concatenation_tmp);

                if(p < tl->trajectory_array[t]->current_number_position-1){
                strcat(line_to_json, "],\n");
                }
                else{
                strcat(line_to_json, "]\n");
                }
                fputs(line_to_json, fptr);
                memset(line_to_json, 0, LINEMAXLENGTH);
            }
            memset(line_to_json, 0, LINEMAXLENGTH);
            strcat(line_to_json, "            ],\n");    
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);
            strncpy(line_to_json,"    \"timestamps\": [\n",LINEMAXLENGTH);
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);
            for (int p = tl->trajectory_array[t]->current_number_position - 5; p < tl->trajectory_array[t]->current_number_position; p++) {
                  
                if ( strptime(tl->trajectory_array[t]->position_array[p]->timestamp, "%Y-%m-%d %H:%M:%S", &tm) != NULL ){
                epoch = mktime(&tm);
                }

                sprintf(concatenation_tmp, "          %ld",  epoch);
                strcat(line_to_json, concatenation_tmp);
                memset(concatenation_tmp, 0, LINEMAXLENGTH);
                if(p < tl->trajectory_array[t]->current_number_position-1){
                strcat(line_to_json, ",\n");
                }
                fputs(line_to_json, fptr);
                memset(line_to_json, 0, LINEMAXLENGTH);
            }
            strcat(line_to_json, "\n            ]\n");
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);
            
        }
        if (trajectory_saved_to_json || t < tl->current_number_trajectory-1){
            if(t < tl->current_number_trajectory-1){
                strncpy(line_to_json,"  },\n",LINEMAXLENGTH);
            }
            else{
                strncpy(line_to_json,"  }\n",LINEMAXLENGTH);
            }
            fputs(line_to_json, fptr);
            memset(line_to_json, 0, LINEMAXLENGTH);
        }
    }
    memset(line_to_json, 0, LINEMAXLENGTH);
    strncpy(line_to_json,"]\n",LINEMAXLENGTH);
    fputs(line_to_json, fptr);
    memset(line_to_json, 0, LINEMAXLENGTH);

    fclose(fptr);

}


