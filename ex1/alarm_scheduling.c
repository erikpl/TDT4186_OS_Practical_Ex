void setAlarm(int* uyear, int* umonth, int* uday, int* uhour, int* uminute, int *usecond) {
   int child_pid, parent_pid;
   // Calling getpid before forking will return the pid of the parent
   parent_pid = getpid();
   // Gets child pid from fork
   child_pid = fork();
   // From here, both processes run the same code

   if (getpid() == child_pid) {
      time_t t;
      time(&t);
      double diff_t;
      diff_t = difftime(time_repr, t);

      //Waits the amount of seconds and then prints the alarm
      sleep(diff_t);
      printf("\n ALARM! \n");
      
      //Terminates child process
      exit(child_pid); 
   }
   else {
      //Initialize and create a tm_struct
      struct tm tm_struct;
      
//       tm_struct = CreateNewAlarm(&pid, &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

     //Initialize time_t representation
      time_t time_repr;

      //Call function initialize with tm_struct as argument
      time_repr = initialize(tm_struct);
      // SetAlarm calls itself with time_repr as argument
      setAlarm(time_repr);
   }

      // SetAlarm calls itself with time_repr as argument
      setAlarm(time_repr);
}
}
