#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bitvector.h"
#include "trie.h"

#define MAX_USERS 100
#define MAX_COMMANDS 1000

#define STORE_IN_TRIE(index, trie, name)\
	index =-1;\
	index=insert_t(trie, name);\
	if (index==-1){\
		index = search_t(trie, name);\
	}

//universal tries
tnode *cmd_trie;
tnode *uname_trie;

typedef struct _ucontext{
	int u_index;
	int p_index;
}ucontext;

typedef struct _daycontext{
	struct bitvect user_bv;
	struct bitvect *cmd_bv;
	struct _daycontext *next;
	struct tm *date;
	int u_num;
	int *u_indexes;
}daycontext;

void create_context(daycontext* context, int users);
void fetch_commands(daycontext* context);

void main(int argc, char* argv[]){
	int index = -1;
	INITIALIZE_NODE(cmd_trie, '\0');
	INITIALIZE_NODE(uname_trie, '\0');
	int total_user_for_day=0;
	daycontext* program_context=NULL;
	printf("enter the number of distinct users for day1.\n");
	scanf("%d", &total_user_for_day);
	program_context = (daycontext*) malloc(sizeof(daycontext));
	create_context(program_context, total_user_for_day);
	fetch_commands(program_context);

}

void create_context(daycontext* context, int users){
	bv_init(&context->user_bv, MAX_USERS);
	int k, uindex;
	char user[100];
	context->cmd_bv=(struct bitvect*)malloc(sizeof(struct bitvect)*users);
	context->u_indexes=(int*) malloc(sizeof(int)*users);
	for(k=0;k<users;k++){
		bv_init(&context->cmd_bv[k], MAX_COMMANDS);
		printf("Enter the user name:\n");
		scanf("%s", user);
		STORE_IN_TRIE(uindex, uname_trie, user);
		printf("index returned is %d\n", uindex);
		if (bv_isset(&context->user_bv, uindex) == 0){
			bv_set(&context->user_bv, uindex);
			//store it in the array of user indexes, will be used during retrieval
			context->u_indexes[k]=uindex;
		}
		else {
			printf("Same user entered. please re-enter\n");
			k--;
		}
	}
	time_t now;
	now = time(NULL);
	context->date= localtime(&now);
	char date[9]={0};
	sprintf(date, "%02d%02d%d", context->date->tm_mday,context->date->tm_mon,(context->date->tm_year+1900));
	date[8]='\0';
	printf("Date is %s\n", date);
	context->next=NULL;
	context->u_num=users;
	return;
}

void fetch_commands(daycontext* context){
	int j, cindex;
	char cmd_text[100];
	cindex=-1;
	for (j=0;j<context->u_num;j++){
		printf("Enter the commands for user %d or enter 'quit' for next user.\n", (j+1));
		__fpurge(stdin);
		fgets(cmd_text, 100, stdin);
		while(strncmp(cmd_text, "quit", 4) != 0){
			STORE_IN_TRIE(cindex, cmd_trie, cmd_text);
			//cindex = store_in_buffer(cmd_buf, cmd_text, MAX_COMMANDS);
			printf("index returned for command is %d\n", cindex);
			bv_set(&context->cmd_bv[j], cindex);
			fgets(cmd_text, 100, stdin);
		}
	}
	return;
}
