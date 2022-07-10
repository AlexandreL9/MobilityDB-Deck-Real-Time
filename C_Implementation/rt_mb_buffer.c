#include "rt_mb_buffer.h"


void init_position(Position *p, char* timestamp, char* latitude, char* longitude) {
  p->timestamp = malloc( strlen(timestamp) + 1);
  strncpy(p->timestamp, timestamp, strlen(timestamp)+1);
  p->latitude = malloc( strlen(latitude) + 1);
  strncpy(p->latitude, latitude, strlen(latitude)+1);
  p->longitude = malloc( strlen(longitude) + 1);
  strncpy(p->longitude, longitude, strlen(longitude)+1);
}

void init_trajectory(Trajectory *t, char* vehicle_id, int maxBufferPosition) {
  t->vehicle_id = malloc( strlen(vehicle_id) + 1);
  strncpy(t->vehicle_id, vehicle_id, strlen(vehicle_id)+1);
  t->current_number_position = 0;
  t->max_number_position = maxBufferPosition;
  t->position_array = (Position**) malloc((maxBufferPosition+maxBufferPosition/4) * sizeof(Position*)); // buffer + 1/4 buffer before realloc

  t->last_db_insert_time = clock();
}

void insert_position(Trajectory *t, Position* new_position) {
  // Not really need to realloc number of positions since we have MAX_BUFFER_POSITION/t->max_number_position
  if (t->current_number_position == t->max_number_position) {
    t->max_number_position *= 2;
    t->position_array = (Position**) realloc(t->position_array, t->max_number_position * sizeof(Position*));
  }
  t->position_array[t->current_number_position] = new_position;
  t->current_number_position += 1;
}

void free_trajectory_position_array(Trajectory *t){
  int number_position_to_free = t->current_number_position;
  for (int i = 0; i < number_position_to_free; i++) { 
    free(t->position_array[i]->timestamp);
    free(t->position_array[i]->latitude);
    free(t->position_array[i]->longitude);
    free(t->position_array[i]);
    t->current_number_position -= 1;
  }
}

void free_trajectory(Trajectory *t) { 
  free_trajectory_position_array(t);
  free(t->vehicle_id);

  free(t->position_array);
}

void init_trajectory_list(Trajectory_list *tl, size_t default_trajectory_number) {
  tl->trajectory_array = (Trajectory**)malloc(default_trajectory_number * sizeof(Trajectory *));
  tl->current_number_trajectory = 0;
  tl->max_number_trajectory = default_trajectory_number;
}

int search_no_trajectory_index(Trajectory_list *trajl){
  // search for a null index in trajectory_list of a deleted trajectory
  /*
  for (size_t i = 0; i < trajl->current_number_trajectory; i++) { // looking for NULL position in array
    if(tl->trajectory_array[i]==NULL){
      return i; // TODO
    }
  }
  */
  return -1;
}

void insert_trajectory_list(Trajectory_list *tl, Trajectory* new_trajectory) {

  if(search_no_trajectory_index(tl) != -1){ // search for deleted trajectories
    tl->trajectory_array[tl->current_number_trajectory] = new_trajectory;
  }
  else{
    if (tl->current_number_trajectory == tl->max_number_trajectory) {

      tl->max_number_trajectory *= 2;
      tl->trajectory_array = (Trajectory**)realloc(tl->trajectory_array, tl->max_number_trajectory * sizeof(Trajectory*));
    }
    tl->trajectory_array[tl->current_number_trajectory] = new_trajectory;
  }
    tl->current_number_trajectory +=1 ;
}

void free_trajectory_list(Trajectory_list *trajl) {
  for (size_t i = 0; i < trajl->current_number_trajectory; i++) { 
  	free_trajectory(trajl->trajectory_array[i]); // free one trajectory struct contents
  	free(trajl->trajectory_array[i]); // one trajectory struct
  }
}

void insert_trajectory_mobilityDB(Trajectory *t) {
  // empty list of posititons
  free_trajectory_position_array(t);
  //printf("INSERT DB - ellapsed time : %Lf \n", ((long double)clock()-( t->last_db_insert_time))/ CLOCKS_PER_SEC);
  t->last_db_insert_time = clock();
}

void parse_one_kafka_position(char* input_position, char* vehicleUniqueID, char* latitude, char* longitude, char* timestamp, const int vehicleUniqueIDLENGTH, const int latitudeLENGTH, const int longitudeLENGTH, const int timestampLENGTH){
  // parse the values of input_position coming from kafka to VEHICLEUNIQUEID
  // BUSUNIQUEID,LATITUDE,LONGITUDE,TIMESTAMP
  // e.g. "119,-74.00926,40.71325,2022-04-19 02:00:08"

  char * token = strtok(input_position, ",");

  memset(vehicleUniqueID, 0, vehicleUniqueIDLENGTH);
  memset(latitude, 0, latitudeLENGTH);
  memset(longitude, 0, longitudeLENGTH);
  memset(timestamp, 0, timestampLENGTH);
  
  if (token != NULL && strlen(token)<vehicleUniqueIDLENGTH && strlen(token)>0 ) {
      strncpy(vehicleUniqueID, token, strlen(token)+1); // "protection" buffer overflow
      vehicleUniqueID[strlen(token)+1] = '\0';
  }

  token = strtok(NULL, ",");
  if (token != NULL && strlen(token)<(latitudeLENGTH) && strlen(token)>0 ) {
      strncpy(latitude, token, strlen(token)+1); 
      latitude[strlen(token)+1] = '\0';
  }

  token = strtok(NULL, ",");

  if (token != NULL && strlen(token)<(longitudeLENGTH)  && strlen(token)>0 ) {
      strncpy(longitude, token, strlen(token)+1); 
      longitude[strlen(token)+1] = '\0';
  }

  token = strtok(NULL, ",");

  if (token != NULL && strlen(token)<(timestampLENGTH)  && strlen(token)>0 ) {
      strncpy(timestamp, token, strlen(token)+1); 
      timestamp[strlen(token)+1] = '\0';
  }

}

