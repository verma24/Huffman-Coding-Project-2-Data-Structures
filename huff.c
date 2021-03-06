#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huff.h"


//just my way of doing things
#define TRUE 1
#define FALSE -1
#define ENCODE 2
#define YES 1
#define NO 1

//Running everything and saving it correctly.
int main(int argc, char **argv)
{
  //check for when arguement is not given
  if(argc < 2)
    {
      printf("Error, no input given.\n");
      return(0);
    }

  //used 127 instead off 255
  int number = 127;
  int array_freq[number];
  Node * sortedlist;
  char * filename;
  filename = argv[1];
  int size_filename =0;
  size_filename = strlen(filename);
  int rows, cols;   
  ListNode * listtree;
  TreeNode * root;
  // int z;
  unsigned int numChar = 0;
  int array_test[127];
  int inx;

  //get the frequency in an array
  get_freq(filename, array_freq);

  //this will add .huff infront of the output file
  char * outfile = (char *)malloc((size_filename + 5) * sizeof(char));
  strcpy(outfile, filename);
  outfile = strcat(outfile, ".huff");
  
  //buildlist function will return linked list according to their frequency
  sortedlist = build_list(array_freq);
  //simple error check 
  if (sortedlist == NULL)//error check for build list here
    {
      printf("ERROR build list didnot get the right values it");
      return(FALSE);
    }
  
  //we now have the sorted linked list here according to their freq
  listtree = build_ListNode(sortedlist);

  //while loop merges the sorted linked list to create baby trees with similar frequency
    while(listtree -> next != NULL)
    {
      ListNode * two;
      ListNode * three;
      TreeNode * tn2;
      TreeNode * tn1;

      two = listtree -> next;
      three = two -> next;
      tn1 = listtree -> tree_ptr;
      tn2 = two -> tree_ptr;
      
      //free for memory leaks
      free(listtree);
      //memory leak error here
      free(two);
      
      listtree = three;

      //this function takes the treenode 1 and tree node 2 , returns a merged tree
      TreeNode * tmerged = merge_tree(tn1, tn2); 

      //this creates the new list node to the two adjoined treeesss
      ListNode * merged = create_listnode(tmerged);
      
      listtree = insert_listnode(listtree, merged);
    }
  
    
    //free sorted list, to avoid memory errors
    destroy_list(sortedlist);
  
    //the new root with the doubled the values of freq of the two chilren
    root  = listtree -> tree_ptr;
 
    free(listtree);
    
    // now implement a 3d array where row = leafs and cols = height
    rows = leaf_tree(root);
    
    cols = height_Tree(root);
    // printf("row is %d\n\n", rows);
    // printf("cols is %d\n\n",cols);
    int i = 0;
    int j = 0;

    cols++;
    
    int ** array3d = malloc(sizeof(int*) * rows);

    if(array3d == NULL)
      {
	printf("ERROR in malloc of array3d");
      }    

    //needed to just initialize the 3d array;
    for(i = 0; i < rows; i++)
      {
	array3d[i] = malloc(sizeof(int) * cols);	
	for(j = 0; j < cols; j++)
	  {
	    array3d[i][j] = -1;
	  }
      }
    
    // printf("The value of root is %d \n ", root->character);
    
    //this creats the final tree
    build_array3d(root, array3d);
    //this just prints the output bits code for characters
    // print_array3d(array3d, rows);
    
    int inx2 = 0;//index two

    //this jsust stores the total # of chars in char num;
    unsigned int char_num = get_charnum(numChar, array_freq);
       
    //used for copmp[ression
    for(inx = 0; inx < 127; inx++)
      {
	array_test[inx] = -1;
	
	for(inx2 = 0; inx2 < rows; inx2 ++)
	  {
	    if(array3d[inx2][0] == inx)
	      {
		array_test[inx] = inx2;
	      }
	  }
      }
    
    //this send the number of chars
    header_tree(root, char_num, outfile);
    compression(filename, outfile, array3d, array_test);

    return(0);
}


unsigned int get_charnum(unsigned int numChar, int * freqarray)
{

int  i = 0;
     
    for(i = 0; i < 127; i++)
      {
	numChar += freqarray[i];
      }
   
    return numChar;
 
}


//Function to count frequencies of letters
void get_freq(char *Filename, int * frequency)
{
  int car = 0;//we will type cast the char values to get ascii
  int number = 127;
  FILE * fp = fopen(Filename, "r"); 
  
  //initialize each value to zero
  for(car = 0; car < number;car++) 
    {
      frequency[car] = 0;
    }
  
  while(1)//while true till EOF 
    {
      car = fgetc(fp);//get each character
      
      if(car == EOF)
	{
	  break;
	}
      frequency[car]++; //will increase as frequency increases
    }

  fclose(fp); 
}

//Function to build list from frequencies
Node * build_list(int * freq)
{
  int i = 0;
  int j = 0;
  int number = 127;
Node * listnode;

  //we do this to skip all the character which donot occur in the file
  while(freq[i] == 0)
    {
      i++;
    }

  //creat a list from the node
  listnode = List_create(i, freq[i]);

  i++;
  j = i;

  while(j < number)
    {
      if (freq[j] > 0)
	{
	  listnode = ascend_sort(listnode, j, freq[j]);//get the sorted array
	}
      j++;
    }

  return listnode;
}



