#include <ESP8266WiFi.h>  //essa biblioteca já vem com a IDE. Portanto, não é preciso baixar nenhuma biblioteca adicional
#include <PubSubClient.h> //Importa biblioteca MQTT


#define SSID_REDE     ""  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE    ""  //coloque aqui a senha da rede que se deseja conectar

#define ID_MQTT "quarto"
#define MQTT_SERVER "iot2017.nc2.iff.edu.br"
#define MQTT_USER   "saeg2017"
#define MQTT_PASS   "semsenha"
#define PORTA   443

#define TOPICOLAMP1 "/quarto/lampada/lampada1"
#define ledTop1 D2 // Led digital 2
int staTop1 = LOW;

#define TOPICOLAMP2 "/quarto/lampada/lampada2"
#define ledTop2 D5 // Led digital 5
int staTop2 = LOW;




WiFiClient cliente;
PubSubClient clienteMQTT(cliente);



//Função: faz a conexão WiFI
//Parâmetros: nenhum
//Retorno: nenhum
boolean conectaWiFi(void)
{
    
    cliente.stop();
    
    delay(500);
    Serial.println("Conectando-se à rede WiFi...");
    Serial.println();  
    delay(1000);
    WiFi.begin(SSID_REDE, SENHA_REDE);
    
    int contDelay = 0;
    while ((WiFi.status() != WL_CONNECTED) && (contDelay < 60) ) 
    {
        delay(500);
        Serial.print(".");
        contDelay++;
        
    }
    if(WiFi.status() != WL_CONNECTED){
       Serial.println("");
       Serial.println("WiFi não connectado");
       return false;
    }
      
    Serial.println("");
    Serial.println("WiFi connectado com sucesso!");  
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());

    delay(500);
    return true;
}

//Função: inicializa parâmetros de conexão clienteMQTT(endereço do 
//        broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void iniciaMQTT(void){
  clienteMQTT.setServer(MQTT_SERVER, PORTA);
  clienteMQTT.setCallback(mqtt_callback); 
}

// 
//Função: conectando ao servidor por MQTT
//Parâmetros: nenhum
//Retorno: nenhum
void connectaClienteMQTT(void) {
  // Espera até estar conectado ao servidor
  while (!clienteMQTT.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Tentativa de conexão
    if( clienteMQTT.connect(ID_MQTT, MQTT_USER, MQTT_PASS )) {
      Serial.println("connected");
      clienteMQTT.subscribe(TOPICOLAMP1);
      clienteMQTT.subscribe(TOPICOLAMP2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(clienteMQTT.state());
      Serial.println(" try again in 5 seconds");
      // Espera 5 segundo e tenta novamente
      delay(5000);
    }
  }
}

String mensagem(byte* payload, unsigned int length){

  String msg;
 
  //obtem a string do payload recebido
  for(int i = 0; i < length; i++) 
  {
     char c = (char)payload[i];
     msg += c;
  }
  return msg;
}

// 
//Função: Trata o valor do Topico
//Parâmetros: nenhum
//Retorno: nenhum
void trataTopico(char* topic,String msg){
  
    if (strcmp(topic,TOPICOLAMP1)==0){
      if(msg.equals("1")){
        staTop1 = HIGH;
      }
      if(msg.equals("0")){
        staTop1 = LOW;
      }
    }

    if (strcmp(topic,TOPICOLAMP2)==0){
      if(msg.equals("1")){
        staTop2 = HIGH;
      }
      if(msg.equals("0")){
        staTop2 = LOW;
      }
    }
    

    Serial.print("Led Topico 1:");
    Serial.println(staTop1);

    Serial.print("Led Topico 2:");
    Serial.println(staTop2);
  
    
    digitalWrite(ledTop1, staTop1);
    digitalWrite(ledTop2, staTop2);
    
    
}




//Função: função de callback 
//        esta função é chamada toda vez que uma informação de 
//        um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{     
    String msg = mensagem(payload,length);
    
    trataTopico(topic,msg);
    
}
void iniciaGPIO(void){
  
  pinMode(ledTop1, OUTPUT);
  digitalWrite(ledTop1, LOW);

  pinMode(ledTop2, OUTPUT);
  digitalWrite(ledTop2, LOW);

    
}

void setup() {
  Serial.begin(115200);
  delay(10);
  iniciaGPIO();
  if (conectaWiFi()){
     iniciaMQTT();
  }
  
}

void loop() {
 if (WiFi.status() == WL_CONNECTED){
      if (!clienteMQTT.connected()) {
        connectaClienteMQTT();
      }
     
      clienteMQTT.loop(); 
  }else{
    conectaWiFi();   
      
  }

}
