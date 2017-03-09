//Posterity code...


// ********* OLD HISTORY ! FUNCTIONS *******************
/*
  strcpy(temp_cmd,history_array[history_index-1].cmd);
  if(arguments[1] != NULL){
    strcpy(temp_arg,arguments[1]);
  }
  strcat(temp_cmd,temp_arg);
  arguments = parse_input(temp_cmd);
*/


/*
  strcpy(temp_arg,arguments[2]);
  strcat(temp_cmd," ");
  strcat(temp_cmd,temp_arg);
  arguments = parse_input(temp_cmd);
  return arguments;
*/
// *************************************************


/*
if((strcmp(arguments[0],"!") == 0)){
  a = char_to_int(arguments[1]);
  if((a <= 0) || (a > 20)){
    arguments[0] = NULL;
    fprintf(stderr, "ERROR: Not valid history function. Command IDs start a numeral 1.\n");
    return arguments;
  }

  for(i = 0; i < MAX_HISTORY_SIZE; i ++){
    if(history_array[i].command_id == a){
      strcpy(temp_cmd,history_array[i].cmd);
      if(arguments[2] != NULL){
        j = 2;
        do{
          if(arguments[j] != NULL){
            strcpy(temp_arg,arguments[j]);
          }
          strcat(temp_cmd," ");
          strcat(temp_cmd,temp_arg);
          j++;
        }while(arguments[j] != NULL);
        arguments = parse_input(temp_cmd);
        return arguments;
      }else{
        arguments = parse_input(temp_cmd);
        return arguments;
      }
    }
  }
}

*/
