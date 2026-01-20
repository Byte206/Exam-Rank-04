#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

char  *str;
int	  i;
int flag;

int parsesum();
int parseproduct();
int parsefactor();

void  unexpected(char c)
{
	if (flag == 1)
		return ;
	if (c)
		printf("Unexpected token '%c'\n", c);
	else 
		printf("Unexpected end of input\n");
	flag = 1;
	return ;
}

int parsefactor()
{
	if (flag == 1)
		return (1);
	int nb = 0;
	if(str[i] == '(')
	{
		i++;
		nb = parsesum();
		if(str[i] != ')')
			unexpected(str[i]);
		i++;
		return nb;
	}
	else if(isdigit(str[i]))
	{
		nb = str[i] - '0';
		i++;
		if(isdigit(str[i]))
			unexpected(str[i]);
		return nb;
	}
	unexpected(str[i]);
	return (0);
}

int parseproduct()
{
	if (flag == 1)
		return (1);
	int res = parsefactor();
	while(str[i] == '*')
	{
		i++;
		res = res * parsefactor();
	}
	return (res);
}

int	parsesum()
{
	if (flag == 1)
		return (1);
	int res = parseproduct();
	while(str[i] == '+')
	{
		i++;
		res = res + parseproduct();
	}
	return (res);
}

int main(int argc, char **argv)
{
	if (argc != 2 || !argv[1])
		return(1);
	str = argv[1];
	i = 0;
	flag = 0;

	int res = parsesum();
	if(str[i] != '\0')
		unexpected(str[i]);
	if (flag == 1)
		return (1);
	printf("%d\n", res);
	return (0);
}