//This function biulds list nodes from the unsorted array with the frequencies;
ListNode * build_ListNode(Node * freqList)
{
  ListNode * top = NULL;
  TreeNode * treenode = NULL;
  ListNode * newln = NULL;//new list node
  
  while(freqList != NULL)
    {
      treenode = create_TreeNode(freqList -> char_val, freqList -> freq);

      newln = create_listnode(treenode);
      top = insert_listnode(top, newln); 
   
      freqList = freqList -> next;
    }

  return top;//returns top of the tree ie header
}


//Function calculates tree heightThis function calls the tree height helper function which return the tree height
int height_Tree(TreeNode * treen)
{
  if(treen == NULL)
    {
      printf("ERROR in height_Tree\n");
    }

  return Helper_height(treen, 0);
}

//i referenced this part in the report

//This function will write one bit at a time to the outputfile
int Bit_write(FILE * fptr, unsigned char bit, unsigned char * wbit, unsigned char * cbyte)
{
  if((*wbit) == 0)
    {
    *cbyte = 0;
    }

  unsigned char temp;


  int abc = 0;
  int ret;
     
  temp  = bit << (7 - (*wbit));//we perform bithshift  operation here 
  *cbyte |= temp; //equal to or
  
  //IMPORTANT STEP
  if ((*wbit) == 7)
    {
      ret = fwrite(cbyte, sizeof(unsigned char), 1, fptr);
     
      if(ret == 1)
	{
	  abc = 1;
	}
      
      else
	{
	  abc = -1;
	}
    }
 
  *wbit = ((*wbit) + 1) % 8;

  return abc;
}

//Function converts character values to bits using bit shifting
void char_bits(FILE * outfptr, int ch, unsigned char * whichbit, unsigned char * curbyte)
{
  //THIS IS IMPORTANT
  unsigned char bitmask = 0x40;//hexadecimal for 64 

  //prints each character at a time
  //printf("bitmak = %c",bitmask);
  
  while(bitmask > 0)
    {
      
      Bit_write(outfptr, (ch & bitmask) == bitmask, whichbit, curbyte);
      //LEFT SHIFT IT HERE
      bitmask >>= 1;
    }
}

//This function is reponsible to create the bit version of the header in the compressed file. Process in the first way as defined in the pj2 worksheet ie. using the charnums
void header_tree(TreeNode * tn, unsigned int numChar, char * Filename)
{
  FILE * outfptr;
  unsigned char wbit = 0;
  unsigned char cbyte = 0;

  //new line inclusion is important
  unsigned char nline = '\n';
  
  outfptr = fopen(Filename, "w");
  
  if(outfptr == NULL)
    {
      printf("OUtfile is NULL\n");
      return;
    }
  
  Helper_header(tn, outfptr, &wbit, &cbyte);

  //need to sen the folloeing thin
  Bit_write(outfptr, 0, &wbit, &cbyte);
 
  //this accounts for padding here
  fix_bits(outfptr, &wbit, &cbyte);
  
  //numChar elements tot the out file ptr
  fwrite(&numChar, sizeof(unsigned int), 1, outfptr);
  
  //need to also write the newline character
  fwrite(&nline, sizeof(unsigned char), 1, outfptr);
  
  //closing the file
  fclose(outfptr);
}

//Function to compress the file itself. 
int compression(char * infile, char * outfile, int ** array3d, int * mapping)
{
  //keeps track of which bit we are on 
  unsigned char wbit = 0;
  //keeps track of which byte we are on
  unsigned char cbyte = 0;
  
  FILE * infptr = fopen(infile, "r");
  
  if(infptr  == NULL)
    {
      printf("CANNOt open file in compression");
      return(0); 
    }
  
  //append last file
  FILE * ofptr;
  ofptr = fopen(outfile, "a"); 
  
  if(ofptr == NULL)
    {
      fclose(ofptr);
      printf("ERROR outputptr is nULL in compression here");
      return(0);
    }
  
  int char_one;//for once character at a time
  //counters
  int i;
  int j;
  
  while( !feof(infptr) )
    {  
      //get one of the character here.
      char_one = fgetc(infptr);
      
      if(char_one != EOF)
	{
	 i = mapping[char_one];
	 j = 1;
	
	 while(array3d[i][j] != -1)
	   {
	     //this will write all of the values from the array in binarry form
	     Bit_write(ofptr, (array3d[i][j] == 1), &wbit, &cbyte);	      
	     j++;
	   }
	}
    }
  
  //we now take care of extrapadding 
  fix_bits(ofptr, &wbit, &cbyte);
 
  //get rid of memoryleaks here
  fclose(infptr);
  fclose(ofptr);

return(0);
}

//Padding
int fix_bits(FILE * fptr, unsigned char * wbit, unsigned char * cbyte)
{
  int extrabits = 0;
  
  //we keep calling bitwrite to get eh padding correct
  while((*wbit) != 0)
    {
      extrabits = Bit_write(fptr, 0, wbit, cbyte);
    
      //when there are no extrabits 
      if(extrabits == -1)
	{
	  return -1;
	}
    }
 
  //printf("Extrabits are = %d\n", extrabits);

  return extrabits;
}







