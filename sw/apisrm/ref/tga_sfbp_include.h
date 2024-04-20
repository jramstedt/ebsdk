#define PROMPT() {if (data_ptr->sfbp$l_alt_console){printf("Continue ? ");getchar();printf("\n");}else getchar();} 
#define REPEAT(index,count) for( index=0;index<count;index++)
#define CLEAR(i,var,size) for(i=0;i<size;i++)var[i]=0;
