#include <stdlib.h>
#include <stdio.h>

char* op[] = {"AND", "OR", "NOT", "-", "+", "*", "/"};
char* vl[] = {"true", "false", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};

int logo = 0;
int arth = 0;
typedef struct _stringstack
{
  char* string;
  struct _stringstack* next;
  int type;
  //type:
  // 2: arthmetic op
  // 3: logical op
  // 4: arthimetic vl
  // 5: logical vl
  // 6: space
  // 7: semicolon
  //-1: unidentified
} Stack;

int ehhh(char char1)
{
  if(char1 != ' ' && char1 != ';')
  {
    return 0;
  }
  return 1;
}

Stack * tokenizer(char* arr)
{
    Stack* head = (Stack*)malloc(sizeof(Stack*));
    Stack* ptr = head;
    int i;
    char* expression = arr;
    int start = 0;
    for(i = 0; arr[i] != '\0'; i++)
    {
      if(ehhh(expression[i]) || expression[i] == '\0')
      {
        char* str = malloc(sizeof(char) * ((i - start) + 1));
        int j = 0;int al;
        for(j = 0; start+j < i; j++)
        {
          if(ehhh(expression[i]) || expression[i] != '\0')
          {
              str[j] = expression[start + j];
          }
          else
          {
              break;
          }
        }
        //
        ptr->string = str;
        ptr->type = Type(str);
        //
        ptr->next = (Stack*)malloc(sizeof(Stack*));
        ptr = ptr->next;
        i = start + j;
        start = i;
      }
      if(ehhh(expression[i]))
      {
        if(i > 0)
        {
          if(ehhh(expression[i]) && expression[i+1] == ' ')
          {
            char * str = "";
            if(expression[i] == ' ')
            {
              ptr->string = " ";
              ptr->type = 6;
            }
            else
            {
              ptr->string = ";";
              ptr->type = 7;
            }
            ptr->next = (Stack*)malloc(sizeof(Stack*));
            ptr = ptr->next;
            start = i;
            i++;
          }
        }
        char * str = "";
        if(expression[i] == ' ')
        {
          ptr->string = " ";
          ptr->type = 6;
        }
        else
        {
          ptr->string = ";";
          ptr->type = 7;
        }
        ptr->next = (Stack*)malloc(sizeof(Stack*));
        ptr = ptr->next;
        i++;
        start = i;
      }
    }
    if(expression[start + i] != '\0')
    {
    	char* str = malloc(sizeof(char) * ((i - start) + 1));
	int j;
	for(j = 0; j < i; j++)
    	{
      		str[j] = expression[start + j];
    	}
    	ptr->string = str;
    	ptr->type = Type(str);
    	ptr->next = (Stack*)malloc(sizeof(Stack*));
    	ptr = ptr->next;
    	start = i;
    	ptr->next = NULL;
    }
    else
    {
	ptr = NULL;
    }
    return head;
}

int length(char * string)
{
  int i;
  for(i = 0; string[i] != '\0'; i++){}
  return i;
}

int compareTo(char * string1, char * string2)
{
  if(length(string1) != length(string2))
  {
    return 0;
  }
  else
  {
    int i;
    for(i = 0; i < length(string1) + 1; i++)
    {
      if(string1[i] != string2[i])
      {
        return 0;
      }
    }
    return 1;
  }
}

int opType(char* string)
{
  int i;
  for(i = 0; i < 7; i++)
  {
    if(compareTo(op[i], string) == 1)
    {
      return i;
    }
  }
  return -1;
}

int vlType(char* string)
{
  int i;
  for(i = 0; i < 12; i++)
  {
    if(compareTo(vl[i], string) == 1)
    {
      return i;
    }
  }
  return -1;
}

int Type(char* string)
{
  if(vlType(string) != -1)
  {
    if(vlType(string) > 1)
    {
      return 4;
    }
    else
    {
      return 5;
    }
  }
  else if(opType(string) != -1)
  {
    if(opType(string) > 2)
    {
      return 2;
    }
    else
    {
      return 3;
    }
  }
  else
  {
    return -1;
  }
}

