#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>
#define MAX_SIZE 100

//function prototyping for the needed one
void userMenu(char*);
void insert(int, int, int, int, int);

//the node to contain user's data and the next node (typedefed, idk if it's a vocabulary but doesn't really matter)
typedef struct HashNode{
    char username[20];
    char email[36];
    char password[36];
    int balance;
    struct HashNode* next;
}hashNode;

//double linked list structure for candlestick representation
typedef struct Candle{
    char sticks[33];
    int status;
    struct Candle *next, *prev;
}candle;

//single linked list structure for history
typedef struct History{
    int val;
    int pos;
    int tp;
    int sl;
    int reward;
    struct History* next;
}history;

//minimizing the usage of global variable
typedef struct Price{
    int open;
    int close;
    int highest;
    int lowest;
}price;

//global variables
candle* heads = NULL;   //the head node of the candlestick linked list
char chart[33][102];    //2d array to represent the chart
history* head = NULL;   //the head node of the history linked list
hashNode* buckets[MAX_SIZE]; //array of pointers for each bucket in hashtable
price prev_price;       //store the previous price of a stock market, close price used for the next open price of a stock market

//initializing the hashtable
void initializeBuckets(){
    for(int i = 0; i < MAX_SIZE; i++){
        buckets[i] = NULL;
    }
}

//create a node and insert user's data into it
hashNode* createNode(char username[], char email[], char password[], int balance){
    hashNode* newNode = (hashNode*)malloc(sizeof(hashNode));
    strcpy(newNode->username, username);
    strcpy(newNode->email, email);
    strcpy(newNode->password, password);
    newNode->balance = balance;
    newNode->next = NULL;

    return newNode;
}

//hash function, formula: sum of ASCII values of username mod 26
int hashFunc(char username[]){
    int sum = 0;
    for(int i = 0; i < strlen(username); i++){
        sum += username[i];
    }
    return sum % 26;
}

//insert user's data into hashtable if not exist
void insertNode(char username[], char email[], char password[], int balance){
    hashNode* newNode = createNode(username, email, password, balance);
    int index = hashFunc(username);

    hashNode* head = buckets[index];    //head is the first node in the linked list, it's mainly used for traversal and check if the data exist or not
    
    //check the presence of the data
    while(head != NULL){
        if(strcmp(head->username, username) == 0){
            strcpy(head->email, email);
            strcpy(head->password, password);
            head->balance = balance;
            return;
        }
        head = head->next;
    }

    //point back to the head
    head = buckets[index];

    //if head is null, point it to the new node
    if(head == NULL){
        buckets[index] = newNode;
        return;
    }

    //if head is not null, traverse to the end and insert at the end
    while(head->next != NULL){
        head = head->next;
    }
    head->next = newNode;
}

//search if the user's data exist
int search(char username[20]){
    int index = hashFunc(username);
    hashNode* curr = buckets[index];    //curr is the first node in the linked list, acts the same with head node at insert function

    //traverse the whole linked list
    while(curr != NULL){
        if(strcmp(curr->username, username) == 0){
            return 1;   //data exist
        }
        curr = curr->next;  //continue searching
    }
    return 0;   //data doesn't exist
}

