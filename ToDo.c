#define SAVE "/root/Scheduler/savefile.txt"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

// Program to prioritise your tasks. Built by Shawn "Mr.Grey" Mathew on 21st and 22nd September, 2019. Dedicated to chaps like Joel V Zach. Feedback: shawnmathew@pm.me
// exit(0) for normal exit.
// exit(1) for file I/O errors.
// exit(2) for program I/O errors.

 void main()				// All the madness starts here.
{
 FILE *sf;				// Doesn't work if I define it at the file creation point. Might be because fiop calls it first.

 printf("1. Add tasks\n2. Delete tasks\n3. Modify tasks\n4. Todo-list\n9. History\n0. Exit\n");
 int size,i,maxlen,m;			// maxlen stores the string length of the job with the biggest name. Becomes important for formatting output.
 char buf[50],c[4],s[4];		// c[] & s[] are used for holding choices. Why is it 4? Because an integer is 4 bytes on 64-bit systems.
 long int count,jid,dcount,ucount;	// dcount for count of jobs done and ucount for jobs that aren't done.

 struct task				// All tasks will have this structure.
 {
  char name[100],pr[5],done;		// Perhaps I should add a timestamp to each record too.
  long int tid;
 }job;

 size=sizeof(job);

 void input()				// Function to handle all the inputs.
 {
  getc(stdin);				// To handle those pesky newline characters that are still in the STDIN stream after the past user's entry.
  printf("Name\t:");
  if(!fgets(job.name,100,stdin))	// scanf() stops reading after newline and gets() is a bad idea. Hence, fgets().
  {
   printf("Read error on fgets\n");
   fclose(sf);
   exit(2);
  }

  job.name[strlen(job.name)-1] = '\0';	//Overwrite the '\n' caught by fgets() with '\0' to prevent it from ruining my formatting.

  printf("Priority:");			// Should I add a feature that automatically increases priority as each job gets done?
  scanf("%s",&job.pr);
  job.tid = jid;
 }

 void fiop()				// Handles file operations - I don't have to type up these statements every time I need to access the file.
 {
  sf = fopen(SAVE,"r+b");		// Don't use append, since it makes it impossible to edit previous data.
  if(!sf)
  {
   perror("fopen");
   printf("Couldn't open the file.\n");  // You might wanna check permissions and/or available space (highly unlikely, since this will be take a few kBs at most).
   exit(1);
  }
 }

 int maxlenfinder(int p)			// Function to find the task with the largest name. P for purpose - for history or current to do list.
 {
  fiop();
  maxlen = 0;
  while(fread(&job,size,1,sf))
   if(strlen(job.name) > maxlen)		// Since the number of spaces required in history display is not necessary in the To-Do display.
    if(p==1)					// For example, suppose you had a task named "Finish your homework" and you finished it.
     maxlen = strlen(job.name);			// It will remain in history. Then, you have another task "Pray to God", which is a shorter string.
    else					// Without this 'p' variable, you'll get the space corresponding to the former task even if it doesn't exist
     if(!job.done)				// in the current To-Do list.
      maxlen = strlen(job.name);		// p is 0 for a normal To-Do list and 1 for History.

  fclose(sf);
  return maxlen;
 }

 void spacer(int len, int maxl)			// A function to format the output.
 {
  for(m=0;m<=(maxl-len);m++)
   putchar(' ');
  putchar('\t');
 }

 sf = fopen(SAVE,"a+b");			// To create a file if it doesn't exist already.
 jid = 0;					// In case there is no file, jid should start from a definite value, in this case, 0.
 fclose(sf);					// Create the file and then close.

 fiop();
 if(fseek(sf,-size,SEEK_END) == 0)		// To prevent conflicting Task IDs when the program is run intermittently.
 {
  fread(&job,size,1,sf);
  jid = job.tid+1;
 }
 fclose(sf);

 do
 { 						// (It's pronounced it-eh-rey-shun and not ai-teh-rey-shun).
  start:
  printf("\nYour choice >> ");			// Decide, user.
  scanf("%s",&c);

  if(!strcmp(c,"0"))				// You chose to exit.
  {
   printf("Have a good day.\n");		// You'll be back, soon.
//   fclose(sf);				// We don't need this since fclose(sf) is called after every action.
   exit(0);
  }

  else if(!strcmp(c,"1"))			// Add tasks.
  {
   input();
   job.done=0;					// Nobody adds jobs that are already complete, right?

   fiop();
   fseek(sf,0,SEEK_END);
   fwrite(&job,size,1,sf);
   fclose(sf);

   jid++;
  }

  else if(!strcmp(c,"2"))			// Mark tasks as complete.
  {
   printf("Enter the ID:");
   scanf("%s",&buf);

   i=0;
   fiop();
   fseek(sf,0,SEEK_SET);			// Start searching from the beginning.
   while(fread(&job,size,1,sf))
    if(job.tid==atol(buf))
    {

     if(job.done)				// You can't kill a dead person.
     {
      printf("What? That job has already been done!\n");
      break;
     }

     printf("You are manipulating the following data:\nID\t:%ld\nName\t:%s\nPriority:%s\n Are you sure (y or Y for yes)?\n",job.tid,job.name,job.pr);
     scanf("%s",&s);
     if(!(strcmp(s,"y")&&strcmp(s,"Y")))
     {
      job.done = 1;

      fseek(sf,-size,SEEK_CUR);			// About the -size - You don't want to rewrite the next record, do you?
      fwrite(&job,size,1,sf);
      fclose(sf);
      break;
     }
    }
    else i++;
  }

  else if(!strcmp(c,"3"))			// Modify jobs.
  {
   printf("Enter the ID:");
   scanf("%s",&buf);

   fiop();					// Why does this look like 'flop' from a distance?

   i = 0;
   fseek(sf,0,SEEK_SET);			// Start searching from the beginning.
   while(fread(&job,size,1,sf))
    if(job.tid == atol(buf))				// Let's see if strcmp works.
    {
     printf("You are manipulating the following data:\nID\t:%ld\nName\t:%s\nPriority:%s\nDone\t:%d\n Are you sure (y or Y for yes)?\n",job.tid,job.name,job.pr,job.done);
     scanf("%s",&s);
     if(!(strcmp(s,"y")&&strcmp(s,"Y")))	// Everybody gangsta until someone presses Ctrl+C.
     {
      input();
      job.tid=atol(buf);
      printf("Is the job done (y or Y for yes)?\n");
      scanf("%s",&s);
      if(!(strcmp(s,"y")&&strcmp(s,"Y")))
       job.done=1;
      else
       job.done=0;
      fseek(sf,-size,SEEK_CUR);			// Rewrite that particular record here, which is the location of the old version of that record.
      fwrite(&job,size,1,sf);
      fclose(sf);				// If a result is found, then the fclose() outside wouldn't be executed because of the break statement..
      goto start;				// No point in going through all the records if you already found what you need.
     }
     else
      goto start;
    }
    else
     i++;
   fclose(sf);
						// If the record was found, then this part of the code wouldn't be executed anyway.
    printf("Couldn't find that for you. Try again.\n");
  }

  else if(!strcmp(c,"4"))			// To print tasks that need to be fulfilled.
  {
   maxlen = maxlenfinder(0);
   fiop();
   fseek(sf,0,SEEK_SET);			// Start from the beginning.
   count = 0;					// To get a summary.
   while(fread(&job,size,1,sf))
    if(!job.done)
    {
     count++;
     printf("%ld\t%s",job.tid,job.name);
     spacer(strlen(job.name),maxlen);
     printf("%s\n",job.pr);
    }
   printf("\nCount: %ld\n\n",count);
   fclose(sf);
  }

  else if(!strcmp(c,"9"))			// Prints the history of all the jobs you've done (and the ones you haven't too).
  {
   maxlen = maxlenfinder(1);
   fiop();
   fseek(sf,0,SEEK_SET);			// Go to the beginning of the file. Otherwise, you won't see some results.
   count=ucount=dcount=0;
   putchar('\n');				// For neatness.
   while(fread(&job,size,1,sf))
   {
    printf("%ld\t%s",job.tid,job.name);
    spacer(strlen(job.name),maxlen);		// Put an adequate number of spaces between the job name and the next field, which is priority.
    printf("%s",job.pr);
    if(job.done)				// If the job is done, then print 'done' in the next field.
    {
     printf("\tdone\n");
     dcount++;					// Another job down for count.
    }
    else					// Otherwise, print 'not done'.
    {
     printf("\tundone\n");
     ucount++;					// Try to keep this minimal, in real life that is.
    }
    count++;					// Don't know how useful this is. It gives you the count of the jobs.
   }
   printf("\nCompleted jobs	: %ld\n",dcount);
   printf("Incomplete jobs	: %ld",ucount);
   printf("\nTotal count	: %ld\n",count);
   fclose(sf);
  }

  else
   printf("1. Add tasks\n2. Delete tasks\n3. Modify tasks\n4. Todo-list\n0. Exit\n");
 }
 while(1);					// Last line seems quite important, doesn't it?
}
						// The end.
