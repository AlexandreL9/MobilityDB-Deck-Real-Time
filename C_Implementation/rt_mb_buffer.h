#ifndef HEADERFILE_H
#define HEADERFILE_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // for test sleep()

typedef struct
{
   char* timestamp;
   char* latitude;
   char* longitude;
} Position;

typedef struct
{
   char* vehicle_id;
   int current_number_position;
   int max_number_position;
   Position** position_array;
   clock_t last_db_insert_time;
} Trajectory;

typedef struct {
  Trajectory **trajectory_array;
  size_t current_number_trajectory;
  size_t max_number_trajectory;
} Trajectory_list;


void init_position(Position *p, char* timestamp, char* latitude, char* longitude);

void init_trajectory(Trajectory *t, char* vehicle_id, int max_buffer_position);

void insert_position(Trajectory *t, Position* new_position);

void free_trajectory_position_array(Trajectory *t);

void free_trajectory(Trajectory *t);

void init_trajectory_list(Trajectory_list *tl, size_t default_trajectory_number);

int search_no_trajectory_index(Trajectory_list *trajl);

void insert_trajectory_list(Trajectory_list *tl, Trajectory* new_trajectory);

void free_trajectory_list(Trajectory_list *trajl);

void insert_trajectory_mobilityDB(Trajectory *t);

void parse_one_kafka_position(char* input_position, char* vehicleUniqueID, char* latitude, char* longitude, char* timestamp, const int vehicleUniqueIDLENGTH, const int latitudeLENGTH, const int longitudeLENGTH, const int timestampLENGTH);

int check_position_is_valid(char* vehicleUniqueID, char* latitude, char* longitude, char* timestamp);

int trajectory_already_exist(Trajectory_list* tl, char* vehicleUniqueID);

Trajectory* get_or_create_trajectory(Trajectory_list* tl, char* vehicleUniqueID, const int maxBufferPosition);

void check_trajectories_to_db(Trajectory_list* tl, const int dbInsertionInterval, const int VNumber);

void handle_new_position(Trajectory_list* tl, char* input_position, const int dbInsertionInterval, const int VNumber, const int maxBufferPosition);

#endif