//login
void loginUser(){
    //store the username for searching purposes later on this function
    char username[20];

    char email_input[36];   //get the email from user's input
    char email_read[36];    //get the email from user.txt file to compare with email_input
    int valid_email = 0;    //check the validity of the email

    //the idea here is the same with the email one
    char password_input[36];
    char password_read[36];
    int valid_password = 0;

    //store the user's balance to be written to the hashtable if needed
    int balance;

    //using a standard do while loop for input validation
    do{
        email:
        printf("\e[1;1H\e[2J");

        //read from the file
        FILE* file;
        email_input[0] = '\0';
        printf("Email (0 to exit): ");
        scanf("%[^\n]", email_input); getchar();

        //check if the user wants to exit, by typing 0
        int len_email = strlen(email_input);
        if(len_email == 1 && email_input[0] == '0'){
            printf("\n");
            return;
        }

        //check if the input is valid, this case is 0 length
        if(email_input[0] == '\0'){
            printf("\e[0;31mInvalid email!\e[0m\n");
            printf("Press [ENTER] to continue...");
            getchar();
            goto email;
        }
    
        printf("\e[1;1H\e[2J");
		password_input[0] = '\0';
        printf("Password (0 to exit): ");
        scanf("%[^\n]", password_input); getchar();

        //check if the user wants to exit, by typing 0
        int len_password = strlen(password_input);
        if(len_password == 1 && password_input[0] == '0'){
            printf("\n");
            return;
        }

        //check if the input is valid, this case is 0 length
        if(password_input[0] == '\0'){
            printf("\e[0;31mInvalid password!\e[0m\n");
            printf("Press [ENTER] to continue...");
            getchar();
            goto email;
        }

        //compare the input with the user.txt
        file = fopen("users.txt", "r+");
        while(fscanf(file, "%[^#]#%[^#]#%[^#]#%d\n", username, email_read, password_read, &balance) == 4){
            if(strcmp(email_input, email_read) == 0 && strcmp(password_input, password_read) == 0){
                //if the email and password are valid
                valid_email = 1;
                valid_password = 1;
                break;
            }
            //continue comparing
        }
        fclose(file);   //DON'T FORGET to CLOSE the file

        //if the email and password are invalid
        if(valid_email == 0 && valid_password == 0){
            printf("\e[0;31mInvalid email or password!\e[0m\n");
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(email_input[0] == '\0' || password_input[0] == '\0' || valid_email == 0 || valid_password == 0);

    //check if the data already exist in the hashtable or not
    int exist = search(username);
    if(exist == 0){
        //if doesn't exist;
        //insert the data to the hashtable
        insertNode(username, email_read, password_read, balance);
        //and then log in the user
        srand(time(NULL));
        userMenu(username);
    }else{
        //if already exist;
        //log in the user
        srand(time(NULL));
        userMenu(username);
    }
}

//LOT of validation for user's username, password, email
//utilize the do while loop for validation
void registerUser(){
    char username[20];
    username[0] = '\0';
    int check_len = 0;
    int only_alphanum = 0;
    int valid = 0;
    int len;

    do{
        printf("\e[1;1H\e[2J");
        printf("Enter username: ");
        scanf("%[^\n]", username); getchar();

        len = strlen(username);

        //cancel operation by typing 0
        if(len == 1 && username[0] == '0'){
            printf("\n");
            return;
        }

        //validate the valid characters are only alphabet and numbers
        for(int i = 0; i < len; i++){
            if(ispunct(username[i]) || username[i] == ' '){
                only_alphanum = 0;
                printf("\e[0;31mUsername must only contains alphabet & number!\e[0m\n");
                break;
            }else{
                only_alphanum = 1;
            }
        }

        //validate the password must not be empty or more than 20 characters
        if(username[0] == '\0'){
            check_len = 0;
            username[0] = '\0';
            printf("\e[0;31mUsername cannot be empty!\e[0m\n");
        }else{
            check_len = 1;
        }

        if(len > 20){
            check_len = 0;
            username[0] = '\0';
            printf("\e[0;31mUsername must not exceeds 20 characters maximum!\e[0m\n");
        }else if(len <= 20 && username[0] != '\0'){
            check_len = 1;
        }

        if(check_len == 1 && only_alphanum == 1){
            valid = 1;
        }else{
            check_len = 0;
            only_alphanum = 0;
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(username[0] == '\0' || len > 20 || valid == 0);

    char email[36];
    char emailcopy[36];
    email[0] = '\0';
    //some variable to store the validation status
    int valid_length = 0;
    int valid_characters = 0;
    int valid_first_letter = 0;
    int valid_at = 0;
    int valid_dotcom = 0;
    int valid_domain = 0;
    //using the existing valid variable to wrap the valid status up
    valid = 0;

    do{
        printf("\e[1;1H\e[2J");
        printf("Enter email: ");
        scanf("%[^\n]", email); getchar();

        len = strlen(email);

        //copying the email to be written to the file, because the original string is tokenized below at the domain validation
        emailcopy[len + 1];
        emailcopy[len] = '\0';
        strcpy(emailcopy, email);

        //cancel operation
        if(len == 1 && email[0] == '0'){
            printf("\n");
            return;
        }

        //validate the length must be between 6 and 36 characters
        if(len < 6 || len > 36){
            printf("\e[0;31mEmail must not less than 6 characters minimum or exceeds 36 characters maximum!\e[0m\n");
        }else{
            valid_length = 1;
        }

        //validate the valid characters are only alphabet and numbers
        for(int i = 0; i < len; i++){
            if((email[i] >= 'a' && email[i] <= 'z') || (email[i] >= 'A' && email[i] <= 'Z') || (email[i] >= '0' && email[i] <= '9') || email[i] == '@' || email[i] == '.'){
                valid_characters = 1;
            }else{
                valid_characters = 0;
                printf("\e[0;31mEmail must only contains alphabet & number!\e[0m\n");
                break;
            }
        }

        //validate the first character of the email, must be a character
        if((email[0] >= 'a' && email[0] <= 'z') || (email[0] >= 'A' && email[0] <= 'Z')){
            valid_first_letter = 1;
        }else{
            printf("\e[0;31mEmail must start with alphabet!\e[0m\n");
        }

        //validate the email has only 1 '@' character
        int count = 0;
        for(int i = 0; i < len; i++){
            if(email[i] == '@'){
                count++;
            }
        }
        if(count == 1){
            valid_at = 1;
        }else{
            printf("\e[0;31mEmail must only have 1 @ symbol!\e[0m\n");
        }

        //validate .com is included in the email
        for(int i = 0; i < len; i++){
            if(email[i] == '.' && email[i+1] == 'c' && email[i+2] == 'o' && email[i+3] == 'm'){
                valid_dotcom = 1;
                break;
            }else{
                valid_dotcom = 0;
            }
        }
        if(valid_dotcom == 0){
            printf("\e[0;31mEmail must ends with '.com'!\e[0m\n");
        }

        //validate the email has a domain
        if(valid_at == 1){
            char* tok1 = strtok(email, "@");
            char* tok2 = strtok(NULL, ".");
            if(strcmp(tok2, "com") == 0 || tok2 == NULL){
                printf("\e[0;31mEmail must have a domain!\e[0m\n");
            }else{
                valid_domain = 1;
            }
        }else{
            printf("\e[0;31mEmail must have a domain!\e[0m\n");
        }

        //wrap the validation status in one variable so the while condition doesn't take too long
        if(valid_length == 1 && valid_characters == 1 && valid_first_letter == 1 && valid_at == 1 && valid_dotcom == 1 && valid_domain == 1){
            valid = 1;
        }else{
            valid_length = 0;
            valid_characters = 0;
            valid_first_letter = 0;
            valid_at = 0;
            valid_dotcom = 0;
            valid_domain = 0;

            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(len < 6 || len > 36 || valid == 0);

    char password[36];
    //some variable to store the validation status
    int valid_letter = 0;
    int valid_num = 0;
    int valid_symbol = 0;
    //using the existing valid variable to wrap the valid status up
    valid = 0;

    do{
        printf("\e[1;1H\e[2J");
        printf("Enter password: ");
        scanf("%s[^\n]", password); getchar();

        len = strlen(password);

        //cancel operation
        if(len == 1 && password[0] == '0'){
            printf("\n");
            return;
        }

        //validate length must be between 8 and 36
        if(len < 8 || len > 36){
            printf("\e[0;31mPassword must not less than 8 characters minimum or exceeds 36 characters maximum!\e[0m\n");
        }

        //validate cpassword has at least 1 character and 1 numbers
        int count_num = 0, count_letter = 0;
        for(int i = 0; i < len; i++){
            if((password[i] >= 'a' && password[i] <= 'z') || (password[i] >= 'A' && password[i] <= 'Z')){
                count_letter++;
            }else if(password[i] >= '0' && password[i] <= '9'){
                count_num++;
            }
        }
        if(count_letter == 0){
            printf("\e[0;31mPassword must contains at least 1 alphabet\e[0m\n");
        }else if(count_num == 0){
            printf("\e[0;31mPassword must contains at least 1 number\e[0m\n");
        }else{
            valid_letter = 1;
            valid_num = 1;
        }

        //validate valid characters are only alphabet and numbers
        for(int i = 0; i < len; i++){
            if(ispunct(password[i]) || password[i] == ' '){
                valid_symbol = 0;
                break;
            }else{
                valid_symbol = 1;
            }
        }
        if(valid_symbol == 0){
            printf("\e[0;31mPassword must only contains alphabet & numbers\e[0m\n");
        }

        //wrap the validation status in one variable so the while condition doesn't take too long
        if(valid_letter == 1 && valid_num == 1 && valid_symbol == 1){
            valid = 1;
        }else{
            valid_letter = 0;
            valid_num = 0;
            valid_symbol = 0;

            printf("Press [ENTER] to continue...");
            getchar();
        }

    }while(len < 8 || len > 36 || valid == 0);

    //write to the file user.txt once the inputs are valid
    FILE* user;
    user = fopen("users.txt", "a+");
    fprintf(user, "%s#%s#%s#%d\n", username, emailcopy, password, 10000);   //last item is the currency, default currency is 10000
    fclose(user);   //DON'T FORGET to close the file

    printf("\n");
}

//Main menu is pretty much done, standard one
//do while for standard input validation
void mainMenu(){
    int opt;
    do{
        printf("\e[1;1H\e[2J");
        printf("Meta Fi\n");
        for(int i = 0; i < 25; i++){
            printf("-");
        }
        printf("\n");
        printf("1. Log In to Trade\n2. Register Account\n3. Exit Application\n>> ");
        scanf("%d", &opt); getchar();
        printf("\n");
        switch(opt){
            case 1:{
                loginUser();
                break;
            }
            case 2:{
                registerUser();
                break;
            }
            case 3:{
                printf("Thank you for using this program!\n");
                exit(0);
                break;
            }
        }
    }while(opt != 3);
}

//read data from user.txt and insert to the hashtable
void readUserData(){
    char username[20], email[36], password[36];
    int balance;

    FILE* file = fopen("user.txt", "r+");
    while(fscanf(file, "%[^#]#%[^#]#%[^#]#%d\n", username, email, password, &balance) == 4){
        insertNode(username, email, password, balance);
    }
    fclose(file);
}

void hide_cursor(){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

int main(){
	hide_cursor();
    readUserData();
    mainMenu(); //modularizing the code
    return 0;
}

//Initialize the chart
void initializeChart(){
    int j;
    for(int i = 0; i < 33; i++){
        for(j = 0; j < 101; j++){
            chart[i][j] = '-';
        }
        chart[i][j] = '|';
    }
}

void initializePrice(){
    prev_price.open = 0;
    prev_price.close = 0;
    prev_price.highest = 0;
    prev_price.lowest = 0;
}

price generatePrice(){
    price curr;
    if(prev_price.close == 0){
        curr.open = 10;
    }else{
        curr.open = prev_price.close;
    }

    if(curr.open < 10){
        curr.open = 10;
    }else if(curr.open > 330){
        curr.open = 330;
    }

    int price_diff;
    int prob = rand() % 100;
    if(prob < 10){
        curr.close = curr.open;
    }else{
        prob = rand() % 2;
        price_diff = ((rand() % 6) + 1) * 10;

        if(prob == 0){
            curr.close = (curr.open - price_diff);
        }else{
            curr.close = (curr.open + price_diff);
        }
    }

    if(curr.close < 10){
        curr.close = 10;
    }else if(curr.close > 330){
        curr.close = 330;
    }
    
    prob = rand() % 100;
    price_diff = (rand() % 5) * 10;
    if(curr.close > curr.open){
        if(prob < 30){
            curr.highest = curr.close;
        }else{
            curr.highest = (curr.close + price_diff);
        }
    }else{
        if(prob < 30){
            curr.highest = curr.open;
        }else{
            curr.highest = (curr.open + price_diff);
        }
    }

    prob = rand() % 100;
    price_diff = (rand() % 5) * 10;
    if(curr.close > curr.open){
        if(prob < 30){
            curr.lowest = curr.open;
        }else{
            curr.lowest = (curr.open - price_diff);
        }
    }else{
        if(prob < 30){
            curr.lowest = curr.close;
        }else{
            curr.lowest = (curr.close - price_diff);
        }
    }

    prev_price = curr;

    return curr;
}

//create candlestick
candle* createCandle(){
    price curr = generatePrice();
    int open_idx = 33 - (curr.open / 10);
    int close_idx = 33 - (curr.close / 10);
    int highest_idx = 33 - (curr.highest / 10);
    int lowest_idx = 33 - (curr.lowest / 10);

    candle* newNode = (candle*)malloc(sizeof(candle));

    newNode->status = rand() % 2;

    //the data here is the structure of candlestick itself (body and tails)
    for(int i = 0; i < 33; i++){
        newNode->sticks[i] = '-';
    }

    if(open_idx > close_idx){       //meaning open price lower than close price, it's a green candlestick
        newNode->status = 0;
        for(int i = highest_idx; i < close_idx; i++){
            newNode->sticks[i] = (char)179;     //the upper wick (highest price) of a candlestick
        }
        for(int i = close_idx; i <= open_idx; i++){
            newNode->sticks[i] = (char)219;     //the body of a candlestick
        }
        for(int i = open_idx + 1; i <= lowest_idx; i++){
            newNode->sticks[i] = (char)179;     //the lower wick (lowest price) of a candlestick
        }
    }else{                          //vice versa
        newNode->status = 1;
        for(int i = highest_idx; i < open_idx; i++){
            newNode->sticks[i] = (char)179;
        }
        for(int i = open_idx; i <= close_idx; i++){             //pretty much the same
            newNode->sticks[i] = (char)219;
        }
        for(int i = close_idx + 1; i <= lowest_idx; i++){
            newNode->sticks[i] = (char)179;
        }
    }
    
    newNode->next = NULL;
    newNode->prev = NULL;

    return newNode;
}

//insert candlestick to the linked list
void insertCandle(){
    candle* newNode = createCandle();

    if(heads == NULL){
        heads = newNode;
        return;
    }
    candle* temp = heads;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->prev = temp;
}

//display the candlestick statically
void displayStatic(int tp, int sl, int pos){
    initializeChart();

    //if the linked list is empty, it will insert 25 candlestick (initial chart)
    if(heads == NULL){
        for(int i = 0; i < 10; i++){
            insertCandle();
        }
    }

    //dcopy all the candlestick linked list to the chart
    printf("\e[1;1H\e[2J");
    int color[102];
    candle* temp = heads;
    for(int i = 0; i < 33; i++){
        int j = 0;
        while(temp != NULL){
            if(temp->status == 0){
                color[j] = 32;
            }else{
                color[j] = 31;
            }
            chart[i][j] = temp->sticks[i];
            temp = temp->next;
            j+=2;
        }
        temp = heads;
    }

    //display the chart with the candlestick (randomized candlestick color)
    int price = 330;
    for(int i = 0; i < 33; i++){
        for(int j = 0; j < 102; j++){
            if(i == tp && chart[i][j] == '-'){
                printf("\e[1;92m%c\e[0m", chart[i][j]);
            }else if(i == sl && chart[i][j] == '-'){
                printf("\e[1;91m%c\e[0m", chart[i][j]);
            }else if(i == pos && chart[i][j] == '-'){
                printf("\e[1;97m%c\e[0m", chart[i][j]);
            }else if(chart[i][j] == (char)179 || chart[i][j] == (char)219){
                printf("\e[1;%dm%c\e[0m", color[j], chart[i][j]);   //pick the color from color array
            }else{
                printf("\e[0;17m%c\e[0m", chart[i][j]);
            }
        }
        printf("%d\n", price);
        price -= 10;
    }
}

//display the candlestick dynamically
void display(int tp, int sl, int pos){
    initializeChart();
    
    static int counter = 0; //counter for the candlestick
    counter++;

    if(counter >= 40){  //when it reaches certain point, the chart will move forward automatically
        heads = heads->next;
    }

    //here the logic is same as the static one
    if(heads == NULL){
        for(int i = 0; i < 10; i++){
            insertCandle();
        }
    }

    printf("\e[1;1H\e[2J");
    candle* temp = heads;
    int color[102];
    for(int i = 0; i < 33; i++){
        int j = 0;
        while(temp != NULL){
            if(temp->status == 0){
                color[j] = 32;
            }else{
                color[j] = 31;
            }
            chart[i][j] = temp->sticks[i];
            temp = temp->next;
            j+=2;
        }
        temp = heads;
    }

    int price = 330;
    for(int i = 0; i < 33; i++){
        for(int j = 0; j < 102; j++){
            if(i == tp && chart[i][j] == '-'){
                printf("\e[1;92m%c\e[0m", chart[i][j]);
            }else if(i == sl && chart[i][j] == '-'){
                printf("\e[1;91m%c\e[0m", chart[i][j]);
            }else if(i == pos && chart[i][j] == '-'){
                printf("\e[1;97m%c\e[0m", chart[i][j]);
            }else if(chart[i][j] == (char)179 || chart[i][j] == (char)219){
                printf("\e[1;%dm%c\e[0m", color[j], chart[i][j]);
            }else{
                printf("\e[0;17m%c\e[0m", chart[i][j]);
            }
        }
        printf("%d\n", price);
        price -= 10;
    }
}

//move to the next candlestick
void next(int tp, int sl, int pos){
    initializeChart();

    //if the linked list is not empty, it will move to the next candlestick
    if(heads->next != NULL){
        heads = heads->next;
    }

    //the logic here is the same as the display one
    printf("\e[1;1H\e[2J");
    candle* temp = heads;
    int color[102];
    for(int i = 0; i < 33; i++){
        int j = 0;
        while(temp != NULL){
            if(temp->status == 0){
                color[j] = 32;
            }else{
                color[j] = 31;
            }
            chart[i][j] = temp->sticks[i];
            temp = temp->next;
            j+=2;
        }
        temp = heads;
    }

    int price = 330;
    for(int i = 0; i < 33; i++){
        for(int j = 0; j < 102; j++){
            if(i == tp && chart[i][j] == '-'){
                printf("\e[1;92m%c\e[0m", chart[i][j]);
            }else if(i == sl && chart[i][j] == '-'){
                printf("\e[1;91m%c\e[0m", chart[i][j]);
            }else if(i == pos && chart[i][j] == '-'){
                printf("\e[1;97m%c\e[0m", chart[i][j]);
            }else if(chart[i][j] == (char)179 || chart[i][j] == (char)219){
                printf("\e[1;%dm%c\e[0m", color[j], chart[i][j]);
            }else{
                printf("\e[0;17m%c\e[0m", chart[i][j]);
            }
        }
        printf("%d\n", price);
        price -= 10;
    }
}

//move to the previous candlestick
void prev(int tp, int sl, int pos){
    initializeChart();

    //if the linked list is not empty, it will move to the previous candlestick
    if(heads->prev != NULL){
        heads = heads->prev;
    }

    //the logic here is the same as the display one
    printf("\e[1;1H\e[2J");
    candle* temp = heads;
    int color[102];
    for(int i = 0; i < 33; i++){
        int j = 0;
        while(temp != NULL){
            if(temp->status == 0){
                color[j] = 32;
            }else{
                color[j] = 31;
            }
            chart[i][j] = temp->sticks[i];
            temp = temp->next;
            j+=2;
        }
        temp = heads;
    }

    int price = 330;
    for(int i = 0; i < 33; i++){
        for(int j = 0; j < 102; j++){
            if(i == tp && chart[i][j] == '-'){
                printf("\e[1;92m%c\e[0m", chart[i][j]);
            }else if(i == sl && chart[i][j] == '-'){
                printf("\e[1;91m%c\e[0m", chart[i][j]);
            }else if(i == pos && chart[i][j] == '-'){
                printf("\e[1;97m%c\e[0m", chart[i][j]);
            }else if(chart[i][j] == (char)179 || chart[i][j] == (char)219){
                printf("\e[1;%dm%c\e[0m", color[j], chart[i][j]);
            }else{
                printf("\e[0;17m%c\e[0m", chart[i][j]);
            }
        }
        printf("%d\n", price);
        price -= 10;
    }
}

//check the position of the candlestick
int checkPosition(int* tp, int* sl, int* pos){
    initializeChart();

    candle* temp = heads;
    for(int i = 0; i < 33; i++){
        int j = 0;
        while(temp != NULL){
            chart[i][j] = temp->sticks[i];
            temp = temp->next;
            j+=2;
        }
        if(chart[*tp][j-2] == (char)179 || chart[*tp][j-2] == (char)219){ //candlestick touches take profit mark
            *tp = -1;
            *sl = -1;
            *pos = -1;
            return 1;   //return 1 for take profit (balance will be added with the reward)
        }else if(chart[*sl][j-2] == (char)179 || chart[*sl][j-2] == (char)219){   //candlestick touches stop loss mark
            *tp = -1;
            *sl = -1;
            *pos = -1;
            return 0;   //return 0 for stop loss (balance will be subtracted with the reward)
        }
        temp = heads;   //go to the next candlestick
    }
    return -1;  //return -1 if the candlestick hasn't touched any mark
}

//calculate the reward, formula: reward = (value * abs(close_price - position)) / 100
int calculateReward(int val, int pos, int* open_pos){
    int reward;
    reward = (val * abs(prev_price.close - pos)) / 100;
    *open_pos = 0;
    return reward;
}

void banner(int balance){
    printf("\nTrade | Balance: $%d\n", balance);
    for(int i = 0; i < 25; i++){
        printf("-");
    }
    printf("\n");
}

//open a short position (sell)
void openShort(int balance, int* val, int* pos, int* tp, int* sl, int* open_pos, int* tp_index, int* sl_index, int* pos_index){
    if(*open_pos == 1){ //if there is already an active position
        displayStatic(*tp_index, *sl_index, *pos_index);
        printf("\nThere is already an active position!\n");
        printf("Press [ENTER] to continue...");
        getchar();
        return;
    }

    int valid;
    //input validation
    do{
        displayStatic(*tp_index, *sl_index, *pos_index);
        banner(balance);
        valid = 0;

        printf("Value: ");
        scanf("%d", val); getchar();

        if(*val > balance){
            printf("\e[0;31mNot enough balance!\e[0m\n");
        }
        if(*val < 10){
            printf("\e[0;31mInvalid amount! Must be greater than 10\e[0m\n");
        }

        if(*val >= 10 && *val <= balance ){
            valid = 1;
        }else{
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(valid == 0);

    do{
        displayStatic(*tp_index, *sl_index, *pos_index);
        banner(balance);
        valid = 0;

        printf("Position: ");
        scanf("%d", pos); getchar();
    
        if(*pos < 20 || *pos > 330){
            printf("\e[0;31mPosition must be between 20 and 330!\e[0m\n");
        }
        if(*pos % 10 != 0){
            printf("\e[0;31mPosition must be in multiple of 10!\e[0m\n");
        }

        if(*pos >= 20 && *pos <= 330 && *pos % 10 == 0){
            *pos_index = 33 - (*pos / 10);
            valid = 1;
        }else{
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(valid == 0);

    do{
        displayStatic(*tp_index, *sl_index, *pos_index);
        banner(balance);
        valid = 0;

        printf("Take profit: ");
        scanf("%d", tp); getchar();

        if(*tp >= *pos){
            printf("\e[0;31mTake profit must be lower than position!\e[0m\n");
        }
        if(*tp < 10 || *tp > 330){
            printf("\e[0;31mTake profit must be between 10 and 330!\e[0m\n");
        }
        if(*tp % 10 != 0){
            printf("\e[0;31mTake profit must be in multiple of 10!\e[0m\n");
        }

        if(*tp < *pos && *tp >= 10 && *tp <= 330 && *tp % 10 == 0){
            *tp_index = 33 - (*tp / 10);
            valid = 1;
        }else{
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(valid == 0);

    do{
        displayStatic(*tp_index, *sl_index, *pos_index);
        banner(balance);
        valid = 0;

        printf("Stop loss: ");
        scanf("%d", sl); getchar();

        if(*sl <= *pos){
            printf("\e[0;31mStop loss must be higher than position!\e[0m\n");
        }
        if(*sl < 10 || *sl > 330){
            printf("\e[0;31mStop loss must be between 10 and 330!\e[0m\n");
        }
        if(*sl % 10 != 0){
            printf("\e[0;31mStop loss must be in multiple of 10!\e[0m\n");
        }

        if(*sl > *pos && *sl >= 10 && *sl <= 330 && *sl % 10 == 0){
            *sl_index = 33 - (*sl / 10);
            valid = 1;
        }else{
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(valid == 0);

    //set the open position to 1 if the input is valid
    *open_pos = 1;
    displayStatic(*tp_index, *sl_index, *pos_index);
    printf("\nOrder placed successfully\n");
    printf("Press [ENTER] to continue...");
    getchar();
}

void openLong(int balance, int* val, int* pos, int* tp, int* sl, int* open_pos, int* tp_index, int* sl_index, int* pos_index){    
    if(*open_pos == 1){ //if there is already an active position
        displayStatic(*tp_index, *sl_index, *pos_index);
        printf("\nThere is already an active position!\n");
        printf("Press [ENTER] to continue...");
        getchar();
        return;
    }

    int valid;
    //input validation
    do{
        displayStatic(*tp_index, *sl_index, *pos_index);
        banner(balance);
        valid = 0;

        printf("Value: ");
        scanf("%d", val); getchar();

        if(*val > balance){
            printf("\e[0;31mNot enough balance!\e[0m\n");
        }
        if(*val < 10){
            printf("\e[0;31mInvalid amount! Must be greater than 10\e[0m\n");
        }

        if(*val >= 10 && *val <= balance ){
            valid = 1;
        }else{
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(valid == 0);

    do{
        displayStatic(*tp_index, *sl_index, *pos_index);
        banner(balance);
        valid = 0;

        printf("Position: ");
        scanf("%d", pos); getchar();
    
        if(*pos < 20 || *pos > 330){
            printf("\e[0;31mPosition must be between 20 and 330!\e[0m\n");
        }
        if(*pos % 10 != 0){
            printf("\e[0;31mPosition must be in multiple of 10!\e[0m\n");
        }

        if(*pos >= 20 && *pos <= 330 && *pos % 10 == 0){
            *pos_index = 33 - (*pos / 10);
            valid = 1;
        }else{
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(valid == 0);

    do{
        displayStatic(*tp_index, *sl_index, *pos_index);
        banner(balance);
        valid = 0;

        printf("Take profit: ");
        scanf("%d", tp); getchar();

        if(*tp <= *pos){
            printf("\e[0;31mTake profit must be higher than position!\e[0m\n");
        }
        if(*tp < 10 || *tp > 330){
            printf("\e[0;31mTake profit must be between 10 and 330!\e[0m\n");
        }
        if(*tp % 10 != 0){
            printf("\e[0;31mTake profit must be in multiple of 10!\e[0m\n");
        }

        if(*tp > *pos && *tp >= 10 && *tp <= 330 && *tp % 10 == 0){
            *tp_index = 33 - (*tp / 10);
            valid = 1;
        }else{
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(valid == 0);

    do{
        displayStatic(*tp_index, *sl_index, *pos_index);
        banner(balance);
        valid = 0;

        printf("Stop loss: ");
        scanf("%d", sl); getchar();

        if(*sl >= *pos){
            printf("\e[0;31mStop loss must be lower than position!\e[0m\n");
        }
        if(*sl < 10 || *sl > 330){
            printf("\e[0;31mStop loss must be between 10 and 330!\e[0m\n");
        }
        if(*sl % 10 != 0){
            printf("\e[0;31mStop loss must be in multiple of 10!\e[0m\n");
        }

        if(*sl < *pos && *sl >= 10 && *sl <= 330 && *sl % 10 == 0){
            *sl_index = 33 - (*sl / 10);
            valid = 1;
        }else{
            printf("Press [ENTER] to continue...");
            getchar();
        }
    }while(valid == 0);

    //set the open position to 1 if the input is valid
    *open_pos = 1;
    displayStatic(*tp_index, *sl_index, *pos_index);
    printf("\nOrder placed successfully\n");
    printf("Press [ENTER] to continue...");
    getchar();
}

//update the balance
void updateData(char username[], int balance){
    int index = hashFunc(username);
    hashNode* head = buckets[index];

    while(head != NULL && strcmp(head->username, username) != 0){
        head = head->next;
    }

    head->balance = balance;

    //write back to the file to keep the balance updated when user logs out and then log back in
    index = 0;
    FILE* file = fopen("user.txt", "w+");
    while(index < MAX_SIZE){
        head = buckets[index];
        while(head != NULL){
            fprintf(file, "%s#%s#%s#%d\n", head->username, head->email, head->password, head->balance);
            head = head->next;
        }
        index++;
    }
    fclose(file);
}

//trade
void trade(char username[], int balance){
    int opt;
    int open_pos = 0;
    int val = 0;
    int pos = 0;
    int tp = 0;
    int sl = 0;
    int check;
    int reward;
    int tp_index = -1;
    int sl_index = -1;
    int pos_index = -1;

    do{
        printf("\nWelcome to MetaFi, %s | Balance: $%d\n", username, balance);
        for(int i = 0; i < 50; i++){
            printf("-");
        }
        printf("\n");
        printf("1. New day\n2. Previous\n3. Next\n4. Long\n5. Short\n6. Log Out\n>> ");
        scanf("%d", &opt); getchar();

        switch(opt){
            case 1:{    //new day (new candlestick added to the chart)
                insertCandle();
                display(tp_index, sl_index, pos_index);
                if(open_pos == 1){  //if there is an active position
                    check = checkPosition(&tp_index, &sl_index, &pos_index);  //check if candlestick touches take profit or stop loss mark

                    if(check == 1){ //if candlestick touches take profit
                        reward = calculateReward(val, pos, &open_pos);
                        balance += reward;  //add reward to balance
                        printf("\n\e[0;32mProfit: $%d\e[0m\n", reward);
                        insert(val, pos, tp, sl, reward);   //positive reward, insert to the history
                        updateData(username, balance);  //update the data
                        printf("Press [ENTER] to continue...");
                        getchar();
                        displayStatic(tp_index, sl_index, pos_index);
                    }else if(check == 0){   //if candlestick touches stop loss
                        reward = calculateReward(val, pos, &open_pos);
                        balance -= reward;  //subtract reward from balance
                        printf("\n\e[0;31mLoss: $%d\e[0m\n", reward);
                        insert(val, pos, tp, sl, (reward * (-1)));  //negative reward, insert to the history
                        updateData(username, balance);  //update the data
                        printf("Press [ENTER] to continue...");
                        getchar();
                        displayStatic(tp_index, sl_index, pos_index);
                    }
                }
                break;
            }
            case 2:{    //previous candlestick
                prev(tp_index, sl_index, pos_index);
                break;
            }
            case 3:{    //next candlestick
                next(tp_index, sl_index, pos_index);
                break;
            }
            case 4:{    //long position
                openLong(balance, &val, &pos, &tp, &sl, &open_pos, &tp_index, &sl_index, &pos_index);
                displayStatic(tp_index, sl_index, pos_index);
                break;
            }
            case 5:{    //short position
                openShort(balance, &val, &pos, &tp, &sl, &open_pos, &tp_index, &sl_index, &pos_index);
                displayStatic(tp_index, sl_index, pos_index);
                break;
            }
        }
    }while(opt != 6);

    mainMenu(); //if user chooses 6, go back to main menu
}

//create a single node for history
history* createHistory(int val, int pos, int tp, int sl, int reward){
    history* newNode = (history*)malloc(sizeof(history));
    newNode->val = val;
    newNode->pos = pos;
    newNode->tp = tp;
    newNode->sl = sl;
    newNode->reward = reward;
    newNode->next = NULL;

    return newNode;
}

//insert to the history linked list and sort by reward (ascending, priority queue)
void insert(int val, int pos, int tp, int sl, int reward){
    history* newNode = createHistory(val, pos, tp, sl, reward);

    if(head == NULL || reward > head->reward){
        newNode->next = head;
        head = newNode;
    }else{
        history* temp = head;
        while(temp->next != NULL && reward <= temp->next->reward){
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }
}

//display history based on priority
void displayHistory(char username[]){
    printf("\e[1;1H\e[2J");
    if(head == NULL){
        printf("\e[0;33mNo Trade History\e[0m\n");
        printf("Press [ENTER] to continue...");
        getchar();
        return;
    }

    history* temp = head;
    printf("Username: %s\n", username);
    for(int i = 0; i < 25; i++){
        printf("-");
    }
    printf("\n");
    int i = 1;
    while(temp != NULL){
        if(temp->reward < 0){
            printf("No. %d\nValue\t\t\t: $%d\nPosition\t\t: $%d\nTake Profit\t\t: $%d\nStop Loss\t\t: $%d\nReward\t\t\t: \e[0;31m$%d\e[0m\n\n", i, temp->val, temp->pos, temp->tp, temp->sl, temp->reward);
        }else{
            printf("No. %d\nValue\t\t\t: $%d\nPosition\t\t: $%d\nTake Profit\t\t: $%d\nStop Loss\t\t: $%d\nReward\t\t\t: \e[0;32m$%d\e[0m\n\n", i, temp->val, temp->pos, temp->tp, temp->sl, temp->reward);
        }
        temp = temp->next;
        i++;
    }
    printf("\n");
    printf("Press [ENTER] to continue...");
    getchar();
}

//basic trading guide
void guide(){
    printf("\e[1;1H\e[2J");
    printf("\e[0;34mWhat Is Forex?\e[0m\n\"Forex\", short for foreign exchange, is a global decentralized marketplace for trading currencies.\nIn forex trading, participants buy one currency by selling another currency simultaneously, with the aim of profiting from fluctuations in exchange rates.\nIt is one of the largest and most liquid financial markets in the world, operating 24 hours a day, five days a week.");
    printf("\n\n");
    printf("\e[0;33mWhat Is Candle Stick?\e[0m\n\"Canlde Stick\" in charts are a type of financial chart used to represent price movements in trading markets, including forex.\nThey visually display the open, high, low, and close prices for a specific period.\nif the close price is higher than the open price, the candle color will be green\nthen if the close price is less than the open price, the candle color will be red.\nshadow or tick is the highest and the lowest price of a candle.");
    printf("\n\n");
    printf("\e[0;35mWhat Is Position?\e[0m\n\"Position\" in forex trading refers to where we want to enter the market at certain price point");
    printf("\n\n");
    printf("\e[0;32mWhat Is Long?\e[0m\n\"Long\" in forex trading refers to a trading position where a trader buys a currency pair\nwith the expectation that its value will increase over time.");
    printf("\n\n");
    printf("\e[0;31mWhat Is Short?\e[0m\n\"Short\" in forex trading refers to a trading position where a trader sells a currency pair\nwith the expectation that its value will decrease over time.");
    printf("\n\n");
    printf("\e[0;32mWhat is Take Profit?\e[0m\n\"Take Profit\" in forex trading refers to where we will leave the market at the specified price point\nour position will automatically closed after the market price hit our take profit price");
    printf("\n\n");
    printf("\e[0;31mWhat Is Stop Loss?\e[0m\n\"Stop Loss\" in forex trading refers to where we will leave the market at the specified price point\nit's used to make sure that we don't loss all of our money and many more\nsame like take profit, our position will automatically closed after the market price hit our take profit price");
    printf("\n\n");
    printf("Press [ENTER] to continue...");
    getchar();
}

//get balance from the hashtable based on username as the key of the hash function
int getBalance(char username[]){
    int index = hashFunc(username);
    hashNode* head = buckets[index];

    while(head != NULL && strcmp(head->username, username) != 0){
        head = head->next;
    }

    return head->balance;
}

//user menu
void userMenu(char username[]){
    int opt;
    int balance = getBalance(username); //get balance from the hashtable

    do{
        printf("\e[1;1H\e[2J");
        printf("Meta Fi - User Menu\n");
        for(int i = 0; i < 25; i++){
            printf("-");
        }
        printf("\n");
        printf("1. Start Trading\n2. Trade History\n3. Guide\n4. Log out\n>> ");
        scanf("%d",&opt); getchar();

        switch(opt){
            case 1:{    //start trading
                initializePrice();
                display(-1, -1, -1);
                trade(username, balance);   //call trade function and pass the username and balance as arguments
                break;
            }
            case 2:{    //trade history
                displayHistory(username);   //call displayHistory function
                break;
            }
            case 3:{    //guide
                guide();    //call guide function
                break;
            }
        }
    }while(opt != 4);
    //if user chooses 4, go back to main menu
}