Stack * spaceRemover(char* string)
{
	Stack* head = tokenizer(string);
	Stack* prev = head;
	Stack* tk = head;
	Stack* head1 = head;
        if(tk->next != NULL)
        {
                if(tk->next->next != NULL)
                {
                        while(tk->next->next->next != NULL)
                        {
                                tk = tk->next;
                        }
                        if(compareTo(tk->next->string, "") != 0)
                        {
                                tk->next =  NULL;
                        }
                }
        }
	int uko = 1;
	if(head == NULL)
	{

    return NULL;
		uko = 0;
	}
	if(head->next == NULL)
	{

    return head;
		uko = 0;
	}
	int i;
	while(head->next != NULL)
        {
		for(i = 0; head->string[i] != '\0'; i++)
		{
			if(head->string[i] == ' ' && length(head->string) > 1)
			{
				Stack* temp1 = (Stack*)malloc(sizeof(Stack*));
				Stack* temp2 = (Stack*)malloc(sizeof(Stack*));
				char * string1 = " ";
				temp1->string = string1;
				temp1->next = temp2;
				temp2->next = head->next;
				prev->next = temp1;
				char * item = (char*) malloc((length(head->string) - 1)*sizeof(char));
				int j;
				for(j = 0; j < length(head->string); j++)
				{
					item[j] = head->string[1 + j];
				}
				temp2->string = item;
				head = head->next->next;
				break;
			}
		}
		prev = head;
		head = head->next;
        }
	head = head1;
	prev = head;
	while(head->next != NULL && uko == 1)
	{
		if(head->type == 6)
		{
			if(head->next->type == 6)
			{
				if(head->next->next != NULL)
				{
					if(head->next->next->next->type == 6 && prev->type != 6)
					{
						Stack* temp = (Stack*)malloc(sizeof(Stack*));
						char * item = (char*) malloc((length(head->string) + 1)*sizeof(char));
						int i = 0;
						int checker = 1;
						for(i = 0; i < length(head->string); i++)
						{
							item[i+1] = head->next->next->string[i];
							if(item[i+1] == ' ')
							{
								checker = 0;
								break;
							}
						}
						item[0] = ' ';
						temp->string = item;
						if(head->next->next->next != NULL)
						{
							temp->next = head->next->next->next;
						}
						else
						{
							temp->next = NULL;
						}
						temp->type = -1;
						if(checker == 1)
						{
							head->next = temp;
						}
					}

				}
			}
		}
		prev = head;
		head = head->next;
	}
	return head1;
}

