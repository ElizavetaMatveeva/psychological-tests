#include <stdio.h>
#include <time.h>
#define TRUE 1
#define FALSE 0
typedef short int bool;

char getTestNum(FILE* f){
	int testNum;
	char s[500];
	fgets(s, 500, f);
	puts(s);
	scanf("%i", &testNum); // Считываем ответ пользователя
	return testNum+'0';
}

unsigned int countAnswerVariants (FILE* f) { 
	unsigned int var=0; 
	char c=fgetc(f);
	while (c!='\n') { // Пока не дойдём до конца строки, считываем символы и считаем количество встретившихся табуляций
		if (c=='\t') var++ ;
		c=fgetc(f);
	} 
	return var;
} 

unsigned int countQuestions(char* filename){ 
	FILE* f=fopen(filename, "r");
	unsigned int q=0;
	while(!feof(f)){
		char s[2000];
		if (fgets (s, 2000, f)){ // Считываем строку из файла
			if (s[0]!='\n' && s[0]!='\r') q++;  // Увеличиваем счётчик в том случае, если первый считанный символ - не перевод строки и не возврат каретки
		}
	}
	fclose(f);
	return q-1; // Возвращаем значение, уменьшенное на 1, так как в первой строке хранится не вопрос, а название и результаты теста
}

void printQuestion(FILE* f){ // Выводим вопрос посимвольно, пока не встретим табуляцию, сигнализирующую о переходе к вариантам ответа
	char c;
	c=fgetc(f); 
	while (c!='\t'){
		printf("%c", c);
		c=fgetc(f); 
	}
}

void scanVariants(unsigned int var, FILE* f, char variant[var+1][80]){ 
	char c;
	int i, j;
	*variant[0]='0'; // Элемент массива с индексом 0 не удобен в использовании, так как нумерация вариантов ответа начинается с 1, поэтому обнуляем его
	for (i=1; i<var+1; i++){
		c=fgetc(f);
		for (j=0; (j<80 && c!='\t' && c!='\n' && c!=EOF); j++){ // Цикл работает, если ещё есть место для записи, если не осуществлён переход к другому варианту ответа, на новую строку и если не конец файла
			variant[i][j]=c; 
			c=fgetc(f); 
		}
		variant[i][j]='\0'; // После считанного варианта ответа записываем символ конца строки
	}
}

void fileWork(FILE* f, unsigned int var, unsigned int q, char variant[var+1][80]){
 	printQuestion(f); // Вывод вопроса
	scanVariants(var, f, variant); // Считываем варианты ответа	
}

void printSeparatedVariants(unsigned int var, char variant[var+1][80], int num[]){ // Выводим варианты ответа вразброс
	unsigned int answer=1, output=0, random;
	while (output<var){
		random=rand()%(var+1);  // Генерируем случайное значение из нужного диапазона 
		if (*variant[random]!='0') { 
			printf(" %u) %s", answer, variant[random]);
			num[answer]=random; // Запоминаем настоящий номер вариата ответа
			*variant[random]='0'; // Записываем 0, чтобы на следующих шагах цикла не обращаться к этому элементу
			answer++; // Увеличиваем номер выводимого варианта ответа
			output++; // Увеличиваем количество выводов
		}
	}
}

int getAnswer(unsigned int var){
	char c, res;
	int k=2; // Переменная, считающая количество введённых символов 
	fflush(stdin);
	res=getchar(); c=getchar(); // Считываем первые 2 символа, запоминаем 1-ый в переменную res, чтобы вернуть это значение, если был введён нужный символ
	while(c!='\n'){
		c=getchar();
		k++;
	} 
	if (k==2 && res>='1' && res<=var+'0') return res-'0'; // Если пользователь ввёл 2 символа, причём 1-ый из диапазона возможных значений, 2-ой - признак конца ввода, и то возвращаем результат как целое число
	else {
		printf("Error \n");
		return 0;
	}
}

void answerQuestion(unsigned int var, char (*variant)[80], int num[], int result[]){
	bool b=FALSE; // Изначально считаем, что нужный ответ не найден
	printSeparatedVariants(var, variant, num); // Выводим варианты ответа вразброс
	printf("\n"); 
	int ans=getAnswer(var); // Считываем ответ пользователя
	if (ans==0) {
		while (b==FALSE) {
			ans=getAnswer(var); 
			if (ans!=0) b=TRUE; // Ответ найден, если возвращаемый результат не равен 0
		}
	}
	result[num[ans]]++; // Засчитываем выбор пользователя 
}

unsigned int findMax(unsigned int var, int result[]){
	unsigned int imax, i, max=0;
	for (i=1; i<var+1; i++){
		if(result[i]>max) {
			max=result[i];
			imax=i;
		}
	}
	return imax; // Возвращаем номер максимально часто встречающегося варианта ответа
}

void scanSymbols(FILE* f, unsigned int imax){
	char c;
	unsigned int tab=0; // Обнуляем счётчик, отвечающий за количество табуляций
	while (tab<imax){  // Считываем символы из файла до тех пор, пока не дойдём до нужного результата
		c=fgetc(f);
		if (c=='\t') tab++;
	}
} 

void printResult(char* filename, unsigned int imax){
	FILE* f=fopen(filename, "r");
	scanSymbols(f, imax);
	printRes(f);
	fclose(f);
}

void printRes(FILE* f){
	char c=fgetc(f);
	while(c!='\t' && c!='\n'){  // Выводим результат посимвольно до тех пор, пока не дойдём до конца
		printf("%c", c);
		c=fgetc(f);
	}
}

int main(){
	srand(time(0));
	FILE* f=fopen("Greeting.txt", "r"); // Открываем файл, в котором записано приветствие 
	char testNum=getTestNum(f); // Выводим приветствие из файла и считываем ответ пользователя
	fclose(f);
	char filename[]="Test0.txt";
	filename[4]=testNum;
	FILE* f1=fopen(filename, "r");
	unsigned int var=countAnswerVariants(f1); // Считаем количество вариантов ответа на вопросы
	unsigned int q=countQuestions(filename); // Считаем количество вопросов в тесте
	int i, ans, result[var+1], num[var+1]; // Массив result считает для каждого варианта ответа, сколько раз пользователь его выбрал, num записывает реальные номера ответов, выведенных вразброс
	num[0]=0; // Нулевой элемент массива не удобен в использовании, так как нумерация вариантов ответа начинается с 1, поэтому сразу обнуляем его
	for (i=0; i<var+1; i++) result[i]=0; 
	char variant[var+1][80]; // Массив variant будет содержать считанные из файла варианты ответа 
	for (i=0; i<q; i++)	{
		fileWork(f1, var, q, variant); // Работаем с файлом: выводим из файла вопрос и считываем варианты ответа
		answerQuestion(var, variant, num, result);  // Выводим вариант ответа вразброс и считываем ответ пользователя
	}
	unsigned int imax=findMax(var, result); // Ищем вариант ответа, встречающийся чаще всего
	fclose(f1);
	printResult(filename, imax); // Выводим результат теста
	return 0;
}