int check_position_is_valid(char* vehicleUniqueID, char* latitude, char* longitude, char* timestamp){
  // more verification can be added later
  if (strlen(vehicleUniqueID) && strlen(latitude) && strlen(longitude) && strlen(timestamp)){
    return 1;
  }
  return 0;
}

int trajectory_already_exist(Trajectory_list* tl, char* vehicleUniqueID){
  // -1 if not exist else return vehicle trajectory index
  int trajectory_in_array_index = -1;
  for (size_t i = 0; i < tl->current_number_trajectory; i++) {
    // if trajectory already exist
    if(strcmp(tl->trajectory_array[i]->vehicle_id, vehicleUniqueID) == 0){
      trajectory_in_array_index = i;
    }
  }
  return trajectory_in_array_index;
}

Trajectory* get_or_create_trajectory(Trajectory_list* tl, char* vehicleUniqueID, const int maxBufferPosition){
  Trajectory* t;

  int trajectory_in_array_index = trajectory_already_exist(tl, vehicleUniqueID);
  // if trajectory exist
  if(trajectory_in_array_index != -1){ 
    //printf("Trajectory exist\n");
    t = tl->trajectory_array[trajectory_in_array_index];
  }
  // create new trajectory
  else{
    printf("New trajectory\n");
    t = (Trajectory*)malloc(sizeof(Trajectory));
    init_trajectory(t, vehicleUniqueID, maxBufferPosition);
    insert_trajectory_list(tl, t);
  }
  return t;
}

void check_trajectories_to_db(Trajectory_list* tl, const int dbInsertionInterval, const int VNumber){
  // check all trajectories if insert it into db
  for (size_t i = 0; i < tl->current_number_trajectory; i++) {
    if (tl->trajectory_array[i]->current_number_position > tl->trajectory_array[i]->max_number_position ||
        (tl->trajectory_array[i]->current_number_position > 0 &&
        ((long double) clock() - tl->trajectory_array[i]->last_db_insert_time)/ CLOCKS_PER_SEC > dbInsertionInterval)
  ) {
      printf("INSERT DB - Trajectories of %d - ellapsed time : %Lf \n", VNumber, ((long double)clock()-(tl->trajectory_array[i]->last_db_insert_time))/ CLOCKS_PER_SEC);

      insert_trajectory_mobilityDB(tl->trajectory_array[i]);
    }
    
    /*
    // if signal lost/emission stopped then delete trajectory
    if (tl->trajectory_array[i]->current_number_position == 0  && 
      ((long double)(clock() - tl->trajectory_array[i]->last_db_insert_time)/ CLOCKS_PER_SEC) > TRAJECTORY_DELETION_DELAY) { // 15 min last insert
      printf("----LOST -- %Lf \n", ((long double)(clock() - tl->trajectory_array[i]->last_db_insert_time)/ CLOCKS_PER_SEC));
      free_trajectory(t); // list of positions
      free(tl->trajectory_array[i]);
    }
    */    
    
  }
}

/*
void generate_one_position(char* input_position, int vehicleId, const int VNumber){
  // for testing : generate vehicle id trajectory
  // copy new generated position into input_position
  char tmp[86];
  char tmpTraj[16];
  memset(tmp, 0, sizeof tmp);
  memset(tmpTraj, 0, sizeof tmpTraj);
  sprintf(tmpTraj, "%d", vehicleId);
  strcat(tmp,tmpTraj);
  strcat(tmp,",-36.65004933915111,0.2862493716668695,2021-10-14 15:36:40.699041");
  strcpy(input_position, tmp);
  //printf("%s\n",input_position);
}
*/

void handle_new_position(Trajectory_list* tl, char* input_position, const int dbInsertionInterval, const int VNumber, const int maxBufferPosition){
  // should respect standard geo spatial data length
  const int vehicleUniqueIDLENGTH = 16;
  char VEHICLEUNIQUEID[vehicleUniqueIDLENGTH];
  const int latitudeLENGTH = 20;
  char LATITUDE[latitudeLENGTH];
  const int longitudeLENGTH = 20;
  char LONGITUDE[longitudeLENGTH];
  const int timestampLENGTH = 30;
  char TIMESTAMP[timestampLENGTH];

  parse_one_kafka_position(input_position, VEHICLEUNIQUEID, LATITUDE, LONGITUDE, TIMESTAMP, vehicleUniqueIDLENGTH, latitudeLENGTH, longitudeLENGTH, timestampLENGTH);
  
  if(check_position_is_valid(VEHICLEUNIQUEID, LATITUDE, LONGITUDE, TIMESTAMP)){
    Trajectory* t =  get_or_create_trajectory(tl, VEHICLEUNIQUEID, maxBufferPosition);
    // new position
    Position* new_p;
    new_p = (Position*)malloc(sizeof(Position));
    init_position(new_p, TIMESTAMP, LATITUDE, LONGITUDE);
    insert_position(t, new_p);
    check_trajectories_to_db(tl, dbInsertionInterval, VNumber);
    //sleep(0.9);
  }
  else{
    printf("INVALID POSITION\n POSITION FORMAT:\n VEHICLEUNIQUEID,LATITUDE,LONGITUDE,TIMESTAMP\n e.g:119,-74.00926,40.71325,2022-04-19 02:00:08\n");
  
  }
}
