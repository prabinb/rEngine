#include <stdio.h>
#include <stdlib.h>
#include "bitvector.h"

#define COMMAND_HISTORY_FILE "cmd_hist.file"
#define MAX_USERS 100
#define MAX_COMMANDS 1000

#define STORE_IN_BUFFER(bit, buffer, name) \
	bit=-1; int i=0; \
	for (i=0;i<sizeof(buffer)/sizeof(char*);i++){\
		if((buffer[i] != NULL) && (strcmp(buffer[i], name)==0)){\
			bit = i; break;\
		}\
		else if(buffer[i] == NULL){\
			buffer[i] = (char*) malloc((sizeof(char)*(strlen(name)))+1);\
			strcpy(buffer[i], name);\
			bit = i; break;\
		}\
	}\
	if (i==sizeof(buffer)/sizeof(char*)){\
		printf("limit reached for storing usernames,\n");\
		exit(1);\
	}
/*
#define CREATE_CONTEXT(context, users) \
	context = (daycontext*) malloc(sizeof(daycontext));\
	bv_init(&context->user_bv, MAX_USERS);\
	int k, uindex;\
	context->cmd_bv=(struct bitvect*)malloc(sizeof(struct bitvect)*users);\
	for(k=0;k<users;k++){\
		bv_init(&context->cmd_bv[k], MAX_COMMANDS);\
		printf("Enter the user name:\n");\
		scanf("%s", user);\
		STORE_IN_BUFFER(uindex, uname_buf, user);\
		printf("index returned is %d\n", uindex);\
		bv_set(&context->user_bv, uindex);\
	}\
	context->next=NULL;\
	context->num_users=users;

#define FETCH_COMMANDS(context) \
	int j, cindex;\
	char cmd_text[100];\
	cindex=-1;\
	for (j=0;j<context->num_users;j++){\
		printf("Enter the commands for user %d or enter 'quit' for next user.\n", (j+1));\
		fgets(cmd_text, 100, stdin);\
		while(strncmp(cmd_text, "quit", 4) != 0){\
			STORE_IN_BUFFER(cindex, cmd_buf, cmd_text);\
			printf("index returned for command is %d\n", cindex);\
			bv_set(&context->cmd_bv[j], cindex);\
			fgets(cmd_text, 100, stdin);\
		}\
	}
*/

char *uname_buf[MAX_USERS];
char *cmd_buf[MAX_COMMANDS];

typedef struct _ucontext{
	int uindex;
	int pindex;
}ucontext;

// Maintained in memory, mapping between user and user commands
typedef struct _udatatable{
	int uindex;
	char *offset;
}udatatable;

typedef struct _daycontext{
	struct bitvect user_bv;
	struct bitvect *cmd_bv;
	struct _daycontext *next;
	int num_users;
}daycontext;
void create_context(daycontext* context, int users);
void fetch_commands(daycontext* context);

void main(int argc, char* argv[]){
	int index = -1;
	memset(uname_buf,0,MAX_USERS);
	memset(cmd_buf,0,MAX_COMMANDS);
	char user[50];
	int num_users=0;
	daycontext* program_context=NULL;
	printf("enter the number of users for day1.\n");
	scanf("%d", &num_users);
	program_context = (daycontext*) malloc(sizeof(daycontext));
	create_context(program_context, num_users);
	fetch_commands(program_context);
}

void create_context(daycontext* context, int users){
	bv_init(&context->user_bv, MAX_USERS);
	int k, uindex;
	char user[100];
	context->cmd_bv=(struct bitvect*)malloc(sizeof(struct bitvect)*users);
	for(k=0;k<users;k++){
		bv_init(&context->cmd_bv[k], MAX_COMMANDS);
		printf("Enter the user name:\n");
		scanf("%s", user);
		STORE_IN_BUFFER(uindex, uname_buf, user);
		printf("index returned is %d\n", uindex);
		bv_set(&context->user_bv, uindex);
	}
	context->next=NULL;
	context->num_users=users;
	return;
}

void fetch_commands(daycontext* context){
	int j, cindex;
	char cmd_text[100];
	cindex=-1;
	for (j=0;j<context->num_users;j++){
		printf("Enter the commands for user %d or enter 'quit' for next user.\n", (j+1));
		fgets(cmd_text, 100, stdin);
		while(strncmp(cmd_text, "quit", 4) != 0){
			STORE_IN_BUFFER(cindex, cmd_buf, cmd_text);
			printf("index returned for command is %d\n", cindex);
			bv_set(&context->cmd_bv[j], cindex);
			fgets(cmd_text, 100, stdin);
		}
	}
	return;
}