int printingError(char* string)
{
	Stack* head = spaceRemover(string);
	Stack* item = head;
  Stack* unended = head;
  int error_item = 1;
  if(head == NULL)
  {
    printf("Found 0 expressions: 0 logical and 0 arithmetic..\nError: parse error in expression 0 Scan error in expression 0: Missing Expression.\n\t\"\".\n");
    return 0;
  }
  if(head->next == NULL)
  {
    if(head->type == 2)
    {
      arth++;
    }
    else
    {
      logo++;
    }
    printf("Found 1 expressions: %d logical and %d arithmetic..\nError: parse error in expression 0 Scan error in expression 0: Missing operator.\n\t\"\".\n", logo, arth);
    return 0;
  }
	int itemCounter = 3;
	int expressionCounter = 0;
	int express_bin = 0;
	Stack* prev = head;
  int u = 0;
  int n = 0;
  if(head->next->next != NULL)
	{
		head = head->next->next;
	}
  if(prev->type == 3 || prev->type == 2)
  {
    if(opType(head->string) != 2)
    {
      //printf("3opType: %d, string;\n", opType(prev->string), prev->string);
      printf("Error: parse error in expression %d Unexpected Operator.\n\"%s\"\n", 0, prev->string);
      error_item = 0;
      prev = head;
      if(head->next != NULL)
      {
        if(head->next->next != NULL)
        {
          head = head->next;
          head = head->next;
        }
      }
      n = 1;
    }
    if(express_bin == 0)
    {
      if(head->type == 2)
      {
        arth++;
      }
      else
      {
        logo++;
      }
      express_bin = 1;
      expressionCounter++;
    }
  }
  if(prev->type == -1)
  {
    printf("Error: parse error in expression %d Unknown Identifier.\n\"%s\"\n", 0, prev->string);
    if(((prev->type == -1 && head->type == 2)) || ((prev->type == -1 && head->type == 3)))
    {
      if(opType(head->string) != 2)
      {
        printf("Error: parse error in expression %d Unexpected operator.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
      }
    }
    error_item = 0;
    prev = head;
    if(head->type == 2 || head->type == 3)
		{
			if(express_bin == 0)
      {
				if(head->type == 2)
				{
					arth++;
				}
				else
				{
					logo++;
				}
        express_bin = 1;
        expressionCounter++;
			}
		}
    if(head->next != NULL)
    {
        if(head->next->next != NULL)
        {
          head = head->next;
          head = head->next;
          n = 1;
        }
    }
  }
  if(head->next != NULL)
  {
    if(head->next->next == NULL && n == 0)
    {
      printf("Error: parse error in expression %d Missing Opperand.\n\"%s\"\n", (expressionCounter), prev->string);
      error_item = 0;
    }
  }
  else
  {
    if(n == 0)
    {
        printf("Error: parse error in expression %d Missing Opperand.\n\"%s\"\n", (expressionCounter), prev->string);
    }
    error_item = 0;
  }
	while(head != NULL)
	{
    if(head->type == 7)
    {
      expressionCounter++;
    }
		if(head->type == 2 || head->type == 3)
		{
			if(express_bin == 0)
      {
				if(head->type == 2)
				{
					arth++;
				}
				else
				{
					logo++;
				}
        express_bin = 1;
        expressionCounter++;
			}
		}
		if(head->type == 7)
		{
			express_bin = 0;
		}
    else if((itemCounter == 7 && u == 0) && express_bin == 1)
    {
      u = 1;
      printf("Error: parse error in expression %d Expresion has not ended.\n", (expressionCounter ));
      int k = 0;
      printf("\"%s",unended->string);
      unended = unended->next->next;
      for(k = 1; k < 4; k++)
      {
        printf(" %s",unended->string);
        unended = unended->next->next;
      }
      printf("\".\n");
      error_item = 0;
    }

    if(itemCounter >= 7)
		{
      //printf("1itemcounter: |%d|\n", itemCounter);
			if((prev->type == 3 && head->type == 4) || (prev->type == 2 && head->type == 5))
			{
				printf("Error: parse error in expression %d Unknown Identifier.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
			}
      if((prev->type == 3 && head->type == 4) || (prev->type == 2 && head->type == 5))
			{
				printf("Error: parse error in expression %d Unknown Identifier.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
			}
      else if((prev->type == 2 && head->type == 4) || (prev->type == 2 && head->type == 5) || (prev->type == 3 && head->type == 4) || (prev->type == 3 && head->type == 5))
      {
        if(opType(head->string) != 2)
        {
          //printf("1opType: %d, string;\n", opType(prev->string), prev->string);
            printf("Error: parse error in expression %d Unexpected operator.\n\"%s %s\"\n", (expressionCounter), prev->string, head->string);
            error_item = 0;
        }
      }
  		else if((prev->type == 4 && head->type == 4) || (prev->type == 4 && head->type == 5) || (prev->type == 5 && head->type == 4) || (prev->type == 5 && head->type == 5))
      {
        printf("Error: parse error in expression %d Unexpected operand.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
      }
		}
    else
    {
      //printf("2itemcounter: |%d| prev: |%s| head: |%s|\n", itemCounter, prev->string, head->string);
      if((prev->type == 2 && head->type == -1) || (prev->type == 3 && head->type == -1))
  		{
        printf("Error: parse error in expression %d Unknown operand.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
  	  }
  		else if((prev->type == 4 && head->type == -1) || (prev->type == 5 && head->type == -1))
  		{
  		  printf("Error: parse error in expression %d Unknown operator.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
  			if(express_bin == 0)
  			{
          express_bin = 1;
  			}
  		}
  		else if((prev->type == 2 && head->type == 2) || (prev->type == 2 && head->type == 3) || (prev->type == 3 && head->type == 2) || (prev->type == 3 && head->type == 3))
      {
        if(opType(head->string) != 2)
        {
            printf("Error: parse error in expression %d Unexpected operator.\n\"%s %s\"\n", (expressionCounter), prev->string, head->string);
            error_item = 0;
        }
      }
  		// else if((prev->type == 4 && head->type == 4) || (prev->type == 4 && head->type == 5) || (prev->type == 5 && head->type == 4) || (prev->type == 5 && head->type == 5))
      // {
      //   printf("Error: parse error in expression %d Unexpected operand.\n\"%s\"\n", (expressionCounter), head->string);
      //   error_item = 0;
      // }
  		else if((prev->type == 3 && head->type == 4) || (prev->type == 2 && head->type == 5))
  		{
  			printf("Error: parse error in expression %d Operand Type Mismatch.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
  		}
  		else if((prev->type == 4 && head->type == 3) || (prev->type == 5 && head->type == 2))
  		{
  			printf("Error: parse error in expression %d Operator Type Mismatch.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
  		}
  		else if((prev->type == 2 && head->type == 3) || (prev->type == 2 && head->type == 3))
  		{
  			printf("Error: parse error in expression %d Missing Operand.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
  		}
  		else if((prev->type == 4 && head->type == 5) || (prev->type == 5 && head->type == 4))
      {
        printf("Error: parse error in expression %d Missing Operator.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
  		}
      else if((prev->type == 2 && head->type == 6) || (prev->type == 3 && head->type == 6))
  		{
        printf("Error: parse error in expression %d Unknohead->next->typwn operand.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
  	  }
  		else if((prev->type == 4 && head->type == 6) || (prev->type == 5 && head->type == 6))
  		{
  		  printf("Error: parse error in expression %d Unknown operator.\n\"%s\"\n", (expressionCounter), head->string);
        error_item = 0;
  			if(express_bin == 0)
  			{
          express_bin = 1;
  			}
  		}
    }

    //printf("prev: num|%s| type|%d|, head: |%s| type|%d|.\n", prev->string, prev->type, head->string, head->type);
    //printf("ITEM COUNTER: |%d|, prev: num|%s| type|%d|, head: |%s| type|%d|.\n",itemCounter, prev->string, prev->type, head->string, head->type);
    prev = head;
		if(head != NULL)
		{
			if(head->next != NULL)
			{
				if(head->next->type == 6 || (head->type == 6 ))
				{
          //printf("1 head: num|%s| type|%d|, head->next: |%s| type|%d|.\n", head->string, head->type, head->next->string, head->next->type);
          head = head->next;
          itemCounter++;
				}
        if(head->type == 7 && head->next != NULL)
        {
            if(head->next->type != 6)
            {
              if(head->next->next == NULL)
              {
                printf("Error: parse error in expression %d Expresion has not ended.\n\"%s\"\n", (expressionCounter), head->string);
                error_item = 0;
                break;
              }
            }
          }
        }
    }
      //printf(" 2head: num|%s| type|%d|, head->next: |%s| type|%d|.\n\"%s\"\n", head->string, head->type, head->next->string, head->next->type);
      if(head != NULL)
      {
        head = head->next;
      }
      itemCounter++;
      //printf("itemCounter: |%d|.\n", itemCounter);
      if(itemCounter == 6)
  		{
        if(head->next != NULL)
        {
          if(head->type != 7 && head->next->type != 6)
  			  {
  				      printf("Error: parse error in expression %d The Expression has not ended.\n\"%s\"\n", (expressionCounter), head->string);
                error_item = 0;
  				}
  			}
        else if(head->type == 7 && head->next == NULL)
        {
          printf("Error: parse error in expression %d The Expression has not ended.\n\"%s\"\n", (expressionCounter), head->string);
          error_item = 0;
        }
  		}
	}
	printf("Found %d Expressions: %d Logical and %d Arithemtic.\n",  expressionCounter, logo, arth);
  return error_item;
}

int main(int argc, char** argv)
{
    if(argc > 2)
    {
      printf("Error: parse error in expression %d Too many arguements.\n");
      return 0;
    }
    //MORE THAN ONE ARGUEMENT ERROR OUTPUT
    int i = 0;
    while(argv[1][i] != '\0')
    {
      //printf("%c", argv[1][i]);
      i++;
    }
    //printf(".\n");
    //PRINTING OUT INPUT^
    if(i == 0)
    {
       printf("Found 1 expressions: 0 logical and 0 arithmetic..\nError: parse error in expression 0 Scan error in expression 0: incomplete expression.\n\t\"\".\n");
       return 0;
    }
    //EMPTY INPUT
    //printValid(argv[1]);
    //PRINT OUT TOKENIZED STRING
    if(printingError(argv[1]) == 1)
    {
      printf("OK.\n");
    }
    return 0;
}
