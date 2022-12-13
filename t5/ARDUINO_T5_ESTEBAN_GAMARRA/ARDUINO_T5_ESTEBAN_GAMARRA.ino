#include <SoftwareSerial.h>
SoftwareSerial hc06(2,3);

#define BLUE 4
#define GREEN 5
#define RED 6
#define LINE_BUF_SIZE 128   //Maximum input string length
#define ARG_BUF_SIZE 64     //Maximum argument string length
#define MAX_NUM_ARGS 8     //Maximum number of arguments

bool error_flag = false;
float tempe;
char line[LINE_BUF_SIZE];
char args[MAX_NUM_ARGS][ARG_BUF_SIZE]; 
int cmd_help();
int cmd_temp();
int cmd_seta();
int cmd_setb();
int cmd_exit();
int cmd_start();
int TMN=25; 
int TMM=60;

int (*commands_func[])(){
    &cmd_help,
    &cmd_temp,
    &cmd_seta,
    &cmd_setb,
    &cmd_exit,
    &cmd_start,
};
const char *commands_str[] = {
    "help",
    "temp",
    "seta",
    "setb",
    "exit",
    "start",
};
int num_commands = sizeof(commands_str) / sizeof(char *);

void setup() {
    Serial.begin(9600);
    hc06.begin(9600);
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    cli_init();
}
void loop() {
    cmd_led();
    my_cli();
}
   
void cli_init(){ // INTRO
    Serial.println("Welcome to the temperature LED indicator program.");
    Serial.println("IF YOU WANT TO CHANGE THEM GO TO cmd_led TO CHANGE THE VALUES");
    Serial.println("Type \"help\" to see a list of commands.");
} 
void my_cli(){ //programa principal donde se ve si se ejecutan las string
    Serial.print("> ");
    read_line();
    if(!error_flag){
        parse_line();
    }
    if(!error_flag){
        execute();
    }
    memset(line, 0, LINE_BUF_SIZE);
    memset(args, 0, sizeof(args[0][0]) * MAX_NUM_ARGS * ARG_BUF_SIZE);
    error_flag = false;
}

void read_line(){ //donde se interactúa primeramente con las string y el módulo
    String line_string;
    while(!hc06.available()){
      cmd_led();
    }
    if(hc06.available()){
        line_string = hc06.readStringUntil('\n');
        if(line_string.length() < LINE_BUF_SIZE){
          line_string.toCharArray(line, LINE_BUF_SIZE);
          Serial.println(line_string);
        }
        else{
          Serial.println("Input string too long.");
          error_flag = true;
        }
    }
}
void parse_line(){ //se checkea la string
    char *argument;
    int counter = 0;
    argument = strtok(line, " ");
    while((argument != NULL)){
        if(counter < MAX_NUM_ARGS){
            if(strlen(argument) < ARG_BUF_SIZE){
                strcpy(args[counter],argument);
                argument = strtok(NULL, " ");
                counter++;
            }
            else{
                Serial.println("Input string too long.");
                error_flag = true;
                break;
            }
        }
        else{
            break;
        }
    }
 }
int execute(){  
    for(int i=0; i<num_commands; i++){
        if(strcmp(args[0], commands_str[i]) == 0){
            return(*commands_func[i])();
        }
    }
    Serial.println("Invalid command. Type \"help\" for more.");
    return 0;
}
int cmd_help(){//SERIAL
    if(args[1] == NULL){
        help_help();
    }
    else if(strcmp(args[1], commands_str[0]) == 0){
        help_help();
    }
    else{
        help_help();
    }
}
void help_help(){ //SERIAL
    Serial.println("The following commands are available:");
    for(int i=0; i<num_commands; i++){
        Serial.print("  ");
        Serial.println(commands_str[i]);
    }
    Serial.println("");
    Serial.println("*temp* for the current temperature on the LN35");
    Serial.println("*exit* to exit the CLI " );
    Serial.println("You can also type *start* and write *stop* to stop the function, of printing in a loop");

}
int cmd_temp(){ //lee la temperatura una vez y envía el valor al hc06, aquí se ejecuta el monitor y la temperatura actual en la aplicación
  tempe=analogRead(A0);
  tempe=tempe*0.48828125;
  Serial.print("TEMPERATURE: ");
  Serial.print(tempe);
  Serial.print("*C");
  Serial.println("\n");
  hc06.print(tempe);
}
int cmd_led(){ //se prenden los leds según las temperaturas seteadas
    if (((analogRead(A0)*0.48828125)>=TMN)&&((analogRead(A0)*0.48828125)<=TMM)){ 
      digitalWrite(GREEN,HIGH);
      digitalWrite(RED,LOW);
      digitalWrite(BLUE,LOW);
    }
      if(TMN>(analogRead(A0)*0.48828125)){ 
      digitalWrite(BLUE,HIGH);
      digitalWrite(GREEN,LOW);
      digitalWrite(RED,LOW);
   }
    if ((analogRead(A0)*0.48828125)>TMM){
      digitalWrite(RED,HIGH);
      digitalWrite(BLUE,LOW);
      digitalWrite(GREEN,LOW);
    }
}
int cmd_exit(){
    Serial.println("Exiting CLI.");
    while(1);
}
int cmd_seta(){ //setear la minima tempertura, para ello hay un pequeño intervalo en el que se puede meter el input desde el bluetooth
  while(hc06.available()==0){
    if(TMN>=(-55)&&TMN<=(150)){
    Serial.println("Set your minimum Temperature:");
    TMN=hc06.parseInt();
    Serial.println("YOUR MIN IS:");
    Serial.println(TMN);
    }
    if(TMN>=(-55)&&TMN<=(150)){
      break;
    }
    else{
    Serial.println("Wrong input, set again");
    Serial.println("Set your minimum Temperature:");
    TMN=hc06.parseInt();
    }
  }
 }
int cmd_setb(){ //setear la maxima tempertura, para ello hay un pequeño intervalo en el que se puede meter el input desde el bluetooth
    while(hc06.available()==0){
    if((TMM>=(-55))&&TMM<=(150)){
    Serial.println("Set your maximum Temperature:");
    TMM=hc06.parseInt();
    Serial.println("YOUR MAX IS:");
    Serial.println(TMM);
    }
    if((TMM>=(-55))&&TMM<=(150)&&TMM>TMN){
      break;
    }
    else{
    Serial.println("Wrong input, set again");
    Serial.println("Set your maximum Temperature:");
    TMM=hc06.parseInt();
    }
  }
}
int  cmd_start(){ //command start solamente que imprime en el serial y se cancela con S de input
  int i=1;
  while(i=1){
    Serial.print("/");
    Serial.print(analogRead(A0)*0.4882812);
    Serial.print("*C");
    delay(1000);
    Serial.print("\n");
    cmd_led();
    if(Serial.read()=='S'){
      break; 
    }
  }
}
