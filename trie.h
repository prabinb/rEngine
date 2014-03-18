#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/mman.h>

#define boolean unsigned short int

#define INITIALIZE_NODE(n, character) \
	n=(tnode*)malloc(sizeof(tnode)); \
	n->right=n->child=NULL; \
	n->value=character;\
	n->index=0;

#define INITIALIZE_DATA(input, output, len) \
	assert(input!=NULL);\
	len=strlen(input); \
	output=(char*)malloc(sizeof(char)*(len+1));\
	strncpy(output, input, len);\
	if (output[len-1] == '\n'){\
		output[len-1]='\0';\
	}\
	else {\
		output[len]='\0';\
		len=len+1;\
	}

typedef struct _tnode{
	char value;
	struct _tnode* right;
	struct _tnode* child;
	unsigned short int index;
}tnode;

int data_file_size=0;
int max_buffer_size=100000;

void serialize_t(tnode* root, void** data);
void write_to_file(FILE *data_file, tnode *root){
	void * data;
	data = (char*)malloc(sizeof(char)*max_buffer_size);
	//void* save_data_pointer = data;
	serialize_t(root, &data);
	fwrite(data-data_file_size, data_file_size, 1, data_file);
}

//mmap(NULL, length, PROT_READ || PROT_WRITE, MAP_PRIVATE, fd, offset);
void write_t(void *data, int data_size, void** buffer){
	if (data_file_size+data_size>max_buffer_size){
		void *temp = (char*)malloc((sizeof(char))*2*(max_buffer_size));
		*buffer=(*buffer)-data_file_size;
		memcpy(temp, *buffer, data_file_size);
		free(*buffer);
		*buffer=temp+data_file_size;
		max_buffer_size=2*(max_buffer_size);
	}
	strncpy(*buffer, data,  data_size);
	*buffer=*buffer+data_size;
	data_file_size+=data_size;
}

//set first byte if value is present,
// if present set second byte if right node is present
void serialize_t(tnode *root, void **data){
	int value,rvalue, cvalue;
	value=rvalue=cvalue=0;
	if(root==NULL){
		write_t((void*)&value, sizeof(value), data);
		return;
	}
	else{
		value=1;
		write_t((void*)&value, sizeof(value), data);
		write_t((void*)&(root->value), sizeof(root->value), data);
		write_t((void*)&(root->index), sizeof(root->index), data);
		if(root->right){
			rvalue=1;
			write_t((void*)&rvalue, sizeof(rvalue), data);
			serialize_t(root->right, data);
		}
		else {
			rvalue=0;
			write_t((void*)&rvalue, sizeof(rvalue), data);
		}
		
		if(root->child){
			cvalue=1;
			write_t((void*)&cvalue, sizeof(cvalue), data);
			serialize_t(root->child, data);
		}
		else {
			cvalue=0;
			write_t((void*)&cvalue, sizeof(cvalue), data);
		}
		
	}
}

void deserialize_t(tnode *root, void **data);
void read_from_file(FILE *data_file, tnode **root){
	void *data;
	data=(char*)malloc(sizeof(char)*data_file_size);
	assert(data_file!=NULL);
	*root=(tnode*)malloc(sizeof(tnode));
	(*root)->child=(*root)->right=NULL;
	fread(data, data_file_size, 1, data_file);
	if(ferror(data_file)){
		perror("Error while reading from the file");
	}
	deserialize_t(*root, &data);
}
void read_t(void* data, int data_size, void** buffer){
	strncpy(data, *buffer, data_size);
	*buffer=*buffer+data_size;
}
void deserialize_t(tnode *root, void **data){
	assert(root!=NULL);
	int value, rvalue, cvalue;
	char char_data;
	value=rvalue=cvalue=0;
	unsigned short int indexvalue=0;
	read_t((void*)&value, sizeof(value), data);
	if(value==0){
		return; //nothing to read more
	}
	else{
		read_t((void*)&char_data, sizeof(char_data), data);
		root->value=char_data;
		read_t((void*)&indexvalue, sizeof(indexvalue), data);
		root->index=indexvalue;
		read_t((void*)&rvalue, sizeof(rvalue), data);
		if(rvalue==1){
			tnode* right_node=(tnode*)malloc(sizeof(tnode));
			right_node->child=right_node->right=NULL;
			root->right=right_node;
			deserialize_t(root->right, data);
		}
		read_t((void*)&cvalue, sizeof(cvalue), data);
		if (cvalue==1){
			tnode* child_node=(tnode*)malloc(sizeof(tnode));
			child_node->child=child_node->right=NULL;
			root->child=child_node;
			deserialize_t(root->child, data);
		}

	}
}
//return the index if the data is found else return -1.
int insert_t(tnode *root, char* input){
	boolean right=0, found=0;
	assert(strcmp(input, "")!=0);
	char *data;
	int data_len=0;
	INITIALIZE_DATA(input, data, data_len); 
	if (root == NULL){
		INITIALIZE_NODE(root, '\0');//let's say root value is always this
		root->index = 0;// reinitilaized to zero, let this contain the trie word count
	}
	tnode* curr_node=root->child;
	tnode* prev_node = root;
	int i=0;
	while(i<data_len){
		if(curr_node == NULL){
			INITIALIZE_NODE(curr_node, data[i]);
			if(right == 1){
				prev_node->right = curr_node;
				right = 0;
			}
			else {
				prev_node->child = curr_node;
			}
			if (i == data_len-1){
				curr_node->index=++(root->index);
			}
			prev_node = curr_node;
			curr_node=curr_node->child;
			i++;
			found = 0;
		}
		else if(curr_node->value == data[i]){
			prev_node=curr_node;
			curr_node=curr_node->child;
			i++;
			found = 1;
		}
		else{
			prev_node=curr_node;
			curr_node = curr_node->right;
			right = 1;
			
		}
	}
	if (found == 1){
		return prev_node->index;
	}
	return -1;
}

//return the index if the data is found else return -1.
int search_t(tnode *root, char* input){
	assert(strcmp(input, "")!=0);
	if((root == NULL) || (root->child == NULL)){
		return -1;
	}
	int data_len=0;
	char *data;
	INITIALIZE_DATA(input, data, data_len);
	int i=0;
	tnode* curr_node = root->child;
	while(i<data_len){
		if(curr_node->value == data[i]){
			if(curr_node->value == '\0'){
				return curr_node->index;
			}
			curr_node=curr_node->child;
			i++;
		}
		else if(curr_node->right != NULL){
			curr_node=curr_node->right;
		}
		else{
			// not found
			return -1;
		}
		
	}
	return -1;
}
